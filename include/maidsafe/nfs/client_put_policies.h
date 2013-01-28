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

#ifndef MAIDSAFE_NFS_CLIENT_PUT_POLICIES_H_
#define MAIDSAFE_NFS_CLIENT_PUT_POLICIES_H_

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

typedef std::future<ReturnCode> ReturnCodeFuture;
typedef std::vector<ReturnCodeFuture> ReturnCodeFutureVector;
typedef std::promise<ReturnCode> ReturnCodePromise;
typedef std::vector<ReturnCodePromise> ReturnCodePromiseVector;

void ProcessReadyFuture(StringFuture& future,
                        ReturnCodePromiseVector& promises,
                        size_t& index);

void HandlePutFutures(std::shared_ptr<ReturnCodePromiseVector> promises,
                      std::shared_ptr<StringFutureVector> routing_futures);

template<typename SigningFob>
class NoPut {
 public:
  NoPut() {}
  explicit NoPut(routing::Routing& /*routing*/) {}
  NoPut(routing::Routing& /*routing*/, const SigningFob& /*signing_fob*/) {}  // NOLINT (Fraser)

  template<typename Data>
  void Put(const Data& /*data*/, DataMessage::OnError /*on_error*/) {}

 protected:
  ~NoPut() {}
};


// TODO(Fraser#5#): 2013-01-11 - BEFORE_RELEASE - Remove this class.
template<typename SigningFob>
class PutToDataHolder {
 public:
  PutToDataHolder(routing::Routing& routing, const SigningFob& signing_fob)
      : routing_(routing),
        signing_fob_(signing_fob),
        source_(PersonaId(Persona::kClientMaid, routing.kNodeId())) {}

  template<typename Data>
  void Put(const Data& data) {
    DataMessage::Data message_data(Data::name_type::tag_type::kEnumValue, data.name(),
                                   data.Serialise());
    DataMessage data_message(DataMessage::Action::kPut, Persona::kDataHolder, source_,
                             message_data);
    data_message.SignData(signing_fob_.private_key());
    Message message(DataMessage::message_type_identifier, data_message.Serialise());
//    routing::ResponseFunctor callback =
//        [on_error, data_message](const std::vector<std::string>& serialised_messages) {
//          HandlePutResponse<Data>(on_error, data_message, serialised_messages);
//        };
//    routing_.Send(NodeId(data.name()->string()), message.Serialise()->string(), callback,
//                  routing::DestinationType::kGroup, IsCacheable<Data>());
  }

 protected:
  ~PutToDataHolder() {}

 private:
  routing::Routing& routing_;
  SigningFob signing_fob_;
  PersonaId source_;
};

class PutToMaidAccountHolder {
 public:
  PutToMaidAccountHolder(routing::Routing& routing, const passport::Maid& signing_fob)
      : routing_(routing),
        signing_fob_(signing_fob),
        source_(PersonaId(Persona::kClientMaid, routing.kNodeId())) {}

  template<typename Data>
  ReturnCodeFutureVector Put(const Data& data) {
    DataMessage data_message(DataMessage::Action::kPut, Persona::kMaidAccountHolder, source_, data);
    data_message.SignData(signing_fob_.private_key());
    Message message(DataMessage::message_type_identifier, data_message.Serialise());
    auto routing_futures(std::make_shared<StringFutureVector>(
                             routing_.SendGroup(routing_.kNodeId(),
                                                message.Serialise()->string(),
                                                IsCacheable<Data>())));

    ReturnCodeFutureVector return_codes;
    auto promises(std::make_shared<ReturnCodePromiseVector>(routing_futures.size()));
    for (auto& promise : *promises)
      return_codes.push_back(promise.get_future());
    HandlePutFutures(promises, routing_futures);

    return std::move(return_codes);
  }

 protected:
  ~PutToMaidAccountHolder() {}

 private:
  routing::Routing& routing_;
  passport::Maid signing_fob_;
  PersonaId source_;
};



}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_PUT_POLICIES_H_
