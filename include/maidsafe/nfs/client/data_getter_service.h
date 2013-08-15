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
#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"


namespace maidsafe {

namespace nfs_client {

class DataGetterService {
 public:
  typedef nfs::DataGetterServiceMessages PublicMessages;
  typedef void VaultMessages;

  DataGetterService(routing::Routing& routing);

  template<typename T>
  void HandleMessage(const T& message,
                     const typename T::Sender& sender,
                     const typename T::Receiver& receiver) {
    T::invalid_message_type_passed::should_be_one_of_the_specialisations_defined_below;
  }

  typedef nfs::GetResponseFromDataManagerToDataGetter GetResponse;
  template<>
  void HandleMessage<GetResponse>(const GetResponse& message,
                                  const typename GetResponse::Sender& sender,
                                  const typename GetResponse::Receiver& receiver);

  typedef nfs::GetVersionsResponseFromVersionManagerToDataGetter GetVersionsResponse;
  template<>
  void HandleMessage<GetVersionsResponse>(const GetVersionsResponse& message,
                                          const typename GetVersionsResponse::Sender& sender,
                                          const typename GetVersionsResponse::Receiver& receiver);

  typedef nfs::GetBranchResponseFromVersionManagerToDataGetter GetBranchResponse;
  template<>
  void HandleMessage<GetBranchResponse>(const GetBranchResponse& message,
                                        const typename GetBranchResponse::Sender& sender,
                                        const typename GetBranchResponse::Receiver& receiver);

 private:
  //class DataVisitorPut : public boost::static_visitor<> {
  // public:
  //  DataVisitorPut(MaidManagerService* service,
  //                 const nfs::PutRequestFromMaidNodeToMaidManager& message,
  //                 const typename nfs::PutRequestFromMaidNodeToMaidManager::Sender& sender)
  //      : service_(service),
  //        message_(message),
  //        sender_(sender) {}
  //  template<typename DataName>
  //  void operator()(const DataName& data_name) {
  //    service_->HandlePut(data_name, message_, sender_);
  //  }
  //  MaidManagerService* service_;
  //  const nfs::PutRequestFromMaidNodeToMaidManager& message_;
  //  const typename nfs::PutRequestFromMaidNodeToMaidManager::Sender& sender_;
  //};

  routing::Routing& routing_;
};

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_DATA_GETTER_SERVICE_H_
