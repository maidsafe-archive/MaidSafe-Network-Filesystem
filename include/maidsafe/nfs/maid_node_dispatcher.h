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

#ifndef MAIDSAFE_NFS_MAID_NODE_DISPATCHER_H_
#define MAIDSAFE_NFS_MAID_NODE_DISPATCHER_H_

#include "maidsafe/common/error.h"
#include "maidsafe/common/types.h"
#include "maidsafe/passport/types.h"
#include "maidsafe/data_types/data_type_values.h"
#include "maidsafe/data_types/structured_data_versions.h"
#include "maidsafe/routing/message.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

class OwnerDirectory;
class GroupDirectory;
class WorldDirectory;

namespace nfs {

class MaidNodeDispatcher {
 public:
  MaidNodeDispatcher(routing::Routing& routing, const passport::Maid& signing_fob);

  template<typename Data>
  void SendGetRequest(routing::TaskId task_id, const typename Data::Name& data_name);

  template<typename Data>
  void SendPutRequest(const Data& data, const passport::Pmid::Name& pmid_node_hint);

  template<typename Data>
  void SendDeleteRequest(const typename Data::Name& data_name);

  template<typename Data>
  void SendGetVersionsRequest(const typename Data::Name& data_name);

  template<typename Data>
  void SendGetBranchRequest(const typename Data::Name& data_name,
                            const StructuredDataVersions::VersionName& branch_tip);

  template<typename Data>
  void SendPutVersionRequest(const typename Data::Name& data_name,
                             const StructuredDataVersions::VersionName& old_version_name,
                             const StructuredDataVersions::VersionName& new_version_name);

  template<typename Data>
  void SendDeleteBranchUntilForkRequest(const typename Data::Name& data_name,
                                        const StructuredDataVersions::VersionName& branch_tip);

  void SendCreateAccountRequest();

  void SendRemoveAccountRequest();

  void SendRegisterPmidRequest(const passport::Pmid& pmid);

  void SendUnregisterPmidRequest(const passport::Pmid& pmid);

 private:
  MaidNodeDispatcher();
  MaidNodeDispatcher(const MaidNodeDispatcher&);
  MaidNodeDispatcher(MaidNodeDispatcher&&);
  MaidNodeDispatcher& operator=(MaidNodeDispatcher);

  routing::SingleSource Sender() const;

  routing::Routing& routing_;
  const passport::Pmid kSigningFob_;
  static const nfs::Persona kSourcePersona_;
};



// ==================== Implementation =============================================================
template<typename Data>
void MaidNodeDispatcher::SendGetRequest(routing::TaskId task_id,
                                        const typename Data::Name& data_name) {
  typedef GetRequestFromMaidNodeToDataManager NfsMessage;
  typedef routing::Message<NfsMessage::Sender, NfsMessage::Receiver> RoutingMessage;
  static const NfsMessage::Sender kSender(routing::SingleId(routing_.kNodeId()));
  static const routing::Cacheable kCacheable(is_cacheable<Data>::value ? routing::Cacheable::kGet :
                                                                         routing::Cacheable::kNone);
  NfsMessage nfs_message((MessageId(task_id), NfsMessage::Contents(data_name)));
  NfsMessage::Receiver receiver(routing::GroupId(NodeId(data_name->string())));
  RoutingMessage routing_message(nfs_message.Serialise(), kSender, receiver, kCacheable);
  routing_.Send(routing_message);
}

template<typename Data>
void MaidNodeDispatcher::SendGetVersionsRequest(const typename Data::Name& /*data_name*/) {
}

template<typename Data>
void MaidNodeDispatcher::SendPutRequest(const Data& /*data*/,
                                        const passport::Pmid::Name& /*pmid_node_hint*/) {
}

template<typename Data>
void MaidNodeDispatcher::SendDeleteRequest(const typename Data::Name& /*data_name*/) {
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MAID_NODE_DISPATCHER_H_
