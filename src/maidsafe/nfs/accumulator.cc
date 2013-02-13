/***************************************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use.  The use of this code is governed by the licence file licence.txt found in the root of    *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit         *
 *  written permission of the board of directors of MaidSafe.net.                                  *
 **************************************************************************************************/

#include "maidsafe/nfs/accumulator.h"

#include <algorithm>

#include "maidsafe/nfs/handled_request_pb.h"


namespace maidsafe {

namespace nfs {

template <>
typename Accumulator<passport::PublicMaid::name_type>::serialised_requests
Accumulator<passport::PublicMaid::name_type>::SerialiseHandledRequests(
    const passport::PublicMaid::name_type& name) const {
  protobuf::HandledRequests handled_requests;
  protobuf::SyncData* sync_data;
  handled_requests.set_name((name->string()));
  handled_requests.set_updater_name(handled_requests_[0].updater_name.string());
  std::lock_guard<std::mutex> lock(mutex_);
  for (auto& request : handled_requests_) {
    if (request.source_name == name) {
      sync_data = handled_requests.add_sync_data();
      sync_data->set_message_id(request.msg_id->string());
      sync_data->set_source_name(request.source_name->string());
      sync_data->set_action(static_cast<int32_t>(request.action));
      sync_data->set_data_name(request.data_name.string());
      sync_data->set_data_type(static_cast<int32_t>(request.data_type));
      sync_data->set_size(request.size);
      sync_data->set_replication(request.replication);
      sync_data->set_reply(NonEmptyString(request.reply.Serialise()).string());
    }
  }
  return serialised_requests(NonEmptyString(handled_requests.SerializeAsString()));
}

template <>
std::vector<typename Accumulator<passport::PublicMaid::name_type>::SyncData>
Accumulator<passport::PublicMaid::name_type>::ParseHandledRequests(
    const typename Accumulator<passport::PublicMaid::name_type>::serialised_requests&
        serialised_sync_updates) const {
  std::vector<typename Accumulator<passport::PublicMaid::name_type>::SyncData>
      ret_sync_data;
  protobuf::HandledRequests proto_handled_requests;
  if (!proto_handled_requests.ParseFromString(serialised_sync_updates->string()))
    ThrowError(CommonErrors::parsing_error);
  try {
    for (auto index(0); index < proto_handled_requests.sync_data_size(); ++index) {
      ret_sync_data.push_back(
          SyncData(
              MessageId(Identity(proto_handled_requests.sync_data(index).message_id())),
              Identity(proto_handled_requests.updater_name()),
              passport::PublicMaid::name_type(Identity(proto_handled_requests.name())),
              static_cast<DataMessage::Action>(proto_handled_requests.sync_data(index).action()),
              Identity(proto_handled_requests.sync_data(index).data_name()),
              static_cast<DataTagValue>(proto_handled_requests.sync_data(index).data_type()),
              proto_handled_requests.sync_data(index).size(),
              proto_handled_requests.sync_data(index).replication(),
              Reply(Reply::serialised_type(NonEmptyString(
                                               proto_handled_requests.sync_data(index).reply())))));
    }
  }
  catch(const std::exception&) {
    ThrowError(CommonErrors::parsing_error);
  }
  return ret_sync_data;
}

template <>
void Accumulator<passport::PublicMaid::name_type>::HandleSyncUpdates(
    const  NonEmptyString&  serialised_sync_updates)  {
  auto sync_updates(ParseHandledRequests<passport::PublicMaid::name_type>(serialised_sync_updates));
  for (auto& sync_update : sync_updates) {
    if (/* same update from same updater exist in pending_sync_updates_*/)
      continue;
    if (/* request is already handled and is in handled_requests_*/)
      continue;
    if (/* request is in pending_requests_ */)
      continue;
    if (/* no similar request exist add it to pending_sync_updates_*/) {
      // pending_sync_updates_.push_back(SyncData);
    }
    if (/* similar request from different updater exists in pending_sync_updates_ */) {
      // remove the similar entry from the pending_sync_updates_
      // make and add request to the pending requests
    }
  }
}

}  // namespace nfs

}  // namespace maidsafe
