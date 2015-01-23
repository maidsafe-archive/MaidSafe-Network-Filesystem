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

#include "maidsafe/nfs/client/mpid_client.h"

#include "maidsafe/common/on_scope_exit.h"
#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/make_unique.h"

namespace maidsafe {

namespace nfs_client {

std::shared_ptr<MpidClient> MpidClient::MakeShared(const passport::Mpid& mpid) {
  std::shared_ptr<MpidClient> mpid_node_ptr{ new MpidClient{ mpid } };
  mpid_node_ptr->Init();
  return mpid_node_ptr;
}

std::shared_ptr<MpidClient> MpidClient::MakeShared(
    const passport::MpidAndSigner& mpid_and_signer) {
  std::shared_ptr<MpidClient> mpid_node_ptr{ new MpidClient{ mpid_and_signer.first } };
  mpid_node_ptr->Init(mpid_and_signer);
  return mpid_node_ptr;
}

MpidClient::MpidClient(const passport::Mpid& mpid)
    : kMpid_(mpid),
      asio_service_(2),
      rpc_timers_(asio_service_),
      network_health_mutex_(),
      network_health_condition_variable_(),
      network_health_(-1),
      network_health_change_signal_(),
      routing_(maidsafe::make_unique<routing::Routing>(kMpid_)),
      public_pmid_helper_(),
      dispatcher_(*routing_),
      get_handler_(rpc_timers_.get_timer, dispatcher_),
      service_([&]()->std::unique_ptr<MpidNodeService> {
        std::unique_ptr<MpidNodeService> service(
          new MpidNodeService(routing::SingleId(routing_->kNodeId()), rpc_timers_, get_handler_));
        return std::move(service);
      }()) {}

void MpidClient::Stop() {
  dispatcher_.Stop();
  routing_.reset();
  rpc_timers_.CancellAll();
  asio_service_.Stop();
}

MpidClient::OnNetworkHealthChange& MpidClient::network_health_change_signal() {
  return network_health_change_signal_;
}

boost::future<void> MpidClient::SendMessage(const nfs_vault::MpidMessage& mpid_message,
                                            const std::chrono::steady_clock::duration& timeout) {
  typedef MpidNodeService::SendMessageResponse::Contents ResponseContents;
  auto promise(std::make_shared<boost::promise<void>>());
  NodeId node_id;

  auto response_functor([promise](const nfs_client::ReturnCode& result) {
                           HandleSendMessageResponseResult(result, promise);
                        });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(routing::Parameters::group_size - 1,
                                                               response_functor));
  auto task_id(rpc_timers_.send_message_timer.NewTaskId());
  rpc_timers_.send_message_timer.AddTask(
      timeout,
      [op_data](ResponseContents send_message_response) {
        op_data->HandleResponseContents(std::move(send_message_response));
      },
      routing::Parameters::group_size - 1, task_id);
  rpc_timers_.send_message_timer.PrintTaskIds();
  dispatcher_.SendMessageRequest(task_id, mpid_message);
  return promise->get_future();
}

void MpidClient::DeleteMessage(const nfs_vault::MpidMessageAlert& mpid_message_alert) {
  dispatcher_.DeleteMessageRequest(mpid_message_alert);
}

boost::future<typename nfs_vault::MpidMessage> MpidClient::GetMessage(
      const nfs_vault::MpidMessageAlert& mpid_message_alert,
      const std::chrono::steady_clock::duration& timeout) {
  typedef MpidNodeService::GetMessageResponse::Contents ResponseContents;
  auto promise(std::make_shared<boost::promise<nfs_vault::MpidMessage>>());
  NodeId node_id;

  auto response_functor([promise](const MpidMessageOrReturnCode& result) {
                           HandleGetMessageResponseResult(result, promise);
                        });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(routing::Parameters::group_size - 1,
                                                               response_functor));
  auto task_id(rpc_timers_.get_message_timer.NewTaskId());
  rpc_timers_.get_message_timer.AddTask(
      timeout,
      [op_data](ResponseContents get_message_response) {
        op_data->HandleResponseContents(std::move(get_message_response));
      },
      routing::Parameters::group_size - 1, task_id);
  rpc_timers_.get_message_timer.PrintTaskIds();
  dispatcher_.GetMessageRequest(task_id, mpid_message_alert);
  return promise->get_future();
}

boost::future<void> MpidClient::CreateAccount(
    const nfs_vault::MpidAccountCreation& account_creation,
    const std::chrono::steady_clock::duration& timeout) {
  typedef MpidNodeService::CreateAccountResponse::Contents ResponseContents;
  auto promise(std::make_shared<boost::promise<void>>());
  auto response_functor([promise](const ResponseContents &result) {
      HandleCreateAccountResult(result, promise);
  });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(
      routing::Parameters::group_size - 1, response_functor));
  auto task_id(rpc_timers_.create_account_timer.NewTaskId());
  rpc_timers_.create_account_timer.AddTask(
      timeout, [op_data](ResponseContents create_account_response) {
                 op_data->HandleResponseContents(std::move(create_account_response));
               },
      routing::Parameters::group_size - 1, task_id);
  dispatcher_.SendCreateAccountRequest(task_id, account_creation);
  return promise->get_future();
}

void MpidClient::CreateAccount(const passport::PublicMpid& public_mpid,
                               const passport::PublicAnmpid& public_anmpid) {
  nfs_vault::MpidAccountCreation account_creation{ public_mpid, public_anmpid };
  auto create_account_future = CreateAccount(account_creation);
  create_account_future.get();
}

void MpidClient::RemoveAccount(const nfs_vault::MpidAccountRemoval& account_removal) {
  dispatcher_.SendRemoveAccountRequest(account_removal);
}

void MpidClient::Init(const passport::MpidAndSigner& mpid_and_signer) {
  on_scope_exit cleanup_on_error([&] { Stop(); });
  InitRouting();
  CreateAccount(passport::PublicMpid{ mpid_and_signer.first },
                passport::PublicAnmpid{ mpid_and_signer.second });
  cleanup_on_error.Release();
}

void MpidClient::Init() {
  on_scope_exit cleanup_on_error([&] { Stop(); });
  InitRouting();
  cleanup_on_error.Release();
}

void MpidClient::InitRouting() {
  routing::Functors functors(InitialiseRoutingCallbacks());
  routing_->Join(functors);
  // FIXME BEFORE_RELEASE discuss: parallel attempts, max no. of endpoints to try,
  // prioritise live ports. To reduce the blocking duration in case of no network connectivity
  std::unique_lock<std::mutex> lock{ network_health_mutex_ };
  network_health_condition_variable_.wait(lock, [this] {
    return (network_health_ == 100) || (network_health_ < -300000); });
  if (network_health_ < 0)
    BOOST_THROW_EXCEPTION(MakeError(RoutingErrors::not_connected));
}

routing::Functors MpidClient::InitialiseRoutingCallbacks() {
  routing::Functors functors;
  std::shared_ptr<MpidClient> this_ptr(shared_from_this());
  functors.typed_message_and_caching.single_to_single.message_received =
      [this_ptr](const routing::SingleToSingleMessage& message) {
        this_ptr->OnMessageReceived(message);
      };
  functors.typed_message_and_caching.group_to_single.message_received =
      [this_ptr](const routing::GroupToSingleMessage& message) {
        this_ptr->OnMessageReceived(message);
      };

  functors.network_status =
      [this_ptr](const int& network_health) { this_ptr->OnNetworkStatusChange(network_health); };
  functors.close_nodes_change = [](std::shared_ptr<routing::CloseNodesChange> /*close_change*/) {};
  functors.request_public_key = [this_ptr](const NodeId& node_id,
                                       const routing::GivePublicKeyFunctor& give_key) {
    auto future_key(this_ptr->Get(passport::PublicPmid::Name{ Identity{ node_id.string() } },
                                        std::chrono::seconds(10)));
    this_ptr->public_pmid_helper_.AddEntry(std::move(future_key), give_key);
  };

  functors.typed_message_and_caching.single_to_group.message_received =
      [](const routing::SingleToGroupMessage& /*message*/) {};
  functors.typed_message_and_caching.group_to_group.message_received =
      [](const routing::GroupToGroupMessage& /*message*/) {};
  functors.typed_message_and_caching.single_to_group_relay.message_received =
      [](const routing::SingleToGroupRelayMessage& /*message*/) {};
  functors.typed_message_and_caching.single_to_group.put_cache_data =
      [](const routing::SingleToGroupMessage& /*message*/) {};
  functors.typed_message_and_caching.group_to_single.put_cache_data =
      [](const routing::GroupToSingleMessage& /*message*/) {};
  functors.typed_message_and_caching.group_to_group.put_cache_data =
      [](const routing::GroupToGroupMessage& /*message*/) {};
  return functors;
}

void MpidClient::OnNetworkStatusChange(int updated_network_health) {
  std::shared_ptr<MpidClient> this_ptr(shared_from_this());
  asio_service_.service().post([this_ptr, updated_network_health] {
    routing::UpdateNetworkHealth(updated_network_health, this_ptr->network_health_,
        this_ptr->network_health_mutex_, this_ptr->network_health_condition_variable_,
        NodeId(this_ptr->kMpid_.name()->string()));
  });
}

}  // namespace nfs_client

}  // namespace maidsafe
