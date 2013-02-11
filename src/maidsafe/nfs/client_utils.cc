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

#include "maidsafe/nfs/client_utils.h"


namespace maidsafe {

namespace nfs {

namespace detail {

PutDeleteOp::PutDeleteOp(int successes_required, std::function<void(Reply)> callback)
    : mutex_(),
      successes_required_(successes_required),
      callback_(callback),
      replies_(),
      callback_executed_(!callback) {
  if ((callback && successes_required <= 0) || (!callback && successes_required != 0))
    ThrowError(CommonErrors::invalid_parameter);
}

void PutDeleteOp::HandleReply(Reply&& reply) {
  std::function<void(Reply)> callback;
  std::unique_ptr<Reply> result;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (callback_executed_)
      return;
    replies_.push_back(reply);
    std::vector<Reply>::iterator success_itr, most_frequent_itr;
    int successes(0), most_frequent(0);
    typedef std::map<std::error_code, int> Count;
    Count count;
    for (auto itr(replies_.begin()); itr != replies_.end(); ++itr) {
      int this_reply_count(++count[(*itr).error().code()]);
      if ((*itr).IsSuccess()) {
        ++successes;
        success_itr = itr;
      } else if (this_reply_count > most_frequent) {
        most_frequent = this_reply_count;
        most_frequent_itr = itr;
      }
    }

    if (successes == successes_required_) {
      callback = callback_;
      callback_executed_ = true;
      result = std::unique_ptr<Reply>(new Reply(std::move(*success_itr)));
    } else if (replies_.size() == 4) {  // Operation has failed
      callback = callback_;
      callback_executed_ = true;
      result = std::unique_ptr<Reply>(new Reply(std::move(*most_frequent_itr)));
    } else {
      return;
    }
  }
  callback(*result);
}

void HandlePutOrDeleteReply(std::shared_ptr<detail::PutDeleteOp> op,
                            const std::string& serialised_reply) {
  try {
    Reply reply((Reply::serialised_type(NonEmptyString(serialised_reply))));
    op->HandleReply(std::move(reply));
  }
  catch(const maidsafe_error& error) {
    LOG(kWarning) << "nfs error: " << error.code() << " - " << error.what();
    op->HandleReply(Reply(error));
  }
  catch(const std::exception& e) {
    LOG(kWarning) << "nfs error: " << e.what();
    op->HandleReply(Reply(CommonErrors::unknown));
  }
}

}  // namespace detail

}  // namespace nfs

}  // namespace maidsafe
