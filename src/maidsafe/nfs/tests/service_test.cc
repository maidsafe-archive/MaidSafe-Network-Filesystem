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
#include "maidsafe/common/data_types/immutable_data.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"
#include "maidsafe/passport/types.h"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/message_wrapper.h"
#include "maidsafe/nfs/client/data_getter_dispatcher.h"
#include "maidsafe/nfs/client/data_getter_service.h"
#include "maidsafe/nfs/client/get_handler.h"
#include "maidsafe/nfs/client/maid_node_service.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"

namespace maidsafe {

namespace nfs {

namespace test {

class ServiceTest : public testing::Test {};

TEST_F(ServiceTest, BEH_All) {
  passport::Anmaid anmaid;
  passport::Maid maid(anmaid);
  routing::Routing routing(maid);
  AsioService asio_service(2);
  typedef nfs_client::DataGetterService::GetResponse GetResponse;
  nfs_client::DataGetterDispatcher dispatcher(routing);
  routing::Timer<typename GetResponse::Contents> get_timer(asio_service);
  nfs_client::GetHandler<nfs_client::DataGetterDispatcher> get_handler(get_timer,
                                                                       dispatcher);
  routing::Timer<typename nfs_client::DataGetterService::GetVersionsResponse::Contents>
      get_versions_timer(asio_service);
  routing::Timer<typename nfs_client::DataGetterService::GetBranchResponse::Contents>
      get_branch_timer(asio_service);
  Service<nfs_client::DataGetterService> service(
      std::move(std::unique_ptr<nfs_client::DataGetterService>(
          new nfs_client::DataGetterService(
              routing, get_handler, get_versions_timer, get_branch_timer))));

  ImmutableData immutable_data(NonEmptyString(RandomString(10)));
  auto promise(std::make_shared<boost::promise<ImmutableData>>());
  get_handler.Get(immutable_data.name(), promise, std::chrono::milliseconds(500));
  boost::future<ImmutableData> result(promise->get_future());

  GetResponse::Contents contents(immutable_data);
  auto task_id(get_timer.NewTaskId());
  --task_id;
  GetResponse get_response(MessageId(task_id), contents);
  auto serialised_get_response(get_response.Serialise());

  NodeId sender_node_id(NodeId::IdType::kRandomId);
  NodeId sender_group_id(NodeId::IdType::kRandomId);
  NodeId receiver_node_id(routing.kNodeId());
  GetResponse::Sender sender((routing::GroupId(sender_node_id)),
                             (routing::SingleId(sender_group_id)));
  GetResponse::Receiver receiver(receiver_node_id);

  auto response_tuple(ParseMessageWrapper(serialised_get_response));

  service.HandleMessage(response_tuple, sender, receiver);

  ImmutableData retrieved(result.get());
  EXPECT_EQ(immutable_data.name(), retrieved.name())
      << "\nOriginal name:  " << Base64Substr(immutable_data.name()->string())
      << "\nRetrieved name: " << Base64Substr(retrieved.name()->string());
  EXPECT_EQ(immutable_data.data(), retrieved.data());
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
