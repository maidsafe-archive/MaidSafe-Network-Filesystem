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


// TODO(Fraser#5#): 2013-01-11 - BEFORE_RELEASE - Remove this class.  Should be in Vault project.
template<typename SigningFob>
class PutToDataHolder {
 public:
  PutToDataHolder(routing::Routing& routing, const SigningFob& signing_fob)
      : routing_(routing),
        signing_fob_(signing_fob),
        source_(MessageSource(Persona::kClientMaid, routing.kNodeId())) {}

  template<typename Data>
  void Put(const Data& data, DataMessage::OnError on_error) {
    DataMessage::Data message_data(Data::name_type::tag_type::kEnumValue, data.name(),
                                   data.Serialise());
    DataMessage data_message(DataMessage::Action::kPut, Persona::kDataHolder, source_,
                             message_data);
    auto serialised_and_signed(data_message.SerialiseAndSign(signing_fob_.private_key()));
    Message message(serialised_and_signed.first, serialised_and_signed.second);
    routing::ResponseFunctor callback =
        [on_error, message](const std::vector<std::string>& serialised_messages) {
          HandlePutResponse<Data>(on_error, message, serialised_messages);
        };
    routing_.Send(NodeId(data.name()->string()), message.Serialise()->string(), callback,
                  routing::DestinationType::kGroup, IsCacheable<Data>());
  }

 protected:
  ~PutToDataHolder() {}

 private:
  routing::Routing& routing_;
  SigningFob signing_fob_;
  MessageSource source_;
};

class PutToMaidAccountHolder {
 public:
  PutToMaidAccountHolder(routing::Routing& routing, const passport::Maid& signing_fob)
      : routing_(routing),
        signing_fob_(signing_fob),
        source_(MessageSource(Persona::kClientMaid, routing.kNodeId())) {}

  template<typename Data>
  void Put(const Data& data, DataMessage::OnError on_error) {
    NonEmptyString content(data.Serialise());
    Message message(DataMessage::Action::kPut, Persona::kDataHolder, source_,
                    Data::name_type::tag_type::kEnumValue, data.name(), content,
                    asymm::Sign(content, signing_fob_.private_key()));
    routing::ResponseFunctor callback =
        [on_error, message](const std::vector<std::string>& serialised_messages) {
          HandlePutResponse<Data>(on_error, message, serialised_messages);
        };
    routing_.Send(routing_.kNodeId(), message.Serialise()->string(), callback,
                  routing::DestinationType::kGroup, IsCacheable<Data>());
  }

 protected:
  ~PutToMaidAccountHolder() {}

 private:
  routing::Routing& routing_;
  passport::Maid signing_fob_;
  MessageSource source_;
};



}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_PUT_POLICIES_H_
