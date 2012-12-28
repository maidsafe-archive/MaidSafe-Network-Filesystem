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

#include <algorithm>
#include <future>
#include <string>
#include <vector>

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/crypto.h"
#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/utils.h"


namespace maidsafe {

namespace nfs {

class NoGet {
 public:
  explicit NoGet(routing::Routing& /*routing*/) {}

  template<typename Data>
  std::future<Data> Get(const typename Data::name_type& /*name*/) {}

 protected:
  ~NoGet() {}
};

class GetFromMetaDataManager {
 public:
  explicit GetFromMetaDataManager(routing::Routing& routing) : routing_(routing) {}

  template<typename Data>
  std::future<Data> Get(const typename Data::name_type& /*name*/) {}

 protected:
  ~GetFromMetaDataManager() {}

 private:
  routing::Routing& routing_;
};

template<PersonaType persona>
class GetFromDataHolder {
 public:
  explicit GetFromDataHolder(routing::Routing& routing) : routing_(routing) {}

  template<typename Data>
  std::future<Data> Get(const typename Data::name_type& name) {
    auto promise(std::make_shared<std::promise<Data>>());  // NOLINT (Fraser)
    std::future<Data> future(promise->get_future());
    routing::ResponseFunctor callback =
        [promise](const std::vector<std::string>& serialised_messages) {
          HandleGetResponse(promise, serialised_messages);
        };
    Message message(ActionType::kGet, PersonaType::kDataHolder, persona, DataType<Data>(),
                    NodeId(name.data.string()), routing.kNodeId(), NonEmptyString(),
                    asymm::Signature());
    routing_.Send(NodeId(name.data.string()), message.Serialise().data.string(), callback,
                  routing::DestinationType::kGroup, IsCacheable<Data>());
    return future;
  }

 protected:
  ~GetFromDataHolder() {}

 private:
  routing::Routing& routing_;
};


#ifdef TESTING

class GetFromKeyFile {
 public:
  explicit GetFromKeyFile(routing::Routing& /*routing*/) {}

  template<typename Data>
  std::future<Data> Get(const typename Data::name_type& name,
                        const std::vector<passport::Pmid>& pmids);

 protected:
  ~GetFromKeyFile() {}
};

// Only specialised for PublicPmid type - all other types not implemented
template<>
std::future<passport::PublicPmid> GetFromKeyFile::Get<passport::PublicPmid>(
    const typename passport::PublicPmid::name_type& name,
    const std::vector<passport::Pmid>& pmids) {
  std::promise<passport::PublicPmid> promise;
  try {
    auto itr(std::find_if(pmids.begin(), pmids.end(), [&name](const passport::Pmid& pmid) {
      return pmid.name() == name;
    }));
    if (itr == pmids.end())
      ThrowError(NfsErrors::failed_to_get_data);
    promise.set_value(passport::PublicPmid(*itr));
  }
  catch(...) {
    promise.set_exception(std::current_exception());
  }
  return promise.get_future();
}

#endif


}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_GET_POLICIES_H_
