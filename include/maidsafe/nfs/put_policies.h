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

#ifndef MAIDSAFE_NFS_PUT_POLICIES_H_
#define MAIDSAFE_NFS_PUT_POLICIES_H_

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

class NoPut {
 public:
  template<typename Data>
  static void Put(const Data& /*data*/,
                  OnError /*on_error*/,
                  routing::Routing& /*routing*/,
                  const passport::Maid& /*maid*/) {}
 protected:
  ~NoPut() {}
};

class PutToDataHolder {
 public:
  template<typename Data>
  static void Put(const Data& data,
                  OnError on_error,
                  routing::Routing& routing,
                  const passport::Maid& maid) {
    NonEmptyString content(data.Serialise());
    Message message(ActionType::kPut, PersonaType::kDataHolder, PersonaType::kClientMaid,
                    DataType<Data>(), NodeId(data.destination.data.string()), routing.kNodeId(),
                    content, asymm::Sign(content, maid.private_key()));
    routing::ResponseFunctor callback =
        [on_error, message](const std::vector<std::string>& serialised_messages) {
          HandlePutResponse(on_error, message, serialised_messages);
        };
    routing.Send(NodeId(data.name().data.string()), message.Serialise().data.string(), callback,
                 routing::DestinationType::kGroup, IsCacheable<Data>());
  }

 protected:
  ~PutToDataHolder() {}
  static void HandlePutResponse(OnError /*on_error*/, const Message& /*message*/,
                         const std::vector<std::string>& /*serialised_messages*/) {
  }
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_PUT_POLICIES_H_
