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

template<typename SigningFob>
class NoPut {
 public:
  NoPut(routing::Routing& /*routing*/, const SigningFob& /*signing_fob*/) {}  // NOLINT (Fraser)

  template<typename Data>
  void Put(const Data& /*data*/, OnError /*on_error*/) {}

 protected:
  ~NoPut() {}
};

template<typename SigningFob>
class PutToDataHolder {
 public:
  PutToDataHolder(routing::Routing& routing, const SigningFob& signing_fob)
      : routing_(routing),
        signing_fob_(signing_fob) {}

  template<typename Data>
  void Put(const Data& data, OnError on_error) {
    NonEmptyString content(data.Serialise());
    Message message(ActionType::kPut, PersonaType::kDataHolder, PersonaType::kClientMaid,
                    Data::type_enum_value(), NodeId(data.name().data.string()),
                    routing_.kNodeId(), content, asymm::Sign(content, signing_fob_.private_key()));
    routing::ResponseFunctor callback =
        [on_error, message](const std::vector<std::string>& serialised_messages) {
          HandlePutResponse<Data>(on_error, message, serialised_messages);
        };
    routing_.Send(NodeId(data.name().data.string()), message.Serialise().data.string(), callback,
                  routing::DestinationType::kGroup, IsCacheable<Data>());
  }

 protected:
  ~PutToDataHolder() {}

 private:
  routing::Routing& routing_;
  SigningFob signing_fob_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_PUT_POLICIES_H_
