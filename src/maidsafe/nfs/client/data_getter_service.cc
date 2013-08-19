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

#include "maidsafe/nfs/client/data_getter_service.h"


namespace maidsafe {

namespace nfs_client {

DataGetterService::DataGetterService(routing::Routing& routing) : routing_(routing) {}

template<>
void DataGetterService::HandleMessage<DataGetterService::GetResponse>(
    const GetResponse& /*message*/,
    const typename GetResponse::Sender& /*sender*/,
    const typename GetResponse::Receiver& /*receiver*/) {
  ThrowError(CommonErrors::unknown);  // Not implemented.
}

template<>
void DataGetterService::HandleMessage<DataGetterService::GetVersionsResponse>(
    const GetVersionsResponse& /*message*/,
    const typename GetVersionsResponse::Sender& /*sender*/,
    const typename GetVersionsResponse::Receiver& /*receiver*/) {
  ThrowError(CommonErrors::unknown);  // Not implemented.
}

template<>
void DataGetterService::HandleMessage<DataGetterService::GetBranchResponse>(
    const GetBranchResponse& /*message*/,
    const typename GetBranchResponse::Sender& /*sender*/,
    const typename GetBranchResponse::Receiver& /*receiver*/) {
  ThrowError(CommonErrors::unknown);  // Not implemented.
}

}  // namespace nfs_client

}  // namespace maidsafe
