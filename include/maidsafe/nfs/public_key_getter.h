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

#ifndef MAIDSAFE_NFS_PUBLIC_KEY_GETTER_H_
#define MAIDSAFE_NFS_PUBLIC_KEY_GETTER_H_

#include <string>
#include <vector>

#include "maidsafe/common/utils.h"
#include "maidsafe/passport/types.h"
#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/nfs.h"
#include "maidsafe/nfs/reply.h"

namespace maidsafe {

namespace nfs {

class PublicKeyGetter {
 public:
  // all_pmids_from_file should only be non-empty if TESTING is defined
  PublicKeyGetter(routing::Routing& routing,
                  const std::vector<passport::PublicPmid>& public_pmids_from_file =
                      std::vector<passport::PublicPmid>());

  template<typename Data>
  void GetKey(const typename Data::name_type& key_name,
              std::function<void(Reply)> get_key_functor);

 private:
  std::unique_ptr<KeyGetterNfs> key_getter_nfs_;
#ifdef TESTING
  std::vector<passport::PublicPmid> kAllPmids_;
#endif
};

template<typename Data>
void PublicKeyGetter::GetKey(const typename Data::name_type& key_name,
                             std::function<void(Reply)> get_key_functor) {
  static_assert(passport::is_public_key_type<Data>::value,
                "Error, type must be a PublicKey type");
#ifdef TESTING
  if (key_getter_nfs_) {
#endif
    auto get_op(std::make_shared<OperationOp>(1, get_key_functor));
    key_getter_nfs_->Get<Data>(key_name,
                               [get_op](std::string serialised_reply) {
                                 HandleOperationReply(get_op, serialised_reply);
                               });
#ifdef TESTING
  } else {
    auto itr(std::find_if(kAllPmids_.begin(),
                          kAllPmids_.end(),
                          [&key_name] (const passport::PublicPmid& pmid) {
                            return pmid.name() == key_name;
                          }));
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

#endif  // MAIDSAFE_NFS_PUBLIC_KEY_GETTER_H_
