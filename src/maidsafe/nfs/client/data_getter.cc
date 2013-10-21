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

#include "maidsafe/nfs/client/data_getter.h"

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"

namespace maidsafe {

namespace nfs_client {

DataGetter::DataGetter(AsioService& asio_service, routing::Routing& routing,
                       std::vector<passport::PublicPmid> public_pmids_from_file)
    : get_timer_(asio_service),
      get_versions_timer_(asio_service),
      get_branch_timer_(asio_service),
      dispatcher_(routing),
      service_([&]()->std::unique_ptr<DataGetterService> {
  std::unique_ptr<DataGetterService> service(
      new DataGetterService(routing, get_timer_, get_versions_timer_, get_branch_timer_));
  return std::move(service);
}())
#ifdef TESTING
      ,
      kAllPmids_(std::move(public_pmids_from_file))
#endif
{
#ifndef TESTING
  if (!public_pmids_from_file.empty()) {
    LOG(kError) << "Cannot use fake key getter if TESTING is not defined";
    ThrowError(CommonErrors::invalid_parameter);
  }
#endif
}

template <>
boost::future<passport::PublicPmid> DataGetter::Get(
    const typename passport::PublicPmid::Name& data_name,
    const std::chrono::steady_clock::duration& timeout) {
#ifdef TESTING
  if (kAllPmids_.empty()) {
#endif
    typedef DataGetterService::GetResponse::Contents ResponseContents;
    auto promise(std::make_shared<boost::promise<passport::PublicPmid>>());
    HandleGetResult<passport::PublicPmid> response_functor(promise);
    auto op_data(std::make_shared<nfs::OpData<ResponseContents>>(1, response_functor));
    auto task_id(get_timer_.NewTaskId());
    get_timer_.AddTask(timeout,
                       [op_data](ResponseContents get_response) {
                         op_data->HandleResponseContents(std::move(get_response));
                       },
                       // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
                       routing::Parameters::node_group_size * 2, task_id);
    dispatcher_.SendGetRequest(task_id, data_name);
    return promise->get_future();
#ifdef TESTING
  } else {
    boost::promise<passport::PublicPmid> promise;
    try {
      auto itr(std::find_if(
          std::begin(kAllPmids_), std::end(kAllPmids_),
          [&data_name](const passport::PublicPmid & pmid) { return pmid.name() == data_name; }));
      if (itr == kAllPmids_.end())
        ThrowError(NfsErrors::failed_to_get_data);
      promise.set_value(*itr);
    }
    catch (...) {
      promise.set_exception(boost::current_exception());
    }
    return promise.get_future();
  }
#endif
}

}  // namespace nfs_client

}  // namespace maidsafe
