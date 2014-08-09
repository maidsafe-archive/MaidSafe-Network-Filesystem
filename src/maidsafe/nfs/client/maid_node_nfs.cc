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

#include "maidsafe/nfs/client/maid_node_nfs.h"

#include "maidsafe/common/on_scope_exit.h"
#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/make_unique.h"

#include "maidsafe/nfs/vault/account_creation.h"
#include "maidsafe/nfs/vault/account_removal.h"
#include "maidsafe/nfs/vault/pmid_registration.h"

namespace maidsafe {

namespace nfs_client {

namespace {

// NOTE: methods - MakeSharedZeroState() GivePublicPmidKey() & UpdateRequestPublicKeyFunctor()
// are used for ZeroState network setup only.
// This allows a cleint to lookup *only* in a provided public pmid list, when it is
// validating a peer while connecting.
void GivePublicPmidKey(const NodeId& node_id, routing::GivePublicKeyFunctor give_key,
                       const std::vector<passport::PublicPmid>& public_pmids) {
  assert(!public_pmids.empty());
  passport::PublicPmid::Name name(Identity(node_id.string()));
  LOG(kVerbose) << "fetch from local list containing "
                << public_pmids.size() << " pmids";
  auto itr(std::find_if(
      std::begin(public_pmids), std::end(public_pmids),
      [&name](const passport::PublicPmid & pmid) { return pmid.name() == name; }));
  if (itr == public_pmids.end()) {
    LOG(kError) << "can't Get PublicPmid " << HexSubstr(name.value)
                << " from local list";
    assert(false);
  } else {
    LOG(kVerbose) << "Got PublicPmid from local list " << HexSubstr(name.value);
    give_key((*itr).public_key());
  }
}

void UpdateRequestPublicKeyFunctor(routing::RequestPublicKeyFunctor& request_public_key,
                                   const std::vector<passport::PublicPmid>& public_pmids) {
  LOG(kInfo) << " Zero state UpdateRequestPublicKeyFunctor";
  request_public_key = [=](const NodeId& node_id, const routing::GivePublicKeyFunctor& give_key) {
                         GivePublicPmidKey(node_id, give_key, public_pmids);
                       };
}

}  // anonymous namespace


std::shared_ptr<MaidNodeNfs> MaidNodeNfs::MakeShared(const passport::Maid& maid) {
  std::shared_ptr<MaidNodeNfs> maid_node_ptr{ new MaidNodeNfs{ maid } };
  maid_node_ptr->Init();
  return maid_node_ptr;
}

std::shared_ptr<MaidNodeNfs> MaidNodeNfs::MakeShared(
    const passport::MaidAndSigner& maid_and_signer) {
  std::shared_ptr<MaidNodeNfs> maid_node_ptr{ new MaidNodeNfs{ maid_and_signer.first } };
  maid_node_ptr->Init(maid_and_signer);
  return maid_node_ptr;
}

std::shared_ptr<MaidNodeNfs> MaidNodeNfs::MakeSharedZeroState(
    const passport::MaidAndSigner& maid_and_signer,
    const std::vector<passport::PublicPmid>& public_pmids) {
  std::shared_ptr<MaidNodeNfs> maid_node_ptr{ new MaidNodeNfs{ maid_and_signer.first } };
  maid_node_ptr->InitZeroState(maid_and_signer, public_pmids);
  return maid_node_ptr;
}

void MaidNodeNfs::Init(const passport::MaidAndSigner& maid_and_signer) {
  on_scope_exit cleanup_on_error([&] { Stop(); });
  InitRouting();
  CreateAccount(passport::PublicMaid{ maid_and_signer.first },
                passport::PublicAnmaid{ maid_and_signer.second });
  cleanup_on_error.Release();
}

void MaidNodeNfs::InitZeroState(const passport::MaidAndSigner& maid_and_signer,
                                const std::vector<passport::PublicPmid>& public_pmids) {
  on_scope_exit cleanup_on_error([&] { Stop(); });
  InitRouting(public_pmids);
  CreateAccount(passport::PublicMaid{ maid_and_signer.first },
                passport::PublicAnmaid{ maid_and_signer.second });
  cleanup_on_error.Release();
}

void MaidNodeNfs::CreateAccount(const passport::PublicMaid& public_maid,
                                const passport::PublicAnmaid& public_anmaid) {
  LOG(kInfo) << "Calling CreateAccount for maid ID:" << DebugId(public_maid.name());
  nfs_vault::AccountCreation account_creation{ public_maid, public_anmaid };
  auto create_account_future = CreateAccount(account_creation);
  create_account_future.get();
  LOG(kInfo) << " CreateAccount for maid ID:" << DebugId(public_maid.name()) << " succeeded.";
}

void MaidNodeNfs::Init() {
  on_scope_exit cleanup_on_error([&] { Stop(); });
  InitRouting();
  cleanup_on_error.Release();
}

MaidNodeNfs::MaidNodeNfs(const passport::Maid& maid)
    : kMaid_(maid),
      asio_service_(2),
      rpc_timers_(asio_service_),
      network_health_mutex_(),
      network_health_condition_variable_(),
      network_health_(-1),
      network_health_change_signal_(),
      routing_(maidsafe::make_unique<routing::Routing>(kMaid_)),
      public_pmid_helper_(),
      dispatcher_(*routing_),
      service_([&]()->std::unique_ptr<MaidNodeService> {
        std::unique_ptr<MaidNodeService> service(
            new MaidNodeService(routing::SingleId(routing_->kNodeId()), rpc_timers_, get_handler_));
        return std::move(service);
      }()),
      get_handler_(rpc_timers_.get_timer, dispatcher_) {
}

void MaidNodeNfs::Stop() {
  LOG(kVerbose) << "MaidNodeNfs::Stop()";
  dispatcher_.Stop();
  LOG(kVerbose) << "MaidNodeNfs::Stop() : dispatcher_";
  routing_.reset();
  LOG(kVerbose) << "MaidNodeNfs::Stop() : routing_";
  rpc_timers_.CancellAll();
  LOG(kVerbose) << "MaidNodeNfs::Stop() : rpc_timers_";
  asio_service_.Stop();
  LOG(kVerbose) << "MaidNodeNfs::Stop() : asio_service_";
}

MaidNodeNfs::OnNetworkHealthChange& MaidNodeNfs::network_health_change_signal() {
  return network_health_change_signal_;
}

void MaidNodeNfs::InitRouting(std::vector<passport::PublicPmid> public_pmids) {
  routing::Functors functors(InitialiseRoutingCallbacks());
  if (!public_pmids.empty()) {
    UpdateRequestPublicKeyFunctor(functors.request_public_key, public_pmids);
    LOG(kInfo) << "Modified RequestPublicKeyFunctor for Zero state client";
  }
  LOG(kInfo) << "after  InitialiseRoutingCallbacks";
  routing_->Join(functors);
  LOG(kInfo) << "after  routing_.Join()";
  // FIXME BEFORE_RELEASE discuss: parallel attempts, max no. of endpoints to try,
  // prioritise live ports. To reduce the blocking duration in case of no network connectivity
  std::unique_lock<std::mutex> lock{ network_health_mutex_ };
  network_health_condition_variable_.wait(lock, [this] {
    return (network_health_ == 100) || (network_health_ < -300000); });
  if (network_health_ < 0)
    BOOST_THROW_EXCEPTION(MakeError(RoutingErrors::not_connected));
}

routing::Functors MaidNodeNfs::InitialiseRoutingCallbacks() {
  routing::Functors functors;
  std::shared_ptr<MaidNodeNfs> this_ptr(shared_from_this());
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

  // TODO(Prakash) fix routing asserts for clients so client need not to provide callbacks for all
  // functors
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

void MaidNodeNfs::OnNetworkStatusChange(int updated_network_health) {
  std::shared_ptr<MaidNodeNfs> this_ptr(shared_from_this());
  asio_service_.service().post([this_ptr, updated_network_health] {
    routing::UpdateNetworkHealth(updated_network_health, this_ptr->network_health_,
        this_ptr->network_health_mutex_, this_ptr->network_health_condition_variable_,
        NodeId(this_ptr->kMaid_.name()->string()));
  });
}

boost::future<void> MaidNodeNfs::CreateAccount(
    const nfs_vault::AccountCreation& account_creation,
    const std::chrono::steady_clock::duration& timeout) {
  typedef MaidNodeService::CreateAccountResponse::Contents ResponseContents;
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
      // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
      routing::Parameters::group_size * 2, task_id);
  dispatcher_.SendCreateAccountRequest(task_id, account_creation);
  return promise->get_future();
}

void MaidNodeNfs::RemoveAccount(const nfs_vault::AccountRemoval& account_removal) {
  dispatcher_.SendRemoveAccountRequest(account_removal);
}

boost::future<void> MaidNodeNfs::RegisterPmid(const passport::Pmid& pmid,
                                              const std::chrono::steady_clock::duration& timeout) {
  nfs_vault::PmidRegistration pmid_registration{ kMaid_, pmid, false };
  typedef MaidNodeService::RegisterPmidResponse::Contents ResponseContents;
  auto promise(std::make_shared<boost::promise<void>>());
  auto response_functor([promise](const ResponseContents &result) {
      HandleRegisterPmidResult(result, promise);
  });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(
      routing::Parameters::group_size - 1, response_functor));
  auto task_id(rpc_timers_.register_pmid_timer.NewTaskId());
  rpc_timers_.register_pmid_timer.AddTask(
      timeout, [op_data](ResponseContents create_account_response) {
                 op_data->HandleResponseContents(std::move(create_account_response));
               },
      // TODO(Mahmoud): Confirm expected count
      routing::Parameters::group_size - 1, task_id);
  dispatcher_.SendRegisterPmidRequest(task_id, pmid_registration);
  return promise->get_future();
}

void MaidNodeNfs::UnregisterPmid(const passport::PublicPmid::Name& pmid_name) {
  dispatcher_.SendUnregisterPmidRequest(pmid_name);
}

MaidNodeNfs::PmidHealthFuture MaidNodeNfs::GetPmidHealth(
    const passport::PublicPmid::Name& pmid_name,
    const std::chrono::steady_clock::duration& timeout) {
  typedef MaidNodeService::PmidHealthResponse::Contents ResponseContents;
  auto promise(std::make_shared<boost::promise<uint64_t>>());
  auto response_functor([promise](const ResponseContents& result) {
      HandlePmidHealthResult(result, promise);
  });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(
      routing::Parameters::group_size - 1, response_functor));
  auto task_id(rpc_timers_.pmid_health_timer.NewTaskId());
  rpc_timers_.pmid_health_timer.AddTask(
      timeout, [op_data](ResponseContents pmid_health_response) {
                 op_data->HandleResponseContents(std::move(pmid_health_response));
               },
      // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
      routing::Parameters::group_size - 1, task_id);
  dispatcher_.SendPmidHealthRequest(task_id, pmid_name);
  return promise->get_future();
}

}  // namespace nfs_client

}  // namespace maidsafe
