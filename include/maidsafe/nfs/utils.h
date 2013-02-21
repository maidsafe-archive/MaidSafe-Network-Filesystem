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

#ifndef MAIDSAFE_NFS_UTILS_H_
#define MAIDSAFE_NFS_UTILS_H_

#include <functional>
#include <iterator>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "maidsafe/common/node_id.h"

#include "maidsafe/nfs/message.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

class Reply;

namespace detail {

MessageId GetNewMessageId(const NodeId& source_node_id);

}  // namespace detail

template<typename Data>
bool IsCacheable();

// If 'replies' contains >= n successsful replies where n is 'successes_required', returns
// <iterator to nth successful reply, true> otherwise
// <iterator to most frequent failed reply, false>.  If there is more than one most frequent type,
// (e.g. 2 'no_such_element' and 2 'invalid_parameter') the iterator points to the first which
// reaches the frequency.
std::pair<std::vector<Reply>::const_iterator, bool> GetSuccessOrMostFrequentReply(
    const std::vector<Reply>& replies,
    int successes_required);

class PutOrDeleteOp {
 public:
  PutOrDeleteOp(int successes_required, std::function<void(Reply)> callback);
  void HandleReply(Reply&& reply);

 private:
  PutOrDeleteOp(const PutOrDeleteOp&);
  PutOrDeleteOp& operator=(const PutOrDeleteOp&);
  PutOrDeleteOp(PutOrDeleteOp&&);
  PutOrDeleteOp& operator=(PutOrDeleteOp&&);

  mutable std::mutex mutex_;
  int successes_required_;
  std::function<void(Reply)> callback_;
  std::vector<Reply> replies_;
  bool callback_executed_;
};

void HandlePutOrDeleteReply(std::shared_ptr<PutOrDeleteOp> op, const std::string& serialised_reply);

}  // namespace nfs

}  // namespace maidsafe

#include "maidsafe/nfs/utils-inl.h"

#endif  // MAIDSAFE_NFS_UTILS_H_
