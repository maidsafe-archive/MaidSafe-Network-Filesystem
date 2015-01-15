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

#ifndef MAIDSAFE_NFS_VAULT_MAID_ACCOUNT_REMOVAL_H_
#define MAIDSAFE_NFS_VAULT_MAID_ACCOUNT_REMOVAL_H_

#include <string>

#include "maidsafe/common/config.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"

namespace maidsafe {

namespace nfs_vault {

class MaidAccountRemoval {
 public:
  MaidAccountRemoval();
  explicit MaidAccountRemoval(const passport::Anmaid& anmaid);
  explicit MaidAccountRemoval(const std::string& serialised_copy);
  MaidAccountRemoval(const MaidAccountRemoval& other);
  MaidAccountRemoval(MaidAccountRemoval&& other);

  MaidAccountRemoval& operator=(MaidAccountRemoval other);
  std::string Serialise() const;

  passport::PublicAnmaid::Name public_anmaid_name() const { return public_anmaid_name_; }
  bool Validate(const passport::PublicAnmaid& public_anmaid) const;

  friend bool operator==(const MaidAccountRemoval& lhs, const MaidAccountRemoval& rhs);
  friend void swap(MaidAccountRemoval& lhs, MaidAccountRemoval& rhs) MAIDSAFE_NOEXCEPT;

 private:
  NonEmptyString random_data_;
  passport::PublicAnmaid::Name public_anmaid_name_;
  asymm::Signature signature_;
};

}  // namespace nfs_vault

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_VAULT_MAID_ACCOUNT_REMOVAL_H_
