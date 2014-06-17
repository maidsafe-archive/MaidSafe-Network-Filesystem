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

#ifndef MAIDSAFE_NFS_CLIENT_MAID_NODE_DISPATCHER_H_
#define MAIDSAFE_NFS_CLIENT_MAID_NODE_DISPATCHER_H_

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

class MaidNodeDispatcher {
 public:
  explicit MaidNodeDispatcher(routing::Routing& routing);

  void Stop();

  template <typename DataName>
  void SendGetRequest(routing::TaskId task_id, const DataName& data_name);

  template <typename Data>
  void SendPutRequest(routing::TaskId task_id, const Data& data,
                      const passport::PublicPmid::Name& pmid_node_hint);

  template <typename DataName>
  void SendDeleteRequest(const DataName& data_name);

  template <typename DataName>
  void SendCreateVersionTreeRequest(routing::TaskId task_id, const DataName& data_name,
                                    const StructuredDataVersions::VersionName& version_name,
                                    uint32_t max_versions, uint32_t max_branches);

  template <typename DataName>
  void SendGetVersionsRequest(routing::TaskId task_id, const DataName& data_name);

  template <typename DataName>
  void SendGetBranchRequest(routing::TaskId task_id, const DataName& data_name,
                            const StructuredDataVersions::VersionName& branch_tip);

  template <typename DataName>
  void SendPutVersionRequest(routing::TaskId task_id, const DataName& data_name,
                             const StructuredDataVersions::VersionName& old_version_name,
                             const StructuredDataVersions::VersionName& new_version_name);

  template <typename DataName>
  void SendDeleteBranchUntilForkRequest(const DataName& data_name,
                                        const StructuredDataVersions::VersionName& branch_tip);

  void SendCreateAccountRequest(routing::TaskId task_id,
                                const nfs_vault::AccountCreation& account_creation);

  void SendRemoveAccountRequest(const nfs_vault::AccountRemoval& account_removal);

  void SendRegisterPmidRequest(routing::TaskId task_id,
                               const nfs_vault::PmidRegistration& pmid_registration);

  void SendUnregisterPmidRequest(const passport::PublicPmid::Name& pmid_name);

  void SendPmidHealthRequest(routing::TaskId task_id, const passport::PublicPmid::Name& pmid_name);

 private:
  MaidNodeDispatcher();
  MaidNodeDispatcher(const MaidNodeDispatcher&);
  MaidNodeDispatcher(MaidNodeDispatcher&&);
  MaidNodeDispatcher& operator=(MaidNodeDispatcher);

  template <typename Message>
  void CheckSourcePersonaType() const;

  template <typename RoutingMessage>
  void RoutingSend(const RoutingMessage& routing_message);

  bool running_;
  std::mutex running_mutex_;
  routing::Routing& routing_;
  const routing::SingleSource kThisNodeAsSender_;
  const routing::GroupId kMaidManagerReceiver_;
};

// ==================== Implementation =============================================================
template <typename DataName>
void MaidNodeDispatcher::SendGetRequest(routing::TaskId task_id, const DataName& data_name) {
  typedef nfs::GetRequestFromMaidNodeToDataManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  static const routing::Cacheable kCacheable(is_cacheable<typename DataName::data_type>::value ?
      routing::Cacheable::kGet : routing::Cacheable::kNone);
  LOG(kVerbose) << "MaidNodeDispatcher::SendGetRequest for task_id " << task_id
                << ", data name: " << HexSubstr(data_name->string());
  nfs::MessageId message_id(task_id);
  NfsMessage::Contents content(data_name);
  NfsMessage nfs_message(message_id, content);
  NfsMessage::Receiver receiver(routing::GroupId(NodeId(data_name->string())));
  RoutingMessage routing_message(nfs_message.Serialise(), kThisNodeAsSender_, receiver, kCacheable);
  RoutingSend(routing_message);
}

template <typename Data>
void MaidNodeDispatcher::SendPutRequest(routing::TaskId task_id, const Data& data,
                                        const passport::PublicPmid::Name& pmid_node_hint) {
  LOG(kVerbose) << "MaidNodeDispatcher::SendPutRequest for chunk "
                << HexSubstr(data.name().value.string())
                << " to PmidHint " << HexSubstr(pmid_node_hint.value);
  typedef nfs::PutRequestFromMaidNodeToMaidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  NfsMessage::Contents contents;
  contents.data = nfs_vault::DataNameAndContent(data);
  contents.pmid_hint = pmid_node_hint.value;
  NfsMessage nfs_message(nfs::MessageId(task_id), contents);
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMaidManagerReceiver_));
}

template <typename DataName>
void MaidNodeDispatcher::SendDeleteRequest(const DataName& data_name) {
  typedef nfs::DeleteRequestFromMaidNodeToMaidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;

  NfsMessage nfs_message((NfsMessage::Contents(data_name)));
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMaidManagerReceiver_));
}

template <typename DataName>
void MaidNodeDispatcher::SendGetVersionsRequest(routing::TaskId task_id,
                                                const DataName& data_name) {
  typedef nfs::GetVersionsRequestFromMaidNodeToVersionHandler NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;

  nfs::MessageId message_id(task_id);
  NfsMessage::Contents content(data_name);
  NfsMessage nfs_message(message_id, content);
  NfsMessage::Receiver receiver(routing::GroupId(NodeId(data_name->string())));
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, receiver));
}

template <typename DataName>
void MaidNodeDispatcher::SendGetBranchRequest(
    routing::TaskId task_id, const DataName& data_name,
    const StructuredDataVersions::VersionName& branch_tip) {
  typedef nfs::GetBranchRequestFromMaidNodeToVersionHandler NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;

  NfsMessage::Contents contents(data_name, branch_tip);
  NfsMessage nfs_message(nfs::MessageId(task_id), contents);
  NfsMessage::Receiver receiver(routing::GroupId(NodeId(data_name->string())));
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, receiver));
}

template <typename DataName>
void MaidNodeDispatcher::SendCreateVersionTreeRequest(
    routing::TaskId task_id, const DataName& data_name,
    const StructuredDataVersions::VersionName& version_name, uint32_t max_versions,
    uint32_t max_branches) {
  typedef nfs::CreateVersionTreeRequestFromMaidNodeToMaidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;

  NfsMessage::Contents contents(data_name, version_name, max_versions, max_branches);
  NfsMessage nfs_message(nfs::MessageId(task_id), contents);
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMaidManagerReceiver_));
}

template <typename DataName>
void MaidNodeDispatcher::SendPutVersionRequest(routing::TaskId task_id,
    const DataName& data_name,
    const StructuredDataVersions::VersionName& old_version_name,
    const StructuredDataVersions::VersionName& new_version_name) {
  typedef nfs::PutVersionRequestFromMaidNodeToMaidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;

  NfsMessage nfs_message(nfs::MessageId(task_id),
                         NfsMessage::Contents(data_name, old_version_name, new_version_name));
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMaidManagerReceiver_));
}

template <typename DataName>
void MaidNodeDispatcher::SendDeleteBranchUntilForkRequest(
    const DataName& data_name, const StructuredDataVersions::VersionName& branch_tip) {
  typedef nfs::DeleteBranchUntilForkRequestFromMaidNodeToMaidManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;

  NfsMessage::Contents contents(data_name, branch_tip);
  NfsMessage nfs_message(contents);
  RoutingSend(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, kMaidManagerReceiver_));
}

template <typename Message>
void MaidNodeDispatcher::CheckSourcePersonaType() const {
  static_assert(Message::SourcePersona::value == nfs::Persona::kMaidNode,
                "The source Persona must be kMaidNode.");
}

template <typename RoutingMessage>
void MaidNodeDispatcher::RoutingSend(const RoutingMessage& routing_message) {
  std::lock_guard<std::mutex> lock(running_mutex_);
  if (!running_) {
    LOG(kWarning) << " Shutting down. Send ignored !";
    return;
  }
  routing_.Send(routing_message);
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MAID_NODE_DISPATCHER_H_
