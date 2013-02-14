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

#ifndef  MAIDSAFE_NFS_ACCUMULATOR_INL_H_
#define  MAIDSAFE_NFS_ACCUMULATOR_INL_H_

#include <string>
#include <vector>

#include "maidsafe/nfs/reply.h"

namespace maidsafe {

namespace nfs {

template <typename Name>
Accumulator<Name>::Request::Request(const DataMessage& msg_in,
                                    const routing::ReplyFunctor& reply_functor_in,
                                    const Reply& reply_in)
    : msg(msg_in),
      reply_functor(reply_functor_in),
      reply(reply_in) {}

template <typename Name>
Accumulator<Name>::Request::Request(const Request& other)
    : msg(other.msg),
      reply_functor(other.reply_functor),
      reply(other.reply) {}

template <typename Name>
typename Accumulator<Name>::Request& Accumulator<Name>::Request::operator=(const Request& other) {
  msg = other.msg;
  reply_functor = other.reply_functor;
  reply = other.reply;
  return *this;
}

template <typename Name>
Accumulator<Name>::Request::Request(Request&& other)
    : msg(std::move(other.msg)),
      reply_functor(std::move(other.reply_functor)),
      reply(std::move(other.reply)) {}

template <typename Name>
typename Accumulator<Name>::Request& Accumulator<Name>::Request::operator=(Request&& other) {
  msg = std::move(other.msg);
  reply_functor = std::move(other.reply_functor);
  reply = std::move(other.reply);
  return *this;
}

template <typename Name>
Accumulator<Name>::HandledRequest::HandledRequest(
    const MessageId& msg_id_in,
    const Name& source_name_in,
    const DataMessage::Action& action_type_in,
    const Identity& data_name_in,
    const DataTagValue& data_type_in,
    const uint64_t& size_in,
    const uint32_t& replication_in,
    const Reply& reply_in)
    : msg_id(msg_id_in),
      source_name(source_name_in),
      action(action_type_in),
      data_name(data_name_in),
      data_type(data_type_in),
      size(size_in),
      replication(replication_in),
      reply(reply_in) {}

template <typename Name>
Accumulator<Name>::HandledRequest::HandledRequest(const HandledRequest& other)
    : msg_id(other.msg_id),
      source_name(other.source_name),
      action(other.action),
      data_name(other.data_name),
      data_type(other.data_type),
      size(other.size),
      replication(other.replication),
      reply(other.reply) {}

template <typename Name>
typename Accumulator<Name>::HandledRequest& Accumulator<Name>::HandledRequest::operator=(
    const HandledRequest& other) {
  msg_id = other.msg_id;
  source_name = other.source_name;
  action = other.action;
  data_name = other.data_name,
  data_type = other.data_type,
  size = other.size;
  replication = other.replication;
  reply = other.reply;
  return *this;
}

template <typename Name>
Accumulator<Name>::HandledRequest::HandledRequest(HandledRequest&& other)
    : msg_id(std::move(other.msg_id)),
      source_name(std::move(other.source_name)),
      action(other.action),
      data_name(std::move(other.data_name)),
      data_type(other.data_type),
      size(std::move(other.size)),
      replication(other.replication),
      reply(std::move(other.reply)) {}

template <typename Name>
typename Accumulator<Name>::HandledRequest& Accumulator<Name>::HandledRequest::operator=(
    HandledRequest&& other) {
  msg_id = std::move(other.msg_id);
  source_name = std::move(other.source_name);
  action = other.action;
  data_name = std::move(other.data_name),
  data_type = data_type;
  size = std::move(other.size);
  replication = other.replication;
  reply = std::move(other.reply);
  return *this;
}

template <typename Name>
Accumulator<Name>::Accumulator()
    : pending_requests_(),
      handled_requests_(),
      kMaxPendingRequestsCount_(300),
      kMaxHandledRequestsCount_(1000),
      mutex_() {}

template <typename Name>
bool Accumulator<Name>::CheckHandled(const RequestIdentity& request_identity, Reply& reply) const {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = std::find_if(handled_requests_.begin(),
                         handled_requests_.end(),
                         [&request_identity](const HandledRequest& sync_data) {
                         return (sync_data.msg_id == request_identity.first) &&
                                (sync_data.source_name == request_identity.second);
                         });
  if (it != handled_requests_.end()) {
    reply = it->reply;
    return true;
  }
  return false;
}

template <typename Name>
std::vector<Reply> Accumulator<Name>::PushRequest(const Request& request) {
  auto request_identity(std::make_pair(MessageId(Identity(request.msg.message_id())),
                                       Name(Identity(request.msg.source().node_id.string()))));
  auto pending_request(std::make_pair(request_identity, request));
  std::vector<Reply> replies;
  std::lock_guard<std::mutex> lock(mutex_);
  pending_requests_.push_back(pending_request);
  for (auto& request : pending_requests_) {
    if (request.first.first == request_identity.first)
      replies.push_back(request.second.reply);
  }
  if (pending_requests_.size() > kMaxPendingRequestsCount_)
    pending_requests_.pop_front();
  return replies;
}

template <typename Name>
std::vector<typename Accumulator<Name>::Request> Accumulator<Name>::SetHandled(
    const RequestIdentity& request_identity,
    const Reply& reply) {
  std::vector<Request> ret_requests;
  std::lock_guard<std::mutex> lock(mutex_);
  auto itr = pending_requests_.begin();
  while (itr != pending_requests_.end()) {
    if (itr->first == request_identity) {
      ret_requests.push_back(itr->second);
      itr = pending_requests_.erase(itr);
    } else {
      ++itr;
    }
  }
  if (!ret_requests.empty()) {
    handled_requests_.push_back(
        Accumulator::HandledRequest(request_identity.first,
                                    request_identity.second,
                                    ret_requests.at(0).msg.data().action,
                                    ret_requests.at(0).msg.data().name,
                                    ret_requests.at(0).msg.data().type,
                                    ret_requests.at(0).msg.data().content.string().size(),
                                    1,
                                    reply));
  }
  if (handled_requests_.size() > kMaxHandledRequestsCount_)
    handled_requests_.pop_front();
  return ret_requests;
}

}  // namespace nfs

}  // namespace maidsafe


#endif  // MAIDSAFE_NFS_ACCUMULATOR_INL_H_
