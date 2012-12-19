/***************************************************************************************************
 *  Copyright 2012 maidsafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use. The use of this code is governed by the licence file licence.txt found in the root of     *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit written *
 *  permission of the board of directors of MaidSafe.net.                                          *
 **************************************************************************************************/

#ifndef MAIDSAFE_NFS_NFS_H_
#define MAIDSAFE_NFS_NFS_H_

#include <functional>

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/crypto.h"
#include "maidsafe/common/types.h"
// #include "maidsafe/private/data_types/fob.h"
#include "maidsafe/passport/types.h"
#include "maidsafe/data_types/data_types.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/nfs/type_traits.h"
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
class NetworkFileSystem :
    public GetPolicy, public PutPolicy, public PostPolicy, public DeletePolicy {
 public:
  explicit Nfs(Routing routing) : routing_(routing) {}

  template <typename Data>
  std::future<Data> Get(Data::name_type name) {
    return GetPolicy::Get(name);
  }

  template <typename Data>
  void Put(Identity name, Data data, action_callback callback, routing_, fob_) {
    PutPolicy::Put(name, data, callback, routing_, fob_);
  }

  template <typename Data>
  void Post(Identity name, Data message, action_callback callback) {
    PostPolicy::Post(name, message, callback);
  }

  template <typename Data>
  void Delete(Identity name, Data data, action_callback callback) {
    DeletePolicy::Delete(name, data, callback);
  }

 private:
  Routing routing_;
  Fob fob_;
}:

typedef Nfs<GetFromDataHolder,
            PutToDataHolder,
            NoPost,
            DeleteFromDataHolder> ClientNfs;

// TODO(prakash) example .... ClientNfs client_nfs(routing, Pmid pmd);
// client_nfs.Get<ImmutableData>(name, get_callback callback);

}  /*namespace nfs */

}  /* namespace maidsafe */

#endif  // MAIDSAFE_NFS_NFS_H_
