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

#ifndef MAIDSAFE_NFS_CLIENT_GET_HANDLER_H_
#define MAIDSAFE_NFS_CLIENT_GET_HANDLER_H_

#include <map>
#include <tuple>

#include "boost/thread/future.hpp"


#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/service.h"
#include "maidsafe/nfs/client/maid_node_dispatcher.h"
#include "maidsafe/nfs/client/maid_node_service.h"

namespace maidsafe {

namespace nfs_client {

struct GetHandler {
  typedef std::tuple<size_t, std::chrono::steady_clock::time_point> GetInfo;

  GetHandler(routing::Timer<DataNameAndContentOrReturnCode>& get_timer_in,
             MaidNodeDispatcher& dispatcher_in)
      : get_timer(get_timer_in), dispatcher(dispatcher_in), get_info(), mutex() {}

  template <typename DataName>
  void Get(const DataName data_name,
           std::shared_ptr<boost::promise<typename DataName::data_type>> promise,
           const std::chrono::steady_clock::duration& timeout);

 private:
  template <typename DataName>
  void DoGet(const DataName data_name, routing::TaskId task_id,
             std::shared_ptr<nfs::OpData<DataNameAndContentOrReturnCode>> op_data);

 private:
  routing::Timer<DataNameAndContentOrReturnCode>& get_timer;
  MaidNodeDispatcher& dispatcher;
  std::map<routing::TaskId, GetInfo> get_info;
  std::mutex mutex;
};

template <typename DataName>
void GetHandler::Get(const DataName data_name,
                     std::shared_ptr<boost::promise<typename DataName::data_type>> promise,
           const std::chrono::steady_clock::duration& timeout) {
  auto task_id(get_timer.NewTaskId());
  {
    std::lock_guard<std::mutex> lock(mutex);
    get_info.insert(
        std::make_pair(task_id, std::make_tuple(0, std::chrono::steady_clock::now() + timeout)));
  }
  HandleGetResult<typename DataName::data_type> response_functor(promise);
  auto op_data(
           std::make_shared<nfs::OpData<DataNameAndContentOrReturnCode>>(1, response_functor));
  DoGet(data_name, task_id, op_data);
}

template <typename DataName>
void GetHandler::DoGet(const DataName data_name, routing::TaskId task_id,
                       std::shared_ptr<nfs::OpData<DataNameAndContentOrReturnCode>> op_data) {
  {
    std::lock_guard<std::mutex> lock(mutex);
    auto found(get_info.find(task_id));
    assert(found != std::end(get_info));
  }
  get_timer.AddTask(
      std::get<1>(get_info[task_id]) - std::chrono::steady_clock::now(),
      [op_data, data_name, task_id, this](DataNameAndContentOrReturnCode get_response) {
        LOG(kVerbose) << "MaidNodeNfs Get HandleResponseContents for "
                      << HexSubstr(data_name.value);
        GetInfo info;
        {
          std::lock_guard<std::mutex> lock(mutex);
          std::get<0>(get_info[task_id])++;
          info = get_info[task_id];
        }
        auto now(std::chrono::steady_clock::now());
        LOG(kVerbose) << std::get<0>(info) << ", " << HexSubstr(data_name.value);
        if (get_response.return_code && (std::get<1>(info) > now) &&
            (std::get<0>(info) == routing::Parameters::group_size)) {
          LOG(kVerbose) << "DoGet: Retry" << DebugId(data_name.value)
                        << "return code" << get_response.return_code;
          auto new_task_id(get_timer.NewTaskId());
          {
            std::lock_guard<std::mutex> lock(mutex);
            get_info.insert(std::make_pair(new_task_id, std::make_tuple(0, std::get<1>(info))));
            get_info.erase(task_id);
          }
          this->DoGet(data_name, new_task_id, op_data);
        } else if (get_response.content) {
          LOG(kVerbose) << "DoGet: Success" << DebugId(data_name.value);
          {
            std::lock_guard<std::mutex> lock(mutex);
            get_info.erase(task_id);
          }
          op_data->HandleResponseContents(std::move(get_response));
        }
      },
      // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
      routing::Parameters::group_size * 2, task_id);
  dispatcher.SendGetRequest(task_id, data_name);
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_GET_HANDLER_H_
