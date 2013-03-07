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

#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "maidsafe/common/error.h"

#include "maidsafe/nfs/reply.h"
#include "maidsafe/nfs/nfs.h"


namespace maidsafe {

namespace nfs {

namespace detail {

template<typename Data>
class GetOp {
 public:
  GetOp() : mutex_(), promise_(), promise_set_(false), errors_() {}
  std::future<Data> GetFutureFromPromise() { return promise_.get_future(); }
  void SetPromiseValue(Data&& data);
  void HandleFailure(const maidsafe_error& error);

 private:
  GetOp(const GetOp&);
  GetOp& operator=(const GetOp&);
  GetOp(GetOp&&);
  GetOp& operator=(GetOp&&);

  mutable std::mutex mutex_;
  std::promise<Data> promise_;
  bool promise_set_;
  std::vector<maidsafe_error> errors_;
};

}  // namespace detail

template<typename Data>
void Put(ClientMaidNfs& client_maid_nfs,
         const Data& data,
         const passport::PublicPmid::name_type& data_holder_hint,
         int successes_required,
         std::function<void(Reply)> result);

template<typename Data>
std::future<Data> Get(ClientMaidNfs& client_maid_nfs, const typename Data::name_type& name);

template<typename Data>
void Delete(ClientMaidNfs& client_maid_nfs,
            const typename Data::name_type& name,
            int successes_required,
            std::function<void(Reply)> result);

}  // namespace nfs

}  // namespace maidsafe

#include "maidsafe/nfs/client_utils-inl.h"

#endif  // MAIDSAFE_NFS_CLIENT_UTILS_H_
