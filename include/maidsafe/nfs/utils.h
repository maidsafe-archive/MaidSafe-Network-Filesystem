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

#include "maidsafe/nfs/message_wrapper.h"
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

// Put, delete and PublicKeyGet(as use callback only, don't return future)
class OperationOp {
 public:
  OperationOp(int successes_required, std::function<void(Reply)> callback);
  void HandleReply(Reply&& reply);

 private:
  OperationOp(const OperationOp&);
  OperationOp& operator=(const OperationOp&);
  OperationOp(OperationOp&&);
  OperationOp& operator=(OperationOp&&);

  mutable std::mutex mutex_;
  int successes_required_;
  std::function<void(Reply)> callback_;
  std::vector<Reply> replies_;
  bool callback_executed_;
};

// Put, delete and PublicKeyGet(as use callback only, don't return future)
void HandleOperationReply(std::shared_ptr<OperationOp> op, const std::string& serialised_reply);

}  // namespace nfs

}  // namespace maidsafe

#include "maidsafe/nfs/utils-inl.h"

#endif  // MAIDSAFE_NFS_UTILS_H_
