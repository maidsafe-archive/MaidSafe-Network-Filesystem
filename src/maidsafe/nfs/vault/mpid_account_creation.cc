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

#include "maidsafe/nfs/vault/mpid_account_creation.h"

#include <utility>

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/vault/account_creation.pb.h"

namespace maidsafe {

namespace nfs_vault {

MpidAccountCreation::MpidAccountCreation() : public_mpid_ptr(), public_anmpid_ptr() {}

MpidAccountCreation::MpidAccountCreation(const passport::PublicMpid& public_mpid,
                                         const passport::PublicAnmpid& public_anmpid)
    : public_mpid_ptr(new passport::PublicMpid(public_mpid)),
      public_anmpid_ptr(new passport::PublicAnmpid(public_anmpid)) {}

MpidAccountCreation::MpidAccountCreation(const std::string& serialised_copy)
    : public_mpid_ptr(), public_anmpid_ptr() {
  protobuf::MpidAccountCreation proto_account_creation;
  if (!proto_account_creation.ParseFromString(serialised_copy)) {
    LOG(kError) << "Failed to parse account_creation.";
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
  }

  public_mpid_ptr.reset(new passport::PublicMpid(
      passport::PublicMpid::Name(Identity(proto_account_creation.public_mpid_name())),
      passport::PublicMpid::serialised_type(NonEmptyString(proto_account_creation.public_mpid()))));
  public_anmpid_ptr.reset(new passport::PublicAnmpid(
      passport::PublicAnmpid::Name(Identity(proto_account_creation.public_anmpid_name())),
      passport::PublicAnmpid::serialised_type(
          NonEmptyString(proto_account_creation.public_anmpid()))));
}

MpidAccountCreation::MpidAccountCreation(const MpidAccountCreation& other)
    : public_mpid_ptr(new passport::PublicMpid(*other.public_mpid_ptr)),
      public_anmpid_ptr(new passport::PublicAnmpid(*other.public_anmpid_ptr)) {}

MpidAccountCreation::MpidAccountCreation(MpidAccountCreation&& other)
    : public_mpid_ptr(std::move(other.public_mpid_ptr)),
      public_anmpid_ptr(std::move(other.public_anmpid_ptr)) {}

MpidAccountCreation& MpidAccountCreation::operator=(MpidAccountCreation other) {
  swap(*this, other);
  return *this;
}

std::string MpidAccountCreation::Serialise() const {
  protobuf::MpidAccountCreation proto_account_creation;
  proto_account_creation.set_public_mpid_name(public_mpid_ptr->name().value.string());
  proto_account_creation.set_public_mpid(public_mpid_ptr->Serialise()->string());
  proto_account_creation.set_public_anmpid_name(public_anmpid_ptr->name().value.string());
  proto_account_creation.set_public_anmpid(public_anmpid_ptr->Serialise()->string());
  return proto_account_creation.SerializeAsString();
}

bool operator==(const MpidAccountCreation& lhs, const MpidAccountCreation& rhs) {
  bool result(
      lhs.public_mpid_ptr->name() == rhs.public_mpid_ptr->name() &&
      lhs.public_anmpid_ptr->name() == rhs.public_anmpid_ptr->name() &&
      lhs.public_mpid_ptr->validation_token() == rhs.public_mpid_ptr->validation_token() &&
      lhs.public_anmpid_ptr->validation_token() == rhs.public_anmpid_ptr->validation_token());
  // If result is true, the public keys should also be equal, since the names are derived from the
  // public keys.
  assert(!result || (rsa::MatchingKeys(lhs.public_mpid_ptr->public_key(),
                                       rhs.public_mpid_ptr->public_key()) &&
                     rsa::MatchingKeys(lhs.public_anmpid_ptr->public_key(),
                                       rhs.public_anmpid_ptr->public_key())));
  return result;
}

void swap(MpidAccountCreation& lhs, MpidAccountCreation& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.public_mpid_ptr, rhs.public_mpid_ptr);
  swap(lhs.public_anmpid_ptr, rhs.public_anmpid_ptr);
}

}  // namespace nfs_vault

}  // namespace maidsafe
