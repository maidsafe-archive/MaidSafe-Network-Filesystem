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

#ifndef MAIDSAFE_NFS_VAULT_MPID_ACCOUNT_REMOVAL_H_
#define MAIDSAFE_NFS_VAULT_MPID_ACCOUNT_REMOVAL_H_

#include <string>

#include "maidsafe/common/config.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"

namespace maidsafe {

namespace nfs_vault {

class MpidAccountRemoval {
 public:
  MpidAccountRemoval();
  explicit MpidAccountRemoval(const passport::Anmpid& anmpid);
  explicit MpidAccountRemoval(const std::string& serialised_copy);
  MpidAccountRemoval(const MpidAccountRemoval& other);
  MpidAccountRemoval(MpidAccountRemoval&& other);

  MpidAccountRemoval& operator=(MpidAccountRemoval other);
  std::string Serialise() const;

  passport::PublicAnmpid::Name public_anmpid_name() const { return public_anmpid_name_; }
  bool Validate(const passport::PublicAnmpid& public_anmpid) const;

  friend bool operator==(const MpidAccountRemoval& lhs, const MpidAccountRemoval& rhs);
  friend void swap(MpidAccountRemoval& lhs, MpidAccountRemoval& rhs) MAIDSAFE_NOEXCEPT;

 private:
  NonEmptyString random_data_;
  passport::PublicAnmpid::Name public_anmpid_name_;
  asymm::Signature signature_;
};

}  // namespace nfs_vault

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_VAULT_MPID_ACCOUNT_REMOVAL_H_
