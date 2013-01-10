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

#ifndef MAIDSAFE_NFS_REQUEST_QUEUE_H_
#define MAIDSAFE_NFS_REQUEST_QUEUE_H_

#include <deque>
#include <utility>

#include "maidsafe/nfs/utils.h"

namespace maidsafe {

namespace nfs {

class RequestQueue {
 public:
  typedef std::deque<std::pair<int, Identity>> Requests;

  RequestQueue();

  bool Push(int id, const Identity& name);

 private:
  Requests requests_;
  mutable std::mutex mutex_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_REQUEST_QUEUE_H_
