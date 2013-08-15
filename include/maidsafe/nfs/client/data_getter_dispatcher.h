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

#ifndef MAIDSAFE_NFS_CLIENT_DATA_GETTER_DISPATCHER_H_
#define MAIDSAFE_NFS_CLIENT_DATA_GETTER_DISPATCHER_H_

#include "maidsafe/data_types/structured_data_versions.h"
#include "maidsafe/routing/message.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/types.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"


namespace maidsafe {

namespace nfs_client {

class DataGetterDispatcher {
 public:
  explicit DataGetterDispatcher(routing::Routing& routing);

  template<typename Data>
  void SendGetRequest(routing::TaskId task_id, const typename Data::Name& data_name);

  template<typename Data>
  void SendGetVersionsRequest(routing::TaskId task_id, const typename Data::Name& data_name);

  template<typename Data>
  void SendGetBranchRequest(routing::TaskId task_id,
                            const typename Data::Name& data_name,
                            const StructuredDataVersions::VersionName& branch_tip);

 private:
  DataGetterDispatcher();
  DataGetterDispatcher(const DataGetterDispatcher&);
  DataGetterDispatcher(DataGetterDispatcher&&);
  DataGetterDispatcher& operator=(DataGetterDispatcher);

  template<typename Message>
  void CheckSourcePersonaType() const;

  routing::Routing& routing_;
  const routing::SingleSource kThisNodeAsSender_;
};



// ==================== Implementation =============================================================
template<typename Data>
void DataGetterDispatcher::SendGetRequest(routing::TaskId task_id,
                                          const typename Data::Name& data_name) {
  typedef nfs::GetRequestFromDataGetterToDataManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  static const routing::Cacheable kCacheable(is_cacheable<Data>::value ? routing::Cacheable::kGet :
                                                                         routing::Cacheable::kNone);
  NfsMessage nfs_message((nfs::MessageId(task_id), NfsMessage::Contents(data_name)));
  NfsMessage::Receiver receiver(routing::GroupId(NodeId(data_name->string())));
  RoutingMessage routing_message(nfs_message.Serialise(), kThisNodeAsSender_, receiver, kCacheable);
  routing_.Send(routing_message);
}

template<typename Data>
void DataGetterDispatcher::SendGetVersionsRequest(routing::TaskId task_id,
                                                  const typename Data::Name& data_name) {
  typedef nfs::GetVersionsRequestFromDataGetterToVersionManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;

  NfsMessage nfs_message((nfs::MessageId(task_id), NfsMessage::Contents(data_name)));
  NfsMessage::Receiver receiver(routing::GroupId(NodeId(data_name->string())));
  routing_.Send(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, receiver));
}

template<typename Data>
void DataGetterDispatcher::SendGetBranchRequest(
    routing::TaskId task_id,
    const typename Data::Name& data_name,
    const StructuredDataVersions::VersionName& branch_tip) {
  typedef nfs::GetBranchRequestFromDataGetterToVersionManager NfsMessage;
  CheckSourcePersonaType<NfsMessage>();
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;

  NfsMessage::Contents contents;
  contents.data_name = DataName(data_name);
  contents.version_name = branch_tip;
  NfsMessage nfs_message(nfs::MessageId(task_id), contents);
  NfsMessage::Receiver receiver(routing::GroupId(NodeId(data_name->string())));
  routing_.Send(RoutingMessage(nfs_message.Serialise(), kThisNodeAsSender_, receiver));
}

template<typename Message>
void DataGetterDispatcher::CheckSourcePersonaType() const {
  static_assert(Message::SourcePersona::value == nfs::Persona::kDataGetter,
                "The source Persona must be kDataGetter.");
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_DATA_GETTER_DISPATCHER_H_
