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

MaidNodeService::MaidNodeService(routing::Routing& routing) : routing_(routing) {}

template<>
void MaidNodeService::HandleMessage<nfs::GetResponseFromDataManagerToMaidNode>(
    const nfs::GetResponseFromDataManagerToMaidNode& /*message*/,
    const typename nfs::GetResponseFromDataManagerToMaidNode::Sender& /*sender*/,
    const typename nfs::GetResponseFromDataManagerToMaidNode::Receiver& /*receiver*/) {
  ThrowError(CommonErrors::unknown);  // Not implemented.
}

template<>
void MaidNodeService::HandleMessage<nfs::PutResponseFromMaidManagerToMaidNode>(
    const nfs::PutResponseFromMaidManagerToMaidNode& /*message*/,
    const typename nfs::PutResponseFromMaidManagerToMaidNode::Sender& /*sender*/,
    const typename nfs::PutResponseFromMaidManagerToMaidNode::Receiver& /*receiver*/) {
  ThrowError(CommonErrors::unknown);  // Not implemented.
}

template<>
void MaidNodeService::HandleMessage<nfs::GetVersionsResponseFromVersionManagerToMaidNode>(
    const nfs::GetVersionsResponseFromVersionManagerToMaidNode& /*message*/,
    const typename nfs::GetVersionsResponseFromVersionManagerToMaidNode::Sender& /*sender*/,
    const typename nfs::GetVersionsResponseFromVersionManagerToMaidNode::Receiver& /*receiver*/) {
  ThrowError(CommonErrors::unknown);  // Not implemented.
}

template<>
void MaidNodeService::HandleMessage<nfs::GetBranchResponseFromVersionManagerToMaidNode>(
    const nfs::GetBranchResponseFromVersionManagerToMaidNode& /*message*/,
    const typename nfs::GetBranchResponseFromVersionManagerToMaidNode::Sender& /*sender*/,
    const typename nfs::GetBranchResponseFromVersionManagerToMaidNode::Receiver& /*receiver*/) {
  ThrowError(CommonErrors::unknown);  // Not implemented.
}

}  // namespace nfs_client

}  // namespace maidsafe
