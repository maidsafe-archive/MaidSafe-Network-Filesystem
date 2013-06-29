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

std::error_code GetMostFrequentError(const std::vector<maidsafe_error>& errors);

template<typename Data>
class GetOp {
 public:
  GetOp() : mutex_(), promise_(), promise_set_(false), errors_() {}
  std::future<std::unique_ptr<Data>> GetFutureFromPromise() { return promise_.get_future(); }
  void SetPromiseValue(Data&& data);
  void HandleFailure(const maidsafe_error& error);

 private:
  GetOp(const GetOp&);
  GetOp& operator=(const GetOp&);
  GetOp(GetOp&&);
  GetOp& operator=(GetOp&&);

  mutable std::mutex mutex_;
  std::promise<std::unique_ptr<Data>> promise_;
  bool promise_set_;
  std::vector<maidsafe_error> errors_;
};

}  // namespace detail

template<typename Data>
void Put(ClientMaidNfs& client_maid_nfs,
         const Data& data,
         const passport::PublicPmid::name_type& pmid_node_hint,
         int successes_required,
         std::function<void(Reply)> result);

// TODO(Fraser#5#): 2013-06-13 - We have to use a unique_ptr here because MS won't fix their
// implementation of future to not require the shared state's type (e.g. public key) to be
// default-constructible.
template<typename Data>
std::future<std::unique_ptr<Data>> Get(ClientMaidNfs& client_maid_nfs,
                                       const typename Data::name_type& name);

template<typename Data>
void Delete(ClientMaidNfs& client_maid_nfs,
            const typename Data::name_type& name,
            int successes_required,
            std::function<void(Reply)> result);

}  // namespace nfs

}  // namespace maidsafe

#include "maidsafe/nfs/client_utils-inl.h"

#endif  // MAIDSAFE_NFS_CLIENT_UTILS_H_
