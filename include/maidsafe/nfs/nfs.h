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

#include "maidsafe/passport/types.h"

#include "maidsafe/nfs/client_post_policies.h"
#include "maidsafe/nfs/data_policies.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

template<typename PutPolicy, typename GetPolicy, typename DeletePolicy, typename PostPolicy>
class NetworkFileSystem : public PutPolicy,
                          public GetPolicy,
                          public DeletePolicy,
                          public PostPolicy {
 public:
  NetworkFileSystem() : PutPolicy(), GetPolicy(), DeletePolicy(), PostPolicy() {}

//  NetworkFileSystem(routing::Routing& routing)
//      : PutPolicy(routing),
//        GetPolicy(routing),
//        DeletePolicy(routing),
//        PostPolicy(routing) {}

  template<typename SigningFob>
  NetworkFileSystem(routing::Routing& routing, const SigningFob& signing_fob)
      : PutPolicy(routing, signing_fob),
        GetPolicy(routing),
        DeletePolicy(routing, signing_fob),
        PostPolicy(routing, signing_fob) {}
};

//  typedef NetworkFileSystem<GetFromMetadataManager<Persona::kClientMaid>,
//                            PutToDataHolder<passport::Maid>,
//                            NoPost<passport::Maid>,
//                            NoDelete<passport::Maid>> TemporaryClientMaidNfs;

typedef NetworkFileSystem<ClientMaidPutPolicy,
                          ClientMaidGetPolicy,
                          ClientMaidDeletePolicy,
                          NoPost<passport::Maid>> ClientMaidNfs;


typedef DataGetterPolicy KeyGetterNfs;


#ifdef TESTING
typedef GetFromKeyFile FakeKeyGetterNfs;
#endif

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_NFS_H_
