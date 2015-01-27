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
#include <string>
#include <utility>

#include "boost/thread/future.hpp"

#include "maidsafe/common/data_types/data_name_variant.h"

#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/service.h"
#include "maidsafe/nfs/client/maid_node_dispatcher.h"
#include "maidsafe/nfs/client/maid_node_service.h"
#include "maidsafe/nfs/client/mpid_node_dispatcher.h"
#include "maidsafe/nfs/client/mpid_node_service.h"
#include "maidsafe/nfs/client/client_utils.h"

namespace maidsafe {

namespace nfs_client {

template <typename DispatcherType>
class GetHandlerVisitor : public boost::static_visitor<> {
 public:
  GetHandlerVisitor(DispatcherType& dispatcher_in, routing::TaskId task_id)
      : dispatcher_(dispatcher_in), kTaskId_(task_id) {}

  template <typename Name>
  void operator()(const Name& data_name) {
    LOG(kVerbose) << "Get handler visitor sending get request for chunk "
                  << HexSubstr(data_name.value.string());
    dispatcher_.SendGetRequest(kTaskId_, data_name);
  }

 private:
  DispatcherType& dispatcher_;
  const routing::TaskId kTaskId_;
};

class ValidateDataVisitor : public boost::static_visitor<bool> {
 public:
  explicit ValidateDataVisitor(const nfs_vault::Content& content) : content_(content) {}

  template<typename DataNameType>
  result_type operator()(const DataNameType& data_name) {
    return (typename DataNameType::data_type(data_name,
                     typename DataNameType::data_type::serialised_type(
                         NonEmptyString(content_.data)))).name() == data_name;
  }

 private:
  nfs_vault::Content content_;
};

template <typename DispatcherType>
class GetHandler {
  typedef std::tuple<size_t, routing::TaskId, DataNameVariant> GetInfo;
  enum class Operation : int {
    kNoOperation = 0,
    kAddResponse = 1,
    kSendRequest = 2,
    kCancelTask = 3
  };

 public:
  GetHandler(routing::Timer<DataNameAndContentOrReturnCode>& get_timer,
             DispatcherType& dispatcher)
      : get_timer_(get_timer), dispatcher_(dispatcher), get_info_(), mutex_() {}

  template <typename DataName>
  void Get(const DataName& data_name,
           std::shared_ptr<boost::promise<typename DataName::data_type>> promise,
           const std::chrono::steady_clock::duration& timeout);

  void AddResponse(routing::TaskId task_id, const DataNameAndContentOrReturnCode& response);

 private:
  bool ValidateData(const nfs_vault::Content& content, const DataNameVariant& data_name);
  routing::Timer<DataNameAndContentOrReturnCode>& get_timer_;
  DispatcherType& dispatcher_;
  std::map<routing::TaskId, GetInfo> get_info_;
  std::mutex mutex_;
};

template <typename DispatcherType>
template <typename DataName>
void GetHandler<DispatcherType>::Get(
    const DataName& data_name,
    std::shared_ptr<boost::promise<typename DataName::data_type>> promise,
    const std::chrono::steady_clock::duration& timeout) {
  auto task_id(get_timer_.NewTaskId());
  HandleGetResult<typename DataName::data_type> response_functor(promise);
  auto op_data(
           std::make_shared<nfs::OpData<DataNameAndContentOrReturnCode>>(1, response_functor));
  {
    std::lock_guard<std::mutex> lock(mutex_);
    get_info_.insert(std::make_pair(task_id, std::make_tuple(0, task_id,
                                    GetDataNameVariant(DataName::data_type::Tag::kValue,
                                                       data_name.value))));
  }
  get_timer_.AddTask(timeout,
                     [op_data, data_name, task_id, this](
                         DataNameAndContentOrReturnCode get_response) {
                        LOG(kVerbose) << "GetHandler Get HandleResponseContents for "
                                      << HexSubstr(data_name.value);
                        op_data->HandleResponseContents(std::move(get_response));
                        {
                          std::lock_guard<std::mutex> lock(mutex_);
                          auto iter(std::find_if(std::begin(get_info_), std::end(get_info_),
                                                 [task_id](const std::pair<routing::TaskId,
                                                                           GetInfo>& info) {
                                                   return std::get<1>(info.second) == task_id;
                                                 }));
                          if (iter != std::end(get_info_))
                            get_info_.erase(iter);
                        }
                     }, 1, task_id);
  dispatcher_.SendGetRequest(task_id, data_name);
}

template <typename DispatcherType>
void GetHandler<DispatcherType>::AddResponse(routing::TaskId task_id,
                                              const DataNameAndContentOrReturnCode& response) {
  LOG(kVerbose) << " GetHandler::AddResponse "  << task_id;
  Operation operation(Operation::kNoOperation);
  routing::TaskId new_task_id(0);
  GetInfo get_info;

  {
    std::lock_guard<std::mutex> lock(mutex_);
    auto found(get_info_.find(task_id));
    if (found == std::end(get_info_))
      return;

    if (std::get<1>(get_info_[task_id]) == 0)
      return;

    get_info = get_info_[task_id];
    std::get<0>(get_info_[task_id])++;
    if (response.content && ValidateData(*response.content, std::get<2>(get_info_[task_id]))) {
      operation = Operation::kAddResponse;
    } else if (response.return_code &&
               response.return_code->value.code() != make_error_code(CommonErrors::defaulted) &&
               (std::get<0>(get_info_[task_id]) == routing::Parameters::group_size - 1)) {
      new_task_id = get_timer_.NewTaskId();
      get_info_.insert(std::make_pair(new_task_id,
                                      std::make_tuple(0, std::get<1>(get_info_[task_id]),
                                                      std::get<2>(get_info_[task_id]))));
      get_info_.erase(task_id);
      operation = Operation::kSendRequest;
    } else if (response.return_code &&
               response.return_code->value.code() == make_error_code(CommonErrors::defaulted) &&
               !response.content) {
      operation = Operation::kCancelTask;  // timed-out : VERY VERY IMPORTANT PLEASE NOTE
      // THIS HAS TO BE COMPATIBLE WITH THE DEFAULT CONSTRUCTOR OF RETURN-CODE
    }
  }

  LOG(kVerbose) << " GetHandler::AddResponse "  << task_id
                << " original task id: " << std::get<1>(get_info_[task_id])
                << " operation " << static_cast<int>(operation);

  if (operation == Operation::kAddResponse) {
    get_timer_.AddResponse(std::get<1>(get_info), response);
  } else if (operation == Operation::kSendRequest) {
    GetHandlerVisitor<DispatcherType> get_handler_visitor(dispatcher_, new_task_id);
    boost::apply_visitor(get_handler_visitor, std::get<2>(get_info_[new_task_id]));
  } else if (operation == Operation::kCancelTask) {
    get_timer_.CancelTask(std::get<1>(get_info));
  }
}

template <typename DispatcherType>
bool GetHandler<DispatcherType>::ValidateData(const nfs_vault::Content& content,
                              const DataNameVariant& data_name) {
  ValidateDataVisitor validate_data_visitor(content);
  return boost::apply_visitor(validate_data_visitor, data_name);
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_GET_HANDLER_H_
