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

#ifndef MAIDSAFE_NFS_CLIENT_MPID_NODE_SERVICE_H_
#define MAIDSAFE_NFS_CLIENT_MPID_NODE_SERVICE_H_

#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"
#include "maidsafe/nfs/client/get_handler.h"

namespace maidsafe {

namespace nfs_client {

template <typename DispatcherType>
class GetHandler;

class MpidNodeService {
 public:
  typedef nfs::MpidNodeServiceMessages PublicMessages;
  typedef void VaultMessages;
  typedef void HandleMessageReturnType;

  typedef nfs::SendAlertFromMpidManagerToMpidNode MessageAlert;
  typedef nfs::SendMessageResponseFromMpidManagerToMpidNode SendMessageResponse;
  typedef nfs::GetMessageResponseFromMpidManagerToMpidNode GetMessageResponse;
  typedef nfs::CreateAccountResponseFromMpidManagerToMpidNode CreateAccountResponse;
  typedef nfs::GetResponseFromDataManagerToMpidNode GetResponse;
  typedef nfs::GetCachedResponseFromCacheHandlerToMpidNode GetCachedResponse;

  struct RpcTimers {
    explicit RpcTimers(BoostAsioService& asio_service_);
    void CancellAll();

    routing::Timer<MessageAlert::Contents> message_alert_timer;
    routing::Timer<GetMessageResponse::Contents> get_message_timer;
    routing::Timer<SendMessageResponse::Contents> send_message_timer;
    routing::Timer<CreateAccountResponse::Contents> create_account_timer;
    routing::Timer<GetResponse::Contents> get_timer;
  };

  MpidNodeService(const routing::SingleId& receiver, RpcTimers& rpc_timers,
                  GetHandler<MpidNodeDispatcher>& get_handler);

  void HandleMessage(const MessageAlert& message, const MessageAlert::Sender& sender,
                     const MessageAlert::Receiver& receiver);

  void HandleMessage(const GetMessageResponse& message, const GetMessageResponse::Sender& sender,
                     const GetMessageResponse::Receiver& receiver);

  void HandleMessage(const SendMessageResponse& message, const SendMessageResponse::Sender& sender,
                     const SendMessageResponse::Receiver& receiver);

  void HandleMessage(const CreateAccountResponse& message,
                     const CreateAccountResponse::Sender& sender,
                     const CreateAccountResponse::Receiver& receiver);

  void HandleMessage(const GetResponse& message, const GetResponse::Sender& sender,
                     const GetResponse::Receiver& receiver);

  void HandleMessage(const GetCachedResponse& message, const GetCachedResponse::Sender& sender,
                     const GetCachedResponse::Receiver& receiver);

 private:
  const routing::SingleId kReceiver_;
  RpcTimers& rpc_timers_;
  GetHandler<MpidNodeDispatcher>& get_handler_;
};

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MPID_NODE_SERVICE_H_
