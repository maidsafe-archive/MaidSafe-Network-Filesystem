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

#ifndef MAIDSAFE_NFS_CLIENT_DATA_GETTER_H_
#define MAIDSAFE_NFS_CLIENT_DATA_GETTER_H_

#include <functional>
#include <memory>
#include <vector>
#include <mutex>

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
#include "maidsafe/nfs/client/data_getter_dispatcher.h"
#include "maidsafe/nfs/client/data_getter_service.h"
#include "maidsafe/nfs/client/get_handler.h"

namespace maidsafe {

namespace nfs_client {

class DataGetter {
 public:
  typedef boost::future<std::vector<StructuredDataVersions::VersionName>> VersionNamesFuture;

  // all_pmids_from_file should only be non-empty if TESTING is defined
  DataGetter(BoostAsioService& asio_service, routing::Routing& routing);

  // This call only cancels the rpc timers. As routing object is not owned by data getter,
  // it doesn't stop routing.
  void Stop();

  template <typename DataName>
  boost::future<typename DataName::data_type> Get(
      const DataName& data_name,
      const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(120));

  template <typename DataName>
  VersionNamesFuture GetVersions(const DataName& data_name,
                                 const std::chrono::steady_clock::duration& timeout =
                                     std::chrono::seconds(120));

  template <typename DataName>
  VersionNamesFuture GetBranch(const DataName& data_name,
                               const StructuredDataVersions::VersionName& branch_tip,
                               const std::chrono::steady_clock::duration& timeout =
                                   std::chrono::seconds(120));

  // This should be the function used in the GroupToSingle (and maybe also SingleToSingle) functors
  // passed to 'routing.Join'.
  template <typename T>
  void HandleMessage(const T& routing_message);

  nfs::Service<DataGetterService>& service() { return service_; }

 private:
  typedef std::function<void(const DataNameAndContentOrReturnCode&)> GetFunctor;
  typedef std::function<void(const StructuredDataNameAndContentOrReturnCode&)> GetVersionsFunctor;
  typedef std::function<void(const StructuredDataNameAndContentOrReturnCode&)> GetBranchFunctor;
  typedef boost::promise<std::vector<StructuredDataVersions::VersionName>> VersionNamesPromise;

  DataGetter(const DataGetter&);
  DataGetter(DataGetter&&);
  DataGetter& operator=(DataGetter);

  routing::Timer<DataGetterService::GetResponse::Contents> get_timer_;
  routing::Timer<DataGetterService::GetVersionsResponse::Contents> get_versions_timer_;
  routing::Timer<DataGetterService::GetBranchResponse::Contents> get_branch_timer_;
  DataGetterDispatcher dispatcher_;
  GetHandler<DataGetterDispatcher> get_handler_;
  nfs::Service<DataGetterService> service_;
};

// ==================== Implementation =============================================================
template <typename DataName>
boost::future<typename DataName::data_type> DataGetter::Get(
    const DataName& data_name,
    const std::chrono::steady_clock::duration& timeout) {
  LOG(kVerbose) << "MaidClient Get " << HexSubstr(data_name.value);
  auto promise(std::make_shared<boost::promise<typename DataName::data_type>>());
  get_handler_.Get(data_name, promise, timeout);
  return promise->get_future();
}

template <typename DataName>
DataGetter::VersionNamesFuture DataGetter::GetVersions(
    const DataName& data_name, const std::chrono::steady_clock::duration& timeout) {
  typedef DataGetterService::GetVersionsResponse::Contents ResponseContents;
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
DataGetter::VersionNamesFuture DataGetter::GetBranch(
    const DataName& data_name, const StructuredDataVersions::VersionName& branch_tip,
    const std::chrono::steady_clock::duration& timeout) {
  typedef DataGetterService::GetBranchResponse::Contents ResponseContents;
  auto promise(std::make_shared<VersionNamesPromise>());
  auto response_functor([promise](const StructuredDataNameAndContentOrReturnCode &
                                  result) { HandleGetVersionsOrBranchResult(result, promise); });
  auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
  auto task_id(get_branch_timer_.AddTask(timeout, [op_data](ResponseContents get_branch_response) {
                                                    op_data->HandleResponseContents(
                                                        std::move(get_branch_response));
                                                  },
                                         // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
                                         routing::Parameters::group_size * 2));
  dispatcher_.SendGetBranchRequest(task_id, data_name, branch_tip);
  return promise->get_future();
}

template <typename T>
void DataGetter::HandleMessage(const T& routing_message) {
  auto wrapper_tuple(nfs::ParseMessageWrapper(routing_message.contents));
  const auto& destination_persona(std::get<2>(wrapper_tuple));
  static_assert(std::is_same<decltype(destination_persona),
                             const nfs::detail::DestinationTaggedValue&>::value,
                "The value retrieved from the tuple isn't the destination type, but should be.");
  if (destination_persona.data == nfs::Persona::kDataGetter)
    return service_.HandleMessage(wrapper_tuple, routing_message.sender, routing_message.receiver);
  auto action(std::get<0>(wrapper_tuple));
  auto source_persona(std::get<1>(wrapper_tuple).data);
  LOG(kError) << " DataGetter::HandleMessage unhandled message from " << source_persona
              << " " << action << " to " << destination_persona;
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_DATA_GETTER_H_
