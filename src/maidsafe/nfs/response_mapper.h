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

#ifndef MAIDSAFE_NFS_RESPONSE_MAPPER_H_
#define MAIDSAFE_NFS_RESPONSE_MAPPER_H_

#include <exception>
#include <future>
#include <string>
#include <utility>
#include <vector>

#include "maidsafe/common/utils.h"

namespace maidsafe {

namespace nfs {

template <typename FutureType, typename PromiseType, typename Converter>
class ResponseMapper {
 public:
  explicit ResponseMapper(Converter converter);

  typedef std::pair<std::future<FutureType>, std::promise<PromiseType>>  RequestPair;

  void push_back(RequestPair&& pending_pair);

 private:
  void Run();
  void Poll();

  mutable std::mutex mutex_;
  Converter converter_;
  std::vector<RequestPair> active_requests_, pending_requests_;
  std::future<void> worker_future_;
  std::atomic<bool> running_;
};

template <typename FutureType, typename PromiseType, typename Converter>
ResponseMapper<FutureType, PromiseType, Converter>::ResponseMapper(Converter converter)
    : mutex_(),
      converter_(converter),
      active_requests_(),
      pending_requests_(),
      worker_future_(),
      running_(false) {
  Run();
}

template <typename FutureType, typename PromiseType, typename Converter>
void ResponseMapper<FutureType, PromiseType, Converter>::push_back(RequestPair&& pending_pair) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    pending_requests_.push_back(std::move(pending_pair));
    Run();
  }
}

template <typename FutureType, typename PromiseType, typename Converter>
void ResponseMapper<FutureType, PromiseType, Converter>::Run() {
  if (!running_) {
    worker_future_ = std::async(std::launch::async, [this]() { this->Poll(); });  // NOLINT Prakash
    running_ = true;
  }
}

template <typename FutureType, typename PromiseType, typename Converter>
void ResponseMapper<FutureType, PromiseType, Converter>::Poll() {
  while (running_) {
    active_requests_.erase(std::remove_if(active_requests_.begin(), active_requests_.end(),
        [this](RequestPair& request_pair)->bool {
          if (IsReady(request_pair.first)) {
            try {
              request_pair.second.set_value(std::move(converter_(
                                                          std::move(request_pair.first.get()))));
            } catch(std::exception& /*ex*/) {
              request_pair.second.set_exception(std::current_exception());
            }
            return true;
          } else  {
            return false;
          }
        }), active_requests_.end());
      std::this_thread::yield();
    {  // moving new  requests
      std::lock_guard<std::mutex> lock(mutex_);
      std::move(pending_requests_.begin(), pending_requests_.end(),
                std::back_inserter(active_requests_));
      pending_requests_.resize(0);
      if (active_requests_.empty())
        running_ = false;
    }
  }
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_RESPONSE_MAPPER_H_
