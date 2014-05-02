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

#include "maidsafe/common/error.h"

namespace maidsafe {

namespace nfs_client {

namespace {

std::error_code InvalidParameter() {
  static std::error_code code(make_error_code(CommonErrors::invalid_parameter));
  return code;
}

}  // unnamed namespace

MaidNodeService::MaidNodeService(
    routing::Routing& routing, routing::Timer<MaidNodeService::GetResponse::Contents>& get_timer,
    routing::Timer<MaidNodeService::PutResponse::Contents>& put_timer,
    routing::Timer<MaidNodeService::GetVersionsResponse::Contents>& get_versions_timer,
    routing::Timer<MaidNodeService::GetBranchResponse::Contents>& get_branch_timer,
    routing::Timer<MaidNodeService::CreateAccountResponse::Contents>& create_account_timer,
    routing::Timer<MaidNodeService::PmidHealthResponse::Contents>& pmid_health_timer,
    routing::Timer<MaidNodeService::CreateVersionTreeResponse::Contents>& create_version_tree_timer,
    routing::Timer<MaidNodeService::PutVersionResponse::Contents>& put_version_timer,
    routing::Timer<MaidNodeService::RegisterPmidResponse::Contents>& register_pmid_timer)
        : routing_(routing),
          get_timer_(get_timer),
          put_timer_(put_timer),
          get_versions_timer_(get_versions_timer),
          get_branch_timer_(get_branch_timer),
          create_account_timer_(create_account_timer),
          pmid_health_timer_(pmid_health_timer),
          create_version_tree_timer_(create_version_tree_timer),
          put_version_timer_(put_version_timer),
          register_pmid_timer_(register_pmid_timer) {}

void MaidNodeService::HandleMessage(const GetResponse& message,
                                    const GetResponse::Sender& /*sender*/,
                                    const GetResponse::Receiver& receiver) {
  LOG(kVerbose) << "MaidNodeService::HandleMessage GetResponse "
                << HexSubstr(message.Serialise()) << " with content "
                << HexSubstr(message.contents->Serialise());
//   get_timer_.PrintTaskIds();
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  try {
    get_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != InvalidParameter())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MaidNodeService::HandleMessage(const PutResponse& message,
                                    const PutResponse::Sender& /*sender*/,
                                    const PutResponse::Receiver& receiver) {
  LOG(kVerbose) << "MaidNodeService::HandleMessage PutResponse " << message.id;
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  try {
    put_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != InvalidParameter())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MaidNodeService::HandleMessage(const GetCachedResponse& message,
                                    const GetCachedResponse::Sender& /*sender*/,
                                    const GetCachedResponse::Receiver& receiver) {
  LOG(kVerbose) << "MaidNodeService::HandleMessage GetCachedResponse "
                << HexSubstr(message.Serialise()) << " with content "
                << HexSubstr(message.contents->Serialise());
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  try {
    get_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != InvalidParameter())
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
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  try {
    get_versions_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != InvalidParameter())
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
    put_version_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != InvalidParameter())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MaidNodeService::HandleMessage(const GetBranchResponse& message,
                                    const GetBranchResponse::Sender& /*sender*/,
                                    const GetBranchResponse::Receiver& receiver) {
  assert(receiver.data == routing_.kNodeId());
  static_cast<void>(receiver);
  try {
    get_branch_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != InvalidParameter())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MaidNodeService::HandleMessage(const PmidHealthResponse& message,
                                    const PmidHealthResponse::Sender& /*sender*/,
                                    const PmidHealthResponse::Receiver& /*receiver*/) {
  LOG(kInfo) << "Get response for PmidHealth";
  try {
    pmid_health_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != InvalidParameter())
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
    create_account_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != InvalidParameter())
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
    create_version_tree_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != InvalidParameter())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

void MaidNodeService::HandleMessage(const RegisterPmidResponse& message,
                                    const RegisterPmidResponse::Sender& /*sender*/,
                                    const RegisterPmidResponse::Receiver& /*receiver*/) {
  LOG(kInfo) << "Get response for RegisterPmid";
  try {
    register_pmid_timer_.AddResponse(message.id.data, *message.contents);
  }
  catch (const maidsafe_error& error) {
    if (error.code() != InvalidParameter())
      throw;
    else
      LOG(kWarning) << "Timer does not expect:" << message.id.data;
  }
}

}  // namespace nfs_client

}  // namespace maidsafe
