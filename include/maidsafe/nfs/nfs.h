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
#include "maidsafe/private/data_types/fob.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/nfs/type_traits.h"
#include "maidsafe/nfs/get_policies.h"
#include "maidsafe/nfs/put_policies.h"
#include "maidsafe/nfs/post_policies.h"
#include "maidsafe/nfs/delete_policies.h"

namespace maidsafe {

namespace nfs {

template <typename Data>
using get_callback = std::function<Data(Identity)>;

using action_callback = std::function<bool(Identity)>;

// sfinae test for has routing
template <typename T>
class has_routing {
  typedef char one[1];
  typedef char two[2];
  template <typename U> static one test(decltype(&U::Routing());
// the above gets masked out as sfinae will choose two
  template <typename U) static two test(...);
 public:
  static bool const value = sizeof(test<T>(0)) == sizeof(one);
  // use has_routing<T>::value as the test
};
// sfinae test for has fob
template <typename T>
class has_fob {
  typedef char one[1];
  typedef char two[2];
  template <typename U> static one test(decltype(&U::Fob());
  template <typename U) static two test(...);
 public:
  static bool const value = sizeof(test<T>(0)) == sizeof(one);
  // use has_fob<T>::value as the test
};



// host class (default policies work for client objects)
template<typename ID,  // DataHolderAccountHolder,
                       // DataHolder,
                       // MetaDataManager,
                       // MAIDAccountHolder Client (default)
// pass a param whose type is a template dependant on another template
         template <class> class GetPolicy = GetFromMetadataManager,
         template <class> class PutPolicy = PutToMaidAccountHolder,
         template <class> class PostPolicy = PostToAddress,
         template <class> class DeletePolicy = DeleteToMaidAccountHolder>
class NetworkFileSystem :
    public GetPolicy, public PutPolicy, public PostPolicy, public DeletePolicy {
 public:
  // should be 
  // Nfs(ID id,
  //     typename std::enable_if<has_routing<ID>::value, ID>type* = 0,
  //     typename std::enable_if<has_fob<ID>::value, ID>type* =0)  {}
  Nfs(Routing routing, Fob id_fob) : routing_(routing), id_fob_(fob) {}

  template <typename Data>
  void Get(Identity name, get_callback<Data> callback) { // anybody gets free forever
    GetPolicy::Get(name, callback, routing_, fob_);
  }
  template <typename Data>
  void Put(Identity name,Data data, action_callback callbacki, routing_, fob_);

  template <typename Data>
  void Post(Identity name, Data message, action_callback callback);

  template <typename Data>
  void Delete(Identity name, Data data, action_callback callback);

 private:
  Routing routing_; // not needed with the sfinae checks
  Fob fob_;
}:



}  /*namespace nfs */

}  /* namespace maidsafe */

#endif  // MAIDSAFE_NFS_NFS_H_
