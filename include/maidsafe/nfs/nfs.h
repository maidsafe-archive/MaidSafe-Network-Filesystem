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

#include "maidsafe/nfs/response_mapper.h"
#include "maidsafe/nfs/types.h"
#include "maidsafe/nfs/client_get_policies.h"
#include "maidsafe/nfs/client_put_policies.h"
#include "maidsafe/nfs/client_post_policies.h"
#include "maidsafe/nfs/client_delete_policies.h"


namespace maidsafe {

namespace nfs {

namespace detail {

  template <typename Data>
  struct GetPersona {};

  template <>
  struct GetPersona<OwnerDirectory> {
    static const Persona persona = Persona::kOwnerDirectoryManager;
  };

  template <>
  struct GetPersona<GroupDirectory> {
    static const Persona persona = Persona::kGroupDirectoryManager;
  };

  template <>
  struct GetPersona<WorldDirectory> {
    static const Persona persona = Persona::kWorldDirectoryManager;
  };

  template <>
  struct GetPersona<ImmutableData> {
    static const Persona persona = Persona::kMaidAccountHolder;
  };

}  // namespace detail

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

  NetworkFileSystem(nfs::NfsResponseMapper& response_mapper, routing::Routing& routing)
      : GetPolicy(response_mapper, routing),
        PutPolicy(response_mapper, routing),
        PostPolicy(response_mapper, routing),
        DeletePolicy(response_mapper, routing) {}

  template<typename SigningFob>
  NetworkFileSystem(nfs::NfsResponseMapper& response_mapper, routing::Routing& routing,
                    const SigningFob& signing_fob)
      : GetPolicy(response_mapper, routing),
        PutPolicy(response_mapper, routing, signing_fob),
        PostPolicy(response_mapper, routing, signing_fob),
        DeletePolicy(response_mapper, routing, signing_fob) {}
};

typedef NetworkFileSystem<GetFromMetadataManager<Persona::kClientMaid>,
                          PutToDataHolder<passport::Maid>,
                          NoPost<passport::Maid>,
                          NoDelete<passport::Maid>> TemporaryClientMaidNfs;

typedef NetworkFileSystem<GetFromDirectoryManager,
                          PutToDirectoryManager,
                          NoPost<passport::Maid>,
                          DeleteFromDirectoryManager> ClientMaidNfs;

// typedef NetworkFileSystem<GetFromMetadataManager<Persona::kClientMaid>,
//                          PutToMaidAccountHolder,
//                          NoPost<passport::Maid>,
//                          DeleteFromMaidAccountHolder> ClientMaidNfs;

template<typename GetPolicy>
class NetworkFileSystemGetter : public GetPolicy {
 public:
  NetworkFileSystemGetter(nfs::NfsResponseMapper& response_mapper,
                          routing::Routing& routing)
      : GetPolicy(response_mapper, routing) {}
};

typedef NetworkFileSystemGetter<GetFromMetadataManager<Persona::kDataGetter>> KeyGetterNfs;

#ifdef TESTING
typedef GetFromKeyFile FakeKeyGetterNfs;
#endif

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_NFS_H_
