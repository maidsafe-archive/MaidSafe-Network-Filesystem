/*  Copyright 2013 MaidSafe.net limited

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

#ifndef MAIDSAFE_NFS_VAULT_MESSAGES_H_
#define MAIDSAFE_NFS_VAULT_MESSAGES_H_

#include <string>

#include "maidsafe/common/config.h"
#include "maidsafe/common/types.h"
#include "maidsafe/data_types/data_type_values.h"
#include "maidsafe/data_types/structured_data_versions.h"

#include "maidsafe/nfs/vault/pmid_registration.h"


namespace maidsafe {

namespace nfs_vault {

struct Empty {
  Empty() {}
  explicit Empty(const std::string&) {}
  std::string Serialise() const { return ""; }
};

bool operator==(const Empty& /*lhs*/, const Empty& /*rhs*/);


struct DataName {
  template<typename DataNameType>
  explicit DataName(const DataNameType& data_name)
      : type(DataNameType::data_type::Tag::kValue),
        raw_name(data_name.value) {}

  DataName(DataTagValue type_in, Identity raw_name_in);

  DataName();
  DataName(const DataName& other);
  DataName(DataName&& other);
  DataName& operator=(DataName other);

  explicit DataName(const std::string& serialised_copy);
  std::string Serialise() const;

  DataTagValue type;
  Identity raw_name;
};

bool operator==(const DataName& lhs, const DataName& rhs);
void swap(DataName& lhs, DataName& rhs) MAIDSAFE_NOEXCEPT;


struct DataNameAndVersion {
  DataNameAndVersion();
  DataNameAndVersion(const DataNameAndVersion& other);
  DataNameAndVersion(DataNameAndVersion&& other);
  DataNameAndVersion& operator=(DataNameAndVersion other);

  explicit DataNameAndVersion(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName data_name;
  StructuredDataVersions::VersionName version_name;
};

bool operator==(const DataNameAndVersion& lhs, const DataNameAndVersion& rhs);
void swap(DataNameAndVersion& lhs, DataNameAndVersion& rhs) MAIDSAFE_NOEXCEPT;


struct DataNameOldNewVersion {
  DataNameOldNewVersion();
  DataNameOldNewVersion(const DataNameOldNewVersion& other);
  DataNameOldNewVersion(DataNameOldNewVersion&& other);
  DataNameOldNewVersion& operator=(DataNameOldNewVersion other);

  explicit DataNameOldNewVersion(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName data_name;
  StructuredDataVersions::VersionName old_version_name, new_version_name;
};

bool operator==(const DataNameOldNewVersion& lhs, const DataNameOldNewVersion& rhs);
void swap(DataNameOldNewVersion& lhs, DataNameOldNewVersion& rhs) MAIDSAFE_NOEXCEPT;


struct DataNameAndContent {
  template<typename Data>
  explicit DataNameAndContent(const Data& data)
      : name(data.name()),
        content(data.Serialise().data) {}

  DataNameAndContent(DataTagValue type_in, const Identity& name_in,
                     NonEmptyString content_in);

  DataNameAndContent();
  DataNameAndContent(const DataNameAndContent& other);
  DataNameAndContent(DataNameAndContent&& other);
  DataNameAndContent& operator=(DataNameAndContent other);

  explicit DataNameAndContent(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName name;
  NonEmptyString content;
};

bool operator==(const DataNameAndContent& lhs, const DataNameAndContent& rhs);
void swap(DataNameAndContent& lhs, DataNameAndContent& rhs) MAIDSAFE_NOEXCEPT;


struct DataNameAndCost {
  template<typename Data>
  explicit DataNameAndCost(const Data& data)
      : name(data.name()),
        cost(data.cost) {}

  DataNameAndCost(DataTagValue type_in,
                  const Identity& name_in,
                  const int32_t& cost_in);

  DataNameAndCost();
  DataNameAndCost(const DataNameAndCost& other);
  DataNameAndCost(DataNameAndCost&& other);
  DataNameAndCost& operator=(DataNameAndCost other);

  explicit DataNameAndCost(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName name;
  int32_t cost;
};

bool operator==(const DataNameAndCost& lhs, const DataNameAndCost& rhs);
void swap(DataNameAndCost& lhs, DataNameAndCost& rhs) MAIDSAFE_NOEXCEPT;



struct DataAndPmidHint {
  DataAndPmidHint();
  DataAndPmidHint(const DataName& data_name, const NonEmptyString& content,
                  Identity pmid_node_hint);
  DataAndPmidHint(const DataAndPmidHint& other);
  DataAndPmidHint(DataAndPmidHint&& other);
  DataAndPmidHint& operator=(DataAndPmidHint other);

  explicit DataAndPmidHint(const std::string& serialised_copy);
  std::string Serialise() const;

  DataNameAndContent data;
  Identity pmid_hint;
};

bool operator==(const DataAndPmidHint& lhs, const DataAndPmidHint& rhs);
void swap(DataAndPmidHint& lhs, DataAndPmidHint& rhs) MAIDSAFE_NOEXCEPT;

}  // namespace nfs_vault

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_VAULT_MESSAGES_H_
