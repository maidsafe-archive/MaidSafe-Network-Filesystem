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

#include "maidsafe/nfs/vault/maid_account_removal.h"

#include <utility>

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/vault/account_removal.pb.h"

namespace maidsafe {

namespace nfs_vault {

MaidAccountRemoval::MaidAccountRemoval() : random_data_(), public_anmaid_name_(), signature_() {}

MaidAccountRemoval::MaidAccountRemoval(const passport::Anmaid& anmaid)
    : random_data_(RandomString((RandomUint32() % 500) + 1000)),
      public_anmaid_name_(anmaid.name()),
      signature_(asymm::Sign(random_data_, anmaid.private_key())) {}

MaidAccountRemoval::MaidAccountRemoval(const std::string& serialised_copy)
    : random_data_(), public_anmaid_name_(), signature_() {
  protobuf::MaidAccountRemoval proto_account_removal;
  if (!proto_account_removal.ParseFromString(serialised_copy)) {
    LOG(kError) << "Failed to parse account_removal.";
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
  }

  random_data_ = NonEmptyString(proto_account_removal.random_data());
  public_anmaid_name_ =
      passport::PublicAnmaid::Name(Identity(proto_account_removal.public_anmaid_name()));
  signature_ = asymm::Signature(proto_account_removal.signature());
}

MaidAccountRemoval::MaidAccountRemoval(const MaidAccountRemoval& other)
    : random_data_(other.random_data_),
      public_anmaid_name_(other.public_anmaid_name_),
      signature_(other.signature_) {}

MaidAccountRemoval::MaidAccountRemoval(MaidAccountRemoval&& other)
    : random_data_(std::move(other.random_data_)),
      public_anmaid_name_(std::move(other.public_anmaid_name_)),
      signature_(std::move(other.signature_)) {}

MaidAccountRemoval& MaidAccountRemoval::operator=(MaidAccountRemoval other) {
  swap(*this, other);
  return *this;
}

std::string MaidAccountRemoval::Serialise() const {
  protobuf::MaidAccountRemoval proto_account_removal;
  proto_account_removal.set_random_data(random_data_.string());
  proto_account_removal.set_public_anmaid_name(public_anmaid_name_->string());
  proto_account_removal.set_signature(signature_.string());
  return proto_account_removal.SerializeAsString();
}

bool MaidAccountRemoval::Validate(const passport::PublicAnmaid& public_anmaid) const {
  return asymm::CheckSignature(random_data_, signature_, public_anmaid.public_key());
}

bool operator==(const MaidAccountRemoval& lhs, const MaidAccountRemoval& rhs) {
  return lhs.random_data_ == rhs.random_data_ &&
         lhs.public_anmaid_name_ == rhs.public_anmaid_name_ &&
         lhs.signature_ == rhs.signature_;
}

void swap(MaidAccountRemoval& lhs, MaidAccountRemoval& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.random_data_, rhs.random_data_);
  swap(lhs.public_anmaid_name_, rhs.public_anmaid_name_);
  swap(lhs.signature_, rhs.signature_);
}

}  // namespace nfs_vault

}  // namespace maidsafe
