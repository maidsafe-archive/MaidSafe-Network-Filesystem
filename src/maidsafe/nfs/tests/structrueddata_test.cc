/***************************************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use.  The use of this code is governed by the licence file licence.txt found in the root of    *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit         *
 *  written permission of the board of directors of MaidSafe.net.                                  *
 **************************************************************************************************/

#include "maidsafe/nfs/structured_data.h"

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
      RandomUint32(), ImmutableData::name_type(Identity(RandomString(64))));
  versions.push_back(version_name);

  StructuredData structured_data_ori(versions);
  StructuredData structured_data_copy(structured_data_ori);

  uint32_t num_of_versions(RandomUint32() % 256 + 10);
  for(uint32_t i(0); i < num_of_versions; ++i) {
    StructuredDataVersions::VersionName version_name(
        RandomUint32(), ImmutableData::name_type(Identity(RandomString(64))));
    versions.push_back(version_name);
  }

  StructuredData structured_data(versions);
  auto versions_fetched(structured_data.versions());
  EXPECT_EQ(num_of_versions + 1, versions_fetched.size());
  for(uint32_t i(0); i < (num_of_versions + 1); ++i) {
    EXPECT_EQ(versions[i].id, versions_fetched[i].id);
    EXPECT_EQ(versions[i].index, versions_fetched[i].index);
  }
}

TEST_F(StructuredDataTest, BEH_Serialise) {
  {
    bool expect_exception(false);
    try {
      StructuredData structured_data(StructuredData::serialised_type(NonEmptyString(RandomString(64))));
    } catch (...) {
      expect_exception = true;
    }
    EXPECT_TRUE(expect_exception);
  }

  std::vector<StructuredDataVersions::VersionName> versions;
  uint32_t num_of_versions(RandomUint32() % 256 + 10);
  for(uint32_t i(0); i < num_of_versions; ++i) {
    StructuredDataVersions::VersionName version_name(
        RandomUint32(), ImmutableData::name_type(Identity(RandomString(64))));
    versions.push_back(version_name);
  }

  StructuredData structured_data_ori(versions);
  StructuredData structured_data_serialise(structured_data_ori.Serialise());
  auto versions_parsed(structured_data_serialise.versions());
  EXPECT_EQ(num_of_versions, versions_parsed.size());
  for(uint32_t i(0); i < num_of_versions; ++i) {
    EXPECT_EQ(versions[i].id, versions_parsed[i].id);
    EXPECT_EQ(versions[i].index, versions_parsed[i].index);
  }
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
