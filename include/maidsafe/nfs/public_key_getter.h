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

#include "maidsafe/nfs/nfs.h"

#include "maidsafe/passport/types.h"

#include "maidsafe/routing/routing_api.h"


namespace maidsafe {

namespace nfs {

class PublicKeyGetter {
 public:
  typedef boost::variant<passport::PublicAnmid,
                         passport::PublicAnsmid,
                         passport::PublicAntmid,
                         passport::PublicAnmaid,
                         passport::PublicMaid,
                         passport::PublicPmid,
                         passport::PublicAnmpid,
                         passport::PublicMpid> PublicDataType;
  // all_pmids_from_file should only be non-empty if TESTING is defined
  PublicKeyGetter(routing::Routing& routing,
                  const std::vector<passport::Pmid>& pmids_from_file =
                      std::vector<passport::Pmid>());
  ~PublicKeyGetter();
  template<typename Data>
  void HandleGetKey(const typename Data::name_type& key_name,
                    std::function<void(std::future<Data>)> get_key_future);

 private:
  struct PendingKey {
    PendingKey(std::future<PublicDataType> future_in,
               std::function<void(std::future<PublicDataType>)> get_key_future_in)
        : future(std::move(future_in)),
          get_key_future(get_key_future_in) {}
    std::future<PublicDataType> future;
    std::function<void(std::future<PublicDataType>)> get_key_future;
  };
  void Run();
  void AddPendingKey(std::shared_ptr<PendingKey> pending_key);

  std::unique_ptr<KeyGetterNfs> key_getter_nfs_;
  std::unique_ptr<KeyHelperNfs> key_helper_nfs_;
  bool running_;
  std::vector<std::shared_ptr<PendingKey>> pending_keys_;
  std::mutex flags_mutex_, mutex_;
  std::condition_variable condition_;
  std::thread thread_;
};

template<typename Data>
void PublicKeyGetter::HandleGetKey(const typename Data::name_type& key_name,
                                   std::function<void(std::future<Data>)> get_key_future) {
  std::future<Data> future;
#ifdef TESTING
  if (key_getter_nfs_) {
    std::future<Data> future(key_getter_nfs_->Get<Data>(Data::name_type(key_name)));
    std::shared_ptr<PendingKey> pending_key(new PendingKey(std::move(future), get_key_future));
    AddPendingKey(pending_key);
    return;
  } else {
    future = std::move(key_helper_nfs_->Get(Data::name_type(key_name)));
    std::shared_ptr<PendingKey> pending_key(new PendingKey(std::move(future), get_key_future));
    AddPendingKey(pending_key);
    return;
  }
#else
  future = std::move(key_getter_nfs_->Get<Data>(Data::name_type(key_name)));
#endif
  std::shared_ptr<PendingKey> pending_key(new PendingKey(std::move(future), get_key_future));
  AddPendingKey(pending_key);
}

template<typename Future>
bool is_ready(std::future<Future>& f) {
  return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_PUBLIC_KEY_GETTER_H_
