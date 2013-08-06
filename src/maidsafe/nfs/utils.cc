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

#include "maidsafe/nfs/utils.h"

#include <cstdint>
#include <string>

#include "maidsafe/common/crypto.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/routing/parameters.h"

#include "maidsafe/nfs/message.h"


namespace maidsafe {

namespace nfs {

std::pair<std::vector<Message>::const_iterator, bool> GetSuccessOrMostFrequentReply(
    const std::vector<Message>& replies,
    int successes_required) {
  auto most_frequent_itr(std::end(replies));
  int successes(0), most_frequent(0);
  typedef std::map<std::error_code, int> Count;
  Count count;
  for (auto itr(std::begin(replies)); itr != std::end(replies); ++itr) {
    int this_reply_count(++count[(*itr).data().error->code()]);
    if ((*itr).data().IsSuccess()) {
      if (++successes >= successes_required)
        return std::make_pair(itr, true);
    } else if (this_reply_count > most_frequent) {
      most_frequent = this_reply_count;
      most_frequent_itr = itr;
    }
  }
  return std::make_pair(most_frequent_itr, false);
}

OperationOp::OperationOp(int successes_required, std::function<void(Message)> callback)
    : mutex_(),
      successes_required_(successes_required),
      callback_(callback),
      replies_(),
      callback_executed_(!callback) {
  if ((callback && successes_required <= 0) || (!callback && successes_required != 0))
    ThrowError(CommonErrors::invalid_parameter);
}

void OperationOp::HandleReply(Message&& reply) {
  std::function<void(Message)> callback;
  std::unique_ptr<Message> result_ptr;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (callback_executed_)
      return;
    replies_.push_back(std::move(reply));
    auto result(GetSuccessOrMostFrequentReply(replies_, successes_required_));
    if (result.second || replies_.size() == routing::Parameters::node_group_size) {
      // Operation has succeeded or failed overall
      callback = callback_;
      callback_executed_ = true;
      result_ptr = std::unique_ptr<Message>(new Message(*result.first));
    } else {
      return;
    }
  }
  callback(*result_ptr);
}

void HandleOperationReply(std::shared_ptr<OperationOp> op, const std::string& serialised_reply) {
  try {
    Message reply((Message::serialised_type(NonEmptyString(serialised_reply))));
    op->HandleReply(std::move(reply));
  }
  catch(const maidsafe_error& error) {
    LOG(kWarning) << "nfs error: " << error.code() << " - " << error.what();
    op->HandleReply(Message(Persona::kDataGetter, Persona::kDataGetter, Message::Data(error)));
  }
  catch(const std::exception& e) {
    LOG(kWarning) << "nfs error: " << e.what();
    op->HandleReply(Message(Persona::kDataGetter, Persona::kDataGetter,
                    Message::Data(CommonErrors::unknown)));
  }
}

}  // namespace nfs

}  // namespace maidsafe
