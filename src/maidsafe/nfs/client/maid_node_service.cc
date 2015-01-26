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

#include "maidsafe/nfs/client/maid_node_service.h"
#include "maidsafe/nfs/client/get_handler.h"

#include "maidsafe/common/error.h"

namespace maidsafe {

namespace nfs_client {

namespace {

std::error_code NoSuchElement() {
  static std::error_code code(make_error_code(CommonErrors::no_such_element));
  return code;
}

}  // unnamed namespace

MaidNodeService::RpcTimers::RpcTimers(BoostAsioService& asio_service_)
    : put_timer(asio_service_),
      get_versions_timer(asio_service_),
      get_branch_timer(asio_service_),
      create_account_timer(asio_service_),
      create_version_tree_timer(asio_service_),
      put_version_timer(asio_service_) {}

void MaidNodeService::RpcTimers::CancellAll() {
  put_timer.CancelAll();
  get_versions_timer.CancelAll();
  get_branch_timer.CancelAll();
  create_account_timer.CancelAll();
  create_version_tree_timer.CancelAll();
  put_version_timer.CancelAll();
}

MaidNodeService::MaidNodeService(const routing::SingleId& receiver,
                                 RpcTimers& rpc_timers)
    : kReceiver_(receiver), rpc_timers_(rpc_timers) {}

void MaidNodeService::HandleMessage(const PutResponse& message,
                                    const PutResponse::Sender& /*sender*/,
                                    const PutResponse::Receiver& receiver) {
  LOG(kVerbose) << "MaidNodeService::HandleMessage PutResponse " << message.id;
  assert(receiver == kReceiver_);
  static_cast<void>(receiver);
  try {
    rpc_timers_.put_timer.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != NoSuchElement())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MaidNodeService::HandleMessage(const PutFailure& /*message*/,
                                    const PutFailure::Sender& /*sender*/,
                                    const PutFailure::Receiver& /*receiver*/) {
  LOG(kInfo) << "Get response from PutFailure";
  // TODO(Fraser#5#): 2013-08-24 - Decide on how this is to be handled, and implement.
  assert(0);
}

void MaidNodeService::HandleMessage(const GetVersionsResponse& message,
                                    const GetVersionsResponse::Sender& /*sender*/,
                                    const GetVersionsResponse::Receiver& receiver) {
  assert(receiver == kReceiver_);
  static_cast<void>(receiver);
  try {
    rpc_timers_.get_versions_timer.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != NoSuchElement())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MaidNodeService::HandleMessage(const PutVersionResponse& message,
                                    const PutVersionResponse::Sender& /*sender*/,
                                    const PutVersionResponse::Receiver& /*receiver*/) {
  LOG(kInfo) << "Get response for PutVersion";
  try {
    rpc_timers_.put_version_timer.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != NoSuchElement())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MaidNodeService::HandleMessage(const GetBranchResponse& message,
                                    const GetBranchResponse::Sender& /*sender*/,
                                    const GetBranchResponse::Receiver& receiver) {
  assert(receiver == kReceiver_);
  static_cast<void>(receiver);
  try {
    rpc_timers_.get_branch_timer.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != NoSuchElement())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MaidNodeService::HandleMessage(const CreateAccountResponse& message,
                                    const CreateAccountResponse::Sender& /*sender*/,
                                    const CreateAccountResponse::Receiver& /*receiver*/) {
  LOG(kInfo) << "Get response for CreateAccount";
  try {
    rpc_timers_.create_account_timer.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != NoSuchElement())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MaidNodeService::HandleMessage(const CreateVersionTreeResponse& message,
                                    const CreateVersionTreeResponse::Sender& /*sender*/,
                                    const CreateVersionTreeResponse::Receiver& /*receiver*/) {
  LOG(kInfo) << "Get response for CreateVersionTree";
  try {
    rpc_timers_.create_version_tree_timer.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != NoSuchElement())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

}  // namespace nfs_client

}  // namespace maidsafe
