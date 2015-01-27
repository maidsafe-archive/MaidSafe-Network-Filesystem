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

#ifndef MAIDSAFE_NFS_CLIENT_MAID_CLIENT_H_
#define MAIDSAFE_NFS_CLIENT_MAID_CLIENT_H_

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
#include "maidsafe/nfs/client/maid_node_dispatcher.h"
#include "maidsafe/nfs/client/maid_node_service.h"
#include "maidsafe/nfs/client/data_getter.h"

namespace maidsafe {

namespace vault_manager { namespace tools { class PublicPmidStorer; } }

namespace nfs_client {

class MaidClient : public std::enable_shared_from_this<MaidClient>  {
 public:
  typedef boost::future<std::vector<StructuredDataVersions::VersionName>> VersionNamesFuture;
  typedef boost::future<std::unique_ptr<StructuredDataVersions::VersionName>> PutVersionFuture;
  typedef boost::signals2::signal<void(int32_t)> OnNetworkHealthChange;

  // Logging in for already existing maid accounts
  static std::shared_ptr<MaidClient> MakeShared(const passport::Maid& maid);
  // Creates maid account and logs in. Throws on failure to create account.
  static std::shared_ptr<MaidClient> MakeShared(const passport::MaidAndSigner& maid_and_signer);
  // Disconnects from network and all unfinished tasks will be cancelled
  void Stop();

  OnNetworkHealthChange& network_health_change_signal();

  //========================== Data accessors and mutators =========================================
  template <typename DataName>
  boost::future<typename DataName::data_type> Get(
      const DataName& data_name,
      const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(120));

  template <typename Data>
  boost::future<void> Put(const Data& data, const std::chrono::steady_clock::duration& timeout =
                                                std::chrono::seconds(360));

  template <typename DataName>
  void Delete(const DataName& data_name);

  void IncrementReferenceCount(const std::vector<ImmutableData::Name>& /*data_names*/) {}
  void DecrementReferenceCount(const std::vector<ImmutableData::Name>& /*data_names*/) {}

  template <typename DataName>
  void IncrementReferenceCount(const DataName& data_name);

  template <typename DataName>
  void DecrementReferenceCount(const DataName& data_name);

  template <typename DataName>
  boost::future<void> CreateVersionTree(const DataName& data_name,
                         const StructuredDataVersions::VersionName& version_name,
                         uint32_t max_versions, uint32_t max_branches,
                         const std::chrono::steady_clock::duration& timeout =
                             std::chrono::seconds(120));

  template <typename DataName>
  VersionNamesFuture GetVersions(const DataName& data_name,
                                 const std::chrono::steady_clock::duration& timeout =
                                     std::chrono::seconds(120));

  template <typename DataName>
  VersionNamesFuture GetBranch(const DataName& data_name,
                               const StructuredDataVersions::VersionName& branch_tip,
                               const std::chrono::steady_clock::duration& timeout =
                                   std::chrono::seconds(120));

  template <typename DataName>
  PutVersionFuture PutVersion(const DataName& data_name,
                              const StructuredDataVersions::VersionName& old_version_name,
                              const StructuredDataVersions::VersionName& new_version_name,
                              const std::chrono::steady_clock::duration& timeout =
                                  std::chrono::seconds(360));

  template <typename DataName>
  void DeleteBranchUntilFork(const DataName& data_name,
                             const StructuredDataVersions::VersionName& branch_tip);
  // TODO(Prakash): This can move to private section
  boost::future<void> CreateAccount(const nfs_vault::MaidAccountCreation& account_creation,
                                    const std::chrono::steady_clock::duration& timeout =
                                        std::chrono::seconds(240));

  void RemoveAccount(const nfs_vault::MaidAccountRemoval& account_removal);

  friend class vault_manager::tools::PublicPmidStorer;
 private:
  typedef std::function<void(const DataNameAndContentOrReturnCode&)> GetFunctor;
  typedef std::function<void(const StructuredDataNameAndContentOrReturnCode&)> GetVersionsFunctor;
  typedef std::function<void(const StructuredDataNameAndContentOrReturnCode&)> GetBranchFunctor;
  typedef boost::promise<std::vector<StructuredDataVersions::VersionName>> VersionNamesPromise;

  explicit MaidClient(const passport::Maid& maid);

  MaidClient(const MaidClient&);
  MaidClient(MaidClient&&);
  MaidClient& operator=(MaidClient);

  // Creates maid account and logs in. Only used for Zero state client
  // Can only do public key lookup in provided public_pmids (not in network).
  static std::shared_ptr<MaidClient> MakeSharedZeroState(
      const passport::MaidAndSigner& maid_and_signer,
      const std::vector<passport::PublicPmid>& public_pmids);

  void Init(const passport::MaidAndSigner& maid_and_signer);

  void Init();

  void InitZeroState(const passport::MaidAndSigner& maid_and_signer,
                     const std::vector<passport::PublicPmid>& public_pmids);

  void CreateAccount(const passport::PublicMaid& public_maid,
                     const passport::PublicAnmaid& public_anmaid);
  void InitRouting(std::vector<passport::PublicPmid> = std::vector<passport::PublicPmid>());

  routing::Functors InitialiseRoutingCallbacks();
  void OnNetworkStatusChange(int updated_network_health);

  template <typename T>
  void OnMessageReceived(const T& routing_message);

  template <typename Sender, typename Receiver>
  void HandleMessage(const nfs::TypeErasedMessageWrapper& wrapper_tuple, const Sender& sender,
                     const Receiver& receiver);

  const passport::Maid kMaid_;
  BoostAsioService asio_service_;
  MaidNodeService::RpcTimers rpc_timers_;
  std::mutex network_health_mutex_;
  std::condition_variable network_health_condition_variable_;
  int network_health_;
  OnNetworkHealthChange network_health_change_signal_;
  std::unique_ptr<routing::Routing> routing_;
  DataGetter data_getter_;
  nfs::detail::PublicPmidHelper public_pmid_helper_;
  MaidNodeDispatcher dispatcher_;
  nfs::Service<MaidNodeService> service_;
};

void CreateAccount(std::shared_ptr<passport::Maid> maid,
                   std::shared_ptr<passport::Anmaid> anmaid,
                   std::shared_ptr<MaidClient> client_nfs);

// ==================== Implementation =============================================================
template <typename DataName>
boost::future<typename DataName::data_type> MaidClient::Get(
    const DataName& data_name,
    const std::chrono::steady_clock::duration& timeout) {
  return data_getter_.Get(data_name, timeout);
}

template <typename Data>
boost::future<void> MaidClient::Put(const Data& data,
                                     const std::chrono::steady_clock::duration& timeout) {
  LOG(kVerbose) << "MaidClient put " << HexSubstr(data.name().value.string())
                << " of size " << data.Serialise().data.string().size();
  typedef MaidNodeService::PutResponse::Contents ResponseContents;
  auto promise(std::make_shared<boost::promise<void>>());
  NodeId node_id;

  auto response_functor([promise](const nfs_client::ReturnCode& result) {
                           HandlePutResponseResult(result, promise);
                        });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(routing::Parameters::group_size - 1,
                                                               response_functor));
  auto task_id(rpc_timers_.put_timer.NewTaskId());
  rpc_timers_.put_timer.AddTask(
      timeout,
      [op_data, data](ResponseContents put_response) {
        LOG(kVerbose) << "MaidClient Put HandleResponseContents for "
                      << HexSubstr(data.name().value);
        op_data->HandleResponseContents(std::move(put_response));
      },
      routing::Parameters::group_size - 1, task_id);
  rpc_timers_.put_timer.PrintTaskIds();
  dispatcher_.SendPutRequest(task_id, data);
  return promise->get_future();
}

template <typename DataName>
void MaidClient::Delete(const DataName& data_name) {
  dispatcher_.SendDeleteRequest(data_name);
}

template <typename DataName>
void MaidClient::IncrementReferenceCount(const DataName& /*data_name*/) {
}

template <typename DataName>
void MaidClient::DecrementReferenceCount(const DataName& /*data_name*/) {
}

template <typename DataName>
boost::future<void> MaidClient::CreateVersionTree(const DataName& data_name,
                       const StructuredDataVersions::VersionName& version_name,
                       uint32_t max_versions, uint32_t max_branches,
                       const std::chrono::steady_clock::duration& timeout) {
  LOG(kVerbose) << "MaidClient Create Version " << HexSubstr(data_name.value);
  typedef MaidNodeService::CreateVersionTreeResponse::Contents ResponseContents;
  auto promise(std::make_shared<boost::promise<void>>());
  auto response_functor([promise](const nfs_client::ReturnCode& result) {
                           HandleCreateVersionTreeResult(result, promise);
                        });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(rpc_timers_.create_version_tree_timer.NewTaskId());
  rpc_timers_.create_version_tree_timer.AddTask(
      timeout,
      [op_data, data_name](ResponseContents get_response) {
        LOG(kVerbose) << "MaidClient CreateVersionTree HandleResponseContents for "
                      << HexSubstr(data_name.value);
        op_data->HandleResponseContents(std::move(get_response));
      },
      routing::Parameters::group_size * 3, task_id);
  rpc_timers_.create_version_tree_timer.PrintTaskIds();
  dispatcher_.SendCreateVersionTreeRequest(task_id, data_name, version_name, max_versions,
                                           max_branches);
  return promise->get_future();
}

template <typename DataName>
MaidClient::VersionNamesFuture MaidClient::GetVersions(
    const DataName& data_name, const std::chrono::steady_clock::duration& timeout) {
  LOG(kVerbose) << "MaidClient Get Version for " << HexSubstr(data_name.value);
  typedef MaidNodeService::GetVersionsResponse::Contents ResponseContents;
  auto promise(std::make_shared<VersionNamesPromise>());
  auto response_functor([promise](const StructuredDataNameAndContentOrReturnCode&
                                  result) { HandleGetVersionsOrBranchResult(result, promise); });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(rpc_timers_.get_versions_timer.NewTaskId());
  rpc_timers_.get_versions_timer.AddTask(
      timeout, [op_data](ResponseContents get_versions_response) {
                 op_data->HandleResponseContents(std::move(get_versions_response));
               },
      // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
      routing::Parameters::group_size * 2, task_id);
  dispatcher_.SendGetVersionsRequest(task_id, data_name);
  return promise->get_future();
}

template <typename DataName>
MaidClient::VersionNamesFuture MaidClient::GetBranch(
    const DataName& data_name, const StructuredDataVersions::VersionName& branch_tip,
    const std::chrono::steady_clock::duration& timeout) {
  LOG(kVerbose) << "MaidClient Get Branch for " << HexSubstr(data_name.value);
  typedef MaidNodeService::GetBranchResponse::Contents ResponseContents;
  auto promise(std::make_shared<VersionNamesPromise>());
  auto response_functor([promise](const StructuredDataNameAndContentOrReturnCode &
                                  result) { HandleGetVersionsOrBranchResult(result, promise); });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(rpc_timers_.get_branch_timer.NewTaskId());
  rpc_timers_.get_branch_timer.AddTask(timeout,
      [op_data](ResponseContents get_branch_response) {
          op_data->HandleResponseContents(std::move(get_branch_response));
      },
      // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
      routing::Parameters::group_size * 2, task_id);
  dispatcher_.SendGetBranchRequest(task_id, data_name, branch_tip);
  return promise->get_future();
}

template <typename DataName>
MaidClient::PutVersionFuture MaidClient::PutVersion(
    const DataName& data_name, const StructuredDataVersions::VersionName& old_version_name,
    const StructuredDataVersions::VersionName& new_version_name,
    const std::chrono::steady_clock::duration& timeout) {
  LOG(kVerbose) << "MaidClient::PutVersion put new version "
                << DebugId(new_version_name.id) << " after old version "
                << DebugId(old_version_name.id) << " for " << HexSubstr(data_name.value);
  typedef MaidNodeService::PutVersionResponse::Contents ResponseContents;
  auto promise(
      std::make_shared<boost::promise<std::unique_ptr<StructuredDataVersions::VersionName>>>());
  auto response_functor([promise](const nfs_client::TipOfTreeAndReturnCode& result) {
                           HandlePutVersionResult(result, promise);
                        });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(rpc_timers_.put_version_timer.NewTaskId());
  rpc_timers_.put_version_timer.AddTask(
      timeout,
      [op_data, data_name, new_version_name, old_version_name](ResponseContents get_response) {
        LOG(kVerbose) << "MaidClient PutVersion HandleResponseContents put new version "
                      << DebugId(new_version_name.id) << " after old version "
                      << DebugId(old_version_name.id) << " for " << HexSubstr(data_name.value);
        op_data->HandleResponseContents(std::move(get_response));
      },
      routing::Parameters::group_size * 3, task_id);
  rpc_timers_.put_version_timer.PrintTaskIds();
  dispatcher_.SendPutVersionRequest(task_id, data_name, old_version_name, new_version_name);
  return promise->get_future();
}

template <typename DataName>
void MaidClient::DeleteBranchUntilFork(const DataName& data_name,
                                        const StructuredDataVersions::VersionName& branch_tip) {
  dispatcher_.SendDeleteBranchUntilForkRequest(data_name, branch_tip);
}

template <typename T>
void MaidClient::OnMessageReceived(const T& routing_message) {
  auto wrapper_tuple(nfs::ParseMessageWrapper(routing_message.contents));
  const auto& destination_persona(std::get<2>(wrapper_tuple));
  if (destination_persona.data == nfs::Persona::kDataGetter)
    return data_getter_.HandleMessage(routing_message);

  std::shared_ptr<MaidClient> this_ptr(shared_from_this());
  asio_service_.service().post([=] {
    this_ptr->HandleMessage(wrapper_tuple, routing_message.sender, routing_message.receiver);
  });
}

template <typename Sender, typename Receiver>
void MaidClient::HandleMessage(const nfs::TypeErasedMessageWrapper& wrapper_tuple,
                                const Sender& sender, const Receiver& receiver) {
  const auto& destination_persona(std::get<2>(wrapper_tuple));
  static_assert(std::is_same<decltype(destination_persona),
                             const nfs::detail::DestinationTaggedValue&>::value,
                "The value retrieved from the tuple isn't the destination type, but should be.");
  if (destination_persona.data == nfs::Persona::kMaidNode)
    return service_.HandleMessage(wrapper_tuple, sender, receiver);
  auto action(std::get<0>(wrapper_tuple));
  auto source_persona(std::get<1>(wrapper_tuple).data);
  LOG(kError) << " MaidClient::HandleMessage unhandled message from " << source_persona
              << " " << action << " to " << destination_persona;
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MAID_CLIENT_H_
