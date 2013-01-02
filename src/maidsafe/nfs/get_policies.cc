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

#include "maidsafe/nfs/get_policies.h"

namespace maidsafe {

namespace nfs {

#ifdef TESTING

std::future<passport::PublicPmid> GetFromKeyFile::Get(const passport::PublicPmid::name_type& name) {
  std::promise<passport::PublicPmid> promise;
  try {
    auto itr(std::find_if(kAllPmids_.begin(),
                          kAllPmids_.end(),
                          [&name](const passport::Pmid& pmid) { return pmid.name() == name; }));  // NOLINT (Fraser)
    if (itr == kAllPmids_.end())
      ThrowError(NfsErrors::failed_to_get_data);
    promise.set_value(passport::PublicPmid(*itr));
  }
  catch(...) {
    promise.set_exception(std::current_exception());
  }
  return promise.get_future();
}

#endif

}  // namespace nfs

}  // namespace maidsafe
