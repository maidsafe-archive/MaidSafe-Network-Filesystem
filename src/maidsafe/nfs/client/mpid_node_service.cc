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

#include "maidsafe/nfs/client/mpid_node_service.h"
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

MpidNodeService::RpcTimers::RpcTimers(BoostAsioService& asio_service_)
    : message_alert_timer(asio_service_),
      get_message_timer(asio_service_),
      send_message_timer(asio_service_),
      create_account_timer(asio_service_),
      get_timer(asio_service_) {}

void MpidNodeService::RpcTimers::CancellAll() {
  message_alert_timer.CancelAll();
  get_message_timer.CancelAll();
  send_message_timer.CancelAll();
  create_account_timer.CancelAll();
  get_timer.CancelAll();
}

MpidNodeService::MpidNodeService(const routing::SingleId& receiver,
                                 RpcTimers& rpc_timers,
                                 GetHandler<MpidNodeDispatcher>& get_handler)
    : kReceiver_(receiver), rpc_timers_(rpc_timers), get_handler_(get_handler) {}

void MpidNodeService::HandleMessage(const MessageAlert& message,
      const MessageAlert::Sender& /*sender*/, const MessageAlert::Receiver& receiver) {
  assert(receiver == kReceiver_);
  static_cast<void>(receiver);
  try {
    rpc_timers_.message_alert_timer.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != NoSuchElement())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MpidNodeService::HandleMessage(const GetMessageResponse& message,
      const GetMessageResponse::Sender& /*sender*/, const GetMessageResponse::Receiver& receiver) {
  assert(receiver == kReceiver_);
  static_cast<void>(receiver);
  try {
    rpc_timers_.get_message_timer.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != NoSuchElement())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MpidNodeService::HandleMessage(const SendMessageResponse& message,
                                    const SendMessageResponse::Sender& /*sender*/,
                                    const SendMessageResponse::Receiver& receiver) {
  assert(receiver == kReceiver_);
  static_cast<void>(receiver);
  try {
    rpc_timers_.send_message_timer.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != NoSuchElement())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MpidNodeService::HandleMessage(const CreateAccountResponse& message,
                                    const CreateAccountResponse::Sender& /*sender*/,
                                    const CreateAccountResponse::Receiver& receiver) {
  assert(receiver == kReceiver_);
  static_cast<void>(receiver);
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

void MpidNodeService::HandleMessage(const GetResponse& message,
                                    const GetResponse::Sender& /*sender*/,
                                    const GetResponse::Receiver& receiver) {
  assert(receiver == kReceiver_);
  static_cast<void>(receiver);
  try {
    get_handler_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != NoSuchElement())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MpidNodeService::HandleMessage(const GetCachedResponse& message,
                                    const GetCachedResponse::Sender& /*sender*/,
                                    const GetCachedResponse::Receiver& receiver) {
  assert(receiver == kReceiver_);
  static_cast<void>(receiver);
  try {
    get_handler_.AddResponse(message.id.data, *message.contents);
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
