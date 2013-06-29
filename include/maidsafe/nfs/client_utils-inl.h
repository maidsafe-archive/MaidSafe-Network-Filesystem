/* Copyright 2012 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

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

template<typename Data>
void GetOp<Data>::SetPromiseValue(Data&& data) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (promise_set_)
    return;
  promise_set_ = true;
  promise_.set_value(std::move(std::unique_ptr<Data>(new Data(std::move(data)))));
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
         const passport::PublicPmid::name_type& pmid_node_hint,
         int successes_required,
         std::function<void(Reply)> result) {
  auto put_op(std::make_shared<OperationOp>(successes_required, result));
  client_maid_nfs.Put(data,
                      pmid_node_hint,
                      [put_op](std::string serialised_reply) {
                           HandleOperationReply(put_op, serialised_reply);
                      });
}

template<typename Data>
std::future<std::unique_ptr<Data>> Get(ClientMaidNfs& client_maid_nfs,
                                       const typename Data::name_type& name) {
  auto get_op(std::make_shared<detail::GetOp<Data>>());
  client_maid_nfs.Get<Data>(name, [get_op, name] (std::string serialised_reply) {
    try {
      Reply reply((Reply::serialised_type(NonEmptyString(serialised_reply))));
      if (!reply.IsSuccess())
        throw reply.error();
      Data data(name, (typename Data::serialised_type(reply.data())));
      get_op->SetPromiseValue(std::move(data));
    }
    catch(const maidsafe_error& error) {
      LOG(kWarning) << "nfs Get error: " << error.code() << " - " << error.what();
      get_op->HandleFailure(error);
    }
    catch(const std::exception& e) {
      LOG(kWarning) << "nfs Get error: " << e.what();
      get_op->HandleFailure(MakeError(CommonErrors::unknown));
    }
  });
  return get_op->GetFutureFromPromise();
}

template<typename Data>
void Delete(ClientMaidNfs& client_maid_nfs,
            const typename Data::name_type& name,
            int successes_required,
            std::function<void(Reply)> result) {
  auto delete_op(std::make_shared<OperationOp>(successes_required, result));
  client_maid_nfs.Delete<Data>(name,
                               [delete_op] (std::string serialised_reply) {
                                 HandleOperationReply(delete_op, serialised_reply);
                               });
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_UTILS_INL_H_
