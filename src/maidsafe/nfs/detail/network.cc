/*  Copyright 2014 MaidSafe.net limited

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
#include "maidsafe/nfs/detail/network.h"

#include <algorithm>
#include <functional>
#include <type_traits>

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/utils.h"

namespace maidsafe {
namespace nfs {
namespace detail {

namespace {
void ReduceMemoryUsed(std::vector<boost::future<void>>& tokens) {
  MAIDSAFE_CONSTEXPR_OR_CONST Bytes token_size(
      sizeof(std::remove_reference<decltype(tokens)>::type::value_type));

  if (token_size * tokens.capacity() > MegaBytes(1)) {
    if (tokens.capacity() - tokens.size() >= tokens.size() / 2) {
      tokens.shrink_to_fit();
    }
  }
}

}  // namespace

Network::Interface::Interface() {}
Network::Interface::~Interface() {}

Network::Network(std::shared_ptr<Interface> interface)
  : interface_(std::move(interface)),
    waiting_tokens_(),
    waiting_thread_(),
    waiting_notification_(),
    waiting_mutex_(),
    continue_waiting_(true) {
  if (interface_ == nullptr) {
    BOOST_THROW_EXCEPTION(std::system_error(make_error_code(CommonErrors::null_pointer)));
  }
  waiting_thread_ = std::thread(std::bind(&Network::WaitForTokens, this));
}

Network::~Network() {
  try {
    interface_.reset();  // cancels existing operations
    continue_waiting_ = false;
    waiting_notification_.notify_one();
    waiting_thread_.join();
    boost::wait_for_all(waiting_tokens_.begin(), waiting_tokens_.end());
  }
  catch (...) {
  }
}

void Network::WaitForTokens() {
  try {
    std::vector<boost::future<void>> moved_tokens;
    while (continue_waiting_) {
      {
        std::unique_lock<std::mutex> lock(waiting_mutex_);
        waiting_notification_.wait(
            lock,
            [this, &moved_tokens] {
              return !moved_tokens.empty() || !waiting_tokens_.empty() || !continue_waiting_;
            });

        moved_tokens.reserve(moved_tokens.size() + waiting_tokens_.size());
        std::move(
            waiting_tokens_.begin(),
            waiting_tokens_.end(),
            std::back_inserter(moved_tokens));
        waiting_tokens_.clear();
        ReduceMemoryUsed(waiting_tokens_);
      }

      auto token = boost::wait_for_any(moved_tokens.begin(), moved_tokens.end());
      if (token != moved_tokens.end()) {
        token->get();
        moved_tokens.erase(token);
      }

      ReduceMemoryUsed(moved_tokens);
    }

    const std::lock_guard<std::mutex> lock(waiting_mutex_);
    waiting_tokens_.reserve(moved_tokens.size() + waiting_tokens_.size());
    std::move(
        moved_tokens.begin(),
        moved_tokens.end(),
        std::back_inserter(waiting_tokens_));
  }
  catch (const std::exception& exception) {
    LOG(kError) << exception.what();
    throw;  // abort process
  }
  catch (...) {
    LOG(kError) << "Unknown exception";
    throw;  // abort process
  }
}

void Network::AddToWaitList(boost::future<void> completion_token) {
  try {
    {
      const std::lock_guard<std::mutex> lock(waiting_mutex_);
      waiting_tokens_.push_back(std::move(completion_token));
    }
    waiting_notification_.notify_one();
  }
  catch (...) {
    completion_token.get();
    throw;
  }
}

}  // namespace detail
}  // namespace nfs
}  // namespace maidsafe
