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

#ifndef MAIDSAFE_NFS_VAULT_MPID_ACCOUNT_CREATION_H_
#define MAIDSAFE_NFS_VAULT_MPID_ACCOUNT_CREATION_H_

#include <memory>
#include <string>

#include "maidsafe/common/config.h"
#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"

namespace maidsafe {

namespace nfs_vault {

struct MpidAccountCreation {
  MpidAccountCreation();
  MpidAccountCreation(const passport::PublicMpid& public_mpid,
                      const passport::PublicAnmpid& public_anmpid);
  explicit MpidAccountCreation(const std::string& serialised_copy);
  MpidAccountCreation(const MpidAccountCreation& other);
  MpidAccountCreation(MpidAccountCreation&& other);

  passport::PublicMpid public_mpid() const { return *public_mpid_ptr; }
  passport::PublicAnmpid public_anmpid() const { return *public_anmpid_ptr; }

  MpidAccountCreation& operator=(MpidAccountCreation other);
  std::string Serialise() const;

  friend bool operator==(const MpidAccountCreation& lhs, const MpidAccountCreation& rhs);
  friend void swap(MpidAccountCreation& lhs, MpidAccountCreation& rhs) MAIDSAFE_NOEXCEPT;

 private:
  std::unique_ptr<passport::PublicMpid> public_mpid_ptr;
  std::unique_ptr<passport::PublicAnmpid> public_anmpid_ptr;
};

}  // namespace nfs_vault

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_VAULT_MPID_ACCOUNT_CREATION_H_
