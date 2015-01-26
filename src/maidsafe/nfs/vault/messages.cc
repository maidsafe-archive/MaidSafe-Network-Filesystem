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

#include "maidsafe/common/serialisation/serialisation.h"
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
          BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
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

// ========================================== DiffSize =============================================

DiffSize::DiffSize(int64_t size) : diff_size(size) {}

DiffSize::DiffSize(const DiffSize& other) : diff_size(other.diff_size) {}

DiffSize::DiffSize(DiffSize&& other) : diff_size(std::move(other.diff_size)) {}

DiffSize& DiffSize::operator=(DiffSize other) {
  swap(*this, other);
  return *this;
}

DiffSize::DiffSize(const std::string& serialised_copy)
    : diff_size([&serialised_copy]() {
        protobuf::DiffSize proto_size;
        if (!proto_size.ParseFromString(serialised_copy))
          BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
        return proto_size.size();
      }()) {}

std::string DiffSize::Serialise() const {
  protobuf::DiffSize proto_size;
  proto_size.set_size(diff_size);
  return proto_size.SerializeAsString();
}

bool operator==(const DiffSize& lhs, const DiffSize& rhs) { return lhs.diff_size == rhs.diff_size; }

void swap(DiffSize& lhs, DiffSize& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.diff_size, rhs.diff_size);
}

// ==================== DataName ===================================================================
DataName::DataName(DataTagValue type_in, Identity raw_name_in)
    : type(type_in), raw_name(std::move(raw_name_in)) {}

DataName::DataName() : type(DataTagValue::kAnmaidValue), raw_name() {}

DataName::DataName(const DataName& other) : type(other.type), raw_name(other.raw_name) {}

DataName::DataName(DataName&& other)
    : type(std::move(other.type)), raw_name(std::move(other.raw_name)) {}

DataName& DataName::operator=(DataName other) {
  swap(*this, other);
  return *this;
}

DataName::DataName(const std::string& serialised_copy)
    : type(DataTagValue::kAnmaidValue), raw_name() {
  protobuf::DataName proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
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

// ==================== DataNames ==================================
DataNames::DataNames(const std::vector<DataName>& data_names) : data_names_(data_names) {}

DataNames::DataNames() : data_names_() {}

DataNames::DataNames(const DataNames& other) : data_names_(other.data_names_) {}

DataNames::DataNames(DataNames&& other) : data_names_(std::move(other.data_names_)) {}

DataNames& DataNames::operator=(DataNames other) {
  swap(*this, other);
  return *this;
}

DataNames::DataNames(const std::string& serialised_copy) : data_names_() {
  protobuf::DataNames proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
  for (int index(0); index < proto_copy.data_names_size(); ++index) {
    data_names_.push_back(DataName(static_cast<DataTagValue>(proto_copy.data_names(index).type()),
                                   Identity(proto_copy.data_names(index).raw_name())));
  }
}

std::string DataNames::Serialise() const {
  protobuf::DataNames proto_data_names;
  for (const auto& data_name : data_names_) {
    protobuf::DataName proto_data_name;
    proto_data_name.set_type(static_cast<uint32_t>(data_name.type));
    proto_data_name.set_raw_name(data_name.raw_name.string());
    auto value(proto_data_names.add_data_names());
    *value = proto_data_name;
  }
  return proto_data_names.SerializeAsString();
}

bool operator==(const DataNames& lhs, const DataNames& rhs) {
  if (lhs.data_names_.size() != rhs.data_names_.size())
    return false;
  for (const auto& data_name : lhs.data_names_) {
    if (std::find(std::begin(rhs.data_names_), std::end(rhs.data_names_), data_name) ==
        std::end(rhs.data_names_))
      return false;
  }
  return true;
}

void swap(DataNames& lhs, DataNames& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data_names_, rhs.data_names_);
}

// ==================== DataNameAndVersion =========================================================

DataNameAndVersion::DataNameAndVersion() : data_name(), version_name() {}

DataNameAndVersion::DataNameAndVersion(const DataName& data_name_in,
                                       const StructuredDataVersions::VersionName& version_name_in)
    : data_name(data_name_in), version_name(version_name_in) {}

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
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
  data_name = DataName(proto_copy.serialised_data_name());
  version_name =
      ConvertFromString<StructuredDataVersions::VersionName>(proto_copy.serialised_version_name());
}

std::string DataNameAndVersion::Serialise() const {
  protobuf::DataNameAndVersion proto_copy;
  proto_copy.set_serialised_data_name(data_name.Serialise());
  proto_copy.set_serialised_version_name(ConvertToString(version_name));
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

DataNameOldNewVersion::DataNameOldNewVersion(const DataName& name,
                                             const StructuredDataVersions::VersionName& old_version,
                                             const StructuredDataVersions::VersionName& new_version)
    : data_name(name), old_version_name(old_version), new_version_name(new_version) {}

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
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
  data_name = DataName(proto_copy.serialised_data_name());
  if (proto_copy.has_serialised_old_version_name())
    old_version_name = ConvertFromString<StructuredDataVersions::VersionName>(
        proto_copy.serialised_old_version_name());
  new_version_name = ConvertFromString<StructuredDataVersions::VersionName>(
      proto_copy.serialised_new_version_name());
}

std::string DataNameOldNewVersion::Serialise() const {
  protobuf::DataNameOldNewVersion proto_copy;
  proto_copy.set_serialised_data_name(data_name.Serialise());
  if (old_version_name.id->IsInitialised())
    proto_copy.set_serialised_old_version_name(ConvertToString(old_version_name));
  proto_copy.set_serialised_new_version_name(ConvertToString(new_version_name));
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

// ========================== VersionTreeCreation ==================================================

VersionTreeCreation::VersionTreeCreation(const DataName& name,
                                         const StructuredDataVersions::VersionName& version,
                                         uint32_t max_versions_in, uint32_t max_branches_in)
    : data_name(name),
      version_name(version),
      max_versions(max_versions_in),
      max_branches(max_branches_in) {}

VersionTreeCreation::VersionTreeCreation(const VersionTreeCreation& other)
    : data_name(other.data_name),
      version_name(other.version_name),
      max_versions(other.max_versions),
      max_branches(other.max_branches) {}

VersionTreeCreation::VersionTreeCreation(VersionTreeCreation&& other)
    : data_name(std::move(other.data_name)),
      version_name(std::move(other.version_name)),
      max_versions(std::move(other.max_versions)),
      max_branches(std::move(other.max_branches)) {}

VersionTreeCreation& VersionTreeCreation::operator=(VersionTreeCreation other) {
  swap(*this, other);
  return *this;
}

VersionTreeCreation::VersionTreeCreation(const std::string& serialised_copy)
    : data_name(), version_name(), max_versions(0), max_branches(0) {
  protobuf::VersionTreeCreation proto_copy;
  if (!proto_copy.ParseFromString(serialised_copy))
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
  data_name = DataName(proto_copy.serialised_data_name());
  version_name =
      ConvertFromString<StructuredDataVersions::VersionName>(proto_copy.serialised_version_name());
  max_versions = proto_copy.max_versions();
  max_branches = proto_copy.max_branches();
}

std::string VersionTreeCreation::Serialise() const {
  protobuf::VersionTreeCreation proto_copy;
  proto_copy.set_serialised_data_name(data_name.Serialise());
  proto_copy.set_serialised_version_name(ConvertToString(version_name));
  proto_copy.set_max_versions(max_versions);
  proto_copy.set_max_branches(max_branches);
  return proto_copy.SerializeAsString();
}

bool operator==(const VersionTreeCreation& lhs, const VersionTreeCreation& rhs) {
  return lhs.data_name == rhs.data_name && lhs.version_name == rhs.version_name &&
         lhs.max_versions == rhs.max_versions && lhs.max_branches == rhs.max_branches;
}

void swap(VersionTreeCreation& lhs, VersionTreeCreation& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.data_name, rhs.data_name);
  swap(lhs.version_name, rhs.version_name);
  swap(lhs.max_versions, rhs.max_versions);
  swap(lhs.max_branches, rhs.max_branches);
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
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
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
  //   LOG(kVerbose) << "DataNameAndContent comparation : "
  //                 << "lhs.name : " << HexSubstr(lhs.name.Serialise())
  //                 << " ; rhs.name : " << HexSubstr(rhs.name.Serialise())
  //                 << " ; lhs.content : " << HexSubstr(lhs.content.string())
  //                 << " ; rhs.content : " << HexSubstr(rhs.content.string());
  return lhs.name == rhs.name && lhs.content == rhs.content;
}

void swap(DataNameAndContent& lhs, DataNameAndContent& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.name, rhs.name);
  swap(lhs.content, rhs.content);
}
// ========================== Content ==============================================================

Content::Content(const std::string& data_in) : data(data_in) {}

Content::Content() : data() {}

Content::Content(const Content& other) : data(other.data) {}

Content::Content(Content&& other) : data(std::move(other.data)) {}

std::string Content::Serialise() const { return data; }

Content& Content::operator=(Content other) {
  swap(*this, other);
  return *this;
}

bool operator==(const Content& lhs, const Content& rhs) { return lhs.data == rhs.data; }

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
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
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
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
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

DataNameAndSize::DataNameAndSize(DataTagValue type_in, const Identity& name_in, int32_t size_in)
    : name(type_in, name_in), size(size_in) {}

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
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
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

// ========================== DataNameAndContentOrCheckResult ======================================

DataNameAndContentOrCheckResult::DataNameAndContentOrCheckResult(const DataTagValue& type_in,
                                                                 const Identity& name_in,
                                                                 const NonEmptyString& content_in)
    : name(type_in, name_in), content(content_in), check_result() {}

DataNameAndContentOrCheckResult::DataNameAndContentOrCheckResult(const DataTagValue& type_in,
                                                                 const Identity& name_in,
                                                                 const CheckResult& check_result_in)
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
    : name(other.name), content(other.content), check_result(other.check_result) {}

DataNameAndContentOrCheckResult& DataNameAndContentOrCheckResult::operator=(
    DataNameAndContentOrCheckResult other) {
  swap(*this, other);
  return *this;
}

DataNameAndContentOrCheckResult::DataNameAndContentOrCheckResult(
    const std::string& serialised_copy) {
  protobuf::DataNameAndContentOrCheckResult proto;
  if (!proto.ParseFromString(serialised_copy))
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));

  name = DataName(proto.serialised_name());
  if (proto.has_content())
    content.reset(NonEmptyString(proto.content()));
  if (proto.has_check_result())
    check_result.reset(CheckResult(proto.check_result()));

  if (!nfs::CheckMutuallyExclusive(content, check_result)) {
    assert(false);
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
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
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::serialisation_error));
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

std::string PmidHealth::Serialise() const { return serialised_pmid_health; }

bool operator==(const PmidHealth& lhs, const PmidHealth& rhs) {
  return lhs.serialised_pmid_health == rhs.serialised_pmid_health;
}

void swap(PmidHealth& lhs, PmidHealth& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.serialised_pmid_health, rhs.serialised_pmid_health);
}

// ================================= MpidMessageBase =============================================

MpidMessageBase::MpidMessageBase()
    : sender(), receiver(), id(), parent_id(), signed_header() {}

MpidMessageBase::MpidMessageBase(const passport::PublicMpid::Name& sender_in,
                                 const passport::PublicMpid::Name& receiver_in,
                                 int32_t id_in,
                                 int32_t parent_id_in, const MessageHeaderType& signed_header_in)
    : sender(sender_in), receiver(receiver_in), id(id_in), parent_id(parent_id_in),
      signed_header(signed_header_in) {}

MpidMessageBase::MpidMessageBase(const std::string& serialised_copy) {
  protobuf::MpidMessageBase proto;
  if (!proto.ParseFromString(serialised_copy))
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));

  sender = passport::PublicMpid::Name(Identity(proto.sender()));
  receiver = passport::PublicMpid::Name(Identity(proto.receiver()));
  id = proto.id();
  parent_id = proto.parent_id();
  signed_header = MessageHeaderType(proto.signed_header());
}

MpidMessageBase::MpidMessageBase(const MpidMessageBase& other)
    : sender(other.sender), receiver(other.receiver), id(other.id), parent_id(other.parent_id),
      signed_header(other.signed_header) {}

MpidMessageBase::MpidMessageBase(MpidMessageBase&& other)
    : sender(std::move(other.sender)), receiver(std::move(other.receiver)), id(std::move(other.id)),
      parent_id(std::move(other.parent_id)), signed_header(std::move(other.signed_header)) {}

MpidMessageBase& MpidMessageBase::operator=(MpidMessageBase other) {
  swap(*this, other);
  return *this;
}

std::string MpidMessageBase::Serialise() const {
  protobuf::MpidMessageBase proto;
  proto.set_sender(sender->string());
  proto.set_receiver(receiver->string());
  proto.set_id(id);
  proto.set_parent_id(parent_id);
  proto.set_signed_header(signed_header.string());
  return proto.SerializeAsString();
}

bool operator==(const MpidMessageBase& lhs, const MpidMessageBase& rhs) {
  return (lhs.sender == rhs.sender) && (lhs.receiver == rhs.receiver) &&  (lhs.id == rhs.id) &&
         (lhs.parent_id == rhs.parent_id) && (lhs.signed_header == rhs.signed_header);
}

void swap(MpidMessageBase& lhs, MpidMessageBase& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.sender, rhs.sender);
  swap(lhs.receiver, rhs.receiver);
  swap(lhs.id, rhs.id);
  swap(lhs.parent_id, rhs.parent_id);
  swap(lhs.signed_header, rhs.signed_header);
}

// ================================= MpidMessageAlert =============================================

MpidMessageAlert::MpidMessageAlert(const MpidMessageBase& base_in,
                                   const MessageIdType& message_id_in)
    : base(base_in), message_id(message_id_in) {}

MpidMessageAlert::MpidMessageAlert(const std::string& serialised_copy) {
  protobuf::MpidMessageAlert proto;
  if (!proto.ParseFromString(serialised_copy))
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));

  base = MpidMessageBase(proto.serialised_base());
  message_id = MessageIdType(proto.message_id());
}

MpidMessageAlert::MpidMessageAlert(const MpidMessageAlert& other)
    : base(other.base), message_id(other.message_id){}

MpidMessageAlert::MpidMessageAlert(MpidMessageAlert&& other)
    : base(std::move(other.base)), message_id(std::move(other.message_id)) {}

MpidMessageAlert& MpidMessageAlert::operator=(MpidMessageAlert other) {
  swap(*this, other);
  return *this;
}

std::string MpidMessageAlert::Serialise() const {
  protobuf::MpidMessageAlert proto;
  proto.set_serialised_base(base.Serialise());
  proto.set_message_id(message_id.string());
  return proto.SerializeAsString();
}

bool operator==(const MpidMessageAlert& lhs, const MpidMessageAlert& rhs) {
  return (lhs.base == rhs.base) && (lhs.message_id == rhs.message_id);
}

void swap(MpidMessageAlert& lhs, MpidMessageAlert& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.base, rhs.base);
  swap(lhs.message_id, rhs.message_id);
}

// ================================= MpidMessage ==================================================

MpidMessage::MpidMessage()
    : base(), signed_body() {}

MpidMessage::MpidMessage(const MpidMessageBase& base_in, MessageBodyType& signed_body_in)
    : base(base_in), signed_body(signed_body_in) {}

MpidMessage::MpidMessage(const std::string& serialised_copy) {
  protobuf::MpidMessage proto;
  if (!proto.ParseFromString(serialised_copy))
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));

  base = MpidMessageBase(proto.serialised_base());
  signed_body = MessageBodyType(proto.signed_body());
}

MpidMessage::MpidMessage(const MpidMessage& other)
    : base(other.base), signed_body(other.signed_body){}

MpidMessage::MpidMessage(MpidMessage&& other)
    : base(std::move(other.base)), signed_body(std::move(other.signed_body)) {}

MpidMessage& MpidMessage::operator=(MpidMessage other) {
  swap(*this, other);
  return *this;
}

std::string MpidMessage::Serialise() const {
  protobuf::MpidMessage proto;
  proto.set_serialised_base(base.Serialise());
  proto.set_signed_body(signed_body.string());
  return proto.SerializeAsString();
}

bool operator==(const MpidMessage& lhs, const MpidMessage& rhs) {
  return (lhs.base == rhs.base) && (lhs.signed_body == rhs.signed_body);
}

void swap(MpidMessage& lhs, MpidMessage& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.base, rhs.base);
  swap(lhs.signed_body, rhs.signed_body);
}

}  // namespace nfs_vault

}  // namespace maidsafe
