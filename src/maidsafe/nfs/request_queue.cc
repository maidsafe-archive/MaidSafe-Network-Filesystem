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

#include "maidsafe/nfs/request_queue.h"

namespace maidsafe {

namespace nfs {

RequestQueue::RequestQueue()
  : requests_() {}

bool RequestQueue::Push(int id, const Identity& name) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = std::find_if(requests_.begin(),
                         requests_.end(),
                         [&id, &name](const Requests::value_type& request) {
                            return request.first == id && request.second == name;
                         });
  if (it == requests_.end()) {
    requests_.push_back(std::make_pair(id, name));
    if (requests_.size() > 1000)
      requests_.pop_front();
    return true;
  }
  return false;
}

}  // namespace nfs

}  // namespace maidsafe
