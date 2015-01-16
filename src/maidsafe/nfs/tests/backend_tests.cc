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
#include <memory>

#include "asio/use_future.hpp"

#include "network_fixture.h"

#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"
#include "maidsafe/nfs/detail/container_key.h"

namespace maidsafe {
namespace nfs {
namespace detail {
namespace test {

namespace {

class BackendTest : public ::testing::Test, public NetworkFixture {
 protected:
  BackendTest()
    : ::testing::Test(),
      NetworkFixture() {
  }

  static ContainerVersion MakeContainerVersion(ContainerVersion::Index index) {
    return ContainerVersion{
      index, ImmutableData::Name{Identity{RandomString(64)}}};
  }

  static ImmutableData MakeChunk() {
    return ImmutableData{NonEmptyString{RandomString(100)}};
  }

  template<typename Result>
  static std::shared_ptr<boost::future<Result>> MakeFutureError(std::error_code error) {
    const auto return_val(std::make_shared<boost::future<Result>>());
    *return_val = boost::make_exceptional_future<Result>(std::system_error(error));
    return return_val;
  }
};
}  // namespace

TEST_F(BackendTest, BEH_CreateSDV) {
  const ContainerKey container_key{};
  const ContainerVersion container_version{MakeContainerVersion(0)};

  EXPECT_CALL(
      GetNetworkMock(),
      DoCreateSDV(container_key.GetId(), container_version, Network::GetMaxVersions(), 1))
    .Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetBranches(container_key.GetId())).Times(2);
  EXPECT_CALL(GetNetworkMock(), DoGetBranchVersions(container_key.GetId(), container_version))
    .Times(1);
  {
    const auto versions = network()->GetSDVVersions(container_key.GetId(), asio::use_future).get();
    ASSERT_FALSE(versions.valid());
    EXPECT_EQ(make_error_code(VaultErrors::no_such_account), versions.error());
  }
  auto sdv = network()->CreateSDV(container_key.GetId(), container_version, asio::use_future).get();
  EXPECT_TRUE(sdv.valid());

  const auto versions = network()->GetSDVVersions(container_key.GetId(), asio::use_future).get();
  ASSERT_TRUE(versions.valid());
  ASSERT_EQ(1u, versions->size());
  EXPECT_EQ(container_version, versions->front());
}

TEST_F(BackendTest, BEH_ExistingSDVFailure) {
  const ContainerKey container_key{};
  const ContainerVersion container_version{MakeContainerVersion(0)};

  EXPECT_CALL(
      GetNetworkMock(),
      DoCreateSDV(container_key.GetId(), container_version, Network::GetMaxVersions(), 1))
    .Times(2);
  EXPECT_CALL(GetNetworkMock(), DoGetBranches(container_key.GetId())).Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetBranchVersions(container_key.GetId(), container_version))
    .Times(1);

  auto sdv = network()->CreateSDV(container_key.GetId(), container_version, asio::use_future).get();
  EXPECT_TRUE(sdv.valid());

  sdv = network()->CreateSDV(container_key.GetId(), container_version, asio::use_future).get();
  ASSERT_FALSE(sdv.valid());
  EXPECT_EQ(make_error_code(VaultErrors::data_already_exists), sdv.error());

  const auto versions = network()->GetSDVVersions(container_key.GetId(), asio::use_future).get();
  ASSERT_TRUE(versions.valid());
  ASSERT_EQ(1u, versions->size());
  EXPECT_EQ(container_version, versions->front());
}

TEST_F(BackendTest, BEH_UpdateExistingSDV) {
  const ContainerKey container_key{};
  const ContainerVersion container_version1{MakeContainerVersion(0)};
  const ContainerVersion container_version2{MakeContainerVersion(1)};

  EXPECT_CALL(
      GetNetworkMock(),
      DoCreateSDV(container_key.GetId(), container_version1, Network::GetMaxVersions(), 1))
    .Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetBranches(container_key.GetId())).Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetBranchVersions(container_key.GetId(), container_version2))
    .Times(1);
  EXPECT_CALL(GetNetworkMock(), DoPutSDVVersion(container_key.GetId(), container_version1,
                                                container_version2)).Times(1);

  auto sdv = network()->CreateSDV(container_key.GetId(), container_version1, asio::use_future).get();
  EXPECT_TRUE(sdv.valid());

  sdv = network()->PutSDVVersion(
      container_key.GetId(), container_version1, container_version2, asio::use_future).get();
  EXPECT_TRUE(sdv.valid());

  const auto versions = network()->GetSDVVersions(container_key.GetId(), asio::use_future).get();
  ASSERT_TRUE(versions.valid());
  ASSERT_EQ(2u, versions->size());
  EXPECT_EQ(container_version2, (*versions)[0]);
  EXPECT_EQ(container_version1, (*versions)[1]);
}

TEST_F(BackendTest, BEH_PutNonExistingSDVFailure) {
  const ContainerKey container_key{};
  const ContainerVersion container_version1{MakeContainerVersion(0)};
  const ContainerVersion container_version2{MakeContainerVersion(1)};

  EXPECT_CALL(GetNetworkMock(), DoGetBranches(container_key.GetId())).Times(1);
  EXPECT_CALL(
      GetNetworkMock(),
      DoPutSDVVersion(container_key.GetId(), container_version1, container_version2))
    .Times(1);

  const auto sdv = network()->PutSDVVersion(
      container_key.GetId(), container_version1, container_version2, asio::use_future).get();
  ASSERT_FALSE(sdv.valid());
  EXPECT_EQ(make_error_code(VaultErrors::no_such_account), sdv.error());

  const auto versions = network()->GetSDVVersions(container_key.GetId(), asio::use_future).get();
  ASSERT_FALSE(versions.valid());
  EXPECT_EQ(make_error_code(VaultErrors::no_such_account), versions.error());
}

TEST_F(BackendTest, BEH_UpdateExistingSDVBranchFailure) {
  const ContainerKey container_key{};
  const ContainerVersion container_version1{MakeContainerVersion(0)};
  const ContainerVersion container_version2{MakeContainerVersion(1)};
  const ContainerVersion container_version3{MakeContainerVersion(2)};

  EXPECT_CALL(
      GetNetworkMock(),
      DoCreateSDV(container_key.GetId(), container_version1, Network::GetMaxVersions(), 1))
    .Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetBranches(container_key.GetId())).Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetBranchVersions(container_key.GetId(), container_version2))
    .Times(1);
  EXPECT_CALL(
      GetNetworkMock(),
      DoPutSDVVersion(container_key.GetId(), container_version1, container_version2))
    .Times(1);
  EXPECT_CALL(
      GetNetworkMock(),
      DoPutSDVVersion(container_key.GetId(), container_version1, container_version3))
    .Times(1);

  auto sdv = network()->CreateSDV(container_key.GetId(), container_version1, asio::use_future).get();
  EXPECT_TRUE(sdv.valid());

  sdv = network()->PutSDVVersion(
      container_key.GetId(), container_version1, container_version2, asio::use_future).get();
  EXPECT_TRUE(sdv.valid());

  sdv = network()->PutSDVVersion(
      container_key.GetId(), container_version1, container_version3, asio::use_future).get();
  ASSERT_FALSE(sdv.valid());
  EXPECT_EQ(make_error_code(CommonErrors::cannot_exceed_limit), sdv.error());

  const auto versions =
    network()->GetSDVVersions(container_key.GetId(), asio::use_future).get();
  ASSERT_TRUE(versions.valid());
  ASSERT_EQ(2u, versions->size());
  EXPECT_EQ(container_version2, (*versions)[0]);
  EXPECT_EQ(container_version1, (*versions)[1]);
}

// See MAID-658 for DISABLED_
TEST_F(BackendTest, DISABLED_BEH_UpdateExistingSDVBadRootFailure) {
  const ContainerKey container_key{};
  const ContainerVersion container_version1{MakeContainerVersion(0)};
  const ContainerVersion container_version2{MakeContainerVersion(1)};
  const ContainerVersion container_version3{MakeContainerVersion(2)};

  EXPECT_CALL(
      GetNetworkMock(),
      DoCreateSDV(container_key.GetId(), container_version1, Network::GetMaxVersions(), 1))
    .Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetBranches(container_key.GetId())).Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetBranchVersions(container_key.GetId(), container_version1))
    .Times(1);
  EXPECT_CALL(
      GetNetworkMock(),
      DoPutSDVVersion(container_key.GetId(), container_version3, container_version2))
    .Times(1);

  auto sdv = network()->CreateSDV(container_key.GetId(), container_version1, asio::use_future).get();
  EXPECT_TRUE(sdv.valid());

  sdv = network()->PutSDVVersion(
      container_key.GetId(), container_version3, container_version2, asio::use_future).get();
  ASSERT_FALSE(sdv.valid());
  EXPECT_EQ(make_error_code(CommonErrors::invalid_parameter), sdv.error());

  const auto versions = network()->GetSDVVersions(container_key.GetId(), asio::use_future).get();
  ASSERT_TRUE(versions.valid());
  ASSERT_EQ(1u, versions->size());
  EXPECT_EQ(container_version1, versions->front());
}

TEST_F(BackendTest, BEH_UpdateExistingSDVSameTip) {
  const ContainerKey container_key{};
  const ContainerVersion container_version{MakeContainerVersion(0)};

  EXPECT_CALL(
      GetNetworkMock(),
      DoCreateSDV(container_key.GetId(), container_version, Network::GetMaxVersions(), 1))
    .Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetBranches(container_key.GetId())).Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetBranchVersions(container_key.GetId(), container_version))
    .Times(1);
  EXPECT_CALL(
      GetNetworkMock(),
      DoPutSDVVersion(container_key.GetId(), container_version, container_version))
    .Times(1);

  auto sdv = network()->CreateSDV(container_key.GetId(), container_version, asio::use_future).get();
  EXPECT_TRUE(sdv.valid());

  sdv = network()->PutSDVVersion(
      container_key.GetId(), container_version, container_version, asio::use_future).get();
  ASSERT_FALSE(sdv.valid());
  EXPECT_EQ(make_error_code(CommonErrors::invalid_parameter), sdv.error());

  const auto versions = network()->GetSDVVersions(container_key.GetId(), asio::use_future).get();
  ASSERT_TRUE(versions.valid());
  ASSERT_EQ(1u, versions->size());
  EXPECT_EQ(container_version, versions->front());
}

TEST_F(BackendTest, BEH_TwoSDVs) {
  const ContainerKey container_key1{};
  const ContainerKey container_key2{};
  const ContainerVersion container_version1{MakeContainerVersion(0)};
  const ContainerVersion container_version2{MakeContainerVersion(1)};

  EXPECT_CALL(
      GetNetworkMock(),
      DoCreateSDV(container_key1.GetId(), container_version1, Network::GetMaxVersions(), 1))
    .Times(1);
  EXPECT_CALL(
      GetNetworkMock(),
      DoCreateSDV(container_key2.GetId(), container_version1, Network::GetMaxVersions(), 1))
    .Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetBranches(container_key1.GetId())).Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetBranches(container_key2.GetId())).Times(1);
  EXPECT_CALL(
      GetNetworkMock(), DoGetBranchVersions(container_key1.GetId(), container_version2))
    .Times(1);
  EXPECT_CALL(
      GetNetworkMock(), DoGetBranchVersions(container_key2.GetId(), container_version2))
    .Times(1);
  EXPECT_CALL(
      GetNetworkMock(),
      DoPutSDVVersion(container_key1.GetId(), container_version1, container_version2))
    .Times(1);
  EXPECT_CALL(
      GetNetworkMock(),
      DoPutSDVVersion(container_key2.GetId(), container_version1, container_version2))
    .Times(1);


  auto sdv1 = network()->CreateSDV(container_key1.GetId(), container_version1, asio::use_future);
  auto sdv2 = network()->CreateSDV(container_key2.GetId(), container_version1, asio::use_future);

  EXPECT_TRUE(sdv1.get().valid());
  EXPECT_TRUE(sdv2.get().valid());


  sdv1 = network()->PutSDVVersion(
      container_key1.GetId(), container_version1, container_version2, asio::use_future);
  sdv2 = network()->PutSDVVersion(
      container_key2.GetId(), container_version1, container_version2, asio::use_future);

  EXPECT_TRUE(sdv1.get().valid());
  EXPECT_TRUE(sdv2.get().valid());


  auto future_versions1 = network()->GetSDVVersions(container_key1.GetId(), asio::use_future);
  auto future_versions2 = network()->GetSDVVersions(container_key2.GetId(), asio::use_future);

  auto versions = future_versions1.get();
  ASSERT_TRUE(versions.valid());
  EXPECT_EQ(2u, versions->size());
  EXPECT_EQ(container_version2, (*versions)[0]);
  EXPECT_EQ(container_version1, (*versions)[1]);

  versions = future_versions2.get();
  ASSERT_TRUE(versions.valid());
  EXPECT_EQ(2u, versions->size());
  EXPECT_EQ(container_version2, (*versions)[0]);
  EXPECT_EQ(container_version1, (*versions)[1]);
}

// See MAID-657 for DISABLED_
TEST_F(BackendTest, DISABLED_BEH_GetChunkFailure) {
  const ImmutableData chunk_data{MakeChunk()};
  EXPECT_CALL(GetNetworkMock(), DoGetChunk(chunk_data.name())).Times(1);

  auto get_chunk = network()->GetChunk(chunk_data.name(), asio::use_future).get();
  ASSERT_FALSE(get_chunk.valid());
  EXPECT_EQ(make_error_code(CommonErrors::no_such_element), get_chunk.error());
}

TEST_F(BackendTest, BEH_PutChunk) {
  using ::testing::_;

  const ImmutableData chunk_data{MakeChunk()};

  EXPECT_CALL(GetNetworkMock(), DoPutChunk(_)).Times(1);
  EXPECT_CALL(GetNetworkMock(), DoGetChunk(chunk_data.name())).Times(1);

  const auto put_chunk = network()->PutChunk(chunk_data, asio::use_future).get();
  EXPECT_TRUE(put_chunk.valid());

  auto get_chunk = network()->GetChunk(chunk_data.name(), asio::use_future).get();
  ASSERT_TRUE(get_chunk.valid());
  EXPECT_EQ(chunk_data.name(), get_chunk->name());
  EXPECT_EQ(chunk_data.data(), get_chunk->data());
}

TEST_F(BackendTest, BEH_PutChunkTwice) {
  using ::testing::_;

  const ImmutableData chunk_data{MakeChunk()};

  EXPECT_CALL(GetNetworkMock(), DoPutChunk(_)).Times(2);
  EXPECT_CALL(GetNetworkMock(), DoGetChunk(chunk_data.name())).Times(1);

  auto put_chunk1 = network()->PutChunk(chunk_data, asio::use_future);
  auto put_chunk2 = network()->PutChunk(chunk_data, asio::use_future);

  EXPECT_TRUE(put_chunk1.get().valid());
  EXPECT_TRUE(put_chunk2.get().valid());

  const auto get_chunk = network()->GetChunk(chunk_data.name(), asio::use_future).get();
  ASSERT_TRUE(get_chunk.valid());
  EXPECT_EQ(chunk_data.name(), get_chunk->name());
  EXPECT_EQ(chunk_data.data(), get_chunk->data());
}

TEST_F(BackendTest, BEH_InterfaceThrow) {
  using ::testing::_;
  using ::testing::Throw;

  // Make sure every interface function correctly returns errors
  const ContainerKey container_key{};
  const ContainerVersion container_version{MakeContainerVersion(0)};
  const ImmutableData chunk_data{MakeChunk()};

  const auto test_error = make_error_code(AsymmErrors::invalid_private_key);

  EXPECT_CALL(
      GetNetworkMock(),
      DoCreateSDV(container_key.GetId(), container_version, Network::GetMaxVersions(), 1))
    .Times(1).WillOnce(Throw(test_error));
  EXPECT_CALL(GetNetworkMock(), DoGetBranches(container_key.GetId()))
    .Times(1).WillOnce(Throw(test_error));
  EXPECT_CALL(
      GetNetworkMock(),
      DoPutSDVVersion(container_key.GetId(), container_version, container_version))
    .Times(1).WillOnce(Throw(test_error));
  EXPECT_CALL(GetNetworkMock(), DoPutChunk(_)).Times(1).WillOnce(Throw(test_error));
  EXPECT_CALL(GetNetworkMock(), DoGetChunk(chunk_data.name())).Times(1).WillOnce(Throw(test_error));

  EXPECT_THROW(
      network()->CreateSDV(container_key.GetId(), container_version, asio::use_future),
      std::error_code);
  EXPECT_THROW(
      network()->PutSDVVersion(
          container_key.GetId(), container_version, container_version, asio::use_future),
      std::error_code);
  EXPECT_THROW(network()->GetSDVVersions(container_key.GetId(), asio::use_future), std::error_code);
  EXPECT_THROW(network()->PutChunk(chunk_data, asio::use_future), std::error_code);
  EXPECT_THROW(network()->GetChunk(chunk_data.name(), asio::use_future), std::error_code);
}

TEST_F(BackendTest, BEH_InterfaceErrors) {
  using ::testing::_;
  using ::testing::Return;

  // Make sure every interface function correctly returns errors
  const ContainerKey container_key{};
  const ContainerVersion container_version{MakeContainerVersion(0)};
  const ImmutableData chunk_data{MakeChunk()};

  const auto test_error = make_error_code(AsymmErrors::invalid_private_key);

  const auto valid_result(std::make_shared<boost::future<std::vector<ContainerVersion>>>());
  *valid_result = boost::make_ready_future(std::vector<ContainerVersion>{container_version});

  EXPECT_CALL(
      GetNetworkMock(),
      DoCreateSDV(container_key.GetId(), container_version, Network::GetMaxVersions(), 1))
    .Times(1).WillOnce(Return(MakeFutureError<void>(test_error)));
  EXPECT_CALL(GetNetworkMock(), DoGetBranches(container_key.GetId()))
    .Times(2)
    .WillOnce(Return(MakeFutureError<std::vector<ContainerVersion>>(test_error)))
    .WillOnce(Return(valid_result));
  EXPECT_CALL(GetNetworkMock(), DoGetBranchVersions(container_key.GetId(), container_version))
    .Times(1).WillOnce(Return(MakeFutureError<std::vector<ContainerVersion>>(test_error)));
  EXPECT_CALL(
      GetNetworkMock(),
      DoPutSDVVersion(container_key.GetId(), container_version, container_version))
    .Times(1).WillOnce(Return(MakeFutureError<void>((test_error))));
  EXPECT_CALL(GetNetworkMock(), DoPutChunk(_))
    .Times(1).WillOnce(Return(MakeFutureError<void>(test_error)));
  EXPECT_CALL(GetNetworkMock(), DoGetChunk(chunk_data.name()))
    .Times(1).WillOnce(Return(MakeFutureError<ImmutableData>(test_error)));

  auto void_return =
    network()->CreateSDV(container_key.GetId(), container_version, asio::use_future).get();
  ASSERT_FALSE(void_return.valid());
  EXPECT_EQ(test_error, void_return.error());

  void_return = network()->PutSDVVersion(
      container_key.GetId(), container_version, container_version, asio::use_future).get();
  ASSERT_FALSE(void_return.valid());
  EXPECT_EQ(test_error, void_return.error());

  // error on first operation
  auto versions = network()->GetSDVVersions(container_key.GetId(), asio::use_future).get();
  ASSERT_FALSE(versions.valid());
  EXPECT_EQ(test_error, versions.error());

  // error on second operation
  versions = network()->GetSDVVersions(container_key.GetId(), asio::use_future).get();
  ASSERT_FALSE(versions.valid());
  EXPECT_EQ(test_error, versions.error());

  void_return = network()->PutChunk(chunk_data, asio::use_future).get();
  ASSERT_FALSE(void_return.valid());
  EXPECT_EQ(test_error, versions.error());

  auto chunk = network()->GetChunk(chunk_data.name(), asio::use_future).get();
  ASSERT_FALSE(chunk.valid());
  EXPECT_EQ(test_error, chunk.error());
}

}  // namespace test
}  // namespace detail
}  // namespace nfs
}  // namespace maidsafe
