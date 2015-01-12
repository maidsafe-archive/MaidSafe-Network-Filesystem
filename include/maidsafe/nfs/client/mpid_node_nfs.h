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

#ifndef MAIDSAFE_NFS_CLIENT_MPID_NODE_NFS_H_
#define MAIDSAFE_NFS_CLIENT_MPID_NODE_NFS_H_

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

#include "boost/thread/future.hpp"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "maidsafe/common/asio_service.h"
#include "maidsafe/common/data_types/structured_data_versions.h"
#include "maidsafe/passport/passport.h"
#include "maidsafe/passport/types.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/message_wrapper.h"
#include "maidsafe/nfs/service.h"
//#include "maidsafe/nfs/utils.h"
//#include "maidsafe/nfs/client/client_utils.h"
//#include "maidsafe/nfs/client/mpid_node_dispatcher.h"
//#include "maidsafe/nfs/client/mpid_node_service.h"
#include "maidsafe/nfs/client/get_handler.h"

namespace maidsafe {

namespace nfs_client {

class MpidNodeNfs : public std::enable_shared_from_this<MpidNodeNfs>  {
 public:
  static std::shared_ptr<MpidNodeNfs> MakeShared(const passport::MpidAndSigner& mpid_and_signer);
  // Disconnects from network and all unfinished tasks will be cancelled
  void Stop();

 private:
//  typedef std::function<void(const AlertOrReturnCode&)> AlertFunctor;
//  typedef std::function<void(const ContentOrReturnCode&)> GetFunctor;

  explicit MpidNodeNfs(const passport::Mpid& mpid);

  MpidNodeNfs(const MpidNodeNfs&);
  MpidNodeNfs(MpidNodeNfs&&);
  MpidNodeNfs& operator=(MpidNodeNfs);

  template <typename T>
  void OnMessageReceived(const T& routing_message);

  template <typename T>
  void HandleMessage(const T& routing_message);

  const passport::Mpid kMpid_;
  AsioService asio_service_;
//  MpidNodeService::RpcTimers rpc_timers_;
//  MpidNodeDispatcher dispatcher_;
//  nfs::Service<MpidNodeService> service_;
//  GetHandler<MpidNodeDispatcher> get_handler_;
};


template <typename T>
void MpidNodeNfs::OnMessageReceived(const T& routing_message) {
  LOG(kVerbose) << "NFS::OnMessageReceived";
  std::shared_ptr<MpidNodeNfs> this_ptr(shared_from_this());
  asio_service_.service().post([=] {
      LOG(kVerbose) << "NFS::OnMessageReceived invoked task in asio_service";
      this_ptr->HandleMessage(routing_message);
  });
}

template <typename T>
void MpidNodeNfs::HandleMessage(const T& routing_message) {
  LOG(kVerbose) << "MpidNodeNfs::HandleMessage";
  auto wrapper_tuple(nfs::ParseMessageWrapper(routing_message.contents));
  const auto& destination_persona(std::get<2>(wrapper_tuple));
  static_assert(std::is_same<decltype(destination_persona),
                             const nfs::detail::DestinationTaggedValue&>::value,
                "The value retrieved from the tuple isn't the destination type, but should be.");
  if (destination_persona.data == nfs::Persona::kMaidNode)
    return service_.HandleMessage(wrapper_tuple, routing_message.sender, routing_message.receiver);
  auto action(std::get<0>(wrapper_tuple));
  auto source_persona(std::get<1>(wrapper_tuple).data);
  LOG(kError) << " MpidNodeNfs::HandleMessage unhandled message from " << source_persona
              << " " << action << " to " << destination_persona;
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MPID_NODE_NFS_H_
