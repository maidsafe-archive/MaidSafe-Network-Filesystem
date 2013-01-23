/***************************************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use.  The use of this code is governed by the licence file licence.txt found in the root of    *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit         *
 *  written permission of the board of directors of MaidSafe.net.                                  *
 **************************************************************************************************/

#ifndef MAIDSAFE_NFS_PMID_REGISTRATION_H_
#define MAIDSAFE_NFS_PMID_REGISTRATION_H_

#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"


namespace maidsafe {

namespace nfs {

class PmidRegistration {
 public:
  typedef TaggedValue<NonEmptyString, struct SerialisedPmidRegistrationTag> serialised_type;
  PmidRegistration(const passport::Maid& maid,
                   const passport::Pmid& pmid,
                   bool unregister);
  explicit PmidRegistration(const serialised_type& serialised_pmid_registration);
  bool Validate(const passport::PublicMaid& public_maid,
                const passport::PublicPmid& public_pmid) const;
  serialised_type Serialise() const;
  passport::PublicMaid::name_type maid_name() const { return maid_name_; }
  passport::PublicPmid::name_type pmid_name() const { return pmid_name_; }
  bool unregister() const { return unregister_; }

 private:
  passport::PublicMaid::name_type maid_name_;
  passport::PublicPmid::name_type pmid_name_;
  bool unregister_;
  asymm::Signature maid_signature_;
  asymm::Signature pmid_signature_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_PMID_REGISTRATION_H_
