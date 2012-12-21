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

#ifndef MAIDSAFE_NFS_NFS_H_
#define MAIDSAFE_NFS_NFS_H_

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/types.h"
#include "maidsafe/nfs/get_policies.h"
#include "maidsafe/nfs/put_policies.h"
#include "maidsafe/nfs/post_policies.h"
#include "maidsafe/nfs/delete_policies.h"


namespace maidsafe {

namespace nfs {

template<typename GetPolicy,
         typename PutPolicy,
         typename PostPolicy,
         typename DeletePolicy>
class NetworkFileSystem : public GetPolicy,
                          public PutPolicy,
                          public PostPolicy,
                          public DeletePolicy {
 public:
  explicit NetworkFileSystem(routing::Routing& routing) : routing_(routing) {}

  template<typename Data>
  std::future<Data> Get(const Data::name_type& name) {
    return GetPolicy::Get(name, routing_);
  }

  // TODO(Fraser#5#): 2012-12-21 - The signing fob type needs to be fixed - make class template?
  template<typename Data>
  void Put(const Data& data, const OnError& on_error, const Data::signer_type& signing_fob) {
    PutPolicy::Put(data, signing_fob, on_error, routing_);
  }

  template<typename Data>
  void Post(Identity name, Data message, action_callback callback) {
    PostPolicy::Post(name, message, callback);
  }

  template<typename Data>
  void Delete(const Data& data, action_callback callback) {
    DeletePolicy::Delete(name, data, callback);
  }

 private:
  Routing routing_;
};

typedef NetworkFileSystem<GetFromDataHolder, PutToDataHolder, NoPost, NoDelete> ClientMaidNfs;

#ifdef TESTING
typedef NetworkFileSystem<GetFromKeyFile,
                          NoPut,
                          NoPost,
                          NoDelete> KeyHelperNfs;
#endif

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_NFS_H_
