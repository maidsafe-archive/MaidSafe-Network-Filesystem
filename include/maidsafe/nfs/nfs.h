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
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/nfs/type_traits.h"

namespace maidsafe { 

namespace nfs {

template <typename DATA>
using get_callback = std::function<DATA(Identity)>;

using action_callback = std::function<bool(Identity)>;

template<typename ID>  // PMID MPID MAID etc.
class Nfs {
 public:
  Nfs(Routing routing) : routing_(routing) {}

  template <typename DATA>
  void Get(Identity name, get_callback<DATA>);  // anybody gets free forever

  template <typename DATA>
  void Put(Identity name,DATA data, action_callback);  // may require make/take payment

  template <typename DATA>
  void Post(Identity name, DATA message, action_callback);  // no payment

  template <typename DATA>
  void Delete(Identity name, DATA data, action_callback);  // may require take payment

 private:
  Routing routing_;
}:



}  /*namespace nfs */  

}  /* namespace maidsafe */

#endif  // MAIDSAFE_NFS_NFS_H_
