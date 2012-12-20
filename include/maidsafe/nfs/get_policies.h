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

#ifndef MAIDSAFE_NFS_GET_POLICIES_H_
#define MAIDSAFE_NFS_GET_POLICIES_H_

#include <functional>
#include <future>

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/crypto.h"
#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/utils.h"


namespace args = std::placeholders;

namespace maidsafe {

namespace nfs {

class GetFromMetaDataManager {
 public:
  template<typename T>
  static void Get<>(name, callback, routing, fob) {}
 protected:
  ~GetFromMetaDataManager() {}
};

class GetFromDataHolder {
 public:
  template<typename Data>
  std::future<Data> Get(const Data::name_type& name, routing::Routing& routing) {
    auto promise(std::make_shared<std::promise<Data>>());
    std::future<Data> future(promise->get_future());
    routing::ResponseFunctor callback =
        [promise](const std::vector<std::string>& serialised_messages) {
          HandleGetResponse(promise, serialised_messages);
        };
    bool is_cacheable(is_long_term_cacheable<Data>::value || is_short_term_cacheable<Data>::value);
    //construct serialised nfs::Message
    routing.Send(name, name, serialised_message, callback, bptime::seconds(11),
                 routing::DestinationType::kGroup, is_cacheable);
    return future;
  }

 protected:
  ~GetFromDataHolder() {}
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_GET_POLICIES_H_
