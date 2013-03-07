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

#ifndef MAIDSAFE_NFS_CLIENT_POST_POLICIES_H_
#define MAIDSAFE_NFS_CLIENT_POST_POLICIES_H_

#include "maidsafe/routing/routing_api.h"


namespace maidsafe {

namespace nfs {

template<typename SigningFob>
class NoPost {
 public:
  NoPost() {}
  NoPost(routing::Routing&, const SigningFob&) {}
  template<typename Data>
  void Post(const typename Data::name_type& /*name*/) {}

 protected:
  ~NoPost() {}
};

}  // namespace nfs

}  // namespace maidsafe

#include "maidsafe/nfs/client_post_policies-inl.h"

#endif  // MAIDSAFE_NFS_CLIENT_POST_POLICIES_H_
