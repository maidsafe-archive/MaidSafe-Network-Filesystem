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

#ifndef MAIDSAFE_NFS_VAULT_PMID_REGISTRATION_H_
#define MAIDSAFE_NFS_VAULT_PMID_REGISTRATION_H_

#include <string>

#include "maidsafe/common/config.h"
#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"

namespace maidsafe {

namespace nfs_vault {

class PmidRegistration {
 public:
  PmidRegistration();
  PmidRegistration(const passport::Maid& maid, const passport::Pmid& pmid, bool unregister);
  explicit PmidRegistration(const std::string& serialised_copy);
  PmidRegistration(const PmidRegistration& other);
  PmidRegistration(PmidRegistration&& other);
  PmidRegistration& operator=(PmidRegistration other);
  bool Validate(const passport::PublicMaid& public_maid,
                const passport::PublicPmid& public_pmid) const;
  std::string Serialise() const;
  passport::PublicMaid::Name maid_name() const { return maid_name_; }
  passport::PublicPmid::Name pmid_name() const { return pmid_name_; }
  bool unregister() const { return unregister_; }

  friend bool operator==(const PmidRegistration& lhs, const PmidRegistration& rhs);
  friend void swap(PmidRegistration& lhs, PmidRegistration& rhs) MAIDSAFE_NOEXCEPT;

 private:
  passport::PublicMaid::Name maid_name_;
  passport::PublicPmid::Name pmid_name_;
  bool unregister_;
  asymm::Signature maid_signature_;
  asymm::Signature pmid_signature_;
};

bool operator==(const PmidRegistration& lhs, const PmidRegistration& rhs);
void swap(PmidRegistration& lhs, PmidRegistration& rhs) MAIDSAFE_NOEXCEPT;

}  // namespace nfs_vault

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_VAULT_PMID_REGISTRATION_H_
