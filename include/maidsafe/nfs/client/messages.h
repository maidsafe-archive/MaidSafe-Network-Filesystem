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

#ifndef MAIDSAFE_NFS_CLIENT_MESSAGES_H_
#define MAIDSAFE_NFS_CLIENT_MESSAGES_H_

#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

#include "boost/optional.hpp"

#include "maidsafe/common/config.h"
#include "maidsafe/common/error.h"
#include "maidsafe/common/types.h"
#include "maidsafe/data_types/data_type_values.h"

#include "maidsafe/nfs/client/structured_data.h"
#include "maidsafe/nfs/vault/messages.h"
#include "maidsafe/nfs/utils.h"


namespace maidsafe {

namespace nfs_client {

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

void swap(ReturnCode& lhs, ReturnCode& rhs) MAIDSAFE_NOEXCEPT;



struct DataNameAndReturnCode {
  DataNameAndReturnCode();
  DataNameAndReturnCode(const DataNameAndReturnCode& other);
  DataNameAndReturnCode(DataNameAndReturnCode&& other);
  DataNameAndReturnCode& operator=(DataNameAndReturnCode other);

  explicit DataNameAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  nfs_vault::DataName name;
  ReturnCode return_code;
};

void swap(DataNameAndReturnCode& lhs, DataNameAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;



struct DataNameVersionAndReturnCode {
  DataNameVersionAndReturnCode();
  DataNameVersionAndReturnCode(const DataNameVersionAndReturnCode& other);
  DataNameVersionAndReturnCode(DataNameVersionAndReturnCode&& other);
  DataNameVersionAndReturnCode& operator=(DataNameVersionAndReturnCode other);

  explicit DataNameVersionAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  nfs_vault::DataNameAndVersion data_name_and_version;
  ReturnCode return_code;
};

void swap(DataNameVersionAndReturnCode& lhs, DataNameVersionAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;



struct DataNameOldNewVersionAndReturnCode {
  DataNameOldNewVersionAndReturnCode();
  DataNameOldNewVersionAndReturnCode(const DataNameOldNewVersionAndReturnCode& other);
  DataNameOldNewVersionAndReturnCode(DataNameOldNewVersionAndReturnCode&& other);
  DataNameOldNewVersionAndReturnCode& operator=(DataNameOldNewVersionAndReturnCode other);

  explicit DataNameOldNewVersionAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  nfs_vault::DataNameOldNewVersion data_name_old_new_version;
  ReturnCode return_code;
};

void swap(DataNameOldNewVersionAndReturnCode& lhs,
          DataNameOldNewVersionAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;



struct DataAndReturnCode {
  DataAndReturnCode();
  DataAndReturnCode(const DataAndReturnCode& other);
  DataAndReturnCode(DataAndReturnCode&& other);
  DataAndReturnCode& operator=(DataAndReturnCode other);

  explicit DataAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  nfs_vault::DataNameAndContent data;
  ReturnCode return_code;
};

void swap(DataAndReturnCode& lhs, DataAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;



struct DataOrDataNameAndReturnCode {
  template<typename Data>
  explicit DataOrDataNameAndReturnCode(const Data& data_in)
      : data(nfs_vault::DataNameAndContent(data_in)),
        data_name_and_return_code() {}

  DataOrDataNameAndReturnCode();
  DataOrDataNameAndReturnCode(const DataOrDataNameAndReturnCode& other);
  DataOrDataNameAndReturnCode(DataOrDataNameAndReturnCode&& other);
  DataOrDataNameAndReturnCode& operator=(DataOrDataNameAndReturnCode other);

  explicit DataOrDataNameAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  boost::optional<nfs_vault::DataNameAndContent> data;
  boost::optional<DataNameAndReturnCode> data_name_and_return_code;
};

template<>
DataOrDataNameAndReturnCode::DataOrDataNameAndReturnCode(
    const DataNameAndReturnCode& data_name_and_return_code_in);

void swap(DataOrDataNameAndReturnCode& lhs, DataOrDataNameAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;



struct StructuredDataOrDataNameAndReturnCode {
  StructuredDataOrDataNameAndReturnCode();
  StructuredDataOrDataNameAndReturnCode(const StructuredDataOrDataNameAndReturnCode& other);
  StructuredDataOrDataNameAndReturnCode(StructuredDataOrDataNameAndReturnCode&& other);
  StructuredDataOrDataNameAndReturnCode& operator=(StructuredDataOrDataNameAndReturnCode other);

  explicit StructuredDataOrDataNameAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  boost::optional<StructuredData> structured_data;
  boost::optional<DataNameAndReturnCode> data_name_and_return_code;
};

void swap(StructuredDataOrDataNameAndReturnCode& lhs,
          StructuredDataOrDataNameAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;



struct DataPmidHintAndReturnCode {
  DataPmidHintAndReturnCode();
  DataPmidHintAndReturnCode(const DataPmidHintAndReturnCode& other);
  DataPmidHintAndReturnCode(DataPmidHintAndReturnCode&& other);
  DataPmidHintAndReturnCode& operator=(DataPmidHintAndReturnCode other);

  explicit DataPmidHintAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  nfs_vault::DataAndPmidHint data_and_pmid_hint;
  ReturnCode return_code;
};

void swap(DataPmidHintAndReturnCode& lhs, DataPmidHintAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;



struct PmidRegistrationAndReturnCode {
  PmidRegistrationAndReturnCode();
  PmidRegistrationAndReturnCode(const PmidRegistrationAndReturnCode& other);
  PmidRegistrationAndReturnCode(PmidRegistrationAndReturnCode&& other);
  PmidRegistrationAndReturnCode& operator=(PmidRegistrationAndReturnCode other);

  explicit PmidRegistrationAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  nfs_vault::PmidRegistration pmid_registration;
  ReturnCode return_code;
};

void swap(PmidRegistrationAndReturnCode& lhs, PmidRegistrationAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;

}  // namespace nfs_client



namespace nfs {

template<>
bool IsSuccess<nfs_client::DataOrDataNameAndReturnCode>(
    const nfs_client::DataOrDataNameAndReturnCode& response);

template<>
std::error_code ErrorCode<nfs_client::DataOrDataNameAndReturnCode>(
    const nfs_client::DataOrDataNameAndReturnCode& response);

template<>
bool IsSuccess<nfs_client::StructuredDataOrDataNameAndReturnCode>(
    const nfs_client::StructuredDataOrDataNameAndReturnCode& response);

template<>
std::error_code ErrorCode<nfs_client::StructuredDataOrDataNameAndReturnCode>(
    const nfs_client::StructuredDataOrDataNameAndReturnCode& response);

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MESSAGES_H_
