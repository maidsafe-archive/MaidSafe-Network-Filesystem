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

#include "maidsafe/nfs/public_key_getter.h"

#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"


namespace maidsafe {

namespace nfs {

PublicKeyGetter::PublicKeyGetter(routing::Routing& routing,
                                 const std::vector<passport::Pmid>& pmids_from_file)
    : key_getter_nfs_(pmids_from_file.empty() ? new KeyGetterNfs(routing) : nullptr),
      key_helper_nfs_(pmids_from_file.empty() ? nullptr : new KeyHelperNfs(pmids_from_file)),
      running_(true),
      pending_keys_(),
      flags_mutex_(),
      mutex_(),
      condition_(),
      thread_([this] { Run(); }) {
#ifndef TESTING
  if (use_key_helper) {
    LOG(kError) << "Cannot use fake key getter if TESTING is not defined";
    ThrowError(NfsErrors::invalid_parameter);
  }
#endif
}

PublicKeyGetter::~PublicKeyGetter() {
  {
    std::lock_guard<std::mutex> flags_lock(flags_mutex_);
    running_ = false;
  }
  condition_.notify_one();
  thread_.join();
}

void PublicKeyGetter::HandleGetKey(const NodeId& node_id,
                                   const routing::GivePublicKeyFunctor& give_key) {
#ifdef TESTING
  std::future<passport::PublicPmid> future;
  if (key_getter_nfs_) {
    future = std::move(key_getter_nfs_->Get<passport::PublicPmid>(
        passport::PublicPmid::name_type(Identity(node_id.string()))));
  } else {
    future = std::move(key_helper_nfs_->Get(
        passport::PublicPmid::name_type(Identity(node_id.string()))));
  }
#else
  std::future<passport::PublicPmid> future(key_getter_nfs_->Get<passport::PublicPmid>(
      passport::PublicPmid::name_type(Identity(node_id.string()))));
#endif
  std::shared_ptr<PendingKey> pending_key(new PendingKey(std::move(future), give_key));
  AddPendingKey(pending_key);
}

void PublicKeyGetter::AddPendingKey(std::shared_ptr<PendingKey> pending_key) {
  std::lock_guard<std::mutex> flags_lock(flags_mutex_);
  if (!running_)
    return;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    pending_keys_.push_back(pending_key);
  }
  condition_.notify_one();
}

void PublicKeyGetter::Run() {
  auto running = [this]()->bool {
    std::lock_guard<std::mutex> flags_lock(flags_mutex_);
    return running_;
  };

  while (running()) {
    std::shared_ptr<PendingKey> ready_pending_key;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      while (pending_keys_.empty())
        condition_.wait(lock);

      auto itr_pending_key(pending_keys_.begin());
      while (itr_pending_key != pending_keys_.end()) {
        if (is_ready((*itr_pending_key)->future)) {
          ready_pending_key.swap(*itr_pending_key);
          pending_keys_.erase(itr_pending_key);
          break;
        }
      }
    }
    if (ready_pending_key) {
      passport::PublicPmid public_pmid = ready_pending_key->future.get();
      ready_pending_key->give_key(public_pmid.public_key());
    }
  }
}

}  // namespace nfs

}  // namespace maidsafe
