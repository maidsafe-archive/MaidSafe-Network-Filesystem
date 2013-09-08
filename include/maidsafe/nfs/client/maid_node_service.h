/*  Copyright 2013 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.novinet.com/license

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#ifndef MAIDSAFE_NFS_CLIENT_MAID_NODE_SERVICE_H_
#define MAIDSAFE_NFS_CLIENT_MAID_NODE_SERVICE_H_

#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"


namespace maidsafe {

namespace nfs_client {

class MaidNodeService {
 public:
  typedef nfs::MaidNodeServiceMessages PublicMessages;
  typedef void VaultMessages;

  typedef nfs::GetResponseFromDataManagerToMaidNode GetResponse;
  typedef nfs::GetVersionsResponseFromVersionManagerToMaidNode GetVersionsResponse;
  typedef nfs::GetBranchResponseFromVersionManagerToMaidNode GetBranchResponse;
  typedef nfs::PutResponseFromMaidManagerToMaidNode PutResponse;

  MaidNodeService(
      routing::Routing& routing,
      routing::Timer<MaidNodeService::GetResponse::Contents>& get_timer,
      routing::Timer<MaidNodeService::GetVersionsResponse::Contents>& get_versions_timer,
      routing::Timer<MaidNodeService::GetBranchResponse::Contents>& get_branch_timer);

  template<typename T>
  void HandleMessage(const T& /*message*/,
                     const typename T::Sender& /*sender*/,
                     const typename T::Receiver& /*receiver*/) {
    T::invalid_message_type_passed::should_be_one_of_the_specialisations_defined_below;
  }

 private:
  template<typename Data>
  void HandlePutResponse(const nfs::PutRequestFromMaidNodeToMaidManager& message,
                         const typename nfs::PutRequestFromMaidNodeToMaidManager::Sender& sender);

  routing::Routing& routing_;
  routing::Timer<MaidNodeService::GetResponse::Contents>& get_timer_;
  routing::Timer<MaidNodeService::GetVersionsResponse::Contents>& get_versions_timer_;
  routing::Timer<MaidNodeService::GetBranchResponse::Contents>& get_branch_timer_;
};

template<>
void MaidNodeService::HandleMessage<MaidNodeService::GetResponse>(
    const GetResponse& message,
    const typename GetResponse::Sender& sender,
    const typename GetResponse::Receiver& receiver);

template<>
void MaidNodeService::HandleMessage<MaidNodeService::GetVersionsResponse>(
    const GetVersionsResponse& message,
    const typename GetVersionsResponse::Sender& sender,
    const typename GetVersionsResponse::Receiver& receiver);

template<>
void MaidNodeService::HandleMessage<MaidNodeService::GetBranchResponse>(
    const GetBranchResponse& message,
    const typename GetBranchResponse::Sender& sender,
    const typename GetBranchResponse::Receiver& receiver);

template<>
void MaidNodeService::HandleMessage<MaidNodeService::PutResponse>(
    const PutResponse& message,
    const typename PutResponse::Sender& sender,
    const typename PutResponse::Receiver& receiver);

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MAID_NODE_SERVICE_H_
