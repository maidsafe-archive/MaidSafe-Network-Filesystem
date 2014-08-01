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

#include "maidsafe/nfs/client/data_getter_service.h"

namespace maidsafe {

namespace nfs_client {

DataGetterService::DataGetterService(
    routing::Routing& routing, routing::Timer<DataGetterService::GetResponse::Contents>& get_timer,
    routing::Timer<DataGetterService::GetVersionsResponse::Contents>& get_versions_timer,
    routing::Timer<DataGetterService::GetBranchResponse::Contents>& get_branch_timer)
        : routing_(routing),
          get_timer_(get_timer),
          get_versions_timer_(get_versions_timer),
          get_branch_timer_(get_branch_timer) {}

void DataGetterService::HandleMessage(const GetResponse& message,
                                      const GetResponse::Sender& /*sender*/,
                                      const GetResponse::Receiver& receiver) {
  LOG(kVerbose) << "DataGetterService::HandleMessage GetResponse with message id "
                << message.id.data << " with DataNameAndContentOrReturnCode "
                << HexSubstr(message.contents->Serialise());
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  static_cast<void>(routing_);
  try {
    get_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != make_error_code(CommonErrors::no_such_element))
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void DataGetterService::HandleMessage(const GetCachedResponse& message,
                                      const GetCachedResponse::Sender& /*sender*/,
                                      const GetCachedResponse::Receiver& receiver) {
  LOG(kVerbose) << "DataGetterService::GetCachedResponse GetResponse "
                << HexSubstr(message.Serialise()) << " with content "
                << HexSubstr(message.contents->Serialise());
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  static_cast<void>(routing_);
  try {
    get_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != make_error_code(CommonErrors::invalid_parameter))
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void DataGetterService::HandleMessage(const GetVersionsResponse& message,
                                      const GetVersionsResponse::Sender& /*sender*/,
                                      const GetVersionsResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  try {
    get_versions_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != make_error_code(CommonErrors::invalid_parameter))
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void DataGetterService::HandleMessage(const GetBranchResponse& message,
                                      const GetBranchResponse::Sender& /*sender*/,
                                      const GetBranchResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  try {
    get_branch_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != make_error_code(CommonErrors::invalid_parameter))
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

}  // namespace nfs_client

}  // namespace maidsafe
