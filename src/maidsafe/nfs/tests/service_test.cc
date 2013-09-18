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

#include "maidsafe/nfs/service.h"

#include "maidsafe/common/asio_service.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/types.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"
#include "maidsafe/passport/types.h"
#include "maidsafe/data_types/immutable_data.h"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/message_wrapper.h"
#include "maidsafe/nfs/client/data_getter_service.h"
#include "maidsafe/nfs/client/maid_node_service.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"


namespace maidsafe {

namespace nfs {

namespace test {

TEST(MaidNodeService, BEH_All) {
  passport::Anmaid anmaid;
  passport::Maid maid(anmaid);
  routing::Routing routing(maid);
  AsioService asio_service(2);
  routing::Timer<nfs_client::MaidNodeService::GetResponse::Contents> get_timer(asio_service);
  routing::Timer<nfs_client::MaidNodeService::GetVersionsResponse::Contents>
      get_versions_timer(asio_service);
  routing::Timer<nfs_client::MaidNodeService::GetBranchResponse::Contents>
      get_branch_timer(asio_service);
  maidsafe::nfs::Service<nfs_client::MaidNodeService> service(
      std::move(std::unique_ptr<nfs_client::MaidNodeService>(
          new nfs_client::MaidNodeService(routing, get_timer, get_versions_timer,
                                            get_branch_timer))));

  ImmutableData immutable_data(NonEmptyString(RandomString(10)));
  nfs_client::DataNameAndContentOrReturnCode contents(immutable_data);

  typedef nfs::GetResponseFromDataManagerToMaidNode GetResponse;
  GetResponse get_response(contents);
  auto serialised_get_response(get_response.Serialise());

  NodeId sender_node_id(NodeId::kRandomId);
  NodeId sender_group_id(NodeId::kRandomId);
  NodeId receiver_node_id(NodeId::kRandomId);
  GetResponse::Sender sender((routing::GroupId(sender_node_id)),
                             (routing::SingleId(sender_group_id)));
  GetResponse::Receiver receiver(receiver_node_id);

  auto response_tuple(ParseMessageWrapper(serialised_get_response));

  service.HandleMessage(response_tuple, sender, receiver);
}

TEST(DataGetterService, BEH_All) {
  passport::Anmaid anmaid;
  passport::Maid maid(anmaid);
  routing::Routing routing(maid);
  AsioService asio_service(2);
  routing::Timer<nfs_client::DataGetterService::GetResponse::Contents> get_timer(asio_service);
  routing::Timer<nfs_client::DataGetterService::GetVersionsResponse::Contents>
      get_versions_timer(asio_service);
  routing::Timer<nfs_client::DataGetterService::GetBranchResponse::Contents>
      get_branch_timer(asio_service);
  maidsafe::nfs::Service<nfs_client::DataGetterService> service(
      std::move(std::unique_ptr<nfs_client::DataGetterService>(
          new nfs_client::DataGetterService(routing, get_timer, get_versions_timer,
                                            get_branch_timer))));

  ImmutableData immutable_data(NonEmptyString(RandomString(10)));
  nfs_client::DataNameAndContentOrReturnCode contents(immutable_data);

  typedef nfs::GetResponseFromDataManagerToDataGetter GetResponse;
  GetResponse get_response(contents);
  auto serialised_get_response(get_response.Serialise());

  NodeId sender_node_id(NodeId::kRandomId);
  NodeId sender_group_id(NodeId::kRandomId);
  NodeId receiver_node_id(NodeId::kRandomId);
  GetResponse::Sender sender((routing::GroupId(sender_node_id)),
                             (routing::SingleId(sender_group_id)));
  GetResponse::Receiver receiver(receiver_node_id);

  auto response_tuple(ParseMessageWrapper(serialised_get_response));

  service.HandleMessage(response_tuple, sender, receiver);
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
