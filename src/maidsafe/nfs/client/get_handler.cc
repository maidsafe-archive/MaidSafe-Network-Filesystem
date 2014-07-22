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

#include "maidsafe/nfs/client/get_handler.h"

namespace maidsafe {

namespace nfs_client {

void GetHandler::AddResponse(routing::TaskId task_id,
                             const DataNameAndContentOrReturnCode& response) {
  Operation operation(Operation::kNoOperation);
  routing::TaskId new_task_id(0);
  GetInfo get_info;

  {
    std::lock_guard<std::mutex> lock(mutex_);
    auto found(get_info_.find(task_id));
    if (found == std::end(get_info_))
      return;
    get_info = get_info_[task_id];
    std::get<0>(get_info_[task_id])++;
    if (response.content && ValidateData(*response.content, std::get<2>(get_info_[task_id]))) {
      get_info_.erase(task_id);
      operation = Operation::kAddResponse;
    } else if (response.return_code &&
               (std::get<0>(get_info_[task_id]) >= routing::Parameters::group_size)) {
      new_task_id = get_timer_.NewTaskId();
      get_info_.insert(std::make_pair(new_task_id,
                                     std::make_tuple(0, std::get<1>(get_info_[task_id]),
                                                     std::get<2>(get_info_[task_id]))));
      get_info_.erase(task_id);
      operation = Operation::kSendRequest;
    } else  if (!response.return_code && !response.content) {
      get_info_.erase(task_id);
      operation = Operation::kCancelTask;
    }
  }

  LOG(kVerbose) << static_cast<int>(operation) << " GetHandler::AddResponse "  << task_id
                <<  " original task id: " << std::get<1>(get_info_[task_id]);

  if (operation == Operation::kAddResponse) {
    get_timer_.AddResponse(std::get<1>(get_info), response);
  } else if (operation == Operation::kSendRequest) {
    GetHandlerVisitor get_handler_visitor(dispatcher_, new_task_id);
    boost::apply_visitor(get_handler_visitor, std::get<2>(get_info_[new_task_id]));
  } else if (operation == Operation::kCancelTask) {
    get_timer_.CancelTask(std::get<1>(get_info));
  }
}

bool GetHandler::ValidateData(const nfs_vault::Content& content,
                              const DataNameVariant& data_name) {
  ValidateDataVisitor validate_data_visitor(content);
  return boost::apply_visitor(validate_data_visitor, data_name);
}

}  // namespace nfs_client

}  // namespace maidsafe

