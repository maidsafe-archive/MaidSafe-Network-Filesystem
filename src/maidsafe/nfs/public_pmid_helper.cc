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

#include "maidsafe/nfs/public_pmid_helper.h"

namespace maidsafe {

namespace nfs {

namespace detail {

PublicPmidHelper::PublicPmidHelper()
    : mutex_(),
      running_(false),
      new_functors_(),
      new_futures_(),
      worker_future_() {}

PublicPmidHelper::~PublicPmidHelper() {
  try {
    if (worker_future_.valid())
      worker_future_.get();
  } catch (const std::exception& ex) {
    LOG(kWarning) << "Error in ~PublicPmidHelper() : " << boost::diagnostic_information(ex);
  }
}

void PublicPmidHelper::AddEntry(boost::future<passport::PublicPmid>&& future,
                                routing::GivePublicKeyFunctor give_key) {
  std::lock_guard<std::mutex> lock(mutex_);
  new_functors_.push_back(give_key);
  new_futures_.push_back(std::move(future));
  Run();
}

void PublicPmidHelper::Run() {
  if (!running_) {
    LOG(kVerbose) << "starting thread !";
    running_ = true;
    worker_future_ = std::async(std::launch::async, [this]() { this->Poll(); });
  }
}

void PublicPmidHelper::Poll() {
  std::vector<boost::future<passport::PublicPmid>> futures;
  std::vector<routing::GivePublicKeyFunctor> functors;

  for (;;) {
    {
      assert(futures.size() == functors.size());
      std::lock_guard<std::mutex> lock(mutex_);
      std::move(new_futures_.begin(), new_futures_.end(), std::back_inserter(futures));
      std::move(new_functors_.begin(), new_functors_.end(), std::back_inserter(functors));
      new_futures_.resize(0);
      new_functors_.resize(0);

      if (futures.empty()) {
        LOG(kVerbose) << "reset running_ to false";
        assert(functors.empty());
        running_ = false;
        break;
      }
    }
//     LOG(kVerbose) << " polling on (" << futures.size() << ") futures";
    auto ready_future_itr = boost::wait_for_any(futures.begin(), futures.end());
    auto index = ready_future_itr - futures.begin();
    try {
      auto public_pmid = ready_future_itr->get();
      LOG(kVerbose) << " got public_pmid of " << HexSubstr(public_pmid.name()->string())
                    << " from network";
      functors.at(index)(public_pmid.public_key());
    } catch (const std::exception& /*e*/) {
      functors.at(index)(asymm::PublicKey());
    }
    futures.erase(ready_future_itr);
    functors.erase(functors.begin() + index);
  };
}

}  // namespace detail

}  // namespace nfs

}  // namespace maidsafe
