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

#include "maidsafe/nfs/client/maid_node_service.h"


namespace maidsafe {

namespace nfs_client {

MaidNodeService::MaidNodeService(
    routing::Routing& routing,
    routing::Timer<MaidNodeService::GetResponse::Contents>& get_timer,
    routing::Timer<MaidNodeService::GetVersionsResponse::Contents>& get_versions_timer,
    routing::Timer<MaidNodeService::GetBranchResponse::Contents>& get_branch_timer)
        : routing_(routing),
          get_timer_(get_timer),
          get_versions_timer_(get_versions_timer),
          get_branch_timer_(get_branch_timer) {}

template<>
void MaidNodeService::HandleMessage<MaidNodeService::GetResponse>(
    const GetResponse& message,
    const typename GetResponse::Sender& /*sender*/,
    const typename GetResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  get_timer_.AddResponse(message.message_id.data, *message.contents);
}

template<>
void MaidNodeService::HandleMessage<MaidNodeService::GetVersionsResponse>(
    const GetVersionsResponse& message,
    const typename GetVersionsResponse::Sender& /*sender*/,
    const typename GetVersionsResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  get_versions_timer_.AddResponse(message.message_id.data, *message.contents);
}

template<>
void MaidNodeService::HandleMessage<MaidNodeService::GetBranchResponse>(
    const GetBranchResponse& message,
    const typename GetBranchResponse::Sender& /*sender*/,
    const typename GetBranchResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  get_branch_timer_.AddResponse(message.message_id.data, *message.contents);
}

template<>
void MaidNodeService::HandleMessage<MaidNodeService::PutResponse>(
    const PutResponse& /*message*/,
    const typename PutResponse::Sender& /*sender*/,
    const typename PutResponse::Receiver& /*receiver*/) {
  // TODO(Fraser#5#): 2013-08-24 - Decide on how this is to be handled, and implement.
  assert(0);
}

}  // namespace nfs_client

}  // namespace maidsafe
