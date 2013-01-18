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

#include "maidsafe/nfs/types.h"
#include "maidsafe/nfs/client_get_policies.h"
#include "maidsafe/nfs/client_put_policies.h"
#include "maidsafe/nfs/client_post_policies.h"
#include "maidsafe/nfs/client_delete_policies.h"


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
  NetworkFileSystem() : GetPolicy(), PutPolicy(), PostPolicy(), DeletePolicy() {}

  explicit NetworkFileSystem(routing::Routing& routing)
      : GetPolicy(routing),
        PutPolicy(routing),
        PostPolicy(routing),
        DeletePolicy(routing) {}

  template<typename SigningFob>
  NetworkFileSystem(routing::Routing& routing, const SigningFob& signing_fob)
      : GetPolicy(routing),
        PutPolicy(routing, signing_fob),
        PostPolicy(routing, signing_fob),
        DeletePolicy(routing, signing_fob) {}
};

typedef NetworkFileSystem<GetFromMetadataManager<Persona::kClientMaid>,
                          PutToDataHolder<passport::Maid>,
                          NoPost<passport::Maid>,
                          NoDelete<passport::Maid>> TemporaryClientMaidNfs;

typedef NetworkFileSystem<GetFromMetadataManager<Persona::kClientMaid>,
                          PutToMaidAccountHolder,
                          NoPost<passport::Maid>,
                          DeleteFromMaidAccountHolder> ClientMaidNfs;

template<typename GetPolicy>
class NetworkFileSystemGetter : public GetPolicy {
 public:
  explicit NetworkFileSystemGetter(routing::Routing& routing) : GetPolicy(routing) {}
};

typedef NetworkFileSystemGetter<GetFromMetadataManager<Persona::kDataGetter>> KeyGetterNfs;

#ifdef TESTING
typedef GetFromKeyFile FakeKeyGetterNfs;
#endif

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_NFS_H_
