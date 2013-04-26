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
    : key_getter_nfs_(public_pmids_from_file.empty() ? new KeyGetterNfs(routing) : nullptr)
#ifdef TESTING
      , kAllPmids_(public_pmids_from_file)
#endif
{
#ifndef TESTING
  if (!public_pmids_from_file.empty()) {
    LOG(kError) << "Cannot use fake key getter if TESTING is not defined";
    ThrowError(NfsErrors::invalid_parameter);
  }
#endif
}

template<>
void PublicKeyGetter::GetKey<passport::PublicPmid>(
    const typename passport::PublicPmid::name_type& key_name,
    std::function<void(Reply)> get_key_functor) {
#ifdef TESTING
  if (key_getter_nfs_) {
#endif
    auto get_op(std::make_shared<OperationOp>(1, get_key_functor));
    key_getter_nfs_->Get<passport::PublicPmid>(key_name,
                                               [get_op](std::string serialised_reply) {
                                                 HandleOperationReply(get_op, serialised_reply);
                                               });
#ifdef TESTING
  } else {
    auto itr(std::find_if(kAllPmids_.begin(), kAllPmids_.end(),
        [&key_name](const passport::PublicPmid& pmid) { return pmid.name() == key_name; }));
    if (itr == kAllPmids_.end()) {
      Reply reply(NfsErrors::failed_to_get_data);
      return get_key_functor(reply);
    }
    Reply reply(CommonErrors::success, (*itr).Serialise().data);
    get_key_functor(reply);
  }
#endif
}

}  // namespace nfs

}  // namespace maidsafe
