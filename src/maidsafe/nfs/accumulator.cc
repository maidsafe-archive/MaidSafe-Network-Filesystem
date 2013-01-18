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


namespace maidsafe {

namespace nfs {

Accumulator::Accumulator()
    : pending_requests_(),
      handled_requests_(),
      kMaxPendingRequestsCount_(300),
      kMaxHandledRequestsCount_(1000),
      mutex_() {}

bool Accumulator::CheckHandled(const RequestIdentity& request_identity,
                               ReturnCode& ret_code) const {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = std::find_if(handled_requests_.begin(),
                         handled_requests_.end(),
                         [&request_identity](const HandledRequests::value_type& request) {
                            return request.first == request_identity;
                         });
  if (it != handled_requests_.end()) {
    ret_code = it->second;
    return true;
  }
  return false;
}

std::vector<ReturnCode> Accumulator::PushRequest(const Request& request) {
  RequestIdentity request_identity = std::make_pair(request.msg.message_id(),
                                                    request.msg.source().persona);
  auto pending_request = std::make_pair(request_identity, request);
  std::vector<ReturnCode> ret_codes;
  std::lock_guard<std::mutex> lock(mutex_);
  pending_requests_.push_back(pending_request);
  for (auto& request : pending_requests_) {
    if (request.first == request_identity)
      ret_codes.push_back(request.second.ret_code);
  }
  if (pending_requests_.size() > kMaxPendingRequestsCount_)
    pending_requests_.pop_front();
  return ret_codes;
}

std::vector<Accumulator::Request> Accumulator::SetHandled(const RequestIdentity& request_identity,
                                                          const ReturnCode& ret_code) {
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

  handled_requests_.push_back(std::make_pair(request_identity, ret_code));
  if (handled_requests_.size() > kMaxHandledRequestsCount_)
    handled_requests_.pop_front();
  return ret_requests;
}

}  // namespace nfs

}  // namespace maidsafe
