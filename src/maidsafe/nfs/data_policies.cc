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

#include "maidsafe/nfs/data_policies.h"

#include "maidsafe/nfs/reply.h"


namespace maidsafe {

namespace nfs {

// ============================== ClientMaid ==============================

// Put to OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kPut>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kOwnerDirectoryManager, routing_.kNodeId());
}

// Get from OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kGet>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kOwnerDirectoryManager, routing_.kNodeId());
}

// Delete from OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kDelete>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kOwnerDirectoryManager, routing_.kNodeId());
}

// Put to GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kPut>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kGroupDirectoryManager, NodeId(name->string()));
}

// Get from GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kGet>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kGroupDirectoryManager, NodeId(name->string()));
}

// Delete from GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kDelete>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kGroupDirectoryManager, NodeId(name->string()));
}

// Put to WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kPut>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kWorldDirectoryManager, routing_.kNodeId());
}

// Get from WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kGet>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kWorldDirectoryManager, routing_.kNodeId());
}

// Delete from WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kClientMaid,
                     DataMessage::Action::kDelete>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kWorldDirectoryManager, routing_.kNodeId());
}

// Put for all data types requires signature.
template<>
void DataPolicy<passport::Maid,
                Persona::kClientMaid,
                DataMessage::Action::kPut>::SignDataIfRequired(DataMessage& data_message) const {
  data_message.SignData(kSigningFob_->private_key());
}

// Delete for all data types requires signature.
template<>
void DataPolicy<passport::Maid,
                Persona::kClientMaid,
                DataMessage::Action::kDelete>::SignDataIfRequired(DataMessage& data_message) const {
  data_message.SignData(kSigningFob_->private_key());
}



// ============================== DataGetter ==============================

// Get from OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     DataMessage::Action::kGet>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kOwnerDirectoryManager, routing_.kNodeId());
}

// Get from GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     DataMessage::Action::kGet>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kGroupDirectoryManager, NodeId(name->string()));
}

// Get from WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     DataMessage::Action::kGet>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*data_holder_name*/) const {
  return PersonaId(Persona::kWorldDirectoryManager, routing_.kNodeId());
}



// ============================== FakeKeyGetter ==============================
#ifdef TESTING

template<>
void GetFromKeyFile::Get<passport::PublicPmid>(const typename passport::PublicPmid::name_type& name,
                                               const routing::ResponseFunctor& callback) {
  auto itr(std::find_if(kAllPmids_.begin(), kAllPmids_.end(),
      [&name](const passport::PublicPmid& pmid) { return pmid.name() == name; }));
  if (itr == kAllPmids_.end()) {
    Reply reply(NfsErrors::failed_to_get_data);
    return callback(reply.Serialise()->string());
  }
  Reply reply(CommonErrors::success, (*itr).Serialise().data);
  callback(reply.Serialise()->string());
}

#endif

}  // namespace nfs

}  // namespace maidsafe
