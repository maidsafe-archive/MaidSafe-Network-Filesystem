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
#include "maidsafe/data_types/data_types.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/nfs/type_traits.h"
#include "maidsafe/nfs/get_policies.h"
#include "maidsafe/nfs/put_policies.h"
#include "maidsafe/nfs/post_policies.h"
#include "maidsafe/nfs/delete_policies.h"

namespace maidsafe {

namespace nfs {

class Fob {
};

template <typename Data>
using get_callback = std::function<Data(Identity)>;

using action_callback = std::function<bool(Identity)>;

template<typename GetPolicy,
         typename PutPolicy,
         typename PostPolicy,
         typename DeletePolicy>
class NetworkFileSystem :
    public GetPolicy, public PutPolicy, public PostPolicy, public DeletePolicy {
 public:
  Nfs(Routing routing, Fob id_fob) : routing_(routing), id_fob_(fob) {}

  template <typename Data>
  void Get(Data::name_type name, get_callback<Data> callback) {
    GetPolicy::Get(name, callback, routing_, fob_);
  }
  template <typename Data>
  void Put(Identity name, Data data, action_callback callback, routing_, fob_);

  template <typename Data>
  void Post(Identity name, Data message, action_callback callback);

  template <typename Data>
  void Delete(Identity name, Data data, action_callback callback);

 private:
  Routing routing_;
  Fob fob_;
}:

typedef Nfs<GetFromMetadataManager,
            PutToMaidAccountHolder,
            PostToAddress,
            DeleteFromMaidAccountHolder> ClientNfs;


}  /*namespace nfs */

}  /* namespace maidsafe */

#endif  // MAIDSAFE_NFS_NFS_H_
