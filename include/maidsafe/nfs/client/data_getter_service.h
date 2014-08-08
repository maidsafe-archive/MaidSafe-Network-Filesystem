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

#ifndef MAIDSAFE_NFS_CLIENT_DATA_GETTER_SERVICE_H_
#define MAIDSAFE_NFS_CLIENT_DATA_GETTER_SERVICE_H_

#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"
#include "maidsafe/nfs/client/get_handler.h"
#include "maidsafe/nfs/client/data_getter_dispatcher.h"

namespace maidsafe {

namespace nfs_client {

class DataGetterService {
 public:
  typedef nfs::DataGetterServiceMessages PublicMessages;
  typedef void VaultMessages;
  typedef void HandleMessageReturnType;

  typedef nfs::GetResponseFromDataManagerToDataGetter GetResponse;
  typedef nfs::GetCachedResponseFromCacheHandlerToDataGetter GetCachedResponse;
  typedef nfs::GetVersionsResponseFromVersionHandlerToDataGetter GetVersionsResponse;
  typedef nfs::GetBranchResponseFromVersionHandlerToDataGetter GetBranchResponse;

  DataGetterService(
      routing::Routing& routing,
      GetHandler<DataGetterDispatcher>& get_handler,
      routing::Timer<DataGetterService::GetVersionsResponse::Contents>& get_versions_timer,
      routing::Timer<DataGetterService::GetBranchResponse::Contents>& get_branch_timer);

  void HandleMessage(const GetResponse& message, const GetResponse::Sender& sender,
                     const GetResponse::Receiver& receiver);

  void HandleMessage(const GetCachedResponse& message, const GetCachedResponse::Sender& sender,
                     const GetCachedResponse::Receiver& receiver);

  void HandleMessage(const GetVersionsResponse& message, const GetVersionsResponse::Sender& sender,
                     const GetVersionsResponse::Receiver& receiver);

  void HandleMessage(const GetBranchResponse& message, const GetBranchResponse::Sender& sender,
                     const GetBranchResponse::Receiver& receiver);

 private:
  routing::Routing& routing_;
  GetHandler<DataGetterDispatcher>& get_handler_;
  routing::Timer<DataGetterService::GetVersionsResponse::Contents>& get_versions_timer_;
  routing::Timer<DataGetterService::GetBranchResponse::Contents>& get_branch_timer_;
};


}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_DATA_GETTER_SERVICE_H_
