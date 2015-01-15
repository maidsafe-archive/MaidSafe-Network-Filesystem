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

#ifndef MAIDSAFE_NFS_CLIENT_MPID_CLIENT_H_
#define MAIDSAFE_NFS_CLIENT_MPID_CLIENT_H_

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

class MpidClient : public std::enable_shared_from_this<MpidClient>  {
 public:
   // Logging in for already existing mpid accounts
  static std::shared_ptr<MpidClient> MakeShared(const passport::Mpid& mpid);
  // Creates mpid account and logs in. Throws on failure to create account.
  static std::shared_ptr<MpidClient> MakeShared(const passport::MpidAndSigner& mpid_and_signer);
  // Disconnects from network and all unfinished tasks will be cancelled
  void Stop();

  template <typename DataName>
  boost::future<typename DataName::data_type> Get(
      const DataName& data_name,
      const std::chrono::steady_clock::duration& timeout = std::chrono::seconds(120));

 private:
//  typedef std::function<void(const AlertOrReturnCode&)> AlertFunctor;
//  typedef std::function<void(const ContentOrReturnCode&)> GetFunctor;

  explicit MpidClient(const passport::Mpid& mpid);

  MpidClient(const MpidClient&);
  MpidClient(MpidClient&&);
  MpidClient& operator=(MpidClient);

  void Init(const passport::MpidAndSigner& mpid_and_signer);
  void Init();
  void InitRouting();

  void CreateAccount(const passport::PublicMpid& public_mpid,
                     const passport::PublicAnmpid& public_anmpid);

  routing::Functors InitialiseRoutingCallbacks();
  void OnNetworkStatusChange(int updated_network_health);

  template <typename T>
  void OnMessageReceived(const T& routing_message);

  template <typename T>
  void HandleMessage(const T& routing_message);

  const passport::Mpid kMpid_;
  AsioService asio_service_;
//  MpidNodeService::RpcTimers rpc_timers_;
  std::mutex network_health_mutex_;
  std::condition_variable network_health_condition_variable_;
  int network_health_;
//  OnNetworkHealthChange network_health_change_signal_;
  std::unique_ptr<routing::Routing> routing_;
  nfs::detail::PublicPmidHelper public_pmid_helper_;
//  MpidNodeDispatcher dispatcher_;
//  nfs::Service<MpidNodeService> service_;
//  GetHandler<MpidNodeDispatcher> get_handler_;
};

// ==================== Implementation =============================================================
template <typename DataName>
boost::future<typename DataName::data_type> MpidClient::Get(
    const DataName& data_name,
    const std::chrono::steady_clock::duration& /*timeout*/) {
  LOG(kVerbose) << "MpidClient Get " << HexSubstr(data_name.value);
  auto promise(std::make_shared<boost::promise<typename DataName::data_type>>());
//  get_handler_.Get(data_name, promise, timeout);
  return promise->get_future();
}

template <typename T>
void MpidClient::OnMessageReceived(const T& routing_message) {
  LOG(kVerbose) << "NFS::OnMessageReceived";
  std::shared_ptr<MpidClient> this_ptr(shared_from_this());
  asio_service_.service().post([=] {
      LOG(kVerbose) << "NFS::OnMessageReceived invoked task in asio_service";
      this_ptr->HandleMessage(routing_message);
  });
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_MPID_CLIENT_H_
