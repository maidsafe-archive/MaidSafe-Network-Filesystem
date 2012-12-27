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

#ifndef MAIDSAFE_NFS_GET_POLICIES_H_
#define MAIDSAFE_NFS_GET_POLICIES_H_

#include <future>
#include <string>
#include <vector>

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/crypto.h"
#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/utils.h"


namespace maidsafe {

namespace nfs {

class NoGet {
 public:
  template<typename Data>
  static std::future<Data> Get(const typename Data::name_type& /*name*/, routing::Routing& /*routing*/) {}
 protected:
  ~NoGet() {}
};

class GetFromMetaDataManager {
 public:
  template<typename Data>
    void Get(const typename Data::name_type& /*name*/, routing::Routing& /*routing*/) {}
 protected:
  ~GetFromMetaDataManager() {}
};

template<PersonaType persona>
class GetFromDataHolder {
 public:
  template<typename Data>
  static std::future<Data> Get(const typename Data::name_type& name, routing::Routing& routing) {
    auto promise(std::make_shared<std::promise<Data>>());  // NOLINT (Fraser)
    std::future<Data> future(promise->get_future());
    routing::ResponseFunctor callback =
        [promise](const std::vector<std::string>& serialised_messages) {
          HandleGetResponse(promise, serialised_messages);
        };
    Message message(ActionType::kGet, PersonaType::kDataHolder, persona, DataType<Data>(),
                    NodeId(name.data.string()), routing.kNodeId(), NonEmptyString(),
                    asymm::Signature());
    routing.Send(NodeId(name.data.string()), message.Serialise().data.string(), callback,
                 routing::DestinationType::kGroup, IsCacheable<Data>());
    return future;
  }

 protected:
  ~GetFromDataHolder() {}
};


#ifdef TESTING

//class GetFromKeyFile {
// public:
//  template<typename Data>
//  static std::future<Data> Get(const typename Data::name_type& name, routing::Routing& routing) {
//    std::promise<Data> promise;
//    Data fetched_key(ReadFromKeyFile());
//    promise.set_value(fetched_key);
//    return promise.get_future();
//  }

// protected:
//  ~GetFromKeyFile() {}
//};

#endif


}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_GET_POLICIES_H_
