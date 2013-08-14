/* Copyright 2013 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

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



struct DataName {
  template<typename DataNameType>
  explicit DataName(const DataNameType& data_name)
      : type(DataNameType::data_type::Tag::kValue),
        raw_name(data_name.value) {}

  DataName(DataTagValue type_in, const Identity& raw_name_in);

  DataName();
  DataName(const DataName& other);
  DataName(DataName&& other);
  DataName& operator=(DataName other);

  explicit DataName(const std::string& serialised_copy);
  std::string Serialise() const;

  DataTagValue type;
  Identity raw_name;
};

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

void swap(DataNameOldNewVersion& lhs, DataNameOldNewVersion& rhs) MAIDSAFE_NOEXCEPT;



struct DataNameAndContent {
  template<typename Data>
  explicit DataNameAndContent(const Data& data)
      : name(data.name()),
        content(data.Serialise().data) {}

  DataNameAndContent(DataTagValue type_in,
                     const Identity& name_in,
                     const NonEmptyString& content_in);

  DataNameAndContent();
  DataNameAndContent(const DataNameAndContent& other);
  DataNameAndContent(DataNameAndContent&& other);
  DataNameAndContent& operator=(DataNameAndContent other);

  explicit DataNameAndContent(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName name;
  NonEmptyString content;
};

void swap(DataNameAndContent& lhs, DataNameAndContent& rhs) MAIDSAFE_NOEXCEPT;



struct DataAndPmidHint {
  DataAndPmidHint();
  DataAndPmidHint(const DataAndPmidHint& other);
  DataAndPmidHint(DataAndPmidHint&& other);
  DataAndPmidHint& operator=(DataAndPmidHint other);

  explicit DataAndPmidHint(const std::string& serialised_copy);
  std::string Serialise() const;

  DataNameAndContent data;
  Identity pmid_hint;
};

void swap(DataAndPmidHint& lhs, DataAndPmidHint& rhs) MAIDSAFE_NOEXCEPT;

}  // namespace nfs_vault

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_VAULT_MESSAGES_H_
