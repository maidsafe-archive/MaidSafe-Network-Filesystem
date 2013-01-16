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

namespace maidsafe {

namespace nfs {

Accumulator::Accumulator()
  : pending_requests_(), handled_requests_(), mutex_() {}

bool Accumulator::CheckHandled(const RequestsIdentity& request_identity,
                               ReturnCode& ret_code) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = std::find_if(handled_requests_.begin(),
                         handled_requests_.end(),
                         [&request_identity](const HandledRequests::value_type& request) {
                            return request.first.first == request_identity.first &&
                                   request.first.second == request_identity.second;
                         });
  if (it != handled_requests_.end()) {
    ret_code = it->second;
    return true;
  }
  return false;
}

std::vector<ReturnCode> Accumulator::PushRequest(const Request& request) {
  std::lock_guard<std::mutex> lock(mutex_);
  RequestsIdentity request_identity = std::make_pair(request.msg.id(),
                                                     request.msg.source().persona_type);
  pending_requests_.push_back(std::make_pair(request_identity, request));
  std::vector<ReturnCode> ret_codes;
  for (auto& request : pending_requests_) {
    if ((request.first.first == request_identity.first) &&
        (request.first.second == request_identity.second))
      ret_codes.push_back(request.second.ret_code);
  }
  return ret_codes;
}

std::vector<Accumulator::Request> Accumulator::SetHandled(
    const RequestsIdentity& request_identity, const ReturnCode& ret_code) {
  std::lock_guard<std::mutex> lock(mutex_);
  std::vector<Request> ret_requests;
  auto itr = pending_requests_.begin();
  do {
    if ((itr->first.first == request_identity.first) &&
        (itr->first.second == request_identity.second)) {
      ret_requests.push_back(itr->second);
      itr = pending_requests_.erase(itr);
    } else {
      ++itr;
    }
  } while (itr != pending_requests_.end());

  handled_requests_.push_back(std::make_pair(request_identity, ret_code));
  if (handled_requests_.size() > 1000)
    handled_requests_.pop_front();
  return ret_requests;
}

}  // namespace nfs

}  // namespace maidsafe
