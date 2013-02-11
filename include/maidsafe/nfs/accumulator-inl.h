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
  auto request_identity(std::make_pair(Identity(request.msg.message_id()),
                                       Identity(request.msg.source().node_id.string())));
  auto pending_request(std::make_pair(request_identity, request));
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

}  // namespace nfs

}  // namespace maidsafe


#endif  // MAIDSAFE_NFS_ACCUMULATOR_INL_H_
