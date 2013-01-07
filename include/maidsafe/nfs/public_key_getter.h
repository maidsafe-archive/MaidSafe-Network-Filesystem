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
  typedef passport::PublicPmid PublicData;
  // all_pmids_from_file should only be non-empty if TESTING is defined
  PublicKeyGetter(routing::Routing& routing,
                  const std::vector<passport::Pmid>& pmids_from_file =
                      std::vector<passport::Pmid>());
  ~PublicKeyGetter();
  void HandleGetKey(const PublicData::name_type& key_name,
                    std::function<void(std::future<PublicData>)> get_key_future);

 private:
  struct PendingKey {
    PendingKey(std::future<PublicData> future_in,
               std::function<void(std::future<PublicData>)> get_key_future_in)
        : future(std::move(future_in)),
          get_key_future(get_key_future_in) {}
    std::future<PublicData> future;
    std::function<void(std::future<PublicData>)> get_key_future;
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

template<typename PublicData>
bool is_ready(std::future<PublicData>& f) {
  return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_PUBLIC_KEY_GETTER_H_
