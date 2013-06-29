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

#include <string>

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/message.h"
#include "maidsafe/nfs/message_wrapper.h"
#include "maidsafe/nfs/persona_id.h"

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

template <typename SigningFob, Persona source_persona>
class ClientPostPolicy {
 public:
  ClientPostPolicy(routing::Routing& routing, const SigningFob& signing_fob)
      : routing_(routing),
        kSigningFob_(new SigningFob(signing_fob)),
        kSource_(source_persona, routing_.kNodeId()) {}

  void RegisterPmid(const NonEmptyString& serialised_pmid_registration,
                    const routing::ResponseFunctor& callback) {
    Message message(Persona::kMaidManager, kSource_,
                    Message::Data(kSigningFob_->name().data, serialised_pmid_registration,
                                  MessageAction::kRegisterPmid));
    MessageWrapper message_wrapper(message.Serialise());
    routing_.SendGroup(NodeId(message.data().name.string()),
                       message_wrapper.Serialise()->string(), false, callback);
  }

  void UnregisterPmid(const NonEmptyString& serialised_pmid_unregistration,
                      const routing::ResponseFunctor& callback) {
    Message message(Persona::kMaidManager, kSource_,
                    Message::Data(kSigningFob_->name().data, serialised_pmid_unregistration,
                                  MessageAction::kUnregisterPmid));
    MessageWrapper message_wrapper(message.Serialise());
    routing_.SendGroup(NodeId(message.data().name.string()),
                       message_wrapper.Serialise()->string(), false, callback);
  }

 private:
  routing::Routing& routing_;
  const std::unique_ptr<const SigningFob> kSigningFob_;
  const PersonaId kSource_;
};

typedef ClientPostPolicy<passport::Maid, Persona::kMaidNode> ClientMaidPostPolicy;

}  // namespace nfs

}  // namespace maidsafe

#include "maidsafe/nfs/client_post_policies-inl.h"

#endif  // MAIDSAFE_NFS_CLIENT_POST_POLICIES_H_
