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

#ifndef MAIDSAFE_NFS_PUBLIC_KEY_GETTER_H_
#define MAIDSAFE_NFS_PUBLIC_KEY_GETTER_H_

#include <chrono>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "boost/variant/variant.hpp"
#include "boost/variant/static_visitor.hpp"

#include "maidsafe/common/utils.h"
#include "maidsafe/passport/types.h"
#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/nfs.h"


namespace maidsafe {

namespace nfs {

class PublicKeyGetter {
 public:
  // all_pmids_from_file should only be non-empty if TESTING is defined
  PublicKeyGetter(routing::Routing& routing,
                  const std::vector<passport::PublicPmid>& public_pmids_from_file =
                      std::vector<passport::PublicPmid>());
  ~PublicKeyGetter();
  template<typename Data>
  void HandleGetKey(const typename Data::name_type& key_name,
                    std::function<void(std::future<Data>)> get_key_future);

 private:
  template<typename Data>
  struct PendingKey {
    PendingKey(std::future<Data> future_in,
               std::function<void(std::future<Data>)> get_key_future_in)
        : future(std::move(future_in)),
          get_key_future(get_key_future_in) {}
    std::future<Data> future;
    std::function<void(std::future<Data>)> get_key_future;
  };

  typedef boost::variant<std::shared_ptr<PendingKey<passport::PublicAnmid>>,
                         std::shared_ptr<PendingKey<passport::PublicAnsmid>>,
                         std::shared_ptr<PendingKey<passport::PublicAntmid>>,
                         std::shared_ptr<PendingKey<passport::PublicAnmaid>>,
                         std::shared_ptr<PendingKey<passport::PublicMaid>>,
                         std::shared_ptr<PendingKey<passport::PublicPmid>>,
                         std::shared_ptr<PendingKey<passport::PublicAnmpid>>,
                         std::shared_ptr<PendingKey<passport::PublicMpid>>> PendingKeyVariant;

  void Run();
  void AddPendingKey(PendingKeyVariant pending_key);
  class IsReady;
  class InvokeFunctor;

  std::unique_ptr<KeyGetterNfs> key_getter_nfs_;
  std::unique_ptr<FakeKeyGetterNfs> fake_key_getter_nfs_;
  bool running_;
  std::vector<PendingKeyVariant> pending_keys_;
  std::mutex flags_mutex_, mutex_;
  std::condition_variable condition_;
  std::thread thread_;
};

template<typename Data>
void PublicKeyGetter::HandleGetKey(const typename Data::name_type& key_name,
                                   std::function<void(std::future<Data>)> get_key_future) {
#ifdef TESTING
  if (key_getter_nfs_) {
#endif
    std::future<Data> future(std::move(key_getter_nfs_->Get<Data>(key_name)));
    PendingKeyVariant pending_key(
        std::make_shared<PendingKey<Data>>(std::move(future), get_key_future));
    AddPendingKey(pending_key);
#ifdef TESTING
  } else {
    std::future<passport::PublicPmid> future(
        std::move(fake_key_getter_nfs_->Get(key_name)));
    PendingKeyVariant pending_key(
        std::make_shared<PendingKey<passport::PublicPmid>>(std::move(future), get_key_future));
    AddPendingKey(pending_key);
  }
#endif
}

class PublicKeyGetter::IsReady : public boost::static_visitor<bool> {
 public:
  template<typename PublicKeyType>
  bool operator()(std::shared_ptr<PendingKey<PublicKeyType>> pending_key) const {
    return maidsafe::IsReady(pending_key->future);
  }
};

class PublicKeyGetter::InvokeFunctor : public boost::static_visitor<> {
 public:
  template<typename PublicKeyType>
  void operator()(std::shared_ptr<PendingKey<PublicKeyType>> pending_key) const {
    if (pending_key)
      pending_key->get_key_future(std::move(pending_key->future));
  }
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_PUBLIC_KEY_GETTER_H_
