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

#ifndef MAIDSAFE_NFS_CLIENT_MAID_NODE_NFS_H_
#define MAIDSAFE_NFS_CLIENT_MAID_NODE_NFS_H_

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "boost/thread/future.hpp"

#include "maidsafe/common/asio_service.h"
#include "maidsafe/common/data_types/structured_data_versions.h"
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

namespace maidsafe {

namespace nfs_client {

class MaidNodeNfs {
 public:
  typedef boost::future<std::vector<StructuredDataVersions::VersionName>> VersionNamesFuture;
  typedef boost::future<uint64_t> PmidHealthFuture;

  MaidNodeNfs(AsioService& asio_service, routing::Routing& routing,
              passport::PublicPmid::Name pmid_node_hint);

  passport::PublicPmid::Name pmid_node_hint() const;
  void set_pmid_node_hint(const passport::PublicPmid::Name& pmid_node_hint);

  template <typename DataName>
  boost::future<typename DataName::data_type> Get(
      const DataName& data_name,
      const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(10));

  template <typename Data>
  void Put(const Data& data);

  template <typename DataName>
  void Delete(const DataName& data_name);

  template <typename DataName>
  boost::future<void> CreateVersionTree(const DataName& data_name,
                         const StructuredDataVersions::VersionName& version_name,
                         uint32_t max_versions, uint32_t max_branches,
                         const std::chrono::steady_clock::duration& timeout =
                             std::chrono::seconds(10));

  template <typename DataName>
  VersionNamesFuture GetVersions(const DataName& data_name,
                                 const std::chrono::steady_clock::duration& timeout =
                                     std::chrono::seconds(10));

  template <typename DataName>
  VersionNamesFuture GetBranch(const DataName& data_name,
                               const StructuredDataVersions::VersionName& branch_tip,
                               const std::chrono::steady_clock::duration& timeout =
                                   std::chrono::seconds(10));

  template <typename DataName>
  boost::future<void> PutVersion(const DataName& data_name,
                                 const StructuredDataVersions::VersionName& old_version_name,
                                 const StructuredDataVersions::VersionName& new_version_name,
                                 const std::chrono::steady_clock::duration& timeout =
                                     std::chrono::seconds(10));

  template <typename DataName>
  void DeleteBranchUntilFork(const DataName& data_name,
                             const StructuredDataVersions::VersionName& branch_tip);

  boost::future<void> CreateAccount(const nfs_vault::AccountCreation& account_creation,
                                    const std::chrono::steady_clock::duration& timeout =
                                        std::chrono::seconds(10));

  void RemoveAccount(const nfs_vault::AccountRemoval& account_removal);

  void RegisterPmid(const nfs_vault::PmidRegistration& pmid_registration);

  void UnregisterPmid(const passport::PublicPmid::Name& pmid_name);

  PmidHealthFuture GetPmidHealth(const passport::PublicPmid::Name& pmid_name,
                                 const std::chrono::steady_clock::duration& timeout =
                                     std::chrono::seconds(10));

  // This should be the function used in the GroupToSingle (and maybe also SingleToSingle) functors
  // passed to 'routing.Join'.
  template <typename T>
  void HandleMessage(const T& routing_message);

 private:
  typedef std::function<void(const DataNameAndContentOrReturnCode&)> GetFunctor;
  typedef std::function<void(const StructuredDataNameAndContentOrReturnCode&)> GetVersionsFunctor;
  typedef std::function<void(const StructuredDataNameAndContentOrReturnCode&)> GetBranchFunctor;
  typedef boost::promise<std::vector<StructuredDataVersions::VersionName>> VersionNamesPromise;
  typedef std::function<void(const AvailableSizeAndReturnCode&)> PmidHealthFunctor;

  MaidNodeNfs(const MaidNodeNfs&);
  MaidNodeNfs(MaidNodeNfs&&);
  MaidNodeNfs& operator=(MaidNodeNfs);

  routing::Timer<MaidNodeService::GetResponse::Contents> get_timer_;
  routing::Timer<MaidNodeService::GetVersionsResponse::Contents> get_versions_timer_;
  routing::Timer<MaidNodeService::GetBranchResponse::Contents> get_branch_timer_;
  routing::Timer<MaidNodeService::CreateAccountResponse::Contents> create_account_timer_;
  routing::Timer<MaidNodeService::PmidHealthResponse::Contents> pmid_health_timer_;
  routing::Timer<MaidNodeService::CreateVersionTreeResponse::Contents> create_verstion_tree_timer_;
  routing::Timer<MaidNodeService::PutVersionResponse::Contents> put_verstion_timer_;
  MaidNodeDispatcher dispatcher_;
  nfs::Service<MaidNodeService> service_;
  mutable std::mutex pmid_node_hint_mutex_;
  passport::PublicPmid::Name pmid_node_hint_;
};

// ==================== Implementation =============================================================
template <typename DataName>
boost::future<typename DataName::data_type> MaidNodeNfs::Get(
    const DataName& data_name,
    const std::chrono::steady_clock::duration& timeout) {
  LOG(kVerbose) << "MaidNodeNfs Get " << HexSubstr(data_name.value);
  typedef MaidNodeService::GetResponse::Contents ResponseContents;
  auto promise(std::make_shared<boost::promise<typename DataName::data_type>>());
  HandleGetResult<typename DataName::data_type> response_functor(promise);
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(get_timer_.NewTaskId());
  get_timer_.AddTask(timeout,
                     [op_data, data_name](ResponseContents get_response) {
                        LOG(kVerbose) << "MaidNodeNfs Get HandleResponseContents for "
                                      << HexSubstr(data_name.value);
                        op_data->HandleResponseContents(std::move(get_response));
                     },
                     // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
                     routing::Parameters::group_size * 2, task_id);
  get_timer_.PrintTaskIds();
  dispatcher_.SendGetRequest(task_id, data_name);
  return promise->get_future();
}

template <typename Data>
void MaidNodeNfs::Put(const Data& data) {
  dispatcher_.SendPutRequest(data, pmid_node_hint());
}

template <typename DataName>
void MaidNodeNfs::Delete(const DataName& data_name) {
  dispatcher_.SendDeleteRequest(data_name);
}

template <typename DataName>
boost::future<void> MaidNodeNfs::CreateVersionTree(const DataName& data_name,
                       const StructuredDataVersions::VersionName& version_name,
                       uint32_t max_versions, uint32_t max_branches,
                       const std::chrono::steady_clock::duration& timeout) {
  LOG(kVerbose) << "MaidNodeNfs Get " << HexSubstr(data_name.value);
  typedef MaidNodeService::CreateVersionTreeResponse::Contents ResponseContents;
  auto promise(std::make_shared<boost::promise<void>>());
  auto response_functor([promise](const nfs_client::ReturnCode& result) {
                           HandleCreateVersionTreeResult(result, promise);
                        });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(create_verstion_tree_timer_.NewTaskId());
  create_verstion_tree_timer_.AddTask(
      timeout,
      [op_data, data_name](ResponseContents get_response) {
        LOG(kVerbose) << "MaidNodeNfs CreateVersionTree HandleResponseContents for "
                      << HexSubstr(data_name.value);
        op_data->HandleResponseContents(std::move(get_response));
      },
      routing::Parameters::group_size * 3, task_id);
  create_verstion_tree_timer_.PrintTaskIds();
  dispatcher_.SendCreateVersionTreeRequest(task_id, data_name, version_name, max_versions,
                                           max_branches);
  return promise->get_future();
}

template <typename DataName>
MaidNodeNfs::VersionNamesFuture MaidNodeNfs::GetVersions(
    const DataName& data_name, const std::chrono::steady_clock::duration& timeout) {
  typedef MaidNodeService::GetVersionsResponse::Contents ResponseContents;
  auto promise(std::make_shared<VersionNamesPromise>());
  auto response_functor([promise](const StructuredDataNameAndContentOrReturnCode&
                                  result) { HandleGetVersionsOrBranchResult(result, promise); });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(get_versions_timer_.NewTaskId());
  get_versions_timer_.AddTask(
      timeout, [op_data](ResponseContents get_versions_response) {
                 op_data->HandleResponseContents(std::move(get_versions_response));
               },
      // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
      routing::Parameters::group_size * 2, task_id);
  dispatcher_.SendGetVersionsRequest(task_id, data_name);
  return promise->get_future();
}

template <typename DataName>
MaidNodeNfs::VersionNamesFuture MaidNodeNfs::GetBranch(
    const DataName& data_name, const StructuredDataVersions::VersionName& branch_tip,
    const std::chrono::steady_clock::duration& timeout) {
  typedef MaidNodeService::GetBranchResponse::Contents ResponseContents;
  auto promise(std::make_shared<VersionNamesPromise>());
  auto response_functor([promise](const StructuredDataNameAndContentOrReturnCode &
                                  result) { HandleGetVersionsOrBranchResult(result, promise); });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(get_branch_timer_.NewTaskId());
  get_branch_timer_.AddTask(timeout,
      [op_data](ResponseContents get_branch_response) {
          op_data->HandleResponseContents(std::move(get_branch_response));
      },
      // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
      routing::Parameters::group_size * 2, task_id);
  dispatcher_.SendGetBranchRequest(task_id, data_name, branch_tip);
  return promise->get_future();
}

template <typename DataName>
boost::future<void> MaidNodeNfs::PutVersion(
    const DataName& data_name, const StructuredDataVersions::VersionName& old_version_name,
    const StructuredDataVersions::VersionName& new_version_name,
    const std::chrono::steady_clock::duration& timeout) {
  LOG(kVerbose) << "MaidNodeNfs Put Version " << HexSubstr(data_name.value);
  typedef MaidNodeService::CreateVersionTreeResponse::Contents ResponseContents;
  auto promise(std::make_shared<boost::promise<void>>());
  auto response_functor([promise](const nfs_client::DataNameAndTipOfTreeOrReturnCode& result) {
                           HandlePutVersionResult(result, promise);
                        });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(put_verstion_timer_.NewTaskId());
  put_verstion_timer_.AddTask(
      timeout,
      [op_data, data_name](ResponseContents get_response) {
        LOG(kVerbose) << "MaidNodeNfs CreateVersionTree HandleResponseContents for "
                      << HexSubstr(data_name.value);
        op_data->HandleResponseContents(std::move(get_response));
      },
      routing::Parameters::group_size * 3, task_id);
  put_verstion_timer_.PrintTaskIds();
  dispatcher_.SendPutVersionRequest(task_id, data_name, old_version_name, new_version_name);
  return promise->get_future();
}

template <typename DataName>
void MaidNodeNfs::DeleteBranchUntilFork(const DataName& data_name,
                                        const StructuredDataVersions::VersionName& branch_tip) {
  dispatcher_.SendDeleteBranchUntilForkRequest(data_name, branch_tip);
}

template <typename T>
void MaidNodeNfs::HandleMessage(const T& routing_message) {
  LOG(kVerbose) << "MaidNodeNfs::HandleMessage";
  auto wrapper_tuple(nfs::ParseMessageWrapper(routing_message.contents));
  const auto& destination_persona(std::get<2>(wrapper_tuple));
  static_assert(std::is_same<decltype(destination_persona),
                             const nfs::detail::DestinationTaggedValue&>::value,
                "The value retrieved from the tuple isn't the destination type, but should be.");
  if (destination_persona.data == nfs::Persona::kMaidNode)
    return service_.HandleMessage(wrapper_tuple, routing_message.sender, routing_message.receiver);
  auto action(std::get<0>(wrapper_tuple));
  auto source_persona(std::get<1>(wrapper_tuple).data);
  LOG(kError) << " MaidNodeNfs::HandleMessage unhandled message from " << source_persona
              << " " << action << " to " << destination_persona;
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MAID_NODE_NFS_H_
