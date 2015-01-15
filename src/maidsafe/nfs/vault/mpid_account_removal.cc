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

#include "maidsafe/nfs/vault/mpid_account_removal.h"

#include <utility>

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/vault/account_removal.pb.h"

namespace maidsafe {

namespace nfs_vault {

MpidAccountRemoval::MpidAccountRemoval() : random_data_(), public_anmpid_name_(), signature_() {}

MpidAccountRemoval::MpidAccountRemoval(const passport::Anmpid& anmpid)
    : random_data_(RandomString((RandomUint32() % 500) + 1000)),
      public_anmpid_name_(anmpid.name()),
      signature_(asymm::Sign(random_data_, anmpid.private_key())) {}

MpidAccountRemoval::MpidAccountRemoval(const std::string& serialised_copy)
    : random_data_(), public_anmpid_name_(), signature_() {
  protobuf::MpidAccountRemoval proto_account_removal;
  if (!proto_account_removal.ParseFromString(serialised_copy)) {
    LOG(kError) << "Failed to parse account_removal.";
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
  }

  random_data_ = NonEmptyString(proto_account_removal.random_data());
  public_anmpid_name_ =
      passport::PublicAnmpid::Name(Identity(proto_account_removal.public_anmpid_name()));
  signature_ = asymm::Signature(proto_account_removal.signature());
}

MpidAccountRemoval::MpidAccountRemoval(const MpidAccountRemoval& other)
    : random_data_(other.random_data_),
      public_anmpid_name_(other.public_anmpid_name_),
      signature_(other.signature_) {}

MpidAccountRemoval::MpidAccountRemoval(MpidAccountRemoval&& other)
    : random_data_(std::move(other.random_data_)),
      public_anmpid_name_(std::move(other.public_anmpid_name_)),
      signature_(std::move(other.signature_)) {}

MpidAccountRemoval& MpidAccountRemoval::operator=(MpidAccountRemoval other) {
  swap(*this, other);
  return *this;
}

std::string MpidAccountRemoval::Serialise() const {
  protobuf::MpidAccountRemoval proto_account_removal;
  proto_account_removal.set_random_data(random_data_.string());
  proto_account_removal.set_public_anmpid_name(public_anmpid_name_->string());
  proto_account_removal.set_signature(signature_.string());
  return proto_account_removal.SerializeAsString();
}

bool MpidAccountRemoval::Validate(const passport::PublicAnmpid& public_anmpid) const {
  return asymm::CheckSignature(random_data_, signature_, public_anmpid.public_key());
}

bool operator==(const MpidAccountRemoval& lhs, const MpidAccountRemoval& rhs) {
  return lhs.random_data_ == rhs.random_data_ &&
         lhs.public_anmpid_name_ == rhs.public_anmpid_name_ &&
         lhs.signature_ == rhs.signature_;
}

void swap(MpidAccountRemoval& lhs, MpidAccountRemoval& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.random_data_, rhs.random_data_);
  swap(lhs.public_anmpid_name_, rhs.public_anmpid_name_);
  swap(lhs.signature_, rhs.signature_);
}

}  // namespace nfs_vault

}  // namespace maidsafe
