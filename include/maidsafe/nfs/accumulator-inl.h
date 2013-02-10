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
#include "maidsafe/nfs/handled_request_pb.h"

namespace maidsafe {

namespace nfs {

template <typename Name>
Accumulator<Name>::Accumulator()
    : pending_requests_(),
      handled_requests_(),
      kMaxPendingRequestsCount_(300),
      kMaxHandledRequestsCount_(1000),
      mutex_() {}

template <typename Name>
bool Accumulator<Name>::CheckHandled(const Accumulator::RequestIdentity& request_identity,
                                     Reply& reply) const {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = std::find_if(handled_requests_.begin(),
                         handled_requests_.end(),
                         [&request_identity](const typename HandledRequests::value_type& request) {
                            return request.first == request_identity;
                         });
  if (it != handled_requests_.end()) {
    reply = it->second;
    return true;
  }
  return false;
}

template <typename Name>
std::vector<Reply> Accumulator<Name>::PushRequest(const Request& request) {
  auto request_identity = std::make_pair(Identity(request.msg.message_id()),
                                         Identity(request.msg.source().node_id.string()));
  auto pending_request = std::make_pair(request_identity, request);
  std::vector<Reply> replies;
  std::lock_guard<std::mutex> lock(mutex_);
  pending_requests_.push_back(pending_request);
  for (auto& request : pending_requests_) {
    if (Identity(request.first.first) == Identity(request_identity.first))
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

  handled_requests_.push_back(std::make_pair(request_identity, reply));
  if (handled_requests_.size() > kMaxHandledRequestsCount_)
    handled_requests_.pop_front();
  return ret_requests;
}

template <typename Name>
typename Accumulator<Name>::serialised_type
    Accumulator<Name>::SerialiseHandledRequests(const typename Name::name_type& name) const {
  protobuf::HandledRequests handled_requests;
  protobuf::HandledRequest* handled_request;
  handled_requests.set_name((name->string()));
  std::lock_guard<std::mutex> lock(mutex_);
  for (auto& request : handled_requests_) {
    if (request.first.second == name) {
      handled_request = handled_requests.add_request();
      handled_request->set_message_id(request.first.first->string());
      handled_request->set_reply(request.second.Serialise()->string());
    }
  }
  return serialised_type(NonEmptyString(handled_requests.SerializeAsString()));
}

template <typename Name>
std::vector<typename Accumulator<Name>::HandledRequest> Accumulator<Name>::ParseHandledRequests(
    const typename Accumulator<Name>::serialised_type& serialised_message) {
  std::vector<typename Accumulator<Name>::HandledRequest> ret_handled_requests;
  protobuf::HandledRequests proto_handled_requests;
  if (!proto_handled_requests.ParseFromString(serialised_message->string()))
    ThrowError(CommonErrors::parsing_error);
  try {
    for (auto index(0); index < proto_handled_requests.request_size(); ++index) {
      ret_handled_requests.push_back(
          std::make_pair(std::make_pair(
                             Identity(proto_handled_requests.request(index).message_id()),
                             Identity(proto_handled_requests.name())),
              Reply(Reply::serialised_type(
                  NonEmptyString(proto_handled_requests.request(index).reply())))));
    }
  }
  catch(const std::exception&) {
    ThrowError(CommonErrors::parsing_error);
  }
  return ret_handled_requests;
}

}  // namespace nfs

}  // namespace maidsafe


#endif  // MAIDSAFE_NFS_ACCUMULATOR_INL_H_
