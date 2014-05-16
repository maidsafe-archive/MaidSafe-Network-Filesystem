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

#include "maidsafe/common/data_types/data_name_variant.h"

#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/service.h"
#include "maidsafe/nfs/client/maid_node_dispatcher.h"
#include "maidsafe/nfs/client/maid_node_service.h"
#include "maidsafe/nfs/client/client_utils.h"

namespace maidsafe {

namespace nfs_client {

class GetHandlerVisitor : public boost::static_visitor<> {
 public:
  GetHandlerVisitor(MaidNodeDispatcher& dispatcher_in, routing::TaskId task_id)
      : dispatcher_(dispatcher_in), kTaskId_(task_id) {}

  template <typename Name>
  void operator()(const Name& data_name) {
    LOG(kVerbose) << "Get handler visitor sending get request for chunk "
                  << HexSubstr(data_name.value.string());
    dispatcher_.SendGetRequest(kTaskId_, data_name);
  }

 private:
  MaidNodeDispatcher& dispatcher_;
  const routing::TaskId kTaskId_;
};

class GetHandler {
  typedef std::tuple<size_t, routing::TaskId, DataNameVariant, size_t> GetInfo;
  enum class Operation : int {
    kNoOperation = 0,
    kAddResponse = 1,
    kSendRequest = 2,
    kCancelTask = 3
  };

 public:
  GetHandler(routing::Timer<DataNameAndContentOrReturnCode>& get_timer_in,
             MaidNodeDispatcher& dispatcher_in)
      : get_timer(get_timer_in), dispatcher(dispatcher_in), get_info(), mutex() {}

  template <typename DataName>
  void Get(const DataName& data_name,
           std::shared_ptr<boost::promise<typename DataName::data_type>> promise,
           const std::chrono::steady_clock::duration& timeout);

  void AddResponse(routing::TaskId task_id, const DataNameAndContentOrReturnCode& response);

 private:
  template <typename DataName>
  void DoGet(const DataName& data_name,
             std::shared_ptr<nfs::OpData<DataNameAndContentOrReturnCode>> op_data,
             const std::chrono::steady_clock::duration& timeout,
             const routing::TaskId task_id);

  routing::Timer<DataNameAndContentOrReturnCode>& get_timer;
  MaidNodeDispatcher& dispatcher;
  std::map<routing::TaskId, GetInfo> get_info;
  std::mutex mutex;
};

template <typename DataName>
void GetHandler::Get(const DataName& data_name,
                     std::shared_ptr<boost::promise<typename DataName::data_type>> promise,
           const std::chrono::steady_clock::duration& timeout) {
  auto task_id(get_timer.NewTaskId());
  HandleGetResult<typename DataName::data_type> response_functor(promise);
  auto op_data(
           std::make_shared<nfs::OpData<DataNameAndContentOrReturnCode>>(1, response_functor));
  {
    std::lock_guard<std::mutex> lock(mutex);
    get_info.insert(std::make_pair(task_id, std::make_tuple(0, task_id,
                                   GetDataNameVariant(DataName::data_type::Tag::kValue,
                                                      data_name.value), 1)));
  }
  DoGet(data_name, op_data, timeout, task_id);
}

template <typename DataName>
void GetHandler::DoGet(const DataName& data_name,
                       std::shared_ptr<nfs::OpData<DataNameAndContentOrReturnCode>> op_data,
                       const std::chrono::steady_clock::duration& timeout,
                       const routing::TaskId task_id) {
  get_timer.AddTask(
      timeout,
      [op_data, data_name, timeout, task_id, this](DataNameAndContentOrReturnCode response) {
        LOG(kVerbose) << "GetHandler Get HandleResponseContents for " << HexSubstr(data_name.value);
        size_t timeout_count(2);
        {
          std::lock_guard<std::mutex> lock(this->mutex);
          for (auto iter(std::begin(this->get_info)); iter != std::end(this->get_info);) {
            if (std::get<1>(iter->second) == task_id) {
              timeout_count = std::get<3>(iter->second);
              this->get_info.erase(iter);
              break;
            } else {
              iter++;
            }
          }
        }
        if (timeout_count == 2)
          return;

        LOG(kVerbose) << "GetHandler Get HandleResponseContents for "
                      << HexSubstr(data_name.value) << ", "
                      << timeout_count;

        if (response.content) {
          LOG(kVerbose) << "GetHandler::DoGet::Timer::Functor::Final::Data " << task_id;
          op_data->HandleResponseContents(std::move(response));

        } else {
          if (timeout_count == 1) {
            auto new_task_id(get_timer.NewTaskId());
            LOG(kVerbose) << "GetHandler::DoGet::Timer::Functor::Retry " << task_id;
            {
              std::lock_guard<std::mutex> lock(mutex);
              get_info.insert(std::make_pair(new_task_id, std::make_tuple(0, task_id,
                                             GetDataNameVariant(DataName::data_type::Tag::kValue,
                                                                data_name.value), 0)));
            }
            DoGet(data_name, op_data, timeout, new_task_id);
          } else if ((timeout_count == 0)) {
            LOG(kVerbose) << "GetHandler::DoGet::Timer::Functor::Final::Timeout " << task_id;
            op_data->HandleResponseContents(std::move(response));
          }
        }
      },
      // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
      routing::Parameters::group_size * 2, task_id);
  dispatcher.SendGetRequest(task_id, data_name);
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_GET_HANDLER_H_
