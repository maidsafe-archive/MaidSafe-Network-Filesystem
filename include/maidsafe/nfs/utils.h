/*  Copyright 2013 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#ifndef MAIDSAFE_NFS_UTILS_H_
#define MAIDSAFE_NFS_UTILS_H_

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <string>
#include <system_error>
#include <vector>

#include "maidsafe/common/error.h"
#include "maidsafe/common/utils.h"
#include "maidsafe/routing/parameters.h"
#include "maidsafe/routing/api_config.h"

#include "maidsafe/nfs/public_pmid_helper.h"
#include "maidsafe/nfs/public_mpid_helper.h"

namespace maidsafe {

namespace nfs {

namespace detail {
template <typename T>
void DoGetPublicKey(T& persona, const NodeId& node_id,
                    routing::GivePublicKeyFunctor give_key,
                    std::vector<passport::PublicPmid>& public_pmids_from_file,
                    detail::PublicPmidHelper& public_pmid_helper) {
  passport::PublicPmid::Name name(Identity(node_id.string()));
  if (!public_pmids_from_file.empty()) {
//     LOG(kVerbose) << "fetch from local list containing "
//                   << public_pmids_from_file.size() << " pmids";
    auto itr(std::find_if(
        std::begin(public_pmids_from_file), std::end(public_pmids_from_file),
        [&name](const passport::PublicPmid & pmid) { return pmid.name() == name; }));
    if (itr != public_pmids_from_file.end()) {
      LOG(kVerbose) << "got public_pmid of " << HexSubstr(name.value) << " from local";
      give_key((*itr).public_key());
      return;
    }
//     if (itr == public_pmids_from_file.end()) {
//       LOG(kWarning) << "can't Get PublicPmid " << HexSubstr(name.value)
//                     << " from local";
//     } else {
//       LOG(kVerbose) << "got public_pmid of " << HexSubstr(name.value) << " from local";
//       give_key((*itr).public_key());
//       return;
//     }
  }
  auto future_key(persona.Get(name, std::chrono::seconds(10)));
  public_pmid_helper.AddEntry(std::move(future_key), give_key);
}

}  // namespace detail

template <typename A, typename B>
bool CheckMutuallyExclusive(const A& a, const B& b) {
  return (!a != !b);
}

template <typename MessageContents>
bool IsSuccess(const MessageContents& response) {
  LOG(kVerbose) << "IsSuccess return_code " << response.return_code.value.what();
  return response.return_code.value.code().value() == static_cast<int>(CommonErrors::success);
}

template <typename MessageContents>
std::error_code ErrorCode(const MessageContents& response) {
  return response.return_code.value.code();
}

// If 'responses' contains >= n successsful responses where n is 'successes_required', returns
// <iterator to nth successful reply, true> otherwise
// <iterator to most frequent failed reply, false>.  If there is more than one most frequent type,
// (e.g. 2 'no_such_element' and 2 'invalid_parameter') the iterator points to the first which
// reaches the frequency.
template <typename MessageContents>
std::pair<typename std::vector<MessageContents>::const_iterator, bool>
    GetSuccessOrMostFrequentResponse(const std::vector<MessageContents>& responses,
                                     int successes_required);

template <typename MessageContents>
class OpData {
 public:
  OpData(int successes_required, std::function<void(MessageContents)> callback);
  void HandleResponseContents(MessageContents&& response_contents);

 private:
  OpData(const OpData&);
  OpData(OpData&&);
  OpData& operator=(OpData);

  mutable std::mutex mutex_;
  int successes_required_;
  std::function<void(MessageContents)> callback_;
  std::vector<MessageContents> responses_;
  bool callback_executed_;
};

// ==================== Implementation =============================================================
template <typename MessageContents>
std::pair<typename std::vector<MessageContents>::const_iterator, bool>
GetSuccessOrMostFrequentResponse(const std::vector<MessageContents>& responses,
                                 int successes_required) {
  LOG(kVerbose) << "GetSuccessOrMostFrequentResponse responses.size() : "
                << responses.size() << " successes_required : " << successes_required;
  auto most_frequent_itr(std::end(responses));
  int successes(0), most_frequent(0);
  typedef std::map<std::error_code, int> Count;
  Count count;
  for (auto itr(std::begin(responses)); itr != std::end(responses); ++itr) {
    int this_reply_count(++count[ErrorCode(*itr)]);
    LOG(kVerbose) << "GetSuccessOrMostFrequentResponse this_reply_count : " << this_reply_count;
    if (IsSuccess(*itr)) {
      LOG(kVerbose) << "GetSuccessOrMostFrequentResponse successes : " << successes;
      if (++successes >= successes_required) {
        LOG(kVerbose) << "GetSuccessOrMostFrequentResponse return succeeded";
        return std::make_pair(itr, true);
      }
    } else {
      LOG(kVerbose) << "GetSuccessOrMostFrequentResponse failed";
      if (this_reply_count > most_frequent) {
        most_frequent = this_reply_count;
        most_frequent_itr = itr;
      }
    }
  }
  return std::make_pair(most_frequent_itr, false);
}

template <typename MessageContents>
OpData<MessageContents>::OpData(int successes_required,
                                std::function<void(MessageContents)> callback)
    : mutex_(),
      successes_required_(successes_required),
      callback_(callback),
      responses_(),
      callback_executed_(!callback) {
  if (!callback || successes_required <= 0) {
    LOG(kError) << "invalid parameters for OpData constructor";
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::invalid_parameter));
  }
}

template <typename MessageContents>
void OpData<MessageContents>::HandleResponseContents(MessageContents&& response_contents) {
  LOG(kVerbose) << "OpData<MessageContents>::HandleResponseContents";
  std::function<void(MessageContents)> callback;
  std::unique_ptr<MessageContents> result_ptr;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (callback_executed_) {
      LOG(kInfo) << "OpData<MessageContents>::HandleResponseContents already called back";
      return;
    }
    responses_.push_back(std::move(response_contents));
    auto result(GetSuccessOrMostFrequentResponse(responses_, successes_required_));
    // TODO(Fraser#5#): 2013-08-18 - Confirm expected count
    if (result.second || responses_.size() > (routing::Parameters::group_size / 2U)) {
      // Operation has succeeded or failed overall
      callback = callback_;
      callback_executed_ = true;
      result_ptr = std::unique_ptr<MessageContents>(new MessageContents(*result.first));
    } else {
      LOG(kWarning) << "OpData<MessageContents>::HandleResponseContents"
                    << " incorrect result or not enough result";
      return;
    }
  }
  LOG(kInfo) << "OpData<MessageContents>::HandleResponseContents call back";
  callback(*result_ptr);
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_UTILS_H_
