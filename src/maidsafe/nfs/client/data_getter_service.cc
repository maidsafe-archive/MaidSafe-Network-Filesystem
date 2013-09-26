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

template <>
void DataGetterService::HandleMessage<DataGetterService::GetResponse>(
    const GetResponse& message, const typename GetResponse::Sender& /*sender*/,
    const typename GetResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  static_cast<void>(routing_);
  get_timer_.AddResponse(message.message_id.data, *message.contents);
}

template <>
void DataGetterService::HandleMessage<DataGetterService::GetVersionsResponse>(
    const GetVersionsResponse& message, const typename GetVersionsResponse::Sender& /*sender*/,
    const typename GetVersionsResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  get_versions_timer_.AddResponse(message.message_id.data, *message.contents);
}

template <>
void DataGetterService::HandleMessage<DataGetterService::GetBranchResponse>(
    const GetBranchResponse& message, const typename GetBranchResponse::Sender& /*sender*/,
    const typename GetBranchResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  get_branch_timer_.AddResponse(message.message_id.data, *message.contents);
}

}  // namespace nfs_client

}  // namespace maidsafe
