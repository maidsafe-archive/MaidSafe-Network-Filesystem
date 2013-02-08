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

template<typename SigningFob, Persona source_persona, DataMessage::Action action>
DataPolicy<SigningFob, source_persona, action>::DataPolicy(routing::Routing& routing)
    : routing_(routing),
      kSigningFob_(),
      kSource_(PersonaId(source_persona, routing.kNodeId())) {}

template<typename SigningFob, Persona source_persona, DataMessage::Action action>
DataPolicy<SigningFob, source_persona, action>::DataPolicy(routing::Routing& routing,
                                                           const SigningFob& signing_fob)
    : routing_(routing),
      kSigningFob_(new SigningFob(signing_fob)),
      kSource_(PersonaId(source_persona, routing.kNodeId())) {}

template<typename SigningFob, Persona source_persona, DataMessage::Action action>
template<typename Data>
void DataPolicy<SigningFob, source_persona, action>::ExecuteAction(
    routing::ResponseFunctor callback,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& data_holder_name) {
  auto destination(GetDestination<Data>(name, data_holder_name));
  auto data_message(ConstructDataMessage<Data>(destination.persona, name, content,
                                               data_holder_name));
  SignDataIfRequired(data_message);
  auto message(ConstructMessage(data_message));
  Send<Data>(destination.node_id, message.Serialise()->string(), callback);
}

// Default doesn't sign data_message
template<typename SigningFob, Persona source_persona, DataMessage::Action action>
Message DataPolicy<SigningFob, source_persona, action>::ConstructMessage(
    const DataMessage& data_message) const {
  return Message(DataMessage::message_type_identifier, data_message.Serialise());
}



// ============================== ClientMaid ==============================

// Put to MaidAccountHolder for all data types except directories.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kPut>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kMaidAccountHolder, routing_.kNodeId());
}

// Get from MetadataManager for all data types except directories.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kGet>::GetDestination(
    const typename Data::name_type& name,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kMetadataManager, NodeId(name->string()));
}

// Delete from MaidAccountHolder for all data types except directories.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kDelete>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kMaidAccountHolder, routing_.kNodeId());
}

// Put to OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kPut>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const;

// Get from OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kGet>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const;

// Delete from OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kDelete>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const;

// Put to GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kPut>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const;

// Get from GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kGet>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const;

// Delete from GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kDelete>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const;

// Put to WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kPut>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const;

// Get from WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kGet>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const;

// Delete from WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kDelete>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const;

// Put for all data types uses data_holder_hint.
template<>
template<typename Data>
DataMessage DataPolicy<passport::Maid,
                       Persona::kClientMaid,
                       DataMessage::Action::kPut>::ConstructDataMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& data_holder_hint) const {
  return DataMessage(destination_persona, kSource_,
                     DataMessage::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                                       DataMessage::Action::kPut),
                     data_holder_hint);
}

// Get for all data types doesn't use data_holder_hint.
template<>
template<typename Data>
DataMessage DataPolicy<passport::Maid,
                       Persona::kClientMaid,
                       DataMessage::Action::kGet>::ConstructDataMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*data_holder_hint*/) const {
  return DataMessage(destination_persona, kSource_,
                     DataMessage::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                                       DataMessage::Action::kGet));
}

// Delete for all data types doesn't use data_holder_hint.
template<>
template<typename Data>
DataMessage DataPolicy<passport::Maid,
                       Persona::kClientMaid,
                       DataMessage::Action::kDelete>::ConstructDataMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*data_holder_hint*/) const {
  return DataMessage(destination_persona, kSource_,
                     DataMessage::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                                       DataMessage::Action::kDelete));
}

// Get for all data types should use the Cacheable trait of the data type.
template<>
template<typename Data>
void DataPolicy<passport::Maid,
                Persona::kClientMaid,
                DataMessage::Action::kGet>::Send(const NodeId& destination,
                                                 const std::string& serialised_message,
                                                 const routing::ResponseFunctor& callback) {
  routing_.SendGroup(destination, serialised_message, IsCacheable<Data>(), callback);
}



// ============================== MaidAccountHolder ==============================

// Put to MetadataManager for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kMaidAccountHolder,
                     DataMessage::Action::kPut>::GetDestination(
    const typename Data::name_type& name,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kMetadataManager, NodeId(name->string()));
}

// Delete from MetadataManager for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kMaidAccountHolder,
                     DataMessage::Action::kDelete>::GetDestination(
    const typename Data::name_type& name,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kMetadataManager, NodeId(name->string()));
}

// Put for all data types uses data_holder_hint.
template<>
template<typename Data>
DataMessage DataPolicy<passport::Pmid,
                       Persona::kMaidAccountHolder,
                       DataMessage::Action::kPut>::ConstructDataMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& data_holder_hint) const {
  return DataMessage(destination_persona, kSource_,
                     DataMessage::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                                       DataMessage::Action::kPut),
                     data_holder_hint);
}

// Delete for all data types doesn't use data_holder_hint.
template<>
template<typename Data>
DataMessage DataPolicy<passport::Pmid,
                       Persona::kMaidAccountHolder,
                       DataMessage::Action::kDelete>::ConstructDataMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*data_holder_hint*/) const {
  return DataMessage(destination_persona, kSource_,
                     DataMessage::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                                       DataMessage::Action::kDelete));
}



// ============================== MetadataManager ==============================

// Put to PmidAccountHolder for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kMetadataManager,
                     DataMessage::Action::kPut>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& data_holder_name) const {
  return PersonaId(Persona::kPmidAccountHolder, NodeId(data_holder_name->string()));
}

// Get from DataHolder for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kMetadataManager,
                     DataMessage::Action::kGet>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& data_holder_name) const {
  return PersonaId(Persona::kDataHolder, NodeId(data_holder_name->string()));
}

// Delete from PmidAccountHolder for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kMetadataManager,
                     DataMessage::Action::kDelete>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& data_holder_name) const {
  return PersonaId(Persona::kPmidAccountHolder, NodeId(data_holder_name->string()));
}

// Put for all data types uses data_holder field of DataMessage.
template<>
template<typename Data>
DataMessage DataPolicy<passport::Pmid,
                       Persona::kMetadataManager,
                       DataMessage::Action::kPut>::ConstructDataMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& data_holder_name) const {
  return DataMessage(destination_persona, kSource_,
                     DataMessage::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                                       DataMessage::Action::kPut),
                     data_holder_name);
}

// Get for all data types doesn't use data_holder field of DataMessage.
template<>
template<typename Data>
DataMessage DataPolicy<passport::Pmid,
                       Persona::kMetadataManager,
                       DataMessage::Action::kGet>::ConstructDataMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return DataMessage(destination_persona, kSource_,
                     DataMessage::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                                       DataMessage::Action::kGet));
}

// Delete for all data types uses data_holder field of DataMessage.
template<>
template<typename Data>
DataMessage DataPolicy<passport::Pmid,
                       Persona::kMetadataManager,
                       DataMessage::Action::kDelete>::ConstructDataMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& data_holder_name) const {
  return DataMessage(destination_persona, kSource_,
                     DataMessage::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                                       DataMessage::Action::kDelete),
                     data_holder_name);
}



// ============================== PmidAccountHolder ==============================

// Put to DataHolder for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kPmidAccountHolder,
                     DataMessage::Action::kPut>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& data_holder_name) const {
  return PersonaId(Persona::kDataHolder, NodeId(data_holder_name->string()));
}

// Delete from DataHolder for all data types.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Pmid,
                     Persona::kPmidAccountHolder,
                     DataMessage::Action::kDelete>::GetDestination(
    const typename Data::name_type& /*name*/,
    const passport::PublicPmid::name_type& data_holder_name) const {
  return PersonaId(Persona::kDataHolder, NodeId(data_holder_name->string()));
}

// Put for all data types doesn't use data_holder field of DataMessage.
template<>
template<typename Data>
DataMessage DataPolicy<passport::Pmid,
                       Persona::kPmidAccountHolder,
                       DataMessage::Action::kPut>::ConstructDataMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return DataMessage(destination_persona, kSource_,
                     DataMessage::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                                       DataMessage::Action::kPut));
}

// Delete for all data types doesn't use data_holder field of DataMessage.
template<>
template<typename Data>
DataMessage DataPolicy<passport::Pmid,
                       Persona::kPmidAccountHolder,
                       DataMessage::Action::kDelete>::ConstructDataMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return DataMessage(destination_persona, kSource_,
                     DataMessage::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                                       DataMessage::Action::kDelete));
}

// Put for all data types should be via routing's SendDirect rather than SendGroup
template<>
template<typename Data>
void DataPolicy<passport::Pmid,
                Persona::kPmidAccountHolder,
                DataMessage::Action::kPut>::Send(const NodeId& destination,
                                                 const std::string& serialised_message,
                                                 const routing::ResponseFunctor& callback) {
  routing_.SendDirect(destination, serialised_message, false, callback);
}

// Delete for all data types should be via routing's SendDirect rather than SendGroup
template<>
template<typename Data>
void DataPolicy<passport::Pmid,
                Persona::kPmidAccountHolder,
                DataMessage::Action::kDelete>::Send(const NodeId& destination,
                                                    const std::string& serialised_message,
                                                    const routing::ResponseFunctor& callback) {
  routing_.SendDirect(destination, serialised_message, false, callback);
}



// ============================== DataGetter ==============================

// Get from MetadataManager for all data types except directories.
template<>
template<typename Data>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     DataMessage::Action::kGet>::GetDestination(
    const typename Data::name_type& name,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kMetadataManager, NodeId(name->string()));
}

// Get from OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     DataMessage::Action::kGet>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const;

// Get from GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     DataMessage::Action::kGet>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const;

// Get from WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     DataMessage::Action::kGet>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const;

// Get for all data types doesn't use data_holder_hint.
template<>
template<typename Data>
DataMessage DataPolicy<passport::Maid,
                       Persona::kDataGetter,
                       DataMessage::Action::kGet>::ConstructDataMessage(
    Persona destination_persona,
    const typename Data::name_type& name,
    const NonEmptyString& content,
    const passport::PublicPmid::name_type& /*data_holder_hint*/) const {
  return DataMessage(destination_persona, kSource_,
                     DataMessage::Data(Data::name_type::tag_type::kEnumValue, name.data, content,
                                       DataMessage::Action::kGet));
}

// Get for all data types should use the Cacheable trait of the data type.
template<>
template<typename Data>
void DataPolicy<passport::Maid,
                Persona::kDataGetter,
                DataMessage::Action::kGet>::Send(const NodeId& destination,
                                                 const std::string& serialised_message,
                                                 const routing::ResponseFunctor& callback) {
  routing_.SendGroup(destination, serialised_message, IsCacheable<Data>(), callback);
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_DATA_POLICIES_INL_H_
