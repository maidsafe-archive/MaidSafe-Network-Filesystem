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
Accumulator<Name>::PendingRequest::PendingRequest(const DataMessage& msg_in,
                                                  const routing::ReplyFunctor& reply_functor_in,
                                                  const Reply& reply_in)
    : msg(msg_in),
      reply_functor(reply_functor_in),
      reply(reply_in) {}

template <typename Name>
Accumulator<Name>::PendingRequest::PendingRequest(const PendingRequest& other)
    : msg(other.msg),
      reply_functor(other.reply_functor),
      reply(other.reply) {}

template <typename Name>
typename Accumulator<Name>::PendingRequest& Accumulator<Name>::PendingRequest::operator=(
    const PendingRequest& other) {
  msg = other.msg;
  reply_functor = other.reply_functor;
  reply = other.reply;
  return *this;
}

template <typename Name>
Accumulator<Name>::PendingRequest::PendingRequest(PendingRequest&& other)
    : msg(std::move(other.msg)),
      reply_functor(std::move(other.reply_functor)),
      reply(std::move(other.reply)) {}

template <typename Name>
typename Accumulator<Name>::PendingRequest& Accumulator<Name>::PendingRequest::operator=(
    PendingRequest&& other) {
  msg = std::move(other.msg);
  reply_functor = std::move(other.reply_functor);
  reply = std::move(other.reply);
  return *this;
}

template <typename Name>
Accumulator<Name>::HandledRequest::HandledRequest(const MessageId& msg_id_in,
                                                  const Name& source_name_in,
                                                  const DataMessage::Action& action_type_in,
                                                  const Identity& data_name_in,
                                                  const DataTagValue& data_type_in,
                                                  const int32_t& size_in,
                                                  const Reply& reply_in)
    : msg_id(msg_id_in),
      source_name(source_name_in),
      action(action_type_in),
      data_name(data_name_in),
      data_type(data_type_in),
      size(size_in),
      reply(reply_in) {}

template <typename Name>
Accumulator<Name>::HandledRequest::HandledRequest(const HandledRequest& other)
    : msg_id(other.msg_id),
      source_name(other.source_name),
      action(other.action),
      data_name(other.data_name),
      data_type(other.data_type),
      size(other.size),
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
  reply = other.reply;
  return *this;
}

template <typename Name>
Accumulator<Name>::HandledRequest::HandledRequest(HandledRequest&& other)
    : msg_id(std::move(other.msg_id)),
      source_name(std::move(other.source_name)),
      action(std::move(other.action)),
      data_name(std::move(other.data_name)),
      data_type(std::move(other.data_type)),
      size(std::move(other.size)),
      reply(std::move(other.reply)) {}

template <typename Name>
typename Accumulator<Name>::HandledRequest& Accumulator<Name>::HandledRequest::operator=(
    HandledRequest&& other) {
  msg_id = std::move(other.msg_id);
  source_name = std::move(other.source_name);
  action = std::move(other.action);
  data_name = std::move(other.data_name),
  data_type = std::move(data_type);
  size = std::move(other.size);
  reply = std::move(other.reply);
  return *this;
}

template <typename Name>
Accumulator<Name>::Accumulator()
    : pending_requests_(),
      handled_requests_(),
      kMaxPendingRequestsCount_(300),
      kMaxHandledRequestsCount_(1000) {}

template <typename Name>
bool Accumulator<Name>::CheckHandled(const DataMessage& data_message, Reply& reply_out) const {
  auto it = std::find_if(handled_requests_.begin(),
                         handled_requests_.end(),
                         [&data_message](const HandledRequest& handled_request) {
                         return (handled_request.msg_id == data_message.message_id()) &&
                                (handled_request.source_name ==
                                    Name(Identity(data_message.source().node_id.string())));
                         });
  if (it != handled_requests_.end()) {
    reply_out = it->reply;
    return true;
  }
  return false;
}

template <typename Name>
std::vector<Reply> Accumulator<Name>::PushRequest(const PendingRequest& pending_request) {
  std::vector<Reply> replies;
  pending_requests_.push_back(pending_request);
  for (auto& request : pending_requests_) {
    if (request.msg.message_id() == pending_request.message_id() &&
        request.msg.source().node_id == pending_request.source().node_id)
      replies.push_back(request.reply);
  }
  if (pending_requests_.size() > kMaxPendingRequestsCount_)
    pending_requests_.pop_front();
  return replies;
}

template <typename Name>
std::vector<typename Accumulator<Name>::PendingRequest> Accumulator<Name>::SetHandled(
    const DataMessage& data_message,
    const Reply& reply) {
  std::vector<PendingRequest> ret_requests;
  auto itr = pending_requests_.begin();
  while (itr != pending_requests_.end()) {
    if ((*itr).msg.message_id() == data_message.message_id() &&
        (*itr).msg.source().node_id == data_message.source().node_id) {
      ret_requests.push_back(*itr);
      itr = pending_requests_.erase(itr);
    } else {
      ++itr;
    }
  }
  assert(!ret_requests.empty());

  handled_requests_.push_back(
      Accumulator::HandledRequest(data_message.message_id(),
                                  Name(Identity(data_message.source().node_id.string())),
                                  data_message.data().action,
                                  data_message.data().name,
                                  data_message.data().type,
                                  data_message.data().content.string().size(),
                                  reply));
  if (handled_requests_.size() > kMaxHandledRequestsCount_)
    handled_requests_.pop_front();
  return ret_requests;
}

}  // namespace nfs

}  // namespace maidsafe


#endif  // MAIDSAFE_NFS_ACCUMULATOR_INL_H_
