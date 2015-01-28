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

#include <set>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>

#include "boost/optional.hpp"
#include "boost/expected/expected.hpp"

#include "maidsafe/common/config.h"
#include "maidsafe/common/error.h"
#include "maidsafe/common/types.h"
#include "maidsafe/common/data_types/data_type_values.h"

#include "maidsafe/nfs/client/structured_data.h"
#include "maidsafe/nfs/vault/messages.h"
#include "maidsafe/nfs/utils.h"

namespace maidsafe {

namespace nfs_client {

// ==================== ReturnCode =================================================================
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

  ReturnCode(const maidsafe_error& error = MakeError(CommonErrors::defaulted));
  ReturnCode(const ReturnCode& other);
  ReturnCode(ReturnCode&& other);
  ReturnCode& operator=(ReturnCode other);

  explicit ReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  maidsafe_error value;
};

bool operator==(const ReturnCode& lhs, const ReturnCode& rhs);
void swap(ReturnCode& lhs, ReturnCode& rhs) MAIDSAFE_NOEXCEPT;

// ==================== AvailableSizeAndReturnCode =================================================
struct AvailableSizeAndReturnCode {
  AvailableSizeAndReturnCode();
  AvailableSizeAndReturnCode(uint64_t size, const ReturnCode& return_code);
  AvailableSizeAndReturnCode(const AvailableSizeAndReturnCode& other);
  AvailableSizeAndReturnCode(AvailableSizeAndReturnCode&& other);
  AvailableSizeAndReturnCode& operator=(AvailableSizeAndReturnCode other);

  explicit AvailableSizeAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  nfs_vault::AvailableSize available_size;
  ReturnCode return_code;
};

bool operator==(const AvailableSizeAndReturnCode& lhs, const AvailableSizeAndReturnCode& rhs);
void swap(AvailableSizeAndReturnCode& lhs, AvailableSizeAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;

// ==================== DataNameAndReturnCode ======================================================
struct DataNameAndReturnCode {
  template<typename DataNameType>
  DataNameAndReturnCode(const DataNameType& data_name, const ReturnCode& return_code_in)
      : name(data_name), return_code(return_code_in) {}
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

// ============================ DataNameAndSizeAndReturnCode ======================================
struct DataNameAndSizeAndReturnCode {
  template<typename DataNameType>
  DataNameAndSizeAndReturnCode(const DataNameType& data_name, uint64_t size_in,
                            const ReturnCode& return_code_in)
      : name(data_name), size(size_in), return_code(return_code_in) {}
  DataNameAndSizeAndReturnCode();
  DataNameAndSizeAndReturnCode(nfs_vault::DataName data_name, uint64_t size_in, ReturnCode code);
  DataNameAndSizeAndReturnCode(const DataNameAndSizeAndReturnCode& other);
  DataNameAndSizeAndReturnCode(DataNameAndSizeAndReturnCode&& other);
  DataNameAndSizeAndReturnCode& operator=(DataNameAndSizeAndReturnCode other);

  explicit DataNameAndSizeAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  nfs_vault::DataName name;
  uint64_t size;
  ReturnCode return_code;
};

bool operator==(const DataNameAndSizeAndReturnCode& lhs, const DataNameAndSizeAndReturnCode& rhs);
void swap(DataNameAndSizeAndReturnCode& lhs, DataNameAndSizeAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;

// ==================== DataNamesAndReturnCode =====================================================
struct DataNamesAndReturnCode {
  explicit DataNamesAndReturnCode(const ReturnCode& code);
  DataNamesAndReturnCode(const std::vector<nfs_vault::DataName>& data_names,
                         const ReturnCode& code);
  DataNamesAndReturnCode(const DataNamesAndReturnCode &other);
  DataNamesAndReturnCode(DataNamesAndReturnCode&& other);
  DataNamesAndReturnCode& operator=(DataNamesAndReturnCode other);

  template<typename DataNameType>
  void AddDataName(const DataNameType& data_name) {
    names.insert(nfs_vault::DataName(data_name));
  }

  void AddDataName(const DataTagValue& tag_value, const Identity& identity);

  explicit DataNamesAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  std::set<nfs_vault::DataName> names;
  ReturnCode return_code;
};

bool operator==(const DataNamesAndReturnCode& lhs, const DataNamesAndReturnCode& rhs);
void swap(DataNamesAndReturnCode& lhs, DataNamesAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;

// ==================== DataNameVersionAndReturnCode ===============================================
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

// ==================== DataNameOldNewVersionAndReturnCode =========================================
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

// ==================== DataAndReturnCode ==========================================================
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

// ==================== DataNameAndContentOrReturnCode =============================================
struct DataNameAndContentOrReturnCode {
  template <typename Data>
  explicit DataNameAndContentOrReturnCode(const Data& data_in)
      : name(nfs_vault::DataName(data_in.name())), content(data_in.Serialise().data.string()),
        return_code() {}

  template <typename DataNameType>
  DataNameAndContentOrReturnCode(const DataNameType& data_name, const ReturnCode& return_code_in)
      : name(nfs_vault::DataName(data_name)), content(), return_code(return_code_in) {}

  DataNameAndContentOrReturnCode();
  DataNameAndContentOrReturnCode(const DataNameAndContentOrReturnCode& other);
  DataNameAndContentOrReturnCode(DataNameAndContentOrReturnCode&& other);
  DataNameAndContentOrReturnCode& operator=(DataNameAndContentOrReturnCode other);

  explicit DataNameAndContentOrReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  nfs_vault::DataName name;
  boost::optional<nfs_vault::Content> content;
  boost::optional<ReturnCode> return_code;
};

template <>
DataNameAndContentOrReturnCode::DataNameAndContentOrReturnCode(
    const DataNameAndReturnCode& data_name_and_return_code_in);

bool operator==(const DataNameAndContentOrReturnCode& lhs,
                const DataNameAndContentOrReturnCode& rhs);
void swap(DataNameAndContentOrReturnCode& lhs,
          DataNameAndContentOrReturnCode& rhs) MAIDSAFE_NOEXCEPT;

// ==================== StructuredDataNameAndContentOrReturnCode ===================================
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

// =============================== TipOfTreeAndReturnCode =======================================
struct TipOfTreeAndReturnCode {
  TipOfTreeAndReturnCode();
  explicit TipOfTreeAndReturnCode(const ReturnCode return_code_in);
  TipOfTreeAndReturnCode(const TipOfTreeAndReturnCode& other);
  TipOfTreeAndReturnCode(TipOfTreeAndReturnCode&& other);
  TipOfTreeAndReturnCode& operator=(TipOfTreeAndReturnCode other);

  explicit TipOfTreeAndReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  boost::optional<StructuredDataVersions::VersionName> tip_of_tree;
  ReturnCode return_code;
};

bool operator==(const TipOfTreeAndReturnCode& lhs, const TipOfTreeAndReturnCode& rhs);
void swap(TipOfTreeAndReturnCode& lhs, TipOfTreeAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;

// ==================== PmidRegistrationAndReturnCode ==============================================
struct PmidRegistrationAndReturnCode {
  PmidRegistrationAndReturnCode();
  PmidRegistrationAndReturnCode(nfs_vault::PmidRegistration pmid_health,
                                ReturnCode return_code_in);
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

// ==================== DataNameAndSizeAndSpaceAndReturnCode ======================================
struct DataNameAndSizeAndSpaceAndReturnCode {
  template <typename DataNameType>
  DataNameAndSizeAndSpaceAndReturnCode(const DataNameType& name_in, uint64_t size_in,
                                int64_t available_space_in,
                                const nfs_client::ReturnCode& code_in)
      : name(name_in), size(size_in), available_space(available_space_in), return_code(code_in) {}

  DataNameAndSizeAndSpaceAndReturnCode(const DataTagValue& type_in, const Identity& name_in,
                                       uint64_t size_in, int64_t available_space_in,
                                       const nfs_client::ReturnCode& code_in);
  explicit DataNameAndSizeAndSpaceAndReturnCode(const std::string& serialised_copy);
  DataNameAndSizeAndSpaceAndReturnCode();
  DataNameAndSizeAndSpaceAndReturnCode(const DataNameAndSizeAndSpaceAndReturnCode& other);
  DataNameAndSizeAndSpaceAndReturnCode(DataNameAndSizeAndSpaceAndReturnCode&& other);
  DataNameAndSizeAndSpaceAndReturnCode& operator=(DataNameAndSizeAndSpaceAndReturnCode other);
  std::string Serialise() const;

  nfs_vault::DataName name;
  uint64_t size;
  int64_t available_space;
  nfs_client::ReturnCode return_code;
};

void swap(DataNameAndSizeAndSpaceAndReturnCode& lhs,
          DataNameAndSizeAndSpaceAndReturnCode& rhs) MAIDSAFE_NOEXCEPT;
bool operator==(const DataNameAndSizeAndSpaceAndReturnCode& lhs,
                const DataNameAndSizeAndSpaceAndReturnCode& rhs);

// ================================= MpidMessageOrReturnCode =======================================

struct MpidMessageOrReturnCode {
  MpidMessageOrReturnCode();
  MpidMessageOrReturnCode(
      const boost::expected<nfs_vault::MpidMessage, maidsafe_error>& expected);
  explicit MpidMessageOrReturnCode(const std::string& serialised_copy);
  MpidMessageOrReturnCode(const MpidMessageOrReturnCode& other);
  MpidMessageOrReturnCode(MpidMessageOrReturnCode&& other);
  MpidMessageOrReturnCode& operator=(MpidMessageOrReturnCode other);

  std::string Serialise() const;
  nfs_vault::MpidMessage mpid_message;
  ReturnCode return_code;
};

bool operator==(const MpidMessageOrReturnCode& lhs, const MpidMessageOrReturnCode& rhs);
void swap(MpidMessageOrReturnCode& lhs, MpidMessageOrReturnCode& rhs) MAIDSAFE_NOEXCEPT;

}  // namespace nfs_client

namespace nfs {

template <>
bool IsSuccess<nfs_client::ReturnCode>(const nfs_client::ReturnCode& response);

template <>
std::error_code ErrorCode<nfs_client::ReturnCode>(const nfs_client::ReturnCode& response);

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
