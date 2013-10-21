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

#include "maidsafe/nfs/vault/messages.h"

#include <cstdint>

#include "maidsafe/nfs/utils.h"
#include "maidsafe/nfs/vault/messages.pb.h"

namespace maidsafe {

namespace nfs_vault {

// ========================== Empty ================================================================

bool operator==(const Empty& /*lhs*/, const Empty& /*rhs*/) { return true; }

// ================================= AvailableSize =================================================

// AvailableSize::AvailableSize() : available_size(0) {}

AvailableSize::AvailableSize(uint64_t size) : available_size(size) {}

AvailableSize::AvailableSize(const AvailableSize& other) : available_size(other.available_size) {}

AvailableSize::AvailableSize(AvailableSize&& other)
    : available_size(std::move(other.available_size)) {}

AvailableSize& AvailableSize::operator=(AvailableSize other) {
  swap(*this, other);
  return *this;
}

AvailableSize::AvailableSize(const std::string& serialised_copy)
    : available_size([&serialised_copy]() {
                       protobuf::AvailableSize proto_size;
                       if (!proto_size.ParseFromString(serialised_copy))
                         ThrowError(CommonErrors::parsing_error);
                       return proto_size.size();
                     }()) {}

std::string AvailableSize::Serialise() const {
  protobuf::AvailableSize proto_size;
  proto_size.set_size(available_size);
  return proto_size.SerializeAsString();
}

bool operator==(const AvailableSize& lhs, const AvailableSize& rhs) {
  return lhs.available_size == rhs.available_size;
}

void swap(AvailableSize& lhs, AvailableSize& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.available_size, rhs.available_size);
}

// ==================== DataName ===================================================================
DataName::DataName(DataTagValue type_in, Identity raw_name_in)
    : type(type_in), raw_name(std::move(raw_name_in)) {}

DataName::DataName() : type(DataTagValue::kAnmidValue), raw_name() {}

DataName::DataName(const DataName& other) : type(other.type), raw_name(other.raw_name) {}

DataName::DataName(DataName&& other)
    : type(std::move(other.type)), raw_name(std::move(other.raw_name)) {}

DataName& DataName::operator=(DataName other) {
  swap(*this, other);
  return *this;
}

DataName::DataName(const std::string& serialised_copy)
    : type(DataTagValue::kAnmidValue), raw_name() {
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

bool operator==(const DataName& lhs, const DataName& rhs) {
  return lhs.type == rhs.type && lhs.raw_name == rhs.raw_name;
}

bool operator<(const DataName& lhs, const DataName& rhs) {
  return lhs.raw_name.string() < rhs.raw_name.string();
}

void swap(DataName& lhs, DataName& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.type, rhs.type);
  swap(lhs.raw_name, rhs.raw_name);
}

// ==================== DataNameAndVersion =========================================================

DataNameAndVersion::DataNameAndVersion() : data_name(), version_name() {}

DataNameAndVersion::DataNameAndVersion(const DataNameAndVersion& other)
    : data_name(other.data_name), version_name(other.version_name) {}

DataNameAndVersion::DataNameAndVersion(DataNameAndVersion&& other)
    : data_name(std::move(other.data_name)), version_name(std::move(other.version_name)) {}

DataNameAndVersion& DataNameAndVersion::operator=(DataNameAndVersion other) {
  swap(*this, other);
  return *this;
}

DataNameAndVersion::DataNameAndVersion(const std::string& serialised_copy)
    : data_name(), version_name() {
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

bool operator==(const DataNameAndVersion& lhs, const DataNameAndVersion& rhs) {
  return lhs.data_name == rhs.data_name && lhs.version_name == rhs.version_name;
}

void swap(DataNameAndVersion& lhs, DataNameAndVersion& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data_name, rhs.data_name);
  swap(lhs.version_name, rhs.version_name);
}

// ==================== DataNameOldNewVersion ======================================================

DataNameOldNewVersion::DataNameOldNewVersion()
    : data_name(), old_version_name(), new_version_name() {}

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
    : data_name(), old_version_name(), new_version_name() {
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

bool operator==(const DataNameOldNewVersion& lhs, const DataNameOldNewVersion& rhs) {
  return lhs.data_name == rhs.data_name && lhs.old_version_name == rhs.old_version_name &&
         lhs.new_version_name == rhs.new_version_name;
}

void swap(DataNameOldNewVersion& lhs, DataNameOldNewVersion& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data_name, rhs.data_name);
  swap(lhs.old_version_name, rhs.old_version_name);
  swap(lhs.new_version_name, rhs.new_version_name);
}

// ==================== DataNameAndContent =========================================================

DataNameAndContent::DataNameAndContent(DataTagValue type_in, const Identity& name_in,
                                       NonEmptyString content_in)
    : name(type_in, name_in), content(std::move(content_in)) {}

DataNameAndContent::DataNameAndContent() : name(), content() {}

DataNameAndContent::DataNameAndContent(const DataNameAndContent& other)
    : name(other.name), content(other.content) {}

DataNameAndContent::DataNameAndContent(DataNameAndContent&& other)
    : name(std::move(other.name)), content(std::move(other.content)) {}

DataNameAndContent& DataNameAndContent::operator=(DataNameAndContent other) {
  swap(*this, other);
  return *this;
}

DataNameAndContent::DataNameAndContent(const std::string& serialised_copy) : name(), content() {
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

bool operator==(const DataNameAndContent& lhs, const DataNameAndContent& rhs) {
  return lhs.name == rhs.name && lhs.content == rhs.content;
}

void swap(DataNameAndContent& lhs, DataNameAndContent& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.name, rhs.name);
  swap(lhs.content, rhs.content);
}
// ========================== Content ==============================================================

Content::Content(const std::string &data_in) : data(data_in) {}

Content::Content() : data() {}

Content::Content(const Content& other) : data(other.data) {}

Content::Content(Content&& other) : data(std::move(other.data)) {}

std::string Content::Serialise() const {
  return data;
}

bool operator==(const Content& lhs, const Content& rhs) {
  return lhs.data == rhs.data;
}

void swap(Content& lhs, Content& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data, rhs.data);
}

// ==================== DataNameAndRandomString ====================================================

DataNameAndRandomString::DataNameAndRandomString(DataTagValue type_in, const Identity& name_in,
                                                 NonEmptyString random_string_in)
    : name(type_in, name_in), random_string(std::move(random_string_in)) {}

DataNameAndRandomString::DataNameAndRandomString() : name(), random_string() {}

DataNameAndRandomString::DataNameAndRandomString(const DataNameAndRandomString& other)
    : name(other.name), random_string(other.random_string) {}

DataNameAndRandomString::DataNameAndRandomString(DataNameAndRandomString&& other)
    : name(std::move(other.name)), random_string(std::move(other.random_string)) {}

DataNameAndRandomString& DataNameAndRandomString::operator=(DataNameAndRandomString other) {
  swap(*this, other);
  return *this;
}

DataNameAndRandomString::DataNameAndRandomString(const std::string& serialised_copy)
    : name(), random_string() {
  protobuf::DataNameAndRandomString proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  name = DataName(proto_copy.serialised_name());
  random_string = NonEmptyString(proto_copy.random_string());
}

std::string DataNameAndRandomString::Serialise() const {
  protobuf::DataNameAndRandomString proto_copy;
  proto_copy.set_serialised_name(name.Serialise());
  proto_copy.set_random_string(random_string.string());
  return proto_copy.SerializeAsString();
}

bool operator==(const DataNameAndRandomString& lhs, const DataNameAndRandomString& rhs) {
  return lhs.name == rhs.name && lhs.random_string == rhs.random_string;
}

void swap(DataNameAndRandomString& lhs, DataNameAndRandomString& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.name, rhs.name);
  swap(lhs.random_string, rhs.random_string);
}

// ==================== DataNameAndCost ============================================================

DataNameAndCost::DataNameAndCost() : name(), cost(0) {}

DataNameAndCost::DataNameAndCost(DataTagValue type_in, const Identity& name_in, int32_t cost_in)
    : name(type_in, name_in), cost(cost_in) {}

DataNameAndCost::DataNameAndCost(const DataNameAndCost& other)
    : name(other.name), cost(other.cost) {}

DataNameAndCost::DataNameAndCost(DataNameAndCost&& other)
    : name(std::move(other.name)), cost(std::move(other.cost)) {}

DataNameAndCost& DataNameAndCost::operator=(DataNameAndCost other) {
  swap(*this, other);
  return *this;
}

DataNameAndCost::DataNameAndCost(const std::string& serialised_copy) : name(), cost(0) {
  protobuf::DataNameAndCost proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  name = DataName(proto_copy.serialised_name());
  cost = proto_copy.cost();
}

std::string DataNameAndCost::Serialise() const {
  protobuf::DataNameAndCost proto_copy;
  proto_copy.set_serialised_name(name.Serialise());
  proto_copy.set_cost(cost);
  return proto_copy.SerializeAsString();
}

bool operator==(const DataNameAndCost& lhs, const DataNameAndCost& rhs) {
  return lhs.name == rhs.name && lhs.cost == rhs.cost;
}

void swap(DataNameAndCost& lhs, DataNameAndCost& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.name, rhs.name);
  swap(lhs.cost, rhs.cost);
}

// ==================== DataNameAndSize ============================================================

DataNameAndSize::DataNameAndSize() : name(), size(0) {}

DataNameAndSize::DataNameAndSize(DataTagValue type_in, const Identity& name_in,
    int32_t size_in) : name(type_in, name_in), size(size_in) {}

DataNameAndSize::DataNameAndSize(const DataNameAndSize& other)
    : name(other.name), size(other.size) {}

DataNameAndSize::DataNameAndSize(DataNameAndSize&& other)
    : name(std::move(other.name)), size(std::move(other.size)) {}

DataNameAndSize& DataNameAndSize::operator=(DataNameAndSize other) {
  swap(*this, other);
  return *this;
}

DataNameAndSize::DataNameAndSize(const std::string& serialised_copy) : name(), size(0) {
  protobuf::DataNameAndSize proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  name = DataName(proto_copy.serialised_name());
  size = proto_copy.size();
}

std::string DataNameAndSize::Serialise() const {
  protobuf::DataNameAndSize proto_copy;
  proto_copy.set_serialised_name(name.Serialise());
  proto_copy.set_size(size);
  return proto_copy.SerializeAsString();
}

bool operator==(const DataNameAndSize& lhs, const DataNameAndSize& rhs) {
  return lhs.name == rhs.name && lhs.size == rhs.size;
}

void swap(DataNameAndSize& lhs, DataNameAndSize& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.name, rhs.name);
  swap(lhs.size, rhs.size);
}

// ==================== DataAndPmidHint ============================================================

DataAndPmidHint::DataAndPmidHint() : data(), pmid_hint() {}

DataAndPmidHint::DataAndPmidHint(const DataName& data_name, const NonEmptyString& content,
                                 Identity pmid_node_hint)
    : data(data_name.type, data_name.raw_name, content), pmid_hint(std::move(pmid_node_hint)) {}

DataAndPmidHint::DataAndPmidHint(const DataAndPmidHint& other)
    : data(other.data), pmid_hint(other.pmid_hint) {}

DataAndPmidHint::DataAndPmidHint(DataAndPmidHint&& other)
    : data(std::move(other.data)), pmid_hint(std::move(other.pmid_hint)) {}

DataAndPmidHint& DataAndPmidHint::operator=(DataAndPmidHint other) {
  swap(*this, other);
  return *this;
}

DataAndPmidHint::DataAndPmidHint(const std::string& serialised_copy) : data(), pmid_hint() {
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

bool operator==(const DataAndPmidHint& lhs, const DataAndPmidHint& rhs) {
  return lhs.data == rhs.data && lhs.pmid_hint == rhs.pmid_hint;
}

void swap(DataAndPmidHint& lhs, DataAndPmidHint& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data, rhs.data);
  swap(lhs.pmid_hint, rhs.pmid_hint);
}

// ========================== DataNameAndContentOrCheckResult ======================================

DataNameAndContentOrCheckResult::DataNameAndContentOrCheckResult(
    const DataTagValue& type_in, const Identity& name_in, const NonEmptyString& content_in)
        : name(type_in, name_in), content(content_in), check_result() {}

DataNameAndContentOrCheckResult::DataNameAndContentOrCheckResult(
    const DataTagValue& type_in, const Identity& name_in, const CheckResult& check_result_in)
        : name(type_in, name_in), content(), check_result(check_result_in) {}

DataNameAndContentOrCheckResult::DataNameAndContentOrCheckResult()
    : name(), content(), check_result() {}

DataNameAndContentOrCheckResult::DataNameAndContentOrCheckResult(
    DataNameAndContentOrCheckResult&& other)
        : name(std::move(other.name)),
          content(std::move(other.content)),
          check_result(std::move(other.check_result)) {}

DataNameAndContentOrCheckResult::DataNameAndContentOrCheckResult(
    const DataNameAndContentOrCheckResult& other)
        : name(other.name),
          content(other.content),
          check_result(other.check_result) {}

DataNameAndContentOrCheckResult& DataNameAndContentOrCheckResult::operator=(
    DataNameAndContentOrCheckResult other) {
  swap(*this, other);
  return *this;
}

DataNameAndContentOrCheckResult::DataNameAndContentOrCheckResult(
    const std::string& serialised_copy) {
  protobuf::DataNameAndContentOrCheckResult proto;
  if (!proto.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);

  name = DataName(proto.serialised_name());
  if (proto.has_content())
    content.reset(NonEmptyString(proto.content()));
  if (proto.has_check_result())
    check_result.reset(CheckResult(proto.check_result()));

  if (!nfs::CheckMutuallyExclusive(content, check_result)) {
    assert(false);
    ThrowError(CommonErrors::parsing_error);
  }
}

std::string DataNameAndContentOrCheckResult::Serialise() const {
  protobuf::DataNameAndContentOrCheckResult proto;
  proto.set_serialised_name(name.Serialise());
  if (content)
    proto.set_content(content->string());
  if (check_result)
    proto.set_check_result(check_result->string());

  if (!nfs::CheckMutuallyExclusive(content, check_result)) {
    assert(false);
    ThrowError(CommonErrors::serialisation_error);
  }

  return proto.SerializeAsString();
}

void swap(DataNameAndContentOrCheckResult& lhs,
          DataNameAndContentOrCheckResult& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.name, rhs.name);
  swap(lhs.content, rhs.content);
  swap(lhs.check_result, rhs.check_result);
}

bool operator==(const DataNameAndContentOrCheckResult& lhs,
                const DataNameAndContentOrCheckResult& rhs) {
  if (!(lhs.name == rhs.name))
    return false;

  if ((!lhs.content && rhs.content) || (lhs.content && !rhs.content) ||
      (lhs.content && (*lhs.content != *rhs.content)))
    return false;

  if ((!lhs.check_result && rhs.check_result) || (lhs.check_result && !rhs.check_result) ||
      (lhs.check_result && (*lhs.check_result != *rhs.check_result)))
    return false;

  return true;
}

// ================================= PmidHealth ===================================================

PmidHealth::PmidHealth() : serialised_pmid_health() {}

PmidHealth::PmidHealth(const std::string& serialised_copy)
    : serialised_pmid_health(serialised_copy) {}

PmidHealth::PmidHealth(const PmidHealth& other)
    : serialised_pmid_health(other.serialised_pmid_health) {}

PmidHealth::PmidHealth(PmidHealth&& other)
    : serialised_pmid_health(std::move(other.serialised_pmid_health)) {}

PmidHealth& PmidHealth::operator=(PmidHealth other) {
  swap(*this, other);
  return *this;
}

std::string PmidHealth::Serialise() const {
  protobuf::PmidHealth pmid_health_proto;
  pmid_health_proto.set_serialised_pmid_health(serialised_pmid_health);
  return pmid_health_proto.SerializeAsString();
}

bool operator==(const PmidHealth& lhs, const PmidHealth& rhs) {
  return lhs.serialised_pmid_health == rhs.serialised_pmid_health;
}

void swap(PmidHealth& lhs, PmidHealth& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.serialised_pmid_health, rhs.serialised_pmid_health);
}

}  // namespace nfs_vault

}  // namespace maidsafe
