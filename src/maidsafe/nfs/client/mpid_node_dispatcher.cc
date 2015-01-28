/*  Copyright 2013 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#include "maidsafe/nfs/client/mpid_node_dispatcher.h"

namespace maidsafe {

namespace nfs_client {

MpidNodeDispatcher::MpidNodeDispatcher(routing::Routing& routing)
    : running_(true),
      running_mutex_(),
      routing_(routing),
      kThisNodeAsSender_(routing_.kNodeId()),
      kMpidManagerReceiver_(routing_.kNodeId()) {}


void MpidNodeDispatcher::Stop() {
  std::lock_guard<std::mutex> lock(running_mutex_);
  running_ = false;
  LOG(kWarning) << " MpidNodeDispatcher::Stop() !";
}

void MpidNodeDispatcher::SendMessageRequest(routing::TaskId task_id,
                                            const nfs_vault::MpidMessage& mpid_message) {
  typedef nfs::SendMessageRequestFromMpidNodeToMpidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage::Contents contents(mpid_message);
  NfsMessage nfs_message(nfs::MessageId(task_id), contents);
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMpidManagerReceiver_));
}

void MpidNodeDispatcher::DeleteMessageRequest(
      const nfs_vault::MpidMessageAlert& mpid_message_alert) {
  typedef nfs::DeleteRequestFromMpidNodeToMpidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage nfs_message((NfsMessage::Contents(mpid_message_alert)));
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMpidManagerReceiver_));
}

void MpidNodeDispatcher::GetMessageRequest(routing::TaskId task_id,
                                           const nfs_vault::MpidMessageAlert& mpid_message_alert) {
  typedef nfs::GetMessageRequestFromMpidNodeToMpidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage::Contents contents(mpid_message_alert);
  NfsMessage nfs_message(nfs::MessageId(task_id), contents);
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMpidManagerReceiver_));
}

void MpidNodeDispatcher::SendCreateAccountRequest(
    routing::TaskId task_id,
    const nfs_vault::MpidAccountCreation& mpid_account_creation) {
  typedef nfs::CreateAccountRequestFromMpidNodeToMpidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage nfs_message(nfs::MessageId(task_id), mpid_account_creation);
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMpidManagerReceiver_));
}

void MpidNodeDispatcher::SendRemoveAccountRequest(
    const nfs_vault::MpidAccountRemoval& mpid_account_removal) {
  typedef nfs::RemoveAccountRequestFromMpidNodeToMpidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage nfs_message(mpid_account_removal);
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMpidManagerReceiver_));
}

}  // namespace nfs_client

}  // namespace maidsafe
