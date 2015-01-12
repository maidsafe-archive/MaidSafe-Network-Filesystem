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

#include "maidsafe/nfs/client/mpid_node_nfs.h"

#include "maidsafe/common/on_scope_exit.h"
#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/make_unique.h"

namespace maidsafe {

namespace nfs_client {

std::shared_ptr<MpidNodeNfs> MpidNodeNfs::MakeShared(
    const passport::MpidAndSigner& maid_and_signer) {
  std::shared_ptr<MpidNodeNfs> mpid_node_ptr{ new MpidNodeNfs{ maid_and_signer.first } };
  return mpid_node_ptr;
}

MpidNodeNfs::MpidNodeNfs(const passport::Mpid& mpid)
    : kMpid_(mpid),
      asio_service_(2)
{}

void MpidNodeNfs::Stop() {
  /*LOG(kVerbose) << "MaidNodeNfs::Stop()";
  dispatcher_.Stop();
  LOG(kVerbose) << "MaidNodeNfs::Stop() : dispatcher_";
  routing_.reset();
  LOG(kVerbose) << "MaidNodeNfs::Stop() : routing_";
  rpc_timers_.CancellAll();
  LOG(kVerbose) << "MaidNodeNfs::Stop() : rpc_timers_";
  asio_service_.Stop();
  LOG(kVerbose) << "MaidNodeNfs::Stop() : asio_service_";*/
}

}  // namespace nfs_client

}  // namespace maidsafe
