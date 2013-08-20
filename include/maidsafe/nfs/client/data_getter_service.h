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

#ifndef MAIDSAFE_NFS_CLIENT_DATA_GETTER_SERVICE_H_
#define MAIDSAFE_NFS_CLIENT_DATA_GETTER_SERVICE_H_

#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"


namespace maidsafe {

namespace nfs_client {

class DataGetterService {
 public:
  typedef nfs::DataGetterServiceMessages PublicMessages;
  typedef void VaultMessages;

  typedef nfs::GetResponseFromDataManagerToDataGetter GetResponse;
  typedef nfs::GetVersionsResponseFromVersionManagerToDataGetter GetVersionsResponse;
  typedef nfs::GetBranchResponseFromVersionManagerToDataGetter GetBranchResponse;

  DataGetterService(
      routing::Routing& routing,
      routing::Timer<DataGetterService::GetResponse::Contents>& get_timer,
      routing::Timer<DataGetterService::GetVersionsResponse::Contents>& get_versions_timer,
      routing::Timer<DataGetterService::GetBranchResponse::Contents>& get_branch_timer);

  template<typename T>
  void HandleMessage(const T& /*message*/,
                     const typename T::Sender& /*sender*/,
                     const typename T::Receiver& /*receiver*/) {
    T::invalid_message_type_passed::should_be_one_of_the_specialisations_defined_below;
  }

 private:
  routing::Routing& routing_;
  routing::Timer<DataGetterService::GetResponse::Contents>& get_timer_;
  routing::Timer<DataGetterService::GetVersionsResponse::Contents>& get_versions_timer_;
  routing::Timer<DataGetterService::GetBranchResponse::Contents>& get_branch_timer_;
};

template<>
void DataGetterService::HandleMessage<DataGetterService::GetResponse>(
    const GetResponse& message,
    const typename GetResponse::Sender& sender,
    const typename GetResponse::Receiver& receiver);

template<>
void DataGetterService::HandleMessage<DataGetterService::GetVersionsResponse>(
    const GetVersionsResponse& message,
    const typename GetVersionsResponse::Sender& sender,
    const typename GetVersionsResponse::Receiver& receiver);

template<>
void DataGetterService::HandleMessage<DataGetterService::GetBranchResponse>(
    const GetBranchResponse& message,
    const typename GetBranchResponse::Sender& sender,
    const typename GetBranchResponse::Receiver& receiver);

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_DATA_GETTER_SERVICE_H_
