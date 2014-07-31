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

#include "maidsafe/nfs/client/data_getter.h"

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"

namespace maidsafe {

namespace nfs_client {

DataGetter::DataGetter(AsioService& asio_service, routing::Routing& routing)
    : get_timer_(asio_service),
      get_versions_timer_(asio_service),
      get_branch_timer_(asio_service),
      dispatcher_(routing),
      service_([&]()->std::unique_ptr<DataGetterService> {
                 std::unique_ptr<DataGetterService> service(
                 new DataGetterService(routing, get_timer_, get_versions_timer_,
                                       get_branch_timer_));
                 return std::move(service);
               }()),
      get_handler_(get_timer_, dispatcher_) {
}

}  // namespace nfs_client

}  // namespace maidsafe
