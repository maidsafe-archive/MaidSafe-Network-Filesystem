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

#ifndef MAIDSAFE_NFS_CLIENT_MPID_NODE_DISPATCHER_H_
#define MAIDSAFE_NFS_CLIENT_MPID_NODE_DISPATCHER_H_

#include <string>

#include "maidsafe/common/error.h"
#include "maidsafe/common/types.h"
#include "maidsafe/common/data_types/data_type_values.h"
#include "maidsafe/common/data_types/structured_data_versions.h"
#include "maidsafe/passport/types.h"
#include "maidsafe/routing/message.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/types.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"

namespace maidsafe {

namespace nfs_client {

class MpidNodeDispatcher {
 public:
  explicit MpidNodeDispatcher(routing::Routing& routing);

  MpidNodeDispatcher() = delete;
  MpidNodeDispatcher(const MpidNodeDispatcher&) = delete;
  MpidNodeDispatcher(MpidNodeDispatcher&&) = delete;
  MpidNodeDispatcher& operator=(MpidNodeDispatcher) = delete;

  void Stop();

  void SendMessageRequest(routing::TaskId task_id, const nfs_vault::MpidMessage& mpid_message);
  void DeleteMessageRequest(const nfs_vault::MpidMessageAlert& mpid_message_alert);
  void GetMessageRequest(routing::TaskId task_id,
                         const nfs_vault::MpidMessageAlert& mpid_message_alert);

  void SendCreateAccountRequest(routing::TaskId task_id,
                                const nfs_vault::MpidAccountCreation& account_creation);

  void SendRemoveAccountRequest(const nfs_vault::MpidAccountRemoval& account_removal);

  template <typename DataName>
  void SendGetRequest(routing::TaskId task_id, const DataName& data_name);

 private:
  template <typename Message>
  void CheckSourcePersonaType() const;

  template <typename RoutingMessage>
  void RoutingSend(const RoutingMessage& routing_message);

  bool running_;
  std::mutex running_mutex_;
  routing::Routing& routing_;
  const routing::SingleSource kThisNodeAsSender_;
  const routing::GroupId kMpidManagerReceiver_;
};

// ==================== Implementation =============================================================

template <typename DataName>
void MpidNodeDispatcher::SendGetRequest(routing::TaskId task_id, const DataName& data_name) {
  typedef nfs::GetRequestFromMpidNodeToDataManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  static const routing::Cacheable kCacheable(is_cacheable<typename DataName::data_type>::value ?
      routing::Cacheable::kGet : routing::Cacheable::kNone);
  nfs::MessageId message_id(task_id);
  NfsMessage::Contents content(data_name);
  NfsMessage nfs_message(message_id, content);
  NfsMessage::Receiver receiver(routing::GroupId(NodeId(data_name->string())));
  RoutingMessage routing_message(nfs_message.Serialise(), kThisNodeAsSender_, receiver, kCacheable);
  RoutingSend(routing_message);
}

template <typename Message>
void MpidNodeDispatcher::CheckSourcePersonaType() const {
  static_assert(Message::SourcePersona::value == nfs::Persona::kMpidNode,
                "The source Persona must be kMpidNode.");
}

template <typename RoutingMessage>
void MpidNodeDispatcher::RoutingSend(const RoutingMessage& routing_message) {
  std::lock_guard<std::mutex> lock(running_mutex_);
  if (!running_) {
    LOG(kWarning) << " Shutting down. Send ignored !";
    return;
  }
  routing_.Send(routing_message);
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MPID_NODE_DISPATCHER_H_
