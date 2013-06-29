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

#ifndef MAIDSAFE_NFS_DATA_POLICIES_INL_H_
#define MAIDSAFE_NFS_DATA_POLICIES_INL_H_

#include <string>

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/crypto.h"
#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"

#include "maidsafe/data_types/owner_directory.h"
#include "maidsafe/data_types/group_directory.h"
#include "maidsafe/data_types/world_directory.h"

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/utils.h"


namespace maidsafe {

namespace nfs {

template<typename SigningFob, Persona source_persona, MessageAction action>
DataPolicy<SigningFob, source_persona, action>::DataPolicy(routing::Routing& routing)
    : routing_(routing),
      kSigningFob_(),
      kSource_(PersonaId(source_persona, routing.kNodeId())) {}

template<typename SigningFob, Persona source_persona, MessageAction action>
DataPolicy<SigningFob, source_persona, action>::DataPolicy(routing::Routing& routing,
                                                           const SigningFob& signing_fob)
    : routing_(routing),
      kSigningFob_(new SigningFob(signing_fob)),
      kSource_(PersonaId(source_persona, routing.kNodeId())) {}

template<typename SigningFob, Persona source_persona, MessageAction action>
template<typename Data>
void DataPolicy<SigningFob, source_persona, action>::ExecuteAction(
    routing::ResponseFunctor callback,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& pmid_node_name) {
  auto destination(GetDestination<Data>(name, pmid_node_name));
  auto message(ConstructMessage<Data>(destination.persona, name, content, pmid_node_name));
  SignDataIfRequired(message);
  Send<Data>(destination.node_id, message.Serialise()->string(), callback);
}

// Default doesn't sign _message
template<typename SigningFob, Persona source_persona, MessageAction action>
MessageWrapper DataPolicy<SigningFob, source_persona, action>::ConstructMessageWrapper(
    const Message& message) const {
  return MessageWrapper(message.Serialise());
}

// ============================== ClientMaid ==============================

// Put to MaidManager for all data types except directories.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kPut>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kMaidManager, routing_.kNodeId());
}

// Get from DataManager for all data types except directories.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kGet>::GetDestination(
    const typename Data::name_type& name,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kDataManager, NodeId(name->string()));
}

// Delete from MaidManager for all data types except directories.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kDelete>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kMaidManager, routing_.kNodeId());
}

// Put to OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kPut>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const;

// Get from OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kGet>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const;

// Delete from OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kDelete>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const;

// Put to GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kPut>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const;

// Get from GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kGet>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const;

// Delete from GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kDelete>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const;

// Put to WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kPut>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const;

// Get from WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kGet>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const;

// Delete from WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kDelete>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const;

// Put for all data types uses pmid_node_hint.
template<>
template<typename Data>
Message DataPolicy<passport::Maid,
                   Persona::kMaidNode,
                   MessageAction::kPut>::ConstructMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& pmid_node_hint) const {
  return Message(destination_persona, kSource_,
                 Message::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                               MessageAction::kPut),
                 pmid_node_hint);
}

// Get for all data types doesn't use pmid_node_hint.
template<>
template<typename Data>
Message DataPolicy<passport::Maid,
                   Persona::kMaidNode,
                   MessageAction::kGet>::ConstructMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*pmid_node_hint*/) const {
  return Message(destination_persona, kSource_,
                 Message::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                               MessageAction::kGet));
}

// Delete for all data types doesn't use pmid_node_hint.
template<>
template<typename Data>
Message DataPolicy<passport::Maid,
                   Persona::kMaidNode,
                   MessageAction::kDelete>::ConstructMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*pmid_node_hint*/) const {
  return Message(destination_persona, kSource_,
                 Message::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                               MessageAction::kDelete));
}

// Get for all data types should use the Cacheable trait of the data type.
template<>
template<typename Data>
void DataPolicy<passport::Maid,
                Persona::kMaidNode,
                MessageAction::kGet>::Send(const NodeId& destination,
                                                 const std::string& serialised_message,
                                                 const routing::ResponseFunctor& callback) {
  routing_.SendGroup(destination, serialised_message, IsCacheable<Data>(), callback);
}



// ============================== MaidManager ==============================

// Put to DataManager for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kMaidManager,
                     MessageAction::kPut>::GetDestination(
    const typename Data::name_type& name,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kDataManager, NodeId(name->string()));
}

// Delete from DataManager for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kMaidManager,
                     MessageAction::kDelete>::GetDestination(
    const typename Data::name_type& name,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kDataManager, NodeId(name->string()));
}

// Put for all data types uses pmid_node_hint.
template<>
template<typename Data>
Message DataPolicy<passport::Pmid,
                   Persona::kMaidManager,
                   MessageAction::kPut>::ConstructMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& pmid_node_hint) const {
  return Message(destination_persona, kSource_,
                 Message::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                               MessageAction::kPut),
                 pmid_node_hint);
}

// Delete for all data types doesn't use pmid_node_hint.
template<>
template<typename Data>
Message DataPolicy<passport::Pmid,
                   Persona::kMaidManager,
                   MessageAction::kDelete>::ConstructMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*pmid_node_hint*/) const {
  return Message(destination_persona, kSource_,
                 Message::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                               MessageAction::kDelete));
}



// ============================== DataManager ==============================

// Put to PmidManager for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kDataManager,
                     MessageAction::kPut>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& pmid_node_name) const {
  return PersonaId(Persona::kPmidManager, NodeId(pmid_node_name->string()));
}

// Get from DataHolder for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kDataManager,
                     MessageAction::kGet>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& pmid_node_name) const {
  return PersonaId(Persona::kPmidNode, NodeId(pmid_node_name->string()));
}

// Delete from PmidManager for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kDataManager,
                     MessageAction::kDelete>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& pmid_node_name) const {
  return PersonaId(Persona::kPmidManager, NodeId(pmid_node_name->string()));
}

// Put for all data types uses pmid_node field of Message.
template<>
template<typename Data>
Message DataPolicy<passport::Pmid,
                   Persona::kDataManager,
                   MessageAction::kPut>::ConstructMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& pmid_node_name) const {
  return Message(destination_persona, kSource_,
                 Message::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                               MessageAction::kPut),
                 pmid_node_name);
}

// Get for all data types doesn't use pmid_node field of Message.
template<>
template<typename Data>
Message DataPolicy<passport::Pmid,
                   Persona::kDataManager,
                   MessageAction::kGet>::ConstructMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return Message(destination_persona, kSource_,
                 Message::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                               MessageAction::kGet));
}

// Delete for all data types uses pmid_node field of Message.
template<>
template<typename Data>
Message DataPolicy<passport::Pmid,
                   Persona::kDataManager,
                   MessageAction::kDelete>::ConstructMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& pmid_node_name) const {
  return Message(destination_persona, kSource_,
                 Message::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                               MessageAction::kDelete),
                 pmid_node_name);
}



// ============================== PmidManager ==============================

// Put to DataHolder for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kPmidManager,
                     MessageAction::kPut>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& pmid_node_name) const {
  return PersonaId(Persona::kPmidNode, NodeId(pmid_node_name->string()));
}

// Delete from DataHolder for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kPmidManager,
                     MessageAction::kDelete>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& pmid_node_name) const {
  return PersonaId(Persona::kPmidNode, NodeId(pmid_node_name->string()));
}

// Put for all data types doesn't use pmid_node field of Message.
template<>
template<typename Data>
Message DataPolicy<passport::Pmid,
                   Persona::kPmidManager,
                   MessageAction::kPut>::ConstructMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return Message(destination_persona, kSource_,
                 Message::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                               MessageAction::kPut));
}

// Delete for all data types doesn't use pmid_node field of Message.
template<>
template<typename Data>
Message DataPolicy<passport::Pmid,
                   Persona::kPmidManager,
                   MessageAction::kDelete>::ConstructMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return Message(destination_persona, kSource_,
                 Message::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                               MessageAction::kDelete));
}

// Put for all data types should be via routing's SendDirect rather than SendGroup
template<>
template<typename Data>
void DataPolicy<passport::Pmid,
                Persona::kPmidManager,
                MessageAction::kPut>::Send(const NodeId& destination,
                                                 const std::string& serialised_message,
                                                 const routing::ResponseFunctor& callback) {
  routing_.SendDirect(destination, serialised_message, false, callback);
}

// Delete for all data types should be via routing's SendDirect rather than SendGroup
template<>
template<typename Data>
void DataPolicy<passport::Pmid,
                Persona::kPmidManager,
                MessageAction::kDelete>::Send(const NodeId& destination,
                                                    const std::string& serialised_message,
                                                    const routing::ResponseFunctor& callback) {
  routing_.SendDirect(destination, serialised_message, false, callback);
}



// ============================== DataGetter ==============================

// Get from DataManager for all data types except directories.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     MessageAction::kGet>::GetDestination(
    const typename Data::name_type& name,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kDataManager, NodeId(name->string()));
}

// Get from OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     MessageAction::kGet>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const;

// Get from GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     MessageAction::kGet>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const;

// Get from WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     MessageAction::kGet>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const;

// Get for all data types doesn't use pmid_node_hint.
template<>
template<typename Data>
Message DataPolicy<passport::Maid,
Persona::kDataGetter,
MessageAction::kGet>::ConstructMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*pmid_node_hint*/) const {
  return Message(destination_persona, kSource_,
                 Message::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                               MessageAction::kGet));
}

// Get for all data types should use the Cacheable trait of the data type.
template<>
template<typename Data>
void DataPolicy<passport::Maid,
                Persona::kDataGetter,
                MessageAction::kGet>::Send(const NodeId& destination,
                                                 const std::string& serialised_message,
                                                 const routing::ResponseFunctor& callback) {
  routing_.SendGroup(destination, serialised_message, IsCacheable<Data>(), callback);
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_DATA_POLICIES_INL_H_
