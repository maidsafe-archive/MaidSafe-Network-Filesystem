/*  Copyright 2012 MaidSafe.net limited

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

#include "maidsafe/nfs/vault/account_creation.h"

#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/vault/account_creation.pb.h"

namespace maidsafe {

namespace nfs_vault {


AccountCreation::AccountCreation()
    : public_maid_ptr(), public_anmaid_ptr() {}

AccountCreation::AccountCreation(const passport::PublicMaid& public_maid,
                                 const passport::PublicAnmaid& public_anmaid)
    : public_maid_ptr(new passport::PublicMaid(public_maid)),
      public_anmaid_ptr(new passport::PublicAnmaid(public_anmaid)) {}

AccountCreation::AccountCreation(const std::string& serialised_copy)
    : public_maid_ptr(),
      public_anmaid_ptr() {
  protobuf::AccountCreation proto_account_creation;
  if (!proto_account_creation.ParseFromString(serialised_copy)) {
    LOG(kError) << "Failed to parse account_creation.";
    ThrowError(CommonErrors::parsing_error);
  }

  public_maid_ptr.reset(new passport::PublicMaid(
      passport::PublicMaid::Name(Identity(proto_account_creation.public_maid_name())),
      passport::PublicMaid::serialised_type(NonEmptyString(proto_account_creation.public_maid()))));
  public_anmaid_ptr.reset(new passport::PublicAnmaid(
      passport::PublicAnmaid::Name(Identity(proto_account_creation.public_anmaid_name())),
      passport::PublicAnmaid::serialised_type(NonEmptyString(proto_account_creation.public_anmaid()))));
}

AccountCreation::AccountCreation(const AccountCreation& other)
    : public_maid_ptr(new passport::PublicMaid(*other.public_maid_ptr)),
      public_anmaid_ptr(new passport::PublicAnmaid(*other.public_anmaid_ptr)) {}

AccountCreation::AccountCreation(AccountCreation&& other)
    : public_maid_ptr(std::move(other.public_maid_ptr)),
      public_anmaid_ptr(std::move(other.public_anmaid_ptr)) {}

AccountCreation& AccountCreation::operator=(AccountCreation other) {
  swap(*this, other);
  return *this;
}

passport::PublicMaid AccountCreation::public_maid() {
  return *public_maid_ptr;
}

passport::PublicAnmaid AccountCreation::public_anmaid() {
  return *public_anmaid_ptr;
}

std::string AccountCreation::Serialise() const {
  protobuf::AccountCreation proto_account_creation;
  proto_account_creation.set_public_maid_name(public_maid_ptr->name().value.string());
  proto_account_creation.set_public_maid(public_maid_ptr->Serialise()->string());
  proto_account_creation.set_public_anmaid_name(public_anmaid_ptr->name().value.string());
  proto_account_creation.set_public_anmaid(public_anmaid_ptr->Serialise()->string());
  return proto_account_creation.SerializeAsString();
}

bool operator==(const AccountCreation& lhs, const AccountCreation& rhs) {
  bool result(
      lhs.public_maid_ptr->name() == rhs.public_maid_ptr->name() &&
      lhs.public_anmaid_ptr->name() == rhs.public_anmaid_ptr->name() &&
      lhs.public_maid_ptr->validation_token() == rhs.public_maid_ptr->validation_token() &&
      lhs.public_anmaid_ptr->validation_token() == rhs.public_anmaid_ptr->validation_token());
  // If result is true, the public keys should also be equal, since the names are derived from the
  // public keys.
  assert(!result || (rsa::MatchingKeys(lhs.public_maid_ptr->public_key(),
                                       rhs.public_maid_ptr->public_key()) &&
                     rsa::MatchingKeys(lhs.public_anmaid_ptr->public_key(),
                                       rhs.public_anmaid_ptr->public_key())));
  return result;
}

void swap(AccountCreation& lhs, AccountCreation& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.public_maid_ptr, rhs.public_maid_ptr);
  swap(lhs.public_anmaid_ptr, rhs.public_anmaid_ptr);
}

}  // namespace nfs_vault

}  // namespace maidsafe
