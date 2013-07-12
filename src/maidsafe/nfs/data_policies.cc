/* Copyright 2012 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

#include "maidsafe/nfs/data_policies.h"

#include "maidsafe/nfs/reply.h"


namespace maidsafe {

namespace nfs {

// ============================== ClientMaid ==============================

// Put to OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kPut>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kOwnerDirectoryManager, routing_.kNodeId());
}

// Get from OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kGet>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kOwnerDirectoryManager, routing_.kNodeId());
}

// Delete from OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kDelete>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kOwnerDirectoryManager, routing_.kNodeId());
}

// Put to GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kPut>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kGroupDirectoryManager, NodeId(name->string()));
}

// Get from GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kGet>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kGroupDirectoryManager, NodeId(name->string()));
}

// Delete from GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kDelete>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kGroupDirectoryManager, NodeId(name->string()));
}

// Put to WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kPut>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kWorldDirectoryManager, routing_.kNodeId());
}

// Get from WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kGet>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kWorldDirectoryManager, routing_.kNodeId());
}

// Delete from WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kMaidNode,
                     MessageAction::kDelete>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kWorldDirectoryManager, routing_.kNodeId());
}


// Put for all data types requires signature.
template<>
void DataPolicy<passport::Maid,
                Persona::kMaidNode,
                MessageAction::kPut>::SignDataIfRequired(Message& message) const {
  message.SignData(kSigningFob_->private_key());
}

// Delete for all data types requires signature.
template<>
void DataPolicy<passport::Maid,
                Persona::kMaidNode,
                MessageAction::kDelete>::SignDataIfRequired(Message& message) const {
  message.SignData(kSigningFob_->private_key());
}



// ============================== DataGetter ==============================

// Get from OwnerDirectoryManager for OwnerDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     MessageAction::kGet>::GetDestination<OwnerDirectory>(
    const typename OwnerDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kOwnerDirectoryManager, routing_.kNodeId());
}

// Get from GroupDirectoryManager for GroupDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     MessageAction::kGet>::GetDestination<GroupDirectory>(
    const typename GroupDirectory::name_type& name,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kGroupDirectoryManager, NodeId(name->string()));
}

// Get from WorldDirectoryManager for WorldDirectory data.
template<>
template<>
PersonaId DataPolicy<passport::Maid,
                     Persona::kDataGetter,
                     MessageAction::kGet>::GetDestination<WorldDirectory>(
    const typename WorldDirectory::name_type& /*name*/,
    const passport::PublicPmid::name_type& /*pmid_node_name*/) const {
  return PersonaId(Persona::kWorldDirectoryManager, routing_.kNodeId());
}

}  // namespace nfs

}  // namespace maidsafe
