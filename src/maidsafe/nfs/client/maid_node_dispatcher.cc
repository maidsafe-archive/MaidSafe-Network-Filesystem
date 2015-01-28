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

#include "maidsafe/nfs/client/maid_node_dispatcher.h"

namespace maidsafe {

namespace nfs_client {

MaidNodeDispatcher::MaidNodeDispatcher(routing::Routing& routing)
    : running_(true),
      running_mutex_(),
      routing_(routing),
      kThisNodeAsSender_(routing_.kNodeId()),
      kMaidManagerReceiver_(routing_.kNodeId()) {}


void MaidNodeDispatcher::Stop() {
  std::lock_guard<std::mutex> lock(running_mutex_);
  running_ = false;
  LOG(kWarning) << " MaidNodeDispatcher::Stop() !";
}

void MaidNodeDispatcher::SendCreateAccountRequest(
    routing::TaskId task_id,
    const nfs_vault::MaidAccountCreation& maid_account_creation) {
  typedef nfs::CreateAccountRequestFromMaidNodeToMaidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage nfs_message(nfs::MessageId(task_id), maid_account_creation);
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMaidManagerReceiver_));
}

void MaidNodeDispatcher::SendRemoveAccountRequest(
    const nfs_vault::MaidAccountRemoval& maid_account_removal) {
  typedef nfs::RemoveAccountRequestFromMaidNodeToMaidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage nfs_message(maid_account_removal);
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMaidManagerReceiver_));
}

}  // namespace nfs_client

}  // namespace maidsafe
