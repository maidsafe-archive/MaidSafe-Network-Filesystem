/* Copyright 2012 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

#ifndef MAIDSAFE_NFS_PMID_REGISTRATION_H_
#define MAIDSAFE_NFS_PMID_REGISTRATION_H_

#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"


namespace maidsafe {

namespace nfs {

class PmidRegistration {
 public:
  typedef TaggedValue<NonEmptyString, struct SerialisedPmidRegistrationTag> serialised_type;
  PmidRegistration(const passport::Maid& maid, const passport::Pmid& pmid, bool unregister);
  explicit PmidRegistration(const serialised_type& serialised_pmid_registration);
  PmidRegistration(const PmidRegistration& other);
  PmidRegistration(PmidRegistration&& other);
  PmidRegistration& operator=(PmidRegistration other);
  bool Validate(const passport::PublicMaid& public_maid,
                const passport::PublicPmid& public_pmid) const;
  serialised_type Serialise() const;
  passport::PublicMaid::name_type maid_name() const { return maid_name_; }
  passport::PublicPmid::name_type pmid_name() const { return pmid_name_; }
  bool unregister() const { return unregister_; }

  friend void swap(PmidRegistration& lhs, PmidRegistration& rhs);

 private:
  PmidRegistration();
  passport::PublicMaid::name_type maid_name_;
  passport::PublicPmid::name_type pmid_name_;
  bool unregister_;
  asymm::Signature maid_signature_;
  asymm::Signature pmid_signature_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_PMID_REGISTRATION_H_
