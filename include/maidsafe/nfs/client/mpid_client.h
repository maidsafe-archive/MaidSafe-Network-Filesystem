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

#ifndef MAIDSAFE_NFS_CLIENT_MPID_CLIENT_H_
#define MAIDSAFE_NFS_CLIENT_MPID_CLIENT_H_

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "boost/signals2/signal.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

#include "boost/thread/future.hpp"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "maidsafe/common/asio_service.h"
#include "maidsafe/common/data_types/structured_data_versions.h"
#include "maidsafe/passport/passport.h"
#include "maidsafe/passport/types.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/message_wrapper.h"
#include "maidsafe/nfs/service.h"
#include "maidsafe/nfs/utils.h"
#include "maidsafe/nfs/client/client_utils.h"
#include "maidsafe/nfs/client/mpid_node_dispatcher.h"
#include "maidsafe/nfs/client/mpid_node_service.h"
#include "maidsafe/nfs/client/get_handler.h"

namespace maidsafe {

namespace nfs_client {

class MpidClient : public std::enable_shared_from_this<MpidClient>  {
 public:
  typedef boost::signals2::signal<void(int32_t)> OnNetworkHealthChange;

  // Logging in for already existing mpid accounts
  static std::shared_ptr<MpidClient> MakeShared(const passport::Mpid& mpid);
  // Creates mpid account and logs in. Throws on failure to create account.
  static std::shared_ptr<MpidClient> MakeShared(const passport::MpidAndSigner& mpid_and_signer);
  // Disconnects from network and all unfinished tasks will be cancelled

  MpidClient(const MpidClient&) = delete;
  MpidClient(MpidClient&&) = delete;
  MpidClient& operator=(MpidClient) = delete;

  void Stop();

  OnNetworkHealthChange& network_health_change_signal();

  boost::future<void> SendMessage(const nfs_vault::MpidMessage& mpid_message,
      const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(360));

  void DeleteMessage(const nfs_vault::MpidMessageAlert& mpid_message_alert);

  boost::future<typename nfs_vault::MpidMessage> GetMessage(
      const nfs_vault::MpidMessageAlert& mpid_message_alert,
      const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(120));

  boost::future<void> CreateAccount(const nfs_vault::MpidAccountCreation& account_creation,
                                    const std::chrono::steady_clock::duration& timeout =
                                        std::chrono::seconds(240));

  void RemoveAccount(const nfs_vault::MpidAccountRemoval& account_removal);

  template <typename DataName>
  boost::future<typename DataName::data_type> Get(
      const DataName& data_name,
      const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(120));

 private:
  explicit MpidClient(const passport::Mpid& mpid);

  void Init(const passport::MpidAndSigner& mpid_and_signer);
  void Init();
  void InitRouting();

  void CreateAccount(const passport::PublicMpid& public_mpid,
                     const passport::PublicAnmpid& public_anmpid);

  routing::Functors InitialiseRoutingCallbacks();
  void OnNetworkStatusChange(int updated_network_health);

  template <typename T>
  void OnMessageReceived(const T& routing_message);

  template <typename T>
  void HandleMessage(const T& routing_message);

  const passport::Mpid kMpid_;
  BoostAsioService asio_service_;
  MpidNodeService::RpcTimers rpc_timers_;
  std::mutex network_health_mutex_;
  std::condition_variable network_health_condition_variable_;
  int network_health_;
  OnNetworkHealthChange network_health_change_signal_;
  std::unique_ptr<routing::Routing> routing_;
  nfs::detail::PublicPmidHelper public_pmid_helper_;
  MpidNodeDispatcher dispatcher_;
  GetHandler<MpidNodeDispatcher> get_handler_;
  nfs::Service<MpidNodeService> service_;
};

// ==================== Implementation =============================================================

template <typename DataName>
boost::future<typename DataName::data_type> MpidClient::Get(
    const DataName& data_name,
    const std::chrono::steady_clock::duration& timeout) {
  auto promise(std::make_shared<boost::promise<typename DataName::data_type>>());
  get_handler_.Get(data_name, promise, timeout);
  return promise->get_future();
}

template <typename T>
void MpidClient::OnMessageReceived(const T& routing_message) {
  std::shared_ptr<MpidClient> this_ptr(shared_from_this());
  asio_service_.service().post([=] {
      this_ptr->HandleMessage(routing_message);
  });
}

template <typename T>
void MpidClient::HandleMessage(const T& routing_message) {
  auto wrapper_tuple(nfs::ParseMessageWrapper(routing_message.contents));
  const auto& destination_persona(std::get<2>(wrapper_tuple));
  static_assert(std::is_same<decltype(destination_persona),
                             const nfs::detail::DestinationTaggedValue&>::value,
                "The value retrieved from the tuple isn't the destination type, but should be.");
  if (destination_persona.data == nfs::Persona::kMpidNode)
    return service_.HandleMessage(wrapper_tuple, routing_message.sender, routing_message.receiver);
  auto action(std::get<0>(wrapper_tuple));
  auto source_persona(std::get<1>(wrapper_tuple).data);
  LOG(kError) << " MpidClient::HandleMessage unhandled message from " << source_persona
              << " " << action << " to " << destination_persona;
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MPID_CLIENT_H_
