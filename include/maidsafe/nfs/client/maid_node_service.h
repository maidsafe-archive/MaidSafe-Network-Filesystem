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

#ifndef MAIDSAFE_NFS_CLIENT_MAID_NODE_SERVICE_H_
#define MAIDSAFE_NFS_CLIENT_MAID_NODE_SERVICE_H_

#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"

namespace maidsafe {

namespace nfs_client {

template <typename DispatcherType>
class GetHandler;

class MaidNodeService {
 public:
  typedef nfs::MaidNodeServiceMessages PublicMessages;
  typedef void VaultMessages;
  typedef void HandleMessageReturnType;

  typedef nfs::PutResponseFromMaidManagerToMaidNode PutResponse;
  typedef nfs::PutFailureFromMaidManagerToMaidNode PutFailure;
  typedef nfs::GetVersionsResponseFromVersionHandlerToMaidNode GetVersionsResponse;
  typedef nfs::PutVersionResponseFromMaidManagerToMaidNode PutVersionResponse;
  typedef nfs::GetBranchResponseFromVersionHandlerToMaidNode GetBranchResponse;
  typedef nfs::CreateAccountResponseFromMaidManagerToMaidNode CreateAccountResponse;
  typedef nfs::CreateVersionTreeResponseFromMaidManagerToMaidNode CreateVersionTreeResponse;

  struct RpcTimers {
    explicit RpcTimers(BoostAsioService& asio_service_);
    void CancellAll();

    routing::Timer<PutResponse::Contents> put_timer;
    routing::Timer<GetVersionsResponse::Contents> get_versions_timer;
    routing::Timer<GetBranchResponse::Contents> get_branch_timer;
    routing::Timer<CreateAccountResponse::Contents> create_account_timer;
    routing::Timer<CreateVersionTreeResponse::Contents> create_version_tree_timer;
    routing::Timer<PutVersionResponse::Contents> put_version_timer;
  };

  MaidNodeService(const routing::SingleId& receiver, RpcTimers& rpc_timers);

  void HandleMessage(const PutResponse& message, const PutResponse::Sender& sender,
                     const PutResponse::Receiver& receiver);

  void HandleMessage(const PutFailure& message, const PutFailure::Sender& sender,
                     const PutFailure::Receiver& receiver);

  void HandleMessage(const GetVersionsResponse& message, const GetVersionsResponse::Sender& sender,
                     const GetVersionsResponse::Receiver& receiver);

  void HandleMessage(const PutVersionResponse& message, const PutVersionResponse::Sender& sender,
                     const PutVersionResponse::Receiver& receiver);

  void HandleMessage(const GetBranchResponse& message, const GetBranchResponse::Sender& sender,
                     const GetBranchResponse::Receiver& receiver);

  void HandleMessage(const CreateAccountResponse& message,
                     const CreateAccountResponse::Sender& sender,
                     const CreateAccountResponse::Receiver& receiver);

  void HandleMessage(const CreateVersionTreeResponse& message,
                     const CreateVersionTreeResponse::Sender& sender,
                     const CreateVersionTreeResponse::Receiver& receiver);

 private:
  template <typename Data>
  void HandlePutResponse(const nfs::PutRequestFromMaidNodeToMaidManager& message,
                         const typename nfs::PutRequestFromMaidNodeToMaidManager::Sender& sender);

  const routing::SingleId kReceiver_;
  RpcTimers& rpc_timers_;
};

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MAID_NODE_SERVICE_H_
