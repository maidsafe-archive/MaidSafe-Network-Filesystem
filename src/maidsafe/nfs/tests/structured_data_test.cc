/*  Copyright 2012 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.novinet.com/license

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#include "maidsafe/nfs/client/structured_data.h"

#include <memory>
#include <vector>

#include "maidsafe/common/log.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

namespace maidsafe {

namespace nfs {

namespace test {

class StructuredDataTest : public testing::Test {
 protected:
  StructuredDataTest() {}
};

TEST_F(StructuredDataTest, BEH_Constructor) {
  std::vector<StructuredDataVersions::VersionName> versions;
  StructuredDataVersions::VersionName version_name(
      RandomUint32(), ImmutableData::Name(Identity(RandomString(64))));
  versions.push_back(version_name);

  nfs_client::StructuredData structured_data_ori(versions);
  nfs_client::StructuredData structured_data_copy(structured_data_ori);

  uint32_t num_of_versions(RandomUint32() % 256 + 10);
  for (uint32_t i(0); i < num_of_versions; ++i) {
    StructuredDataVersions::VersionName version_name(
        RandomUint32(), ImmutableData::Name(Identity(RandomString(64))));
    versions.push_back(version_name);
  }

  nfs_client::StructuredData structured_data(versions);
  auto versions_fetched(structured_data.versions);
  EXPECT_EQ(num_of_versions + 1, versions_fetched.size());
  for (uint32_t i(0); i < (num_of_versions + 1); ++i) {
    EXPECT_EQ(versions[i].id, versions_fetched[i].id);
    EXPECT_EQ(versions[i].index, versions_fetched[i].index);
  }
}

TEST_F(StructuredDataTest, BEH_Serialise) {
  {
    bool expect_exception(false);
    try {
      nfs_client::StructuredData structured_data(RandomString(64));
    }
    catch(...) {
      expect_exception = true;
    }
    EXPECT_TRUE(expect_exception);
  }

  std::vector<StructuredDataVersions::VersionName> versions;
  uint32_t num_of_versions(RandomUint32() % 256 + 10);
  for (uint32_t i(0); i < num_of_versions; ++i) {
    StructuredDataVersions::VersionName version_name(
        RandomUint32(), ImmutableData::Name(Identity(RandomString(64))));
    versions.push_back(version_name);
  }

  nfs_client::StructuredData structured_data_ori(versions);
  nfs_client::StructuredData structured_data_serialise(structured_data_ori.Serialise());
  auto versions_parsed(structured_data_serialise.versions);
  EXPECT_EQ(num_of_versions, versions_parsed.size());
  for (uint32_t i(0); i < num_of_versions; ++i) {
    EXPECT_EQ(versions[i].id, versions_parsed[i].id);
    EXPECT_EQ(versions[i].index, versions_parsed[i].index);
  }
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
