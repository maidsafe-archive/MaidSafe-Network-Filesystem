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

#include "maidsafe/nfs/client_put_policies.h"

namespace maidsafe {

namespace nfs {

void ProcessReadyFuture(StringFuture& future,
                        ReturnCodePromiseVector& promises,
                        size_t& index) {
  try {
    std::string serialised_message(future.get());
    // Need '((' when constructing ReturnCode to avoid most vexing parse.
    ReturnCode return_code((ReturnCode::serialised_type(NonEmptyString(serialised_message))));
    promises.at(index++).set_value(std::move(return_code));
  }
  catch(const std::system_error& error) {
    LOG(kWarning) << "Put future problem: " << error.code() << " - " << error.what();
    promises.at(index++).set_exception(std::current_exception());
  }
}

void HandlePutFutures(std::shared_ptr<ReturnCodePromiseVector> promises,
                      std::shared_ptr<StringFutureVector> routing_futures) {
  std::async(
      std::launch::async,
      [promises, routing_futures] {
        size_t next_promise_index(0);
        while (!routing_futures->empty()) {
          std::vector<StringFuture>::iterator itr(routing_futures->begin());
          while ((itr = FindNextReadyFuture(itr, *routing_futures)) != routing_futures->end()) {
            ProcessReadyFuture(*itr, *promises, next_promise_index);
            ++itr;
          }

          std::this_thread::yield();
        }
      });
}

}  // namespace nfs

}  // namespace maidsafe
