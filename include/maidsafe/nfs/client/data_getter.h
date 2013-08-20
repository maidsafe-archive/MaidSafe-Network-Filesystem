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

#ifndef MAIDSAFE_NFS_CLIENT_DATA_GETTER_H_
#define MAIDSAFE_NFS_CLIENT_DATA_GETTER_H_

#include <functional>
#include <memory>
#include <vector>

#include "maidsafe/common/asio_service.h"
#include "maidsafe/passport/types.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/message_wrapper.h"
#include "maidsafe/nfs/service.h"
#include "maidsafe/nfs/utils.h"
#include "maidsafe/nfs/client/data_getter_dispatcher.h"
#include "maidsafe/nfs/client/data_getter_service.h"


namespace maidsafe {

namespace nfs_client {

class DataGetter {
 public:
  typedef std::function<void(const DataOrDataNameAndReturnCode&)> GetFunctor;
  typedef std::function<void(const StructuredDataOrDataNameAndReturnCode&)> GetVersionsFunctor;
  typedef std::function<void(const StructuredDataOrDataNameAndReturnCode&)> GetBranchFunctor;
  // all_pmids_from_file should only be non-empty if TESTING is defined
  DataGetter(AsioService& asio_service,
             routing::Routing& routing,
             const std::vector<passport::PublicPmid>& public_pmids_from_file =
                 std::vector<passport::PublicPmid>());

  template<typename Data>
  void Get(const typename Data::Name& data_name,
           const GetFunctor& response_functor,
           const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(10));

  template<typename Data>
  void GetVersions(
      const typename Data::Name& data_name,
      const GetVersionsFunctor& response_functor,
      const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(10));

  template<typename Data>
  void GetBranch(
      const typename Data::Name& data_name,
      const StructuredDataVersions::VersionName& branch_tip,
      const GetBranchFunctor& response_functor,
      const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(10));

  // This should be the function used in the GroupToSingle (and maybe also SingleToSingle) functors
  // passed to 'routing.Join'.
  template<typename T>
  void HandleMessage(const T& routing_message);

 private:
  DataGetter(const DataGetter&);
  DataGetter(DataGetter&&);
  DataGetter& operator=(DataGetter);

  routing::Timer<DataGetterService::GetResponse::Contents> get_timer_;
  routing::Timer<DataGetterService::GetVersionsResponse::Contents> get_versions_timer_;
  routing::Timer<DataGetterService::GetBranchResponse::Contents> get_branch_timer_;
  DataGetterDispatcher dispatcher_;
  nfs::Service<DataGetterService> service_;
#ifdef TESTING
  std::vector<passport::PublicPmid> kAllPmids_;
#endif
};

template<>
void DataGetter::Get<passport::PublicPmid>(const typename passport::PublicPmid::Name& data_name,
                                           const GetFunctor& response_functor,
                                           const std::chrono::steady_clock::duration& timeout);



// ==================== Implementation =============================================================
template<typename Data>
void DataGetter::Get(const typename Data::Name& data_name,
                     const GetFunctor& response_functor,
                     const std::chrono::steady_clock::duration& timeout) {
  typedef DataGetterService::GetResponse::Contents ResponseContents;
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(get_timer_.AddTask(
      timeout,
      [op_data](ResponseContents get_response) {
          op_data->HandleResponseContents(std::move(get_response));
      },
      // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
      routing::Parameters::node_group_size * 2));
  dispatcher_.SendGetRequest(task_id, data_name);
}

template<typename Data>
void DataGetter::GetVersions(const typename Data::Name& data_name,
                             const GetVersionsFunctor& response_functor,
                             const std::chrono::steady_clock::duration& timeout) {
  typedef DataGetterService::GetVersionsResponse::Contents ResponseContents;
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(get_versions_timer_.AddTask(
      timeout,
      [op_data](ResponseContents get_versions_response) {
          op_data->HandleResponseContents(std::move(get_versions_response));
      },
      // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
      routing::Parameters::node_group_size * 2));
  dispatcher_.SendGetVersionsRequest(task_id, data_name);
}

template<typename Data>
void DataGetter::GetBranch(const typename Data::Name& data_name,
                           const StructuredDataVersions::VersionName& branch_tip,
                           const GetBranchFunctor& response_functor,
                           const std::chrono::steady_clock::duration& timeout) {
  typedef DataGetterService::GetBranchResponse::Contents ResponseContents;
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(get_branch_timer_.AddTask(
      timeout,
      [op_data](ResponseContents get_branch_response) {
          op_data->HandleResponseContents(std::move(get_branch_response));
      },
      // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
      routing::Parameters::node_group_size * 2));
  dispatcher_.SendGetBranchRequest(task_id, data_name, branch_tip);
}

template<typename T>
void DataGetter::HandleMessage(const T& routing_message) {
  auto wrapper_tuple(nfs::ParseMessageWrapper(routing_message.contents));
  const auto& destination_persona(std::get<2>(wrapper_tuple));
  static_assert(std::is_same<decltype(destination_persona),
                             const nfs::detail::DestinationTaggedValue&>::value,
                "The value retrieved from the tuple isn't the destination type, but should be.");
  if (destination_persona.data == nfs::Persona::kDataGetter)
    return service_.HandleMessage(wrapper_tuple, routing_message.sender, routing_message.receiver);
  LOG(kError) << "Unhandled Persona";
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_DATA_GETTER_H_
