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

#include "maidsafe/nfs/client/maid_node_service.h"

namespace maidsafe {

namespace nfs_client {

MaidNodeService::MaidNodeService(
    routing::Routing& routing, routing::Timer<MaidNodeService::GetResponse::Contents>& get_timer,
    routing::Timer<MaidNodeService::GetVersionsResponse::Contents>& get_versions_timer,
    routing::Timer<MaidNodeService::GetBranchResponse::Contents>& get_branch_timer)
        : routing_(routing),
          get_timer_(get_timer),
          get_versions_timer_(get_versions_timer),
          get_branch_timer_(get_branch_timer) {}

void MaidNodeService::HandleMessage(const GetResponse& message,
                                    const GetResponse::Sender& /*sender*/,
                                    const GetResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  get_timer_.AddResponse(message.id.data, *message.contents);
}

void MaidNodeService::HandleMessage(const GetCachedResponse& message,
                                    const GetCachedResponse::Sender& /*sender*/,
                                    const GetCachedResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  get_timer_.AddResponse(message.id.data, *message.contents);
}

void MaidNodeService::HandleMessage(const PutFailure& /*message*/,
                                    const PutFailure::Sender& /*sender*/,
                                    const PutFailure::Receiver& /*receiver*/) {
  // TODO(Fraser#5#): 2013-08-24 - Decide on how this is to be handled, and implement.
  assert(0);
}

void MaidNodeService::HandleMessage(const GetVersionsResponse& message,
                                    const GetVersionsResponse::Sender& /*sender*/,
                                    const GetVersionsResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  get_versions_timer_.AddResponse(message.id.data, *message.contents);
}

void MaidNodeService::HandleMessage(const PutVersionResponse& /*message*/,
                                    const PutVersionResponse::Sender& /*sender*/,
                                    const PutVersionResponse::Receiver& /*receiver*/) {
  // TODO(Mahmoud): Implement this.
  assert(0);
}

void MaidNodeService::HandleMessage(const GetBranchResponse& message,
                                    const GetBranchResponse::Sender& /*sender*/,
                                    const GetBranchResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  get_branch_timer_.AddResponse(message.id.data, *message.contents);
}

void MaidNodeService::HandleMessage(const PmidHealthResponse& /*message*/,
                                    const PmidHealthResponse::Sender& /*sender*/,
                                    const PmidHealthResponse::Receiver& /*receiver*/) {
  // TODO(Mahmoud): not sure about implementation
  assert(false && "Fraser Implement me please");
}

void MaidNodeService::HandleMessage(const CreateAccountResponse& /*message*/,
                                    const CreateAccountResponse::Sender& /*sender*/,
                                    const CreateAccountResponse::Receiver& /*receiver*/) {
  assert(false && "Fraser Implement me please");
}

}  // namespace nfs_client

}  // namespace maidsafe
