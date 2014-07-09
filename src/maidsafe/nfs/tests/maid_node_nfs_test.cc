/*  Copyright 2014 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#include "boost/filesystem/path.hpp"

#include "maidsafe/common/test.h"
#include "maidsafe/passport/passport.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/bootstrap_file_operations.h"

#include "maidsafe/nfs/client/maid_node_nfs.h"

namespace maidsafe {

namespace nfs {

namespace test {

static const size_t kTestChunkSize = 1024 * 1024;

class MaidNodeNfsTest : public testing::Test {
 public:
  MaidNodeNfsTest() {}

 protected:
  void AddClient() {
    routing::Parameters::append_local_live_port_endpoint = true;
    routing::BootstrapContacts bootstrap_contacts;
    auto maid_and_signer(passport::CreateMaidAndSigner());
    clients_.emplace_back(nfs_client::MaidNodeNfs::MakeShared(maid_and_signer, bootstrap_contacts));
  }

  void CompareGetResult(const std::vector<ImmutableData>& chunks,
                        std::vector<boost::future<ImmutableData>>& get_futures) {
    for (size_t index(0); index < chunks.size(); ++index) {
      try {
        auto retrieved(get_futures[index].get());
        EXPECT_EQ(retrieved.data(), chunks[index].data());
      }
      catch (const std::exception& ex) {
        LOG(kError) << "Failed to retrieve chunk: " << DebugId(chunks[index].name())
                    << " because: " << boost::diagnostic_information(ex);
      }
    }
  }

  void GenerateChunks(const size_t iterations) {
    chunks_.clear();
    for (auto index(iterations); index > 0; --index) {
      chunks_.emplace_back(NonEmptyString(RandomString(kTestChunkSize)));
//       std::cout << "Generated : " << DebugId(chunks_.back().name()) << std::endl;
    }
  }

  std::vector<ImmutableData> chunks_;
  std::vector<std::shared_ptr<nfs_client::MaidNodeNfs>> clients_;
};

TEST_F(MaidNodeNfsTest, FUNC_Constructor) {
  routing::Parameters::append_local_live_port_endpoint = true;
  routing::BootstrapContacts bootstrap_contacts;
  auto maid_and_signer(passport::CreateMaidAndSigner());
  {
    auto nfs_new_account = nfs_client::MaidNodeNfs::MakeShared(maid_and_signer, bootstrap_contacts);
  }
  LOG(kInfo) << "joining existing account";
  auto nfs_existing_account =
      nfs_client::MaidNodeNfs::MakeShared(maid_and_signer.first, bootstrap_contacts);
}

TEST_F(MaidNodeNfsTest, FUNC_PutGet) {
  AddClient();
  ImmutableData data(NonEmptyString(RandomString(kTestChunkSize)));
  LOG(kVerbose) << "Before put";
  try {
    auto future(clients_.back()->Put(data));
    EXPECT_NO_THROW(future.get());
    LOG(kVerbose) << "After put";
  } catch (...) {
    EXPECT_TRUE(false) << "Failed to put: " << DebugId(NodeId(data.name()->string()));
  }

  auto future(clients_.back()->Get<ImmutableData::Name>(data.name()));
  try {
    auto retrieved(future.get());
    EXPECT_EQ(retrieved.data(), data.data());
  } catch (...) {
    EXPECT_TRUE(false) << "Failed to retrieve: " << DebugId(NodeId(data.name()->string()));
  }
}

TEST_F(MaidNodeNfsTest, FUNC_MultipleSequentialPuts) {
  routing::Parameters::caching = true;
  const size_t kIterations(10);
  GenerateChunks(kIterations);
  AddClient();
  int index(0);
  for (const auto& chunk : chunks_) {
    auto future(clients_.back()->Put(chunk));
    EXPECT_NO_THROW(future.get()) << "Store failure " << DebugId(NodeId(chunk.name()->string()));
    LOG(kVerbose) << DebugId(NodeId(chunk.name()->string())) << " stored: " << index++;
  }

  std::vector<boost::future<ImmutableData>> get_futures;
  for (const auto& chunk : chunks_) {
    get_futures.emplace_back(clients_.back()->Get<ImmutableData::Name>(
        chunk.name(), std::chrono::seconds(kIterations * 2)));
  }
  CompareGetResult(chunks_, get_futures);
  LOG(kVerbose) << "Multiple sequential puts is finished successfully";
}

TEST_F(MaidNodeNfsTest, FUNC_MultipleParallelPuts) {
  routing::Parameters::caching = false;
  const size_t kIterations(20);
  GenerateChunks(kIterations);
  AddClient();
  std::vector<boost::future<void>> put_futures;
  for (const auto& chunk : chunks_)
    put_futures.emplace_back(clients_.back()->Put(chunk));

  int index(0);
  for (auto& future : put_futures) {
    EXPECT_NO_THROW(future.get()) << "Store failure "
                                  << DebugId(NodeId(chunks_[index].name()->string()));
    LOG(kVerbose) << DebugId(NodeId(chunks_[index].name()->string())) << " stored: " << index;
    ++index;
  }

  std::vector<boost::future<ImmutableData>> get_futures;
  for (const auto& chunk : chunks_) {
    get_futures.emplace_back(clients_.back()->Get<ImmutableData::Name>(
        chunk.name(), std::chrono::seconds(kIterations * 2)));
  }
  CompareGetResult(chunks_, get_futures);
  LOG(kVerbose) << "Multiple parallel puts is finished successfully";
}

TEST_F(MaidNodeNfsTest, FUNC_FailingGet) {
  ImmutableData data(NonEmptyString(RandomString(kTestChunkSize)));
  AddClient();
  auto future(clients_.back()->Get<ImmutableData::Name>(data.name()));
  EXPECT_THROW(future.get(), std::exception) << "must have failed";
}

TEST_F(MaidNodeNfsTest, FUNC_MultipleClientsPut) {
  const size_t kIterations(10);
  GenerateChunks(kIterations);
  const size_t kClientsSize(5);
  for (auto index(kClientsSize); index > 0; --index)
    AddClient();

  for (const auto& chunk : chunks_) {
    LOG(kVerbose) << "Storing: " << DebugId(chunk.name());
    EXPECT_NO_THROW(clients_[RandomInt32() % kClientsSize]->Put(chunk));
  }

  LOG(kVerbose) << "Chunks are sent to be stored...";

  std::vector<boost::future<ImmutableData>> get_futures;
  for (const auto& chunk : chunks_)
    get_futures.emplace_back(
        clients_[RandomInt32() % kClientsSize]->Get<ImmutableData::Name>(chunk.name()));
  CompareGetResult(chunks_, get_futures);
}

/*
// The test below is disbaled as its proper operation assumes a delete funcion is in place
TEST_F(MaidNodeNfsTest, DISABLED_FUNC_PutMultipleCopies) {
  ImmutableData data(NonEmptyString(RandomString(kTestChunkSize)));
  boost::future<ImmutableData> future;
  GetClients().front()->Put(data);
  Sleep(std::chrono::seconds(2));

  GetClients().back()->Put(data);
  Sleep(std::chrono::seconds(2));

  {
    future = GetClients().front()->Get<ImmutableData::Name>(data.name());
    try {
      auto retrieved(future.get());
      EXPECT_EQ(retrieved.data(), data.data());
    }
    catch (...) {
      EXPECT_TRUE(false) << "Failed to retrieve: " << DebugId(NodeId(data.name()->string()));
    }
  }

  LOG(kVerbose) << "1st successful put";

  {
    future = GetClients().back()->Get<ImmutableData::Name>(data.name());
    try {
      auto retrieved(future.get());
      EXPECT_EQ(retrieved.data(), data.data());
    }
    catch (...) {
      EXPECT_TRUE(false) << "Failed to retrieve: " << DebugId(NodeId(data.name()->string()));
    }
  }

  LOG(kVerbose) << "2nd successful put";

  GetClients().front()->Delete<ImmutableData::Name>(data.name());
  Sleep(std::chrono::seconds(2));

  LOG(kVerbose) << "1st Delete the chunk";

  try {
    auto retrieved(env_->Get<ImmutableData>(data.name()));
    EXPECT_EQ(retrieved.data(), data.data());
  }
  catch (...) {
    EXPECT_TRUE(false) << "Failed to retrieve: " << DebugId(NodeId(data.name()->string()));
  }

  LOG(kVerbose) << "Chunk still exist as expected";

  GetClients().back()->Delete<ImmutableData::Name>(data.name());
  Sleep(std::chrono::seconds(2));

  LOG(kVerbose) << "2nd Delete the chunk";

  routing::Parameters::caching = false;

  EXPECT_THROW(env_->Get<ImmutableData>(data.name()), std::exception)
      << "Should have failed to retreive";

  LOG(kVerbose) << "PutMultipleCopies Succeeds";
  routing::Parameters::caching = true;
}
*/


TEST_F(MaidNodeNfsTest, FUNC_PopulateSingleBranchTree) {
  ImmutableData chunk(NonEmptyString(RandomAlphaNumericString(1024)));
  const size_t max_versions(5), max_branch(1);
  GenerateChunks(max_versions * 2);
  StructuredDataVersions::VersionName v_ori(0, chunks_.front().name());
  AddClient();
  auto create_version_future(clients_.back()->CreateVersionTree(chunk.name(), v_ori,
                                                                max_versions, max_branch));
  EXPECT_NO_THROW(create_version_future.get()) << "failure to create version";
  for (size_t index(1); index < (max_versions * 2); ++index) {
    StructuredDataVersions::VersionName v_old(index - 1, chunks_[index - 1].name());
    StructuredDataVersions::VersionName v_new(index, chunks_[index].name());
    auto put_version_future(clients_.back()->PutVersion(chunk.name(), v_old, v_new));
    EXPECT_NO_THROW(put_version_future.get()) << "failure to put version " << index;
//     std::cout << "version " << index << " has been put" << std::endl;
  }
  try {
    auto future(clients_.back()->GetVersions(chunk.name()));
    auto versions(future.get());
    EXPECT_EQ(versions.size(), max_branch);
//     for (auto& version : versions)
//       std::cout << "tip version : " << DebugId(version.id) << std::endl;
    EXPECT_EQ(versions.front().index, max_versions * 2 - 1);
    EXPECT_EQ(versions.front().id, chunks_.back().name());
  } catch (const maidsafe_error& error) {
    EXPECT_TRUE(false) << "Failed to retrieve version: " << boost::diagnostic_information(error);
  }

  try {
    StructuredDataVersions::VersionName v_tip(max_versions * 2 - 1, chunks_.back().name());
    auto future(clients_.back()->GetBranch(chunk.name(), v_tip));
    auto versions(future.get());
    EXPECT_EQ(versions.size(), max_versions);
    for (size_t index(0); index < versions.size(); ++index) {
//       std::cout << "version : " << DebugId(versions[index].id) << std::endl;
      EXPECT_EQ(versions[index].index, max_versions * 2 - index - 1);
      EXPECT_EQ(versions[index].id, chunks_[max_versions * 2 - index - 1].name());
    }
  } catch (const maidsafe_error& error) {
    EXPECT_TRUE(false) << "Failed to retrieve branch: " << boost::diagnostic_information(error);
  }
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
