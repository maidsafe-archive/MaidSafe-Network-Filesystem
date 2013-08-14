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

#include "maidsafe/nfs/messages.h"

#include <cstdint>

//#include "maidsafe/common/crypto.h"
//#include "maidsafe/common/utils.h"
//
#include "maidsafe/nfs/messages.pb.h"
//#include "maidsafe/nfs/utils.h"


namespace maidsafe {

namespace nfs {

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



// ==================== DataName ===================================================================
DataName::DataName(DataTagValue type_in, const Identity& raw_name_in)
    : type(type_in),
      raw_name(raw_name_in) {}

DataName::DataName() : type(DataTagValue::kAnmidValue), raw_name() {}

DataName::DataName(const DataName& other) : type(other.type), raw_name(other.raw_name) {}

DataName::DataName(DataName&& other)
    : type(std::move(other.type)),
      raw_name(std::move(other.raw_name)) {}

DataName& DataName::operator=(DataName other) {
  swap(*this, other);
  return *this;
}

DataName::DataName(const std::string& serialised_copy)
    : type(DataTagValue::kAnmidValue),
      raw_name() {
  protobuf::DataName proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  type = static_cast<DataTagValue>(proto_copy.type());
  raw_name = Identity(proto_copy.raw_name());
}

std::string DataName::Serialise() const {
  protobuf::DataName proto_data_name;
  proto_data_name.set_type(static_cast<uint32_t>(type));
  proto_data_name.set_raw_name(raw_name.string());
  return proto_data_name.SerializeAsString();
}

void swap(DataName& lhs, DataName& rhs) {
  using std::swap;
  swap(lhs.type, rhs.type);
  swap(lhs.raw_name, rhs.raw_name);
}



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

void swap(ReturnCode& lhs, ReturnCode& rhs) {
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
  name = DataName(proto_copy.serialised_name());
  return_code = ReturnCode(proto_copy.serialised_return_code());
}

std::string DataNameAndReturnCode::Serialise() const {
  protobuf::DataNameAndReturnCode proto_copy;
  proto_copy.set_serialised_name(name.Serialise());
  proto_copy.set_serialised_return_code(return_code.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataNameAndReturnCode& lhs, DataNameAndReturnCode& rhs) {
  using std::swap;
  swap(lhs.name, rhs.name);
  swap(lhs.return_code, rhs.return_code);
}


// ==================== DataNameAndReturnCode ======================================================
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
    data = DataNameAndContent(proto_copy.serialised_data_name_and_content());
  if (proto_copy.has_serialised_name() && proto_copy.has_serialised_return_code()) {
    data_name_and_return_code->first = DataName(proto_copy.serialised_name());
    data_name_and_return_code->second = ReturnCode(proto_copy.serialised_return_code());
  }
  if (!CheckMutuallyExclusive(data, data_name_and_return_code)) {
    assert(false);
    ThrowError(CommonErrors::parsing_error);
  }
}

std::string DataOrDataNameAndReturnCode::Serialise() const {
  if (!CheckMutuallyExclusive(data, data_name_and_return_code)) {
    assert(false);
    ThrowError(CommonErrors::serialisation_error);
  }
  protobuf::DataOrDataNameAndReturnCode proto_copy;

  if (data) {
    proto_copy.set_serialised_data_name_and_content(data->Serialise());
  } else {
    proto_copy.set_serialised_name(data_name_and_return_code->first.Serialise());
    proto_copy.set_serialised_return_code(data_name_and_return_code->second.Serialise());
  }
  return proto_copy.SerializeAsString();
}

void swap(DataOrDataNameAndReturnCode& lhs, DataOrDataNameAndReturnCode& rhs) {
  using std::swap;
  swap(lhs.data, rhs.data);
  swap(lhs.data_name_and_return_code, rhs.data_name_and_return_code);
}


// ==================== DataNameAndVersion =========================================================
DataNameAndVersion::DataNameAndVersion() : data_name(), version_name() {}

DataNameAndVersion::DataNameAndVersion(const DataNameAndVersion& other)
    : data_name(other.data_name),
      version_name(other.version_name) {}
  
DataNameAndVersion::DataNameAndVersion(DataNameAndVersion&& other)
    : data_name(std::move(other.data_name)),
      version_name(std::move(other.version_name)) {}

DataNameAndVersion& DataNameAndVersion::operator=(DataNameAndVersion other) {
  swap(*this, other);
  return *this;
}

DataNameAndVersion::DataNameAndVersion(const std::string& serialised_copy)
    : data_name(),
      version_name() {
  protobuf::DataNameAndVersion proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  data_name = DataName(proto_copy.serialised_data_name());
  version_name = StructuredDataVersions::VersionName(proto_copy.serialised_version_name());
}

std::string DataNameAndVersion::Serialise() const {
  protobuf::DataNameAndVersion proto_copy;
  proto_copy.set_serialised_data_name(data_name.Serialise());
  proto_copy.set_serialised_version_name(version_name.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataNameAndVersion& lhs, DataNameAndVersion& rhs) {
  using std::swap;
  swap(lhs.data_name, rhs.data_name);
  swap(lhs.version_name, rhs.version_name);
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
  data_name_and_version = DataNameAndVersion(proto_copy.serialised_data_name_and_version());
  return_code = ReturnCode(proto_copy.serialised_return_code());
}

std::string DataNameVersionAndReturnCode::Serialise() const {
  protobuf::DataNameVersionAndReturnCode proto_copy;
  proto_copy.set_serialised_data_name_and_version(data_name_and_version.Serialise());
  proto_copy.set_serialised_return_code(return_code.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataNameVersionAndReturnCode& lhs, DataNameVersionAndReturnCode& rhs) {
  using std::swap;
  swap(lhs.data_name_and_version, rhs.data_name_and_version);
  swap(lhs.return_code, rhs.return_code);
}



// ==================== DataNameOldNewVersion ======================================================
DataNameOldNewVersion::DataNameOldNewVersion()
    : data_name(),
      old_version_name(),
      new_version_name() {}

DataNameOldNewVersion::DataNameOldNewVersion(const DataNameOldNewVersion& other)
    : data_name(other.data_name),
      old_version_name(other.old_version_name),
      new_version_name(other.new_version_name) {}
  
DataNameOldNewVersion::DataNameOldNewVersion(DataNameOldNewVersion&& other)
    : data_name(std::move(other.data_name)),
      old_version_name(std::move(other.old_version_name)),
      new_version_name(std::move(other.new_version_name)) {}

DataNameOldNewVersion& DataNameOldNewVersion::operator=(DataNameOldNewVersion other) {
  swap(*this, other);
  return *this;
}

DataNameOldNewVersion::DataNameOldNewVersion(const std::string& serialised_copy)
    : data_name(),
      old_version_name(),
      new_version_name() {
  protobuf::DataNameOldNewVersion proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  data_name = DataName(proto_copy.serialised_data_name());
  old_version_name = StructuredDataVersions::VersionName(proto_copy.serialised_old_version_name());
  new_version_name = StructuredDataVersions::VersionName(proto_copy.serialised_new_version_name());
}

std::string DataNameOldNewVersion::Serialise() const {
  protobuf::DataNameOldNewVersion proto_copy;
  proto_copy.set_serialised_data_name(data_name.Serialise());
  proto_copy.set_serialised_old_version_name(old_version_name.Serialise());
  proto_copy.set_serialised_new_version_name(new_version_name.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataNameOldNewVersion& lhs, DataNameOldNewVersion& rhs) {
  using std::swap;
  swap(lhs.data_name, rhs.data_name);
  swap(lhs.old_version_name, rhs.old_version_name);
  swap(lhs.new_version_name, rhs.new_version_name);
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
      DataNameOldNewVersion(proto_copy.serialised_data_name_old_new_version());
  return_code = ReturnCode(proto_copy.serialised_return_code());
}

std::string DataNameOldNewVersionAndReturnCode::Serialise() const {
  protobuf::DataNameOldNewVersionAndReturnCode proto_copy;
  proto_copy.set_serialised_data_name_old_new_version(data_name_old_new_version.Serialise());
  proto_copy.set_serialised_return_code(return_code.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataNameOldNewVersionAndReturnCode& lhs, DataNameOldNewVersionAndReturnCode& rhs) {
  using std::swap;
  swap(lhs.data_name_old_new_version, rhs.data_name_old_new_version);
  swap(lhs.return_code, rhs.return_code);
}



// ==================== DataNameAndContent =========================================================
DataNameAndContent::DataNameAndContent() : name(), content() {}

DataNameAndContent::DataNameAndContent(const DataNameAndContent& other)
    : name(other.name),
      content(other.content) {}
  
DataNameAndContent::DataNameAndContent(DataNameAndContent&& other)
    : name(std::move(other.name)),
      content(std::move(other.content)) {}

DataNameAndContent& DataNameAndContent::operator=(DataNameAndContent other) {
  swap(*this, other);
  return *this;
}

DataNameAndContent::DataNameAndContent(const std::string& serialised_copy)
    : name(),
      content() {
  protobuf::DataNameAndContent proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  name = DataName(proto_copy.serialised_name());
  content = NonEmptyString(proto_copy.content());
}

std::string DataNameAndContent::Serialise() const {
  protobuf::DataNameAndContent proto_copy;
  proto_copy.set_serialised_name(name.Serialise());
  proto_copy.set_content(content.string());
  return proto_copy.SerializeAsString();
}

void swap(DataNameAndContent& lhs, DataNameAndContent& rhs) {
  using std::swap;
  swap(lhs.name, rhs.name);
  swap(lhs.content, rhs.content);
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
  data = DataNameAndContent(proto_copy.serialised_data_name_and_content());
  return_code = ReturnCode(proto_copy.serialised_return_code());
}

std::string DataAndReturnCode::Serialise() const {
  protobuf::DataAndReturnCode proto_copy;
  proto_copy.set_serialised_data_name_and_content(data.Serialise());
  proto_copy.set_serialised_return_code(return_code.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataAndReturnCode& lhs, DataAndReturnCode& rhs) {
  using std::swap;
  swap(lhs.data, rhs.data);
  swap(lhs.return_code, rhs.return_code);
}



// ==================== DataAndPmidHint ============================================================
DataAndPmidHint::DataAndPmidHint() : data(), pmid_hint() {}

DataAndPmidHint::DataAndPmidHint(const DataAndPmidHint& other)
    : data(other.data),
      pmid_hint(other.pmid_hint) {}

DataAndPmidHint::DataAndPmidHint(DataAndPmidHint&& other)
    : data(std::move(other.data)),
      pmid_hint(std::move(other.pmid_hint)) {}

DataAndPmidHint& DataAndPmidHint::operator=(DataAndPmidHint other) {
  swap(*this, other);
  return *this;
}

DataAndPmidHint::DataAndPmidHint(const std::string& serialised_copy)
    : data(),
      pmid_hint() {
  protobuf::DataAndPmidHint proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  data = DataNameAndContent(proto_copy.serialised_data_name_and_content());
  pmid_hint = Identity(proto_copy.pmid_hint());
}

std::string DataAndPmidHint::Serialise() const {
  protobuf::DataAndPmidHint proto_copy;
  proto_copy.set_serialised_data_name_and_content(data.Serialise());
  proto_copy.set_pmid_hint(pmid_hint.string());
  return proto_copy.SerializeAsString();
}

void swap(DataAndPmidHint& lhs, DataAndPmidHint& rhs) {
  using std::swap;
  swap(lhs.data, rhs.data);
  swap(lhs.pmid_hint, rhs.pmid_hint);
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
  data_and_pmid_hint = DataAndPmidHint(proto_copy.serialised_data_and_pmid_hint());
  return_code = ReturnCode(proto_copy.serialised_return_code());
}

std::string DataPmidHintAndReturnCode::Serialise() const {
  protobuf::DataPmidHintAndReturnCode proto_copy;
  proto_copy.set_serialised_data_and_pmid_hint(data_and_pmid_hint.Serialise());
  proto_copy.set_serialised_return_code(return_code.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(DataPmidHintAndReturnCode& lhs, DataPmidHintAndReturnCode& rhs) {
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
  pmid_registration = PmidRegistration(proto_copy.serialised_pmid_registration());
  return_code = ReturnCode(proto_copy.serialised_return_code());
}

std::string PmidRegistrationAndReturnCode::Serialise() const {
  protobuf::PmidRegistrationAndReturnCode proto_copy;
  proto_copy.set_serialised_pmid_registration(pmid_registration.Serialise());
  proto_copy.set_serialised_return_code(return_code.Serialise());
  return proto_copy.SerializeAsString();
}

void swap(PmidRegistrationAndReturnCode& lhs, PmidRegistrationAndReturnCode& rhs) {
  using std::swap;
  swap(lhs.pmid_registration, rhs.pmid_registration);
  swap(lhs.return_code, rhs.return_code);
}

}  // namespace nfs

}  // namespace maidsafe
