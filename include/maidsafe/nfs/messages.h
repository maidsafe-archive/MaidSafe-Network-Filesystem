/* Copyright 2012 MaidSafe.net limited

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

#ifndef MAIDSAFE_NFS_MESSAGES_H_
#define MAIDSAFE_NFS_MESSAGES_H_

#include <string>

#include "maidsafe/common/error.h"
#include "maidsafe/common/types.h"
#include "maidsafe/data_types/data_type_values.h"
#include "maidsafe/data_types/structured_data_versions.h"

#include "maidsafe/nfs/pmid_registration.h"


namespace maidsafe {

namespace nfs {

struct Empty {
  Empty() {}
  explicit Empty(const std::string&) {}
  std::string Serialise() const { return ""; }
};



struct DataName {
  template<typename DataNameType>
  explicit DataName(const DataNameType& data_name)
      : type(DataNameType::tag_type::kEnumValue),
        raw_name(data_name.data) {}

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

void swap(DataName& lhs, DataName& rhs);



struct ReturnCode {
  // This c'tor designed to be used with maidsafe-specific error enums (e.g. CommonErrors::success)
  template<typename ErrorCode>
  explicit ReturnCode(ErrorCode error_code,
                      typename std::enable_if<std::is_error_code_enum<ErrorCode>::value>::type* = 0)
      : value(MakeError(error_code)) {}

  template<typename Error>
  explicit ReturnCode(Error error,
                      typename std::enable_if<!std::is_error_code_enum<Error>::value>::type* = 0)
      : value(error) {}

  ReturnCode();
  ReturnCode(const ReturnCode& other);
  ReturnCode(ReturnCode&& other);
  ReturnCode& operator=(ReturnCode other);

  explicit ReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  maidsafe_error value;
};

void swap(ReturnCode& lhs, ReturnCode& rhs);



struct DataNameAndReturnCode {
  DataNameAndReturnCode();
  DataNameAndReturnCode(const DataNameAndReturnCode& other);
  DataNameAndReturnCode(DataNameAndReturnCode&& other);
  DataNameAndReturnCode& operator=(DataNameAndReturnCode other);

  explicit DataNameAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName name;
  ReturnCode return_code;
};

void swap(DataNameAndReturnCode& lhs, DataNameAndReturnCode& rhs);



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

void swap(DataNameAndVersion& lhs, DataNameAndVersion& rhs);



struct DataNameVersionAndReturnCode {
  DataNameVersionAndReturnCode();
  DataNameVersionAndReturnCode(const DataNameVersionAndReturnCode& other);
  DataNameVersionAndReturnCode(DataNameVersionAndReturnCode&& other);
  DataNameVersionAndReturnCode& operator=(DataNameVersionAndReturnCode other);

  explicit DataNameVersionAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  DataNameAndVersion data_name_and_version;
  ReturnCode return_code;
};

void swap(DataNameVersionAndReturnCode& lhs, DataNameVersionAndReturnCode& rhs);



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

void swap(DataNameOldNewVersion& lhs, DataNameOldNewVersion& rhs);



struct DataNameOldNewVersionAndReturnCode {
  DataNameOldNewVersionAndReturnCode();
  DataNameOldNewVersionAndReturnCode(const DataNameOldNewVersionAndReturnCode& other);
  DataNameOldNewVersionAndReturnCode(DataNameOldNewVersionAndReturnCode&& other);
  DataNameOldNewVersionAndReturnCode& operator=(DataNameOldNewVersionAndReturnCode other);

  explicit DataNameOldNewVersionAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  DataNameOldNewVersion data_name_old_new_version;
  ReturnCode return_code;
};

void swap(DataNameOldNewVersionAndReturnCode& lhs, DataNameOldNewVersionAndReturnCode& rhs);



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

void swap(DataNameAndContent& lhs, DataNameAndContent& rhs);



struct DataAndReturnCode {
  DataAndReturnCode();
  DataAndReturnCode(const DataAndReturnCode& other);
  DataAndReturnCode(DataAndReturnCode&& other);
  DataAndReturnCode& operator=(DataAndReturnCode other);

  explicit DataAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  DataNameAndContent data;
  ReturnCode return_code;
};

void swap(DataAndReturnCode& lhs, DataAndReturnCode& rhs);



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

void swap(DataAndPmidHint& lhs, DataAndPmidHint& rhs);



struct DataPmidHintAndReturnCode {
  DataPmidHintAndReturnCode();
  DataPmidHintAndReturnCode(const DataPmidHintAndReturnCode& other);
  DataPmidHintAndReturnCode(DataPmidHintAndReturnCode&& other);
  DataPmidHintAndReturnCode& operator=(DataPmidHintAndReturnCode other);

  explicit DataPmidHintAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  DataAndPmidHint data_and_pmid_hint;
  ReturnCode return_code;
};

void swap(DataPmidHintAndReturnCode& lhs, DataPmidHintAndReturnCode& rhs);



struct PmidRegistrationAndReturnCode {
  PmidRegistrationAndReturnCode();
  PmidRegistrationAndReturnCode(const PmidRegistrationAndReturnCode& other);
  PmidRegistrationAndReturnCode(PmidRegistrationAndReturnCode&& other);
  PmidRegistrationAndReturnCode& operator=(PmidRegistrationAndReturnCode other);

  explicit PmidRegistrationAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  PmidRegistration pmid_registration;
  ReturnCode return_code;
};

void swap(PmidRegistrationAndReturnCode& lhs, PmidRegistrationAndReturnCode& rhs);




typedef DataName MaidNodeGetRequest, MaidNodeGetVersionRequest, MaidNodeDeleteRequest;
typedef DataAndPmidHint MaidNodePutRequest;

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGES_H_
