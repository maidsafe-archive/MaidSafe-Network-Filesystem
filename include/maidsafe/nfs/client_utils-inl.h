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

#ifndef MAIDSAFE_NFS_CLIENT_UTILS_INL_H_
#define MAIDSAFE_NFS_CLIENT_UTILS_INL_H_

#include <algorithm>
#include <exception>
#include <future>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"

#include "maidsafe/routing/parameters.h"

#include "maidsafe/nfs/reply.h"
#include "maidsafe/nfs/nfs.h"
#include "maidsafe/nfs/utils.h"


namespace maidsafe {

namespace nfs {

namespace detail {

std::error_code GetMostFrequentError(const std::vector<maidsafe_error>& errors) {
  typedef std::map<std::error_code, int> Count;
  Count count;
  for (auto& error : errors)
    ++count[error.code()];
  auto itr(std::max_element(
      count.begin(),
      count.end(),
      [](const Count::value_type& lhs, const Count::value_type& rhs)->bool {
          return lhs.second < rhs.second;
      }));
  return (*itr).first;
}

template<typename Data>
void GetOp<Data>::SetPromiseValue(Data&& data) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (promise_set_)
    return;
  promise_set_ = true;
  promise_.set_value(data);
}

template<typename Data>
void GetOp<Data>::HandleFailure(const maidsafe_error& error) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (promise_set_)
    return;
  errors_.push_back(error);
  if (errors_.size() == routing::Parameters::node_group_size)  // Operation has failed
    promise_.set_exception(std::make_exception_ptr(GetMostFrequentError(errors_)));
}

}  // namespace detail


template<typename Data>
void Put(ClientMaidNfs& client_maid_nfs,
         const Data& data,
         const passport::PublicPmid::name_type& data_holder_hint,
         int successes_required,
         std::function<void(Reply)> result) {
  auto put_op(std::make_shared<PutOrDeleteOp>(successes_required, result));
  client_maid_nfs.Put(data,
                      data_holder_hint,
                      [put_op](std::string serialised_reply) {
                           HandlePutOrDeleteReply(put_op, serialised_reply);
                      });
}

template<typename Data>
std::future<Data> Get(ClientMaidNfs& client_maid_nfs, const typename Data::name_type& name) {
  auto get_op(std::make_shared<detail::GetOp<Data>>());
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

template<typename Data>
void Delete(ClientMaidNfs& client_maid_nfs,
            const typename Data::name_type& name,
            int successes_required,
            std::function<void(Reply)> result) {
  auto delete_op(std::make_shared<PutOrDeleteOp>(successes_required, result));
  client_maid_nfs.Delete(name,
                         [delete_op](std::string serialised_reply) {
                             HandlePutOrDeleteReply(delete_op, serialised_reply);
                         });
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_UTILS_INL_H_
