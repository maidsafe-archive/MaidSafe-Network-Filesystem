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

#ifndef MAIDSAFE_NFS_CLIENT_DELETE_POLICIES_H_
#define MAIDSAFE_NFS_CLIENT_DELETE_POLICIES_H_

#include <string>

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/crypto.h"
#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/response_mapper.h"
#include "maidsafe/nfs/utils.h"


namespace maidsafe {

namespace nfs {

template<typename SigningFob>
class NoDelete {
 public:
  NoDelete(NfsResponseMapper& /*response_mapper*/, routing::Routing& /*routing*/, const SigningFob& /*signing_fob*/) {}  // NOLINT (Fraser)
  template<typename Data>
  void Delete(const typename Data::name_type& /*name*/, DataMessage::OnError /*on_error*/) {}

 protected:
  ~NoDelete() {}
};

class DeleteFromMaidAccountHolder {
 public:
  DeleteFromMaidAccountHolder(NfsResponseMapper& /*response_mapper*/, routing::Routing& routing, const passport::Maid& signing_fob)
      : routing_(routing),
        signing_fob_(signing_fob),
        source_(PersonaId(Persona::kClientMaid, routing.kNodeId())) {}

  template<typename Data>
  void Delete(const typename Data::name_type& name) {
    DataMessage::Data data(Data::name_type::tag_type::kEnumValue, name.data, NonEmptyString(),
                           DataMessage::Action::kDelete);
    DataMessage data_message(Persona::kMaidAccountHolder, source_, data);
    data_message.SignData(signing_fob_.private_key());
    Message message(DataMessage::message_type_identifier, data_message.Serialise());
    std::make_shared<StringFutureVector>(routing_.SendGroup(NodeId(name->string()),
                                                            message.Serialise()->string(),
                                                            IsCacheable<Data>()));
  }

 protected:
  ~DeleteFromMaidAccountHolder() {}

 private:
  routing::Routing& routing_;
  passport::Maid signing_fob_;
  PersonaId source_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_DELETE_POLICIES_H_
