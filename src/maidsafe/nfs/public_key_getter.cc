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

#include "maidsafe/nfs/public_key_getter.h"

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"


namespace maidsafe {

namespace nfs {

PublicKeyGetter::PublicKeyGetter(routing::Routing& routing,
                                 const std::vector<passport::PublicPmid>& public_pmids_from_file)
    : key_getter_nfs_(public_pmids_from_file.empty() ? new KeyGetterNfs(routing) : nullptr),
      fake_key_getter_nfs_(public_pmids_from_file.empty() ?
                           nullptr : new FakeKeyGetterNfs(public_pmids_from_file)) {
#ifndef TESTING
  if (!public_pmids_from_file.empty()) {
    LOG(kError) << "Cannot use fake key getter if TESTING is not defined";
    ThrowError(NfsErrors::invalid_parameter);
  }
#endif
}

}  // namespace nfs

}  // namespace maidsafe
