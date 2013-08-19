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

#include "maidsafe/nfs/service.h"

#include "maidsafe/common/test.h"
#include "maidsafe/common/types.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/routing/routing_api.h"
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
  maidsafe::nfs::Service<nfs_client::MaidNodeService> service(routing);

  ImmutableData immutable_data(NonEmptyString(RandomString(10)));
  nfs_client::DataOrDataNameAndReturnCode contents(immutable_data);

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
  maidsafe::nfs::Service<nfs_client::DataGetterService> service(routing);

  ImmutableData immutable_data(NonEmptyString(RandomString(10)));
  nfs_client::DataOrDataNameAndReturnCode contents(immutable_data);

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
