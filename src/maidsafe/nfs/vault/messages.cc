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

#include "maidsafe/nfs/vault/messages.h"

#include <cstdint>

#include "maidsafe/nfs/vault/messages.pb.h"


namespace maidsafe {

namespace nfs_vault {

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

void swap(DataName& lhs, DataName& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.type, rhs.type);
  swap(lhs.raw_name, rhs.raw_name);
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

void swap(DataNameAndVersion& lhs, DataNameAndVersion& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data_name, rhs.data_name);
  swap(lhs.version_name, rhs.version_name);
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

void swap(DataNameOldNewVersion& lhs, DataNameOldNewVersion& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data_name, rhs.data_name);
  swap(lhs.old_version_name, rhs.old_version_name);
  swap(lhs.new_version_name, rhs.new_version_name);
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

void swap(DataNameAndContent& lhs, DataNameAndContent& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.name, rhs.name);
  swap(lhs.content, rhs.content);
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

void swap(DataAndPmidHint& lhs, DataAndPmidHint& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data, rhs.data);
  swap(lhs.pmid_hint, rhs.pmid_hint);
}

}  // namespace nfs_vault

}  // namespace maidsafe
