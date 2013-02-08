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

#ifndef MAIDSAFE_NFS_CLIENT_UTILS_H_
#define MAIDSAFE_NFS_CLIENT_UTILS_H_

#include <algorithm>
#include <exception>
#include <future>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"

#include "maidsafe/nfs/reply.h"
#include "maidsafe/nfs/nfs.h"


namespace maidsafe {

namespace nfs {

namespace detail {

template<typename Data>
class GetOp {
 public:
  GetOp() : promise_(), errors_(), mutex_(), promise_set_(false) {}

  std::future<Data> GetFutureFromPromise() { return promise_.get_future(); }

  void SetPromiseValue(Data&& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (promise_set_)
      return;
    promise_set_ = true;
    promise_.set_value(data);
  }

  void HandleFailure(const maidsafe_error& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (promise_set_)
      return;
    errors_.push_back(error);
    if (errors_.size() == 4) {
      // Operation has failed, get most frequent error_code.
      promise_set_ = true;
      typedef std::map<std::error_code, int> Count;
      Count count;
      for (auto& error : errors_)
        ++count[error.code()];
      auto itr(std::max_element(
          count.begin(),
          count.end(),
          [](const Count::value_type& lhs, const Count::value_type& rhs)->bool {
              return lhs.second < rhs.second;
          }));
      promise_.set_exception(std::make_exception_ptr((*itr).first));
    }
  }

 private:
  GetOp(const GetOp&);
  GetOp& operator=(const GetOp&);
  GetOp(GetOp&&);
  GetOp& operator=(GetOp&&);

  std::promise<Data> promise_;
  std::vector<maidsafe_error> errors_;
  mutable std::mutex mutex_;
  bool promise_set_;
};

}  // namespace detail

template<typename Data>
std::future<Data> Get(ClientMaidNfs& client_maid_nfs, const typename Data::name_type& name) {
  std::shared_ptr<detail::GetOp<Data>> get_op;
  client_maid_nfs.Get(name, [get_op](std::string serialised_reply)->void {
    try {
      Reply reply((Reply::serialised_type(NonEmptyString(serialised_reply))));
      if (!reply.IsSuccess())
        throw reply.error();
      Data data((Data::serialised_type(reply.data())));
      return get_op->SetPromiseValue(std::move(data));
    }
    catch(const maidsafe_error& error) {
      LOG(kWarning) << "nfs Get error: " << error.code() << " - " << error.what();
      get_op->HandleFailure(error);
    }
    catch(const std::exception& e) {
      LOG(kWarning) << "nfs Get error: " << e.what();
      get_op->HandleFailure(CommonErrors::unknown);
    }
  });
  return get_op->GetFutureFromPromise();
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_UTILS_H_
