/* Copyright 2013 MaidSafe.net limited

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

#include "maidsafe/nfs/maid_node_dispatcher.h"


namespace maidsafe {

namespace nfs {

MaidNodeDispatcher::MaidNodeDispatcher(routing::Routing& routing,
                                       const passport::Maid& signing_fob)
    : routing_(routing),
      kSigningFob_(signing_fob),
      kThisNodeAsSender_(routing_.kNodeId()),
      kMaidManagerReceiver_(routing_.kNodeId()) {}

void MaidNodeDispatcher::SendCreateAccountRequest(routing::TaskId task_id) {
  typedef CreateAccountRequestFromMaidNodeToMaidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage nfs_message(MessageId(task_id), (NfsMessage::Contents()));
  routing_.Send(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMaidManagerReceiver_));
}

void MaidNodeDispatcher::SendRemoveAccountRequest(routing::TaskId task_id) {
  typedef RemoveAccountRequestFromMaidNodeToMaidManager NfsMessage;
  static_assert(NfsMessage::SourcePersona::value == Persona::kMaidNode,
                "The source Persona must be kMaidNode.");
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage nfs_message(MessageId(task_id), (NfsMessage::Contents()));
  routing_.Send(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMaidManagerReceiver_));
}

void MaidNodeDispatcher::SendRegisterPmidRequest(routing::TaskId task_id,
                                                 const PmidRegistration& pmid_registration) {
  typedef RegisterPmidRequestFromMaidNodeToMaidManager NfsMessage;
  static_assert(NfsMessage::SourcePersona::value == Persona::kMaidNode,
                "The source Persona must be kMaidNode.");
  assert(!pmid_registration.unregister());
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage nfs_message(MessageId(task_id), pmid_registration);
  routing_.Send(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMaidManagerReceiver_));
}

void MaidNodeDispatcher::SendUnregisterPmidRequest(routing::TaskId task_id,
                                                   const PmidRegistration& pmid_registration) {
  typedef UnregisterPmidRequestFromMaidNodeToMaidManager NfsMessage;
  static_assert(NfsMessage::SourcePersona::value == Persona::kMaidNode,
                "The source Persona must be kMaidNode.");
  assert(pmid_registration.unregister());
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage nfs_message(MessageId(task_id), pmid_registration);
  routing_.Send(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMaidManagerReceiver_));
}

void MaidNodeDispatcher::SendGetPmidHealthRequest(const passport::Pmid& pmid) {
  typedef GetPmidHealthRequestFromMaidNodeToMaidManager NfsMessage;
  static_assert(NfsMessage::SourcePersona::value == Persona::kMaidNode,
                "The source Persona must be kMaidNode.");
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage nfs_message(NfsMessage::Contents(pmid.name()));
  routing_.Send(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMaidManagerReceiver_));
}

}  // namespace nfs

}  // namespace maidsafe
