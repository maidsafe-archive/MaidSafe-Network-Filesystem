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

#ifndef MAIDSAFE_NFS_CLIENT_DATA_GETTER_H_
#define MAIDSAFE_NFS_CLIENT_DATA_GETTER_H_

#include <functional>
#include <vector>

#include "maidsafe/common/asio_service.h"
#include "maidsafe/passport/types.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/service.h"
#include "maidsafe/nfs/client/data_getter_dispatcher.h"
#include "maidsafe/nfs/client/data_getter_service.h"


namespace maidsafe {

namespace nfs_client {

class DataGetter {
 public:
  // all_pmids_from_file should only be non-empty if TESTING is defined
  DataGetter(AsioService& asio_service,
             routing::Routing& routing,
             const std::vector<passport::PublicPmid>& public_pmids_from_file =
                 std::vector<passport::PublicPmid>());

  //template<typename Data>
  //void GetKey(const typename Data::Name& key_name,
  //            std::function<void(Message)> get_key_functor);

 private:
  routing::Timer<DataGetterService::GetResponse> get_timer_;
  routing::Timer<DataGetterService::GetVersionsResponse> get_versions_timer_;
  routing::Timer<DataGetterService::GetBranchResponse> get_branch_timer_;
  DataGetterDispatcher dispatcher_;
  nfs::Service<DataGetterService> service_;
#ifdef TESTING
  std::vector<passport::PublicPmid> kAllPmids_;
#endif
};

//template<typename Data>
//void PublicKeyGetter::GetKey(const typename Data::Name& key_name,
//                             std::function<void(Message)> get_key_functor) {
//  static_assert(passport::is_public_key_type<Data>::value,
//                "Error, type must be a PublicKey type");
//#ifdef TESTING
//  if (key_getter_nfs_) {
//#endif
//    auto get_op(std::make_shared<OperationOp>(1, get_key_functor));
//    key_getter_nfs_->Get<Data>(key_name,
//                               [get_op](std::string serialised_reply) {
//                                 HandleOperationReply(get_op, serialised_reply);
//                               });
//#ifdef TESTING
//  }
//#endif
//}
//
//template<>
//void PublicKeyGetter::GetKey<passport::PublicPmid>(
//    const typename passport::PublicPmid::Name& key_name,
//    std::function<void(Message)> get_key_functor);

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_DATA_GETTER_H_
