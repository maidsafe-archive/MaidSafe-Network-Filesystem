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


std::shared_ptr<MaidNodeNfs> MaidNodeNfs::MakeShared(const passport::Maid& maid,
    const routing::BootstrapContacts& bootstrap_contacts) {
  std::shared_ptr<MaidNodeNfs> maid_node_ptr{ new MaidNodeNfs{ maid } };
  maid_node_ptr->Init(bootstrap_contacts);
  return maid_node_ptr;
}

std::shared_ptr<MaidNodeNfs> MaidNodeNfs::MakeShared(
    const passport::MaidAndSigner& maid_and_signer,
    const routing::BootstrapContacts& bootstrap_contacts) {
  std::shared_ptr<MaidNodeNfs> maid_node_ptr{ new MaidNodeNfs{ maid_and_signer.first } };
  maid_node_ptr->Init(maid_and_signer, bootstrap_contacts);
  return maid_node_ptr;
}

std::shared_ptr<MaidNodeNfs> MaidNodeNfs::MakeSharedZeroState(
    const passport::MaidAndSigner& maid_and_signer,
    const routing::BootstrapContacts& bootstrap_contacts,
    const std::vector<passport::PublicPmid>& public_pmids) {
  std::shared_ptr<MaidNodeNfs> maid_node_ptr{ new MaidNodeNfs{ maid_and_signer.first } };
  maid_node_ptr->InitZeroState(maid_and_signer, bootstrap_contacts, public_pmids);
  return maid_node_ptr;
}

void MaidNodeNfs::Init(const passport::MaidAndSigner& maid_and_signer,
                       const routing::BootstrapContacts& bootstrap_contacts) {
  on_scope_exit cleanup_on_error([&] { Stop(); });
  InitRouting(bootstrap_contacts);
  CreateAccount(passport::PublicMaid{ maid_and_signer.first },
                passport::PublicAnmaid{ maid_and_signer.second });
  cleanup_on_error.Release();
}

void MaidNodeNfs::InitZeroState(const passport::MaidAndSigner& maid_and_signer,
                                const routing::BootstrapContacts& bootstrap_contacts,
                                const std::vector<passport::PublicPmid>& public_pmids) {
  on_scope_exit cleanup_on_error([&] { Stop(); });
  InitRouting(bootstrap_contacts, public_pmids);
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

void MaidNodeNfs::Init(const routing::BootstrapContacts& bootstrap_contacts) {
  on_scope_exit cleanup_on_error([&] { Stop(); });
  InitRouting(bootstrap_contacts);
  cleanup_on_error.Release();
}

MaidNodeNfs::MaidNodeNfs(const passport::Maid& maid)
    : kMaid_(maid),
      asio_service_(2),
      get_timer_(asio_service_),
      put_timer_(asio_service_),
      get_versions_timer_(asio_service_),
      get_branch_timer_(asio_service_),
      create_account_timer_(asio_service_),
      pmid_health_timer_(asio_service_),
      create_version_tree_timer_(asio_service_),
      put_version_timer_(asio_service_),
      register_pmid_timer_(asio_service_),
      network_health_change_signal_(),
      routing_(kMaid_),
      dispatcher_(routing_),
      service_([&]()->std::unique_ptr<MaidNodeService> {
        std::unique_ptr<MaidNodeService> service(
            new MaidNodeService(routing_, get_timer_, put_timer_, get_versions_timer_,
                                get_branch_timer_, create_account_timer_, pmid_health_timer_,
                                create_version_tree_timer_, put_version_timer_,
                                register_pmid_timer_, get_handler_));
        return std::move(service);
      }()),
      get_handler_(get_timer_, dispatcher_) {
}

void MaidNodeNfs::Stop() {
  asio_service_.Stop();
}

MaidNodeNfs::OnNetworkHealthChange& MaidNodeNfs::network_health_change_signal() {
  return network_health_change_signal_;
}

void MaidNodeNfs::InitRouting(const routing::BootstrapContacts& bootstrap_contacts,
                              std::vector<passport::PublicPmid> public_pmids) {
  routing::Functors functors(InitialiseRoutingCallbacks());
  if (!public_pmids.empty()) {
    UpdateRequestPublicKeyFunctor(functors.request_public_key, public_pmids);
    LOG(kInfo) << "Modified RequestPublicKeyFunctor for Zero state client";
  }
  LOG(kInfo) << "after  InitialiseRoutingCallbacks";
  routing_.Join(functors, bootstrap_contacts);
  LOG(kInfo) << "after  routing_.Join()";
  std::unique_lock<std::mutex> lock(network_health_mutex_);
  // FIXME BEFORE_RELEASE discuss this
  // This should behave differently. In case of new maid account, it should timeout
  // For existing clients, should we try infinitly ?
  network_health_condition_variable_.wait(lock, [this] { return network_health_ >= 100; });
}

routing::Functors MaidNodeNfs::InitialiseRoutingCallbacks() {
  routing::Functors functors;
  std::shared_ptr<MaidNodeNfs> this_ptr(shared_from_this());
  functors.typed_message_and_caching.single_to_single.message_received =
      [=](const routing::SingleToSingleMessage& message) {
        this_ptr->HandleMessage(message);
      };
  functors.typed_message_and_caching.group_to_single.message_received =
      [this_ptr](const routing::GroupToSingleMessage& message) {
        this_ptr->HandleMessage(message);
      };

  functors.network_status =
      [this_ptr](const int& network_health) { this_ptr->OnNetworkStatusChange(network_health); };
  functors.matrix_changed = [](std::shared_ptr<routing::MatrixChange> /*matrix_change*/) {};
  functors.request_public_key = [this_ptr](const NodeId& node_id,
                                       const routing::GivePublicKeyFunctor& give_key) {
    auto future_key(this_ptr->Get(passport::PublicPmid::Name{ Identity{ node_id.string() } },
                                        std::chrono::seconds(10)));
    this_ptr->public_pmid_helper_.AddEntry(std::move(future_key), give_key);
  };

  // TODO(Prakash) fix routing asserts for clients so client need not to provide callbacks for all
  // functors
  functors.typed_message_and_caching.single_to_group.message_received =
      [this](const routing::SingleToGroupMessage& /*message*/) {};
  functors.typed_message_and_caching.group_to_group.message_received =
      [this](const routing::GroupToGroupMessage& /*message*/) {};
  functors.typed_message_and_caching.single_to_group_relay.message_received =
      [this](const routing::SingleToGroupRelayMessage& /*message*/) {};
  functors.typed_message_and_caching.single_to_group.put_cache_data =
      [this](const routing::SingleToGroupMessage& /*message*/) {};
  functors.typed_message_and_caching.group_to_single.put_cache_data =
      [this](const routing::GroupToSingleMessage& /*message*/) {};
  functors.typed_message_and_caching.group_to_group.put_cache_data =
      [this](const routing::GroupToGroupMessage& /*message*/) {};
  functors.new_bootstrap_contact =
      [this](const routing::BootstrapContact& /*bootstrap_contact*/) {};
  return functors;
}

void MaidNodeNfs::OnNetworkStatusChange(int updated_network_health) {
  asio_service_.service().post([=] {
    routing::UpdateNetworkHealth(updated_network_health, network_health_, network_health_mutex_,
                                 network_health_condition_variable_, routing_.kNodeId());
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
  auto task_id(create_account_timer_.NewTaskId());
  create_account_timer_.AddTask(
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
  auto task_id(create_account_timer_.NewTaskId());
  register_pmid_timer_.AddTask(
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
  auto task_id(pmid_health_timer_.NewTaskId());
  pmid_health_timer_.AddTask(
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
