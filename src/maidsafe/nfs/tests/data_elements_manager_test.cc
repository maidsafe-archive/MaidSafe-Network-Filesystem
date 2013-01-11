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

#include "maidsafe/nfs/data_elements_manager.h"

#include <vector>

#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/containers_pb.h"

namespace maidsafe {

namespace nfs {

namespace test {

// to match kVaultDirectory in data_elements_manager.cc
const boost::filesystem::path kVaultDirectory("meta_data_manager");

Identity GenerateIdentity() {
  return Identity(RandomAlphaNumericString(64));
}

class DataElementsManagerTest : public testing::Test {
 public:
  DataElementsManagerTest()
    : kTestRoot_(maidsafe::test::CreateTestPath("MaidSafe_Test_DataElementsManager")),
      vault_root_dir_(*kTestRoot_ / RandomAlphaNumericString(8)),
      vault_metadata_dir_(vault_root_dir_ / kVaultDirectory),
      data_elements_manager_(vault_root_dir_) {}

 private:
  bool CheckDataExistenceAndParsing(const Identity& data_id,
                                    protobuf::DataElementsManaged& element) {
    if (!boost::filesystem::exists(vault_metadata_dir_ / EncodeToBase64(data_id))) {
      LOG(kError) << "Data was not found.";
      return false;
    }
    NonEmptyString serialised_element(ReadFile(vault_metadata_dir_ / EncodeToBase64(data_id)));
    if (!element.ParseFromString(serialised_element.string())) {
      LOG(kError) << "Data did not parse.";
      return false;
    }
    return true;
  }

  bool CheckPmidIds(protobuf::DataElementsManaged& element,
                    const std::set<Identity>& online_pmids,
                    const std::set<Identity>& offline_pmids) {
    std::set<Identity> element_online_pmid_ids;
    for (uint16_t i(0); i < element.online_pmid_id_size(); ++i) {
      try {
        Identity online_pmid_id(element.online_pmid_id(i));
        element_online_pmid_ids.insert(online_pmid_id);
      } catch(const std::exception& ex) {
        LOG(kError) << "Data has online pmid of wrong size.";
        return false;
      }
    }

    if (online_pmids != element_online_pmid_ids) {
      LOG(kError) << "Data's online pmid IDs don't match expectation.";
      LOG(kVerbose) << "\n\nonline - expected:";
      for (auto pmid : online_pmids)
        LOG(kVerbose) << pmid.string();
      LOG(kVerbose) << "\nonline - actual:";
      for (auto pmid : element_online_pmid_ids)
        LOG(kVerbose) << pmid.string();
      return false;
    }

    std::set<Identity> element_offline_pmid_ids;
    for (uint16_t i(0); i < element.offline_pmid_id_size(); ++i) {
      try {
        Identity offline_pmid_id(element.offline_pmid_id(i));
        element_offline_pmid_ids.insert(offline_pmid_id);
      } catch(const std::exception& ex) {
        LOG(kError) << "Data has offline pmid of wrong size.";
        return false;
      }
    }
    if (offline_pmids != element_offline_pmid_ids) {
      LOG(kError) << "Data's offline pmid IDs don't match expectation.";
      LOG(kVerbose) << "\n\noffline - expected:";
      for (auto pmid : offline_pmids)
        LOG(kVerbose) << pmid.string();
      LOG(kVerbose) << "\noffline - actual:";
      for (auto pmid : element_offline_pmid_ids)
        LOG(kVerbose) << pmid.string();
      return false;
    }

    return true;
  }

  bool CheckPmidIds(protobuf::DataElementsManaged& element,
                    const Identity& online_pmid,
                    const Identity& offline_pmid) {
    if (element.online_pmid_id_size() != 1) {
      LOG(kError) << "Expected one online pmid, found " << element.online_pmid_id_size();
      return false;
    }

    if (element.offline_pmid_id_size() != 1) {
      LOG(kError) << "Expected one offline pmid, found " << element.offline_pmid_id_size();
      return false;
    }

    if (element.online_pmid_id(0) != online_pmid.string()) {
      LOG(kError) << "Data's online pmid is wrong.";
      return false;
    }

    if (element.offline_pmid_id(0) != offline_pmid.string()) {
      LOG(kError) << "Data's offline pmid is wrong.";
      return false;
    }

    return true;
  }

 public:
  bool CheckDataExistenceAndIntegrity(const Identity& data_id,
                                      const int32_t& element_size,
                                      const std::set<Identity>& online_pmids,
                                      const std::set<Identity>& offline_pmids) {
    protobuf::DataElementsManaged element;
    if (!CheckDataExistenceAndParsing(data_id, element))
      return false;

    if (element.element_size() != element_size) {
      LOG(kError) << "Data has wrong size.";
      return false;
    }

    if (online_pmids.empty() && (element.online_pmid_id_size() != 0)) {
      LOG(kError) << "Data has too many online pmid IDs.";
      return false;
    }
    if (offline_pmids.empty() && (element.offline_pmid_id_size() != 0)) {
      LOG(kError) << "Data has too many offline pmid IDs.";
      return false;
    }

    if (!CheckPmidIds(element, online_pmids, offline_pmids))
      return false;

    return true;
  }

  bool CheckDataExistenceAndIntegrity(const Identity &data_id,
                                      const int32_t &element_size,
                                      const Identity& online_pmid,
                                      const Identity& offline_pmid) {
    protobuf::DataElementsManaged element;
    if (!CheckDataExistenceAndParsing(data_id, element))
      return false;

    if (element.element_size() != element_size) {
      LOG(kError) << "Data has wrong size.";
      return false;
    }

    if (!CheckPmidIds(element, online_pmid, offline_pmid))
      return false;

    return true;
  }

  const maidsafe::test::TestPath kTestRoot_;
  boost::filesystem::path vault_root_dir_;
  boost::filesystem::path vault_metadata_dir_;
  DataElementsManager data_elements_manager_;
};

class DataElementsManagerOneElementTest : public DataElementsManagerTest {
 public:
  DataElementsManagerOneElementTest()
    : data_id_(GenerateIdentity()),
      element_size_(RandomInt32()),
      online_pmid_id_(GenerateIdentity()),
      offline_pmid_id_(GenerateIdentity()) {}

 protected:
  void SetUp() {
    online_pmid_ids_.insert(online_pmid_id_);
    offline_pmid_ids_.insert(offline_pmid_id_);
  }

 public:
  Identity data_id_;
  int32_t element_size_;
  Identity online_pmid_id_;
  Identity offline_pmid_id_;
  std::set<Identity> online_pmid_ids_;
  std::set<Identity> offline_pmid_ids_;
};

TEST_F(DataElementsManagerOneElementTest, BEH_AddDataElement) {
  // Add and check data element, then re-add it and check again
  for (uint16_t i(0); i < 2; ++i) {
    data_elements_manager_.AddDataElement(data_id_,
                                          element_size_,
                                          online_pmid_id_,
                                          offline_pmid_id_);

    EXPECT_TRUE(CheckDataExistenceAndIntegrity(data_id_,
                                               element_size_,
                                               online_pmid_ids_,
                                               offline_pmid_ids_));
  }
}

TEST_F(DataElementsManagerOneElementTest, BEH_AddChangedDataElement) {
  // Add and check data element, then add another data element with the same id, but with other
  // fields different, and check that original data element's content hasn't changed.
  for (uint16_t i(0); i < 2; ++i) {
    if (i == 0) {
      data_elements_manager_.AddDataElement(data_id_,
                                            element_size_,
                                            online_pmid_id_,
                                            offline_pmid_id_);
    } else {
      data_elements_manager_.AddDataElement(data_id_,
                                            RandomInt32(),
                                            GenerateIdentity(),
                                            GenerateIdentity());
    }

    EXPECT_TRUE(CheckDataExistenceAndIntegrity(data_id_,
                                               element_size_,
                                               online_pmid_ids_,
                                               offline_pmid_ids_));
  }
}

TEST_F(DataElementsManagerOneElementTest, BEH_AddAndRemoveDataElement) {
  data_elements_manager_.AddDataElement(data_id_, element_size_, online_pmid_id_, offline_pmid_id_);
  EXPECT_TRUE(CheckDataExistenceAndIntegrity(data_id_,
                                             element_size_,
                                             online_pmid_ids_,
                                             offline_pmid_ids_));

  data_elements_manager_.RemoveDataElement(data_id_);
  EXPECT_FALSE(boost::filesystem::exists(vault_metadata_dir_ / EncodeToBase64(data_id_)));

  data_elements_manager_.AddDataElement(data_id_, element_size_, online_pmid_id_, offline_pmid_id_);
  EXPECT_TRUE(CheckDataExistenceAndIntegrity(data_id_,
                                             element_size_,
                                             online_pmid_ids_,
                                             offline_pmid_ids_));
}

TEST_F(DataElementsManagerTest, BEH_RemoveNonexistentDataElement) {
  Identity data_id(GenerateIdentity());

  EXPECT_FALSE(boost::filesystem::exists(vault_metadata_dir_ / EncodeToBase64(data_id)));

  // TODO(Alison) - sensitise to NfsErrors::failed_to_find_managed_element?
  EXPECT_THROW(data_elements_manager_.RemoveDataElement(data_id),
               std::exception);

  EXPECT_FALSE(boost::filesystem::exists(vault_metadata_dir_ / EncodeToBase64(data_id)));
}

TEST_F(DataElementsManagerOneElementTest, BEH_AddRemovePmids) {
  // TODO(Alison) - tidy/condense this test
  data_elements_manager_.AddDataElement(data_id_, element_size_, online_pmid_id_, offline_pmid_id_);

  ASSERT_TRUE(CheckDataExistenceAndIntegrity(data_id_,
                                             element_size_,
                                             online_pmid_ids_,
                                             offline_pmid_ids_));

  // Add, and randomly select some for removal later
  std::vector<Identity> online_for_removal;
  std::vector<Identity> offline_for_removal;
  for (uint16_t i(0); i < 20; ++i) {  // TODO(Alison) - max value?
    Identity online_pmid_id(GenerateIdentity());
    data_elements_manager_.AddOnlinePmid(data_id_, online_pmid_id);
    online_pmid_ids_.insert(online_pmid_id);
    ASSERT_TRUE(CheckDataExistenceAndIntegrity(data_id_,
                                               element_size_,
                                               online_pmid_ids_,
                                               offline_pmid_ids_));
    if (RandomUint32() % 3 == 0)
      online_for_removal.push_back(online_pmid_id);

    Identity offline_pmid_id(GenerateIdentity());
    data_elements_manager_.AddOfflinePmid(data_id_, offline_pmid_id);
    offline_pmid_ids_.insert(offline_pmid_id);
    ASSERT_TRUE(CheckDataExistenceAndIntegrity(data_id_,
                                               element_size_,
                                               online_pmid_ids_,
                                               offline_pmid_ids_));
    if (RandomUint32() % 3 == 0)
      offline_for_removal.push_back(offline_pmid_id);
  }

  // Remove some (try to randomise orders a bit)
  while (!online_for_removal.empty() && !offline_for_removal.empty()) {
    if (!online_for_removal.empty() && (RandomUint32() % 4 != 0)) {
      uint16_t index(RandomUint32() % online_for_removal.size());
      Identity to_remove(online_for_removal.at(index));
      data_elements_manager_.RemoveOnlinePmid(data_id_, to_remove);
      online_pmid_ids_.erase(to_remove);
      online_for_removal.erase(online_for_removal.begin() + index);
      ASSERT_TRUE(CheckDataExistenceAndIntegrity(data_id_,
                                                 element_size_,
                                                 online_pmid_ids_,
                                                 offline_pmid_ids_));
    }

    if (!offline_for_removal.empty() && (RandomUint32() % 4 != 0)) {
      uint16_t index(RandomUint32() % offline_for_removal.size());
      Identity to_remove(offline_for_removal.at(index));
      data_elements_manager_.RemoveOfflinePmid(data_id_, to_remove);
      offline_pmid_ids_.erase(to_remove);
      offline_for_removal.erase(offline_for_removal.begin() + index);
      ASSERT_TRUE(CheckDataExistenceAndIntegrity(data_id_,
                                                 element_size_,
                                                 online_pmid_ids_,
                                                 offline_pmid_ids_));
    }
  }


  // Add some more
  for (uint16_t i(0); i < 20; ++i) {  // TODO(Alison) - max value?
    Identity online_pmid_id(GenerateIdentity());
    data_elements_manager_.AddOnlinePmid(data_id_, online_pmid_id);
    online_pmid_ids_.insert(online_pmid_id);
    ASSERT_TRUE(CheckDataExistenceAndIntegrity(data_id_,
                                               element_size_,
                                               online_pmid_ids_,
                                               offline_pmid_ids_));

    Identity offline_pmid_id(GenerateIdentity());
    data_elements_manager_.AddOfflinePmid(data_id_, offline_pmid_id);
    offline_pmid_ids_.insert(offline_pmid_id);
    ASSERT_TRUE(CheckDataExistenceAndIntegrity(data_id_,
                                               element_size_,
                                               online_pmid_ids_,
                                               offline_pmid_ids_));
  }
}

TEST_F(DataElementsManagerOneElementTest, BEH_MovePmids) {
  // TODO(Alison)
  // Move Pmids between online and offline. Check that sets are correct and that the number of
  // 'holders' is updated correctly
}

TEST_F(DataElementsManagerTest, BEH_ManyDataElements) {
  // Add and remove many data elements in various combinations
  std::vector<Identity> data_ids;
  std::vector<int32_t> element_sizes;
  std::vector<Identity> online_pmid_ids;
  std::vector<Identity> offline_pmid_ids;

  // Add elements
  uint16_t max(1000);
  for (uint16_t i(0); i < max; ++i) {
    data_ids.push_back(GenerateIdentity());
    element_sizes.push_back(RandomInt32());
    online_pmid_ids.push_back(GenerateIdentity());
    offline_pmid_ids.push_back(GenerateIdentity());
    data_elements_manager_.AddDataElement(data_ids.back(),
                                          element_sizes.back(),
                                          online_pmid_ids.back(),
                                          offline_pmid_ids.back());
  }

  // Check existence
  for (uint16_t i(0); i < max; ++i) {
    ASSERT_TRUE(CheckDataExistenceAndIntegrity(data_ids.at(i),
                                               element_sizes.at(i),
                                               online_pmid_ids.at(i),
                                               offline_pmid_ids.at(i)));
  }

  // Remove elements
  for (uint16_t i(0); i < max; i += 2) {
    data_elements_manager_.RemoveDataElement(data_ids.at(i));
  }

  // Check non-existence and existence
  for (uint16_t i(0); i < max; i += 2)
    EXPECT_FALSE(boost::filesystem::exists(vault_metadata_dir_ / EncodeToBase64(data_ids.at(i))));

  for (uint16_t i(1); i < max; i += 2) {
    ASSERT_TRUE(CheckDataExistenceAndIntegrity(data_ids.at(i),
                                               element_sizes.at(i),
                                               online_pmid_ids.at(i),
                                               offline_pmid_ids.at(i)));
  }

  // Re-add removed elements
  for (uint16_t i(0); i < max; i += 2) {
    data_elements_manager_.AddDataElement(data_ids.at(i),
                                          element_sizes.at(i),
                                          online_pmid_ids.at(i),
                                          offline_pmid_ids.at(i));
  }

  // Check existence
  for (uint16_t i(0); i < max; ++i) {
    ASSERT_TRUE(CheckDataExistenceAndIntegrity(data_ids.at(i),
                                               element_sizes.at(i),
                                               online_pmid_ids.at(i),
                                               offline_pmid_ids.at(i)));
  }
}

// TODO(Alison) - test Pmid operations on multiple data elements (cf. 'BEH_ManyDataElements' test)

// TODO(Alison) - add threaded tests

// TODO(Alison) - test that badly formatted protobufs are handled correctly (especially with
// strings of length != 64)

// TODO(Alison) - test private functions?

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
