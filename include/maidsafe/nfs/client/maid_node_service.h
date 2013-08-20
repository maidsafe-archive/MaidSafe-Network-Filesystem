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

#ifndef MAIDSAFE_NFS_CLIENT_MAID_NODE_SERVICE_H_
#define MAIDSAFE_NFS_CLIENT_MAID_NODE_SERVICE_H_

#include "maidsafe/routing/routing_api.h"
#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"


namespace maidsafe {

namespace nfs_client {

class MaidNodeService {
 public:
  typedef nfs::MaidNodeServiceMessages PublicMessages;
  typedef void VaultMessages;

  explicit MaidNodeService(routing::Routing& routing);

  template<typename T>
  void HandleMessage(const T& message,
                     const typename T::Sender& sender,
                     const typename T::Receiver& receiver) {
    T::invalid_message_type_passed::should_be_one_of_the_specialisations_defined_below;
  }

 private:
  template<typename Data>
  void HandlePutResponse(const nfs::PutRequestFromMaidNodeToMaidManager& message,
                         const typename nfs::PutRequestFromMaidNodeToMaidManager::Sender& sender);

  routing::Routing& routing_;
};



template<>
void MaidNodeService::HandleMessage<nfs::GetResponseFromDataManagerToMaidNode>(
    const nfs::GetResponseFromDataManagerToMaidNode& message,
    const typename nfs::GetResponseFromDataManagerToMaidNode::Sender& sender,
    const typename nfs::GetResponseFromDataManagerToMaidNode::Receiver& receiver);

template<>
void MaidNodeService::HandleMessage<nfs::PutResponseFromMaidManagerToMaidNode>(
    const nfs::PutResponseFromMaidManagerToMaidNode& message,
    const typename nfs::PutResponseFromMaidManagerToMaidNode::Sender& sender,
    const typename nfs::PutResponseFromMaidManagerToMaidNode::Receiver& receiver);

template<>
void MaidNodeService::HandleMessage<nfs::GetVersionsResponseFromVersionManagerToMaidNode>(
    const nfs::GetVersionsResponseFromVersionManagerToMaidNode& message,
    const typename nfs::GetVersionsResponseFromVersionManagerToMaidNode::Sender& sender,
    const typename nfs::GetVersionsResponseFromVersionManagerToMaidNode::Receiver& receiver);

template<>
void MaidNodeService::HandleMessage<nfs::GetBranchResponseFromVersionManagerToMaidNode>(
    const nfs::GetBranchResponseFromVersionManagerToMaidNode& message,
    const typename nfs::GetBranchResponseFromVersionManagerToMaidNode::Sender& sender,
    const typename nfs::GetBranchResponseFromVersionManagerToMaidNode::Receiver& receiver);

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MAID_NODE_SERVICE_H_
