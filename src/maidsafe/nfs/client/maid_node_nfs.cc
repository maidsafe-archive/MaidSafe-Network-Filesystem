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

#include "maidsafe/nfs/client/maid_node_nfs.h"

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"


namespace maidsafe {

namespace nfs_client {

MaidNodeNfs::MaidNodeNfs(AsioService& asio_service,
                         routing::Routing& routing,
                         const passport::PublicPmid::Name& pmid_node_hint)
    : get_timer_(asio_service),
      get_versions_timer_(asio_service),
      get_branch_timer_(asio_service),
      dispatcher_(routing),
      service_([&]()->std::unique_ptr<MaidNodeService>&& {
          std::unique_ptr<MaidNodeService> service(
              new MaidNodeService(routing, get_timer_, get_versions_timer_, get_branch_timer_));
          return std::move(service);
      }()),
      pmid_node_hint_mutex_(),
      pmid_node_hint_(pmid_node_hint) {}


passport::PublicPmid::Name MaidNodeNfs::pmid_node_hint() const {
  std::lock_guard<std::mutex> lock(pmid_node_hint_mutex_);
  return pmid_node_hint_;
}

void MaidNodeNfs::set_pmid_node_hint(const passport::PublicPmid::Name& pmid_node_hint) {
  std::lock_guard<std::mutex> lock(pmid_node_hint_mutex_);
  pmid_node_hint_ = pmid_node_hint;
}

}  // namespace nfs_client

}  // namespace maidsafe
