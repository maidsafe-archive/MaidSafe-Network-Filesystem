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

#ifndef MAIDSAFE_NFS_VAULT_MESSAGES_H_
#define MAIDSAFE_NFS_VAULT_MESSAGES_H_

#include <cstdint>
#include <string>
#include <vector>

#include "maidsafe/common/config.h"
#include "maidsafe/common/crypto.h"
#include "maidsafe/common/types.h"
#include "maidsafe/common/data_types/data_type_values.h"
#include "maidsafe/common/data_types/structured_data_versions.h"
#include "maidsafe/common/bounded_string.h"

#include "maidsafe/passport/types.h"

#include "maidsafe/nfs/vault/maid_account_creation.h"
#include "maidsafe/nfs/vault/maid_account_removal.h"
#include "maidsafe/nfs/vault/mpid_account_creation.h"
#include "maidsafe/nfs/vault/mpid_account_removal.h"
#include "maidsafe/nfs/vault/pmid_registration.h"


namespace maidsafe {

namespace nfs_vault {

const unsigned int kMaxHeaderSize = 128;
const unsigned int kMaxBodySize = 1024 * 1024;
const unsigned int kIdSize = 64;
using  MessageIdType = detail::BoundedString<kIdSize, kIdSize>;
using  MessageHeaderType = detail::BoundedString<0, kMaxHeaderSize>;
using  MessageBodyType = detail::BoundedString<0, kMaxBodySize>;

// ========================== Empty ================================================================

struct Empty {
  Empty() {}
  explicit Empty(const std::string&) {}
  std::string Serialise() const { return ""; }
};

bool operator==(const Empty& /*lhs*/, const Empty& /*rhs*/);

// ========================== AvailableSize =======================================================

struct AvailableSize {
  explicit AvailableSize(uint64_t size);
  AvailableSize(const AvailableSize& other);
  AvailableSize(AvailableSize&& other);
  AvailableSize& operator=(AvailableSize other);

  explicit AvailableSize(const std::string& serialised_copy);
  std::string Serialise() const;

  uint64_t available_size;
};

bool operator==(const AvailableSize& lhs, const AvailableSize& rhs);
void swap(AvailableSize& lhs, AvailableSize& rhs) MAIDSAFE_NOEXCEPT;

// ================================ DiffSize =======================================================

struct DiffSize {
  explicit DiffSize(int64_t size);
  DiffSize(const DiffSize& other);
  DiffSize(DiffSize&& other);
  DiffSize& operator=(DiffSize other);

  explicit DiffSize(const std::string& serialised_copy);
  std::string Serialise() const;

  int64_t diff_size;
};

bool operator==(const DiffSize& lhs, const DiffSize& rhs);
void swap(DiffSize& lhs, DiffSize& rhs) MAIDSAFE_NOEXCEPT;

// ========================== DataName =============================================================

struct DataName {
  template <typename DataNameType>
  explicit DataName(const DataNameType& data_name)
      : type(DataNameType::data_type::Tag::kValue), raw_name(data_name.value) {}

  DataName(DataTagValue type_in, Identity raw_name_in);

  DataName();
  DataName(const DataName& other);
  DataName(DataName&& other);
  DataName& operator=(DataName other);

  explicit DataName(const std::string& serialised_copy);
  std::string Serialise() const;

  DataTagValue type;
  Identity raw_name;
};

bool operator==(const DataName& lhs, const DataName& rhs);
bool operator<(const DataName& lhs, const DataName& rhs);
void swap(DataName& lhs, DataName& rhs) MAIDSAFE_NOEXCEPT;

// ==================================== DataNames ==================================================

struct DataNames {
  template <typename DataNameType>
  explicit DataNames(const std::vector<DataNameType>& data_names)
      : data_names_() {
    for (const auto& data_name : data_names)
      data_names_.push_back(DataName(data_name));
  }

  explicit DataNames(const std::vector<DataName>& data_names);
  DataNames();
  DataNames(const DataNames& other);
  DataNames(DataNames&& other);
  DataNames& operator=(DataNames other);

  explicit DataNames(const std::string& serialised_copy);
  std::string Serialise() const;

  std::vector<DataName> data_names_;
};

bool operator==(const DataNames& lhs, const DataNames& rhs);
void swap(DataNames& lhs, DataNames& rhs) MAIDSAFE_NOEXCEPT;

// ========================== DataNameAndVersion ===================================================

struct DataNameAndVersion {
  DataNameAndVersion();

  template <typename DataNameType>
  DataNameAndVersion(const DataNameType& data_name_in,
                     const StructuredDataVersions::VersionName& version_name_in)
      : data_name(data_name_in), version_name(version_name_in) {}

  DataNameAndVersion(const DataName& data_name_in,
                     const StructuredDataVersions::VersionName& version_name_in);
  DataNameAndVersion(const DataNameAndVersion& other);
  DataNameAndVersion(DataNameAndVersion&& other);
  DataNameAndVersion& operator=(DataNameAndVersion other);

  explicit DataNameAndVersion(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName data_name;
  StructuredDataVersions::VersionName version_name;
};

bool operator==(const DataNameAndVersion& lhs, const DataNameAndVersion& rhs);
void swap(DataNameAndVersion& lhs, DataNameAndVersion& rhs) MAIDSAFE_NOEXCEPT;

// ========================== DataNameOldNewVersion ================================================

struct DataNameOldNewVersion {
  DataNameOldNewVersion();

  template <typename DataNameType>
  DataNameOldNewVersion(const DataNameType& name,
                        const StructuredDataVersions::VersionName& old_version,
                        const StructuredDataVersions::VersionName& new_version)
      : data_name(name), old_version_name(old_version), new_version_name(new_version) {}

  DataNameOldNewVersion(const DataName& name,
                        const StructuredDataVersions::VersionName& old_version,
                        const StructuredDataVersions::VersionName& new_version);
  DataNameOldNewVersion(const DataNameOldNewVersion& other);
  DataNameOldNewVersion(DataNameOldNewVersion&& other);
  DataNameOldNewVersion& operator=(DataNameOldNewVersion other);

  explicit DataNameOldNewVersion(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName data_name;
  StructuredDataVersions::VersionName old_version_name, new_version_name;
};

bool operator==(const DataNameOldNewVersion& lhs, const DataNameOldNewVersion& rhs);
void swap(DataNameOldNewVersion& lhs, DataNameOldNewVersion& rhs) MAIDSAFE_NOEXCEPT;

// ========================== VersionTreeCreation ==================================================

struct VersionTreeCreation {
  template <typename DataNameType>
  VersionTreeCreation(const DataNameType& name, const StructuredDataVersions::VersionName& version,
                      uint32_t max_versions_in, uint32_t max_branches_in)
      : data_name(name), version_name(version), max_versions(max_versions_in),
        max_branches(max_branches_in) {}

  VersionTreeCreation(const DataName& name, const StructuredDataVersions::VersionName& version,
                      uint32_t max_versions_in, uint32_t max_branches_in);
  VersionTreeCreation(const VersionTreeCreation& other);
  VersionTreeCreation(VersionTreeCreation&& other);
  VersionTreeCreation& operator=(VersionTreeCreation other);

  explicit VersionTreeCreation(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName data_name;
  StructuredDataVersions::VersionName version_name;
  uint32_t max_versions, max_branches;
};

bool operator==(const VersionTreeCreation& lhs, const VersionTreeCreation& rhs);
void swap(VersionTreeCreation& lhs, VersionTreeCreation& rhs) MAIDSAFE_NOEXCEPT;

// ========================== DataNameAndContent ===================================================

struct DataNameAndContent {
  template <typename Data>
  explicit DataNameAndContent(const Data& data)
      : name(data.name()), content(data.Serialise().data) {}

  DataNameAndContent(DataTagValue type_in, const Identity& name_in, NonEmptyString content_in);

  DataNameAndContent();
  DataNameAndContent(const DataNameAndContent& other);
  DataNameAndContent(DataNameAndContent&& other);
  DataNameAndContent& operator=(DataNameAndContent other);

  explicit DataNameAndContent(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName name;
  NonEmptyString content;
};

bool operator==(const DataNameAndContent& lhs, const DataNameAndContent& rhs);
void swap(DataNameAndContent& lhs, DataNameAndContent& rhs) MAIDSAFE_NOEXCEPT;

// ========================== Content ==============================================================

struct Content {
  explicit Content(const std::string& data);
  Content();
  Content(const Content& other);
  Content(Content&& other);
  Content& operator=(Content other);
  std::string Serialise() const;

  std::string data;
};

bool operator==(const Content& lhs, const Content& rhs);
void swap(Content& lhs, Content& rhs) MAIDSAFE_NOEXCEPT;

// ========================== DataNameAndRandomString ==============================================

struct DataNameAndRandomString {
  template <typename DataNameType>
  DataNameAndRandomString(DataNameType name, NonEmptyString random_string_in)
      : name(std::move(name)), random_string(std::move(random_string_in)) {}

  DataNameAndRandomString(DataTagValue type_in, const Identity& name_in,
                          NonEmptyString random_string_in);

  DataNameAndRandomString();
  DataNameAndRandomString(const DataNameAndRandomString& other);
  DataNameAndRandomString(DataNameAndRandomString&& other);
  DataNameAndRandomString& operator=(DataNameAndRandomString other);

  explicit DataNameAndRandomString(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName name;
  NonEmptyString random_string;
};

bool operator==(const DataNameAndRandomString& lhs, const DataNameAndRandomString& rhs);
void swap(DataNameAndRandomString& lhs, DataNameAndRandomString& rhs) MAIDSAFE_NOEXCEPT;

// ========================== DataNameAndCost ======================================================

struct DataNameAndCost {
  template <typename DataNameType>
  DataNameAndCost(const DataNameType& data_name, int32_t cost_in)
      : name(data_name), cost(cost_in) {}
  DataNameAndCost(DataTagValue type_in, const Identity& name_in, int32_t cost_in);

  DataNameAndCost();
  DataNameAndCost(const DataNameAndCost& other);
  DataNameAndCost(DataNameAndCost&& other);
  DataNameAndCost& operator=(DataNameAndCost other);

  explicit DataNameAndCost(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName name;
  int32_t cost;
};

bool operator==(const DataNameAndCost& lhs, const DataNameAndCost& rhs);
void swap(DataNameAndCost& lhs, DataNameAndCost& rhs) MAIDSAFE_NOEXCEPT;

// ========================== DataNameAndSize ======================================================

struct DataNameAndSize {
  template <typename DataNameType>
  DataNameAndSize(const DataNameType& data_name, int32_t size_in)
      : name(data_name), size(size_in) {}
  DataNameAndSize(DataTagValue type_in, const Identity& name_in, int32_t size_in);
  DataNameAndSize();
  DataNameAndSize(const DataNameAndSize& other);
  DataNameAndSize(DataNameAndSize&& other);
  DataNameAndSize& operator=(DataNameAndSize other);

  explicit DataNameAndSize(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName name;
  int32_t size;
};

bool operator==(const DataNameAndSize& lhs, const DataNameAndSize& rhs);
void swap(DataNameAndSize& lhs, DataNameAndSize& rhs) MAIDSAFE_NOEXCEPT;

// ========================== DataNameAndContentOrCheckResult ======================================

struct DataNameAndContentOrCheckResult {
  typedef crypto::SHA512Hash CheckResult;

  template <typename DataNameType>
  DataNameAndContentOrCheckResult(const DataNameType& name_in, const NonEmptyString& content_in)
      : name(name_in), content(content_in), check_result() {}

  template <typename DataNameType>
  DataNameAndContentOrCheckResult(const DataNameType& name_in, const CheckResult& check_result_in)
      : name(name_in), content(), check_result(check_result_in) {}

  DataNameAndContentOrCheckResult(const DataTagValue& type_in, const Identity& name_in,
                                  const NonEmptyString& content_in);
  DataNameAndContentOrCheckResult(const DataTagValue& type_in, const Identity& name_in,
                                  const CheckResult& check_result_in);
  DataNameAndContentOrCheckResult();
  DataNameAndContentOrCheckResult(const DataNameAndContentOrCheckResult& other);
  DataNameAndContentOrCheckResult(DataNameAndContentOrCheckResult&& other);
  DataNameAndContentOrCheckResult& operator=(DataNameAndContentOrCheckResult other);

  explicit DataNameAndContentOrCheckResult(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName name;
  boost::optional<NonEmptyString> content;
  boost::optional<CheckResult> check_result;
};

void swap(DataNameAndContentOrCheckResult& lhs,
          DataNameAndContentOrCheckResult& rhs) MAIDSAFE_NOEXCEPT;
bool operator==(const DataNameAndContentOrCheckResult& lhs,
                const DataNameAndContentOrCheckResult& rhs);

// ================================= PmidHealth ===================================================

struct PmidHealth {
  PmidHealth();
  explicit PmidHealth(const std::string& serialised_copy);
  PmidHealth(const PmidHealth& other);
  PmidHealth(PmidHealth&& other);
  PmidHealth& operator=(PmidHealth other);

  std::string Serialise() const;

  std::string serialised_pmid_health;
};

bool operator==(const PmidHealth& lhs, const PmidHealth& rhs);
void swap(PmidHealth& lhs, PmidHealth& rhs) MAIDSAFE_NOEXCEPT;

// ================================= MpidMessageBase =============================================

struct MpidMessageBase {
  MpidMessageBase();
  MpidMessageBase(const passport::PublicMpid::Name& sender_in,
                  const passport::PublicMpid::Name& receiver_in, int32_t id_in,
                  int32_t parent_id_in, const MessageHeaderType& signed_header_in);
  explicit MpidMessageBase(const std::string& serialised_copy);
  MpidMessageBase(const MpidMessageBase& other);
  MpidMessageBase(MpidMessageBase&& other);
  MpidMessageBase& operator=(MpidMessageBase other);

  std::string Serialise() const;

  passport::PublicMpid::Name sender, receiver;
  int32_t id, parent_id;
  MessageHeaderType signed_header;
};

bool operator==(const MpidMessageBase& lhs, const MpidMessageBase& rhs);
void swap(MpidMessageBase& lhs, MpidMessageBase& rhs) MAIDSAFE_NOEXCEPT;

// ================================= MpidMessageAlert =============================================

struct MpidMessageAlert {
  MpidMessageAlert();
  MpidMessageAlert(const MpidMessageBase& base_in,
                   const MessageIdType& message_id_in);
  explicit MpidMessageAlert(const std::string& serialised_copy);
  MpidMessageAlert(const MpidMessageAlert& other);
  MpidMessageAlert(MpidMessageAlert&& other);
  MpidMessageAlert& operator=(MpidMessageAlert other);

  std::string Serialise() const;

  MpidMessageBase base;
  MessageIdType message_id;
};

bool operator==(const MpidMessageAlert& lhs, const MpidMessageAlert& rhs);
void swap(MpidMessageAlert& lhs, MpidMessageAlert& rhs) MAIDSAFE_NOEXCEPT;

// ================================= MpidMessage ==================================================

struct MpidMessage {
  MpidMessage();
  MpidMessage(const MpidMessageBase& base_in, MessageBodyType& signed_body_in);
  explicit MpidMessage(const std::string& serialised_copy);
  MpidMessage(const MpidMessage& other);
  MpidMessage(MpidMessage&& other);
  MpidMessage& operator=(MpidMessage other);

  std::string Serialise() const;

  MpidMessageBase base;
  MessageBodyType signed_body;
};

bool operator==(const MpidMessage& lhs, const MpidMessage& rhs);
void swap(MpidMessage& lhs, MpidMessage& rhs) MAIDSAFE_NOEXCEPT;

}  // namespace nfs_vault

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_VAULT_MESSAGES_H_
