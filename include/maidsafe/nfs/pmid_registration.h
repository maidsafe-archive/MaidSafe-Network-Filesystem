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


namespace maidsafe {

namespace nfs {

class PmidRegistration {
 public:
  PmidRegistration(Identity maid_id_in,
                   Identity pmid_id_in,
                   bool register_in,
                   NonEmptyString maid_signature_in,
                   NonEmptyString pmid_signature_in)
      : maid_id_(maid_id_in),
        pmid_id_(pmid_id_in),
        register_(register_in),
        maid_signature_(maid_signature_in),
        pmid_signature_(pmid_signature_in) {}
  explicit PmidRegistration(const NonEmptyString& serialised_pmid_registration)
      : maid_id_(),
        pmid_id_(),
        register_(false),
        maid_signature_(NonEmptyString()),
        pmid_signature_(NonEmptyString()) {
    Parse(serialised_pmid_registration);
  }

  void Parse(const NonEmptyString& serialised_pmid_registration);
  NonEmptyString Serialise();
  Identity maid_id() const { return maid_id_; }
  Identity pmid_id() const { return pmid_id_; }

 private:
  Identity maid_id_;
  Identity pmid_id_;
  bool register_;
  NonEmptyString maid_signature_;
  NonEmptyString pmid_signature_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_PMID_REGISTRATION_H_
