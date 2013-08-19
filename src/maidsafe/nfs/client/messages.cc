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

#include "maidsafe/nfs/client/messages.h"

#include <cstdint>

#include "maidsafe/nfs/utils.h"
#include "maidsafe/nfs/client/messages.pb.h"


namespace maidsafe {

namespace nfs_client {

namespace {

maidsafe_error GetError(int error_value, const std::string& error_category_name) {
  if (error_category_name == std::string(GetCommonCategory().name()))
    return MakeError(static_cast<CommonErrors>(error_value));
  if (error_category_name == std::string(GetAsymmCategory().name()))
    return MakeError(static_cast<AsymmErrors>(error_value));
  if (error_category_name == std::string(GetPassportCategory().name()))
    return MakeError(static_cast<PassportErrors>(error_value));
  if (error_category_name == std::string(GetNfsCategory().name()))
    return MakeError(static_cast<NfsErrors>(error_value));
  if (error_category_name == std::string(GetRoutingCategory().name()))
    return MakeError(static_cast<RoutingErrors>(error_value));
  if (error_category_name == std::string(GetVaultCategory().name()))
    return MakeError(static_cast<VaultErrors>(error_value));
  if (error_category_name == std::string(GetLifeStuffCategory().name()))
    return MakeError(static_cast<LifeStuffErrors>(error_value));

  ThrowError(CommonErrors::parsing_error);
  return MakeError(CommonErrors::parsing_error);
}

}  // unnamed namespace



// ==================== ReturnCode =================================================================
ReturnCode::ReturnCode() : value(CommonErrors::success) {}

ReturnCode::ReturnCode(const ReturnCode& other) : value(other.value) {}

ReturnCode::ReturnCode(ReturnCode&& other) : value(std::move(other.value)) {}

ReturnCode& ReturnCode::operator=(ReturnCode other) {
  swap(*this, other);
  return *this;
}

ReturnCode::ReturnCode(const std::string& serialised_copy)
    : value([&serialised_copy] {
          protobuf::ReturnCode proto_copy;
          if (!proto_copy.ParseFromString(serialised_copy))
            ThrowError(CommonErrors::parsing_error);
          return GetError(proto_copy.error_value(), proto_copy.error_category_name());
        }()) {}

std::string ReturnCode::Serialise() const {
  protobuf::ReturnCode proto_copy;
  proto_copy.set_error_value(value.code().value());
  proto_copy.set_error_category_name(value.code().category().name());
  return proto_copy.SerializeAsString();
}

void swap(ReturnCode& lhs, ReturnCode& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.value, rhs.value);
}



// ==================== DataNameAndReturnCode ======================================================
DataNameAndReturnCode::DataNameAndReturnCode() : name(), return_code() {}

DataNameAndReturnCode::DataNameAndReturnCode(const DataNameAndReturnCode& other)
    : name(other.name),
      return_code(other.return_code) {}

DataNameAndReturnCode::DataNameAndReturnCode(DataNameAndReturnCode&& other)
    : name(std::move(other.name)),
      return_code(std::move(other.return_code)) {}

DataNameAndReturnCode& DataNameAndReturnCode::operator=(DataNameAndReturnCode other) {
  swap(*this, other);
  return *this;
}

DataNameAndReturnCode::DataNameAndReturnCode(const std::string& serialised_copy)
    : name(),
      return_code() {
  protobuf::DataNameAndReturnCode proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  name = nfs_vault::DataName(proto_copy.serialised_name());
  return_code = ReturnCode(proto_copy.serialised_return_code());
}

std::string DataNameAndReturnCode::Serialise() const {
  protobuf::DataNameAndReturnCode proto_copy;
  proto_copy.set_serialised_name(name.Serialise());
  proto_copy.set_serialised_return_code(return_code.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataNameAndReturnCode& lhs, DataNameAndReturnCode& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.name, rhs.name);
  swap(lhs.return_code, rhs.return_code);
}



// ==================== DataNameVersionAndReturnCode ===============================================
DataNameVersionAndReturnCode::DataNameVersionAndReturnCode()
    : data_name_and_version(),
      return_code() {}

DataNameVersionAndReturnCode::DataNameVersionAndReturnCode(
    const DataNameVersionAndReturnCode& other)
        : data_name_and_version(other.data_name_and_version),
          return_code(other.return_code) {}

DataNameVersionAndReturnCode::DataNameVersionAndReturnCode(DataNameVersionAndReturnCode&& other)
    : data_name_and_version(std::move(other.data_name_and_version)),
      return_code(std::move(other.return_code)) {}

DataNameVersionAndReturnCode& DataNameVersionAndReturnCode::operator=(
    DataNameVersionAndReturnCode other) {
  swap(*this, other);
  return *this;
}

DataNameVersionAndReturnCode::DataNameVersionAndReturnCode(const std::string& serialised_copy)
    : data_name_and_version(),
      return_code() {
  protobuf::DataNameVersionAndReturnCode proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  data_name_and_version =
      nfs_vault::DataNameAndVersion(proto_copy.serialised_data_name_and_version());
  return_code = ReturnCode(proto_copy.serialised_return_code());
}

std::string DataNameVersionAndReturnCode::Serialise() const {
  protobuf::DataNameVersionAndReturnCode proto_copy;
  proto_copy.set_serialised_data_name_and_version(data_name_and_version.Serialise());
  proto_copy.set_serialised_return_code(return_code.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataNameVersionAndReturnCode& lhs, DataNameVersionAndReturnCode& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data_name_and_version, rhs.data_name_and_version);
  swap(lhs.return_code, rhs.return_code);
}



// ==================== DataNameOldNewVersionAndReturnCode =========================================
DataNameOldNewVersionAndReturnCode::DataNameOldNewVersionAndReturnCode()
    : data_name_old_new_version(),
      return_code() {}

DataNameOldNewVersionAndReturnCode::DataNameOldNewVersionAndReturnCode(
    const DataNameOldNewVersionAndReturnCode& other)
        : data_name_old_new_version(other.data_name_old_new_version),
          return_code(other.return_code) {}

DataNameOldNewVersionAndReturnCode::DataNameOldNewVersionAndReturnCode(
    DataNameOldNewVersionAndReturnCode&& other)
        : data_name_old_new_version(std::move(other.data_name_old_new_version)),
          return_code(std::move(other.return_code)) {}

DataNameOldNewVersionAndReturnCode& DataNameOldNewVersionAndReturnCode::operator=(
    DataNameOldNewVersionAndReturnCode other) {
  swap(*this, other);
  return *this;
}

DataNameOldNewVersionAndReturnCode::DataNameOldNewVersionAndReturnCode(
    const std::string& serialised_copy)
        : data_name_old_new_version(),
          return_code() {
  protobuf::DataNameOldNewVersionAndReturnCode proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  data_name_old_new_version =
      nfs_vault::DataNameOldNewVersion(proto_copy.serialised_data_name_old_new_version());
  return_code = ReturnCode(proto_copy.serialised_return_code());
}

std::string DataNameOldNewVersionAndReturnCode::Serialise() const {
  protobuf::DataNameOldNewVersionAndReturnCode proto_copy;
  proto_copy.set_serialised_data_name_old_new_version(data_name_old_new_version.Serialise());
  proto_copy.set_serialised_return_code(return_code.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataNameOldNewVersionAndReturnCode& lhs,
          DataNameOldNewVersionAndReturnCode& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data_name_old_new_version, rhs.data_name_old_new_version);
  swap(lhs.return_code, rhs.return_code);
}



// ==================== DataAndReturnCode ==========================================================
DataAndReturnCode::DataAndReturnCode() : data(), return_code() {}

DataAndReturnCode::DataAndReturnCode(const DataAndReturnCode& other)
    : data(other.data),
      return_code(other.return_code) {}

DataAndReturnCode::DataAndReturnCode(DataAndReturnCode&& other)
    : data(std::move(other.data)),
      return_code(std::move(other.return_code)) {}

DataAndReturnCode& DataAndReturnCode::operator=(DataAndReturnCode other) {
  swap(*this, other);
  return *this;
}

DataAndReturnCode::DataAndReturnCode(const std::string& serialised_copy)
    : data(),
      return_code() {
  protobuf::DataAndReturnCode proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  data = nfs_vault::DataNameAndContent(proto_copy.serialised_data_name_and_content());
  return_code = ReturnCode(proto_copy.serialised_return_code());
}

std::string DataAndReturnCode::Serialise() const {
  protobuf::DataAndReturnCode proto_copy;
  proto_copy.set_serialised_data_name_and_content(data.Serialise());
  proto_copy.set_serialised_return_code(return_code.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataAndReturnCode& lhs, DataAndReturnCode& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data, rhs.data);
  swap(lhs.return_code, rhs.return_code);
}



// ==================== DataOrDataNameAndReturnCode ================================================
template<>
DataOrDataNameAndReturnCode::DataOrDataNameAndReturnCode(
    const DataNameAndReturnCode& data_name_and_return_code_in)
        : data(),
          data_name_and_return_code(data_name_and_return_code_in) {}

DataOrDataNameAndReturnCode::DataOrDataNameAndReturnCode() : data(), data_name_and_return_code() {}

DataOrDataNameAndReturnCode::DataOrDataNameAndReturnCode(const DataOrDataNameAndReturnCode& other)
    : data(other.data),
      data_name_and_return_code(other.data_name_and_return_code) {}

DataOrDataNameAndReturnCode::DataOrDataNameAndReturnCode(DataOrDataNameAndReturnCode&& other)
    : data(std::move(other.data)),
      data_name_and_return_code(std::move(other.data_name_and_return_code)) {}

DataOrDataNameAndReturnCode& DataOrDataNameAndReturnCode::operator=(
    DataOrDataNameAndReturnCode other) {
  swap(*this, other);
  return *this;
}

DataOrDataNameAndReturnCode::DataOrDataNameAndReturnCode(const std::string& serialised_copy)
    : data(),
      data_name_and_return_code() {
  protobuf::DataOrDataNameAndReturnCode proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);

  if (proto_copy.has_serialised_data_name_and_content())
    data.reset(nfs_vault::DataNameAndContent(proto_copy.serialised_data_name_and_content()));
  if (proto_copy.has_serialised_data_name_and_return_code()) {
    data_name_and_return_code.reset(
        DataNameAndReturnCode(proto_copy.serialised_data_name_and_return_code()));
  }
  if (!nfs::CheckMutuallyExclusive(data, data_name_and_return_code)) {
    assert(false);
    ThrowError(CommonErrors::parsing_error);
  }
}

std::string DataOrDataNameAndReturnCode::Serialise() const {
  if (!nfs::CheckMutuallyExclusive(data, data_name_and_return_code)) {
    assert(false);
    ThrowError(CommonErrors::serialisation_error);
  }
  protobuf::DataOrDataNameAndReturnCode proto_copy;

  if (data)
    proto_copy.set_serialised_data_name_and_content(data->Serialise());
  else
    proto_copy.set_serialised_data_name_and_return_code(data_name_and_return_code->Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataOrDataNameAndReturnCode& lhs, DataOrDataNameAndReturnCode& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data, rhs.data);
  swap(lhs.data_name_and_return_code, rhs.data_name_and_return_code);
}



// ==================== StructuredDataOrDataNameAndReturnCode ======================================
StructuredDataOrDataNameAndReturnCode::StructuredDataOrDataNameAndReturnCode()
    : structured_data(),
      data_name_and_return_code() {}

StructuredDataOrDataNameAndReturnCode::StructuredDataOrDataNameAndReturnCode(
    const StructuredDataOrDataNameAndReturnCode& other)
        : structured_data(other.structured_data),
          data_name_and_return_code(other.data_name_and_return_code) {}

StructuredDataOrDataNameAndReturnCode::StructuredDataOrDataNameAndReturnCode(
    StructuredDataOrDataNameAndReturnCode&& other)
        : structured_data(std::move(other.structured_data)),
          data_name_and_return_code(std::move(other.data_name_and_return_code)) {}

StructuredDataOrDataNameAndReturnCode& StructuredDataOrDataNameAndReturnCode::operator=(
    StructuredDataOrDataNameAndReturnCode other) {
  swap(*this, other);
  return *this;
}

StructuredDataOrDataNameAndReturnCode::StructuredDataOrDataNameAndReturnCode(
    const std::string& serialised_copy)
        : structured_data(),
          data_name_and_return_code() {
  protobuf::StructuredDataOrDataNameAndReturnCode proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);

  if (proto_copy.has_serialised_structured_data())
    structured_data.reset(StructuredData(proto_copy.serialised_structured_data()));
  if (proto_copy.has_serialised_data_name_and_return_code()) {
    data_name_and_return_code.reset(
        DataNameAndReturnCode(proto_copy.serialised_data_name_and_return_code()));
  }
  if (!nfs::CheckMutuallyExclusive(structured_data, data_name_and_return_code)) {
    assert(false);
    ThrowError(CommonErrors::parsing_error);
  }
}

std::string StructuredDataOrDataNameAndReturnCode::Serialise() const {
  if (!nfs::CheckMutuallyExclusive(structured_data, data_name_and_return_code)) {
    assert(false);
    ThrowError(CommonErrors::serialisation_error);
  }
  protobuf::StructuredDataOrDataNameAndReturnCode proto_copy;

  if (structured_data)
    proto_copy.set_serialised_structured_data(structured_data->Serialise());
  else
    proto_copy.set_serialised_data_name_and_return_code(data_name_and_return_code->Serialise());
  return proto_copy.SerializeAsString();
}

void swap(StructuredDataOrDataNameAndReturnCode& lhs,
          StructuredDataOrDataNameAndReturnCode& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.structured_data, rhs.structured_data);
  swap(lhs.data_name_and_return_code, rhs.data_name_and_return_code);
}



// ==================== DataPmidHintAndReturnCode ==================================================
DataPmidHintAndReturnCode::DataPmidHintAndReturnCode() : data_and_pmid_hint(), return_code() {}

DataPmidHintAndReturnCode::DataPmidHintAndReturnCode(const DataPmidHintAndReturnCode& other)
    : data_and_pmid_hint(other.data_and_pmid_hint),
      return_code(other.return_code) {}

DataPmidHintAndReturnCode::DataPmidHintAndReturnCode(DataPmidHintAndReturnCode&& other)
    : data_and_pmid_hint(std::move(other.data_and_pmid_hint)),
      return_code(std::move(other.return_code)) {}

DataPmidHintAndReturnCode& DataPmidHintAndReturnCode::operator=(DataPmidHintAndReturnCode other) {
  swap(*this, other);
  return *this;
}

DataPmidHintAndReturnCode::DataPmidHintAndReturnCode(const std::string& serialised_copy)
    : data_and_pmid_hint(),
      return_code() {
  protobuf::DataPmidHintAndReturnCode proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  data_and_pmid_hint = nfs_vault::DataAndPmidHint(proto_copy.serialised_data_and_pmid_hint());
  return_code = ReturnCode(proto_copy.serialised_return_code());
}

std::string DataPmidHintAndReturnCode::Serialise() const {
  protobuf::DataPmidHintAndReturnCode proto_copy;
  proto_copy.set_serialised_data_and_pmid_hint(data_and_pmid_hint.Serialise());
  proto_copy.set_serialised_return_code(return_code.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataPmidHintAndReturnCode& lhs, DataPmidHintAndReturnCode& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data_and_pmid_hint, rhs.data_and_pmid_hint);
  swap(lhs.return_code, rhs.return_code);
}



// ==================== PmidRegistrationAndReturnCode ==============================================
PmidRegistrationAndReturnCode::PmidRegistrationAndReturnCode()
    : pmid_registration(),
      return_code() {}

PmidRegistrationAndReturnCode::PmidRegistrationAndReturnCode(
    const PmidRegistrationAndReturnCode& other)
        : pmid_registration(other.pmid_registration),
          return_code(other.return_code) {}

PmidRegistrationAndReturnCode::PmidRegistrationAndReturnCode(PmidRegistrationAndReturnCode&& other)
    : pmid_registration(std::move(other.pmid_registration)),
      return_code(std::move(other.return_code)) {}

PmidRegistrationAndReturnCode& PmidRegistrationAndReturnCode::operator=(
    PmidRegistrationAndReturnCode other) {
  swap(*this, other);
  return *this;
}

PmidRegistrationAndReturnCode::PmidRegistrationAndReturnCode(const std::string& serialised_copy)
    : pmid_registration(),
      return_code() {
  protobuf::PmidRegistrationAndReturnCode proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  pmid_registration = nfs_vault::PmidRegistration(proto_copy.serialised_pmid_registration());
  return_code = ReturnCode(proto_copy.serialised_return_code());
}

std::string PmidRegistrationAndReturnCode::Serialise() const {
  protobuf::PmidRegistrationAndReturnCode proto_copy;
  proto_copy.set_serialised_pmid_registration(pmid_registration.Serialise());
  proto_copy.set_serialised_return_code(return_code.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(PmidRegistrationAndReturnCode& lhs,
          PmidRegistrationAndReturnCode& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.pmid_registration, rhs.pmid_registration);
  swap(lhs.return_code, rhs.return_code);
}

}  // namespace nfs_client



namespace nfs {

template<>
bool IsSuccess<nfs_client::DataOrDataNameAndReturnCode>(
    const nfs_client::DataOrDataNameAndReturnCode& response) {
  return response.data;
}

template<>
std::error_code ErrorCode<nfs_client::DataOrDataNameAndReturnCode>(
    const nfs_client::DataOrDataNameAndReturnCode& response) {
  if (response.data_name_and_return_code)
    return response.data_name_and_return_code->return_code.value.code();
  else if (response.data)
    return std::error_code(CommonErrors::success);
  else
    return std::error_code(NfsErrors::timed_out);
}

template<>
bool IsSuccess<nfs_client::StructuredDataOrDataNameAndReturnCode>(
    const nfs_client::StructuredDataOrDataNameAndReturnCode& response) {
  return response.structured_data;
}

template<>
std::error_code ErrorCode<nfs_client::StructuredDataOrDataNameAndReturnCode>(
    const nfs_client::StructuredDataOrDataNameAndReturnCode& response) {
  if (response.data_name_and_return_code)
    return response.data_name_and_return_code->return_code.value.code();
  else if (response.structured_data)
    return std::error_code(CommonErrors::success);
  else
    return std::error_code(NfsErrors::timed_out);
}

}  // namespace nfs

}  // namespace maidsafe
