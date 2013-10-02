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
  template <typename ErrorCode>
  explicit ReturnCode(ErrorCode error_code,
                      typename std::enable_if<std::is_error_code_enum<ErrorCode>::value>::type* = 0)
      : value(MakeError(error_code)) {}

  template <typename Error>
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

bool operator==(const ReturnCode& lhs, const ReturnCode& rhs);
void swap(ReturnCode& lhs, ReturnCode& rhs) MAIDSAFE_NOEXCEPT;

struct DataNameAndReturnCode {
  DataNameAndReturnCode();
  DataNameAndReturnCode(nfs_vault::DataName data_name, ReturnCode code);
  DataNameAndReturnCode(const DataNameAndReturnCode& other);
  DataNameAndReturnCode(DataNameAndReturnCode&& other);
  DataNameAndReturnCode& operator=(DataNameAndReturnCode other);

  explicit DataNameAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  nfs_vault::DataName name;
  ReturnCode return_code;
};

bool operator==(const DataNameAndReturnCode& lhs, const DataNameAndReturnCode& rhs);
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

bool operator==(const DataNameVersionAndReturnCode& lhs, const DataNameVersionAndReturnCode& rhs);
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

bool operator==(const DataNameOldNewVersionAndReturnCode& lhs,
                const DataNameOldNewVersionAndReturnCode& rhs);
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

bool operator==(const DataAndReturnCode& lhs, const DataAndReturnCode& rhs);
void swap(DataAndReturnCode& lhs, DataAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;

struct DataNameAndContentOrReturnCode {
  template <typename Data>
  explicit DataNameAndContentOrReturnCode(const Data& data_in)
      : data(nfs_vault::DataNameAndContent(data_in)), data_name_and_return_code() {}

  DataNameAndContentOrReturnCode();
  DataNameAndContentOrReturnCode(const DataNameAndContentOrReturnCode& other);
  DataNameAndContentOrReturnCode(DataNameAndContentOrReturnCode&& other);
  DataNameAndContentOrReturnCode& operator=(DataNameAndContentOrReturnCode other);

  explicit DataNameAndContentOrReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  boost::optional<nfs_vault::DataNameAndContent> data;
  boost::optional<DataNameAndReturnCode> data_name_and_return_code;
};

template <>
DataNameAndContentOrReturnCode::DataNameAndContentOrReturnCode(
    const DataNameAndReturnCode& data_name_and_return_code_in);

bool operator==(const DataNameAndContentOrReturnCode& lhs,
                const DataNameAndContentOrReturnCode& rhs);
void swap(DataNameAndContentOrReturnCode& lhs,
          DataNameAndContentOrReturnCode& rhs) MAIDSAFE_NOEXCEPT;

struct StructuredDataNameAndContentOrReturnCode {
  StructuredDataNameAndContentOrReturnCode();
  StructuredDataNameAndContentOrReturnCode(const StructuredDataNameAndContentOrReturnCode& other);
  StructuredDataNameAndContentOrReturnCode(StructuredDataNameAndContentOrReturnCode&& other);
  StructuredDataNameAndContentOrReturnCode& operator=(
      StructuredDataNameAndContentOrReturnCode other);

  explicit StructuredDataNameAndContentOrReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  boost::optional<StructuredData> structured_data;
  boost::optional<DataNameAndReturnCode> data_name_and_return_code;
};

bool operator==(const StructuredDataNameAndContentOrReturnCode& lhs,
                const StructuredDataNameAndContentOrReturnCode& rhs);
void swap(StructuredDataNameAndContentOrReturnCode& lhs,
          StructuredDataNameAndContentOrReturnCode& rhs) MAIDSAFE_NOEXCEPT;

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

bool operator==(const DataPmidHintAndReturnCode& lhs, const DataPmidHintAndReturnCode& rhs);
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

bool operator==(const PmidRegistrationAndReturnCode& lhs, const PmidRegistrationAndReturnCode& rhs);
void swap(PmidRegistrationAndReturnCode& lhs, PmidRegistrationAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;

// ========================== DataNameAndContentAndReturnCode ====================================

struct DataNameAndContentAndReturnCode {
  DataNameAndContentAndReturnCode(const DataTagValue& type_in, const Identity& name_in,
                                  nfs_client::ReturnCode code_in, const NonEmptyString& content_in);
  DataNameAndContentAndReturnCode(const DataTagValue& type_in, const Identity& name_in,
                                  nfs_client::ReturnCode code_in);
  DataNameAndContentAndReturnCode();
  DataNameAndContentAndReturnCode(const DataNameAndContentAndReturnCode& other);
  DataNameAndContentAndReturnCode(DataNameAndContentAndReturnCode&& other);
  DataNameAndContentAndReturnCode& operator=(DataNameAndContentAndReturnCode other);

  explicit DataNameAndContentAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  nfs_vault::DataName name;
  nfs_client::ReturnCode return_code;
  boost::optional<NonEmptyString> content;
};

void swap(DataNameAndContentAndReturnCode& lhs,
          DataNameAndContentAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;
bool operator==(const DataNameAndContentAndReturnCode& lhs,
                const DataNameAndContentAndReturnCode& rhs);

// ========================== DataNameAndSpaceAndReturnCode ====================================

struct DataNameAndSpaceAndReturnCode {
  template <typename DataNameType>
  DataNameAndSpaceAndReturnCode(const DataNameType& name_in, const int64_t& available_space_in,
                                const nfs_client::ReturnCode& code_in)
      : name(name_in), available_space(available_space_in), return_code(code_in) {}

  DataNameAndSpaceAndReturnCode(const DataTagValue& type_in, const Identity& name_in,
                                const int64_t& available_space_in,
                                const nfs_client::ReturnCode& code_in);
  explicit DataNameAndSpaceAndReturnCode(const std::string& serialised_copy);
  DataNameAndSpaceAndReturnCode();
  DataNameAndSpaceAndReturnCode(const DataNameAndSpaceAndReturnCode& other);
  DataNameAndSpaceAndReturnCode(DataNameAndSpaceAndReturnCode&& other);
  DataNameAndSpaceAndReturnCode& operator=(DataNameAndSpaceAndReturnCode other);
  std::string Serialise() const;

  nfs_vault::DataName name;
  int64_t available_space;
  nfs_client::ReturnCode return_code;
};

void swap(DataNameAndSpaceAndReturnCode& lhs, DataNameAndSpaceAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;
bool operator==(const DataNameAndSpaceAndReturnCode& lhs, const DataNameAndSpaceAndReturnCode& rhs);

}  // namespace nfs_client

namespace nfs {

template <>
bool IsSuccess<nfs_client::DataNameAndContentOrReturnCode>(
    const nfs_client::DataNameAndContentOrReturnCode& response);

template <>
std::error_code ErrorCode<nfs_client::DataNameAndContentOrReturnCode>(
    const nfs_client::DataNameAndContentOrReturnCode& response);

template <>
bool IsSuccess<nfs_client::StructuredDataNameAndContentOrReturnCode>(
    const nfs_client::StructuredDataNameAndContentOrReturnCode& response);

template <>
std::error_code ErrorCode<nfs_client::StructuredDataNameAndContentOrReturnCode>(
    const nfs_client::StructuredDataNameAndContentOrReturnCode& response);

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MESSAGES_H_
