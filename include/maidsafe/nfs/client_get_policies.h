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

#ifndef MAIDSAFE_NFS_CLIENT_GET_POLICIES_H_
#define MAIDSAFE_NFS_CLIENT_GET_POLICIES_H_

#include <future>
#include <string>
#include <vector>

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"

#include "maidsafe/passport/types.h"

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/response_mapper.h"
#include "maidsafe/nfs/utils.h"



namespace maidsafe {

namespace nfs {

template<typename Data>
bool ProcessReadyFutureSuccessfully(StringFuture& future, std::promise<Data>& promise) {
  try {
    std::string serialised_message(future.get());
    // Need '((' when constructing Message to avoid most vexing parse.
    Message message((Message::serialised_type(NonEmptyString(serialised_message))));
    Data data(ValidateAndParse<Data>(message));
    promise.set_value(std::move(data));
    return true;
  }
  catch(const std::system_error& error) {
    LOG(kWarning) << "Get future problem: " << error.code() << " - " << error.what();
    return false;
  }
}

template<typename Data>
void HandleGetFutures(std::shared_ptr<std::promise<Data> > promise,
                      std::shared_ptr<StringFutureVector> routing_futures) {
  std::async(
      std::launch::async,
      [promise, routing_futures] {
        while (!routing_futures->empty()) {
          std::vector<StringFuture>::iterator itr(routing_futures->begin());
          while ((itr = FindNextReadyFuture(itr, *routing_futures)) != routing_futures->end()) {
            if (ProcessReadyFutureSuccessfully<Data>(*itr, *promise))
              return;
            ++itr;
          }
          std::this_thread::yield();
        }
        promise->set_exception(std::make_exception_ptr(MakeError(NfsErrors::failed_to_get_data)));
      });
}

class NoGet {
 public:
  NoGet() {}
  explicit NoGet(routing::Routing& /*routing*/) {}

  template<typename Data>
  std::future<Data> Get(const typename Data::name_type& /*name*/) {}

 protected:
  ~NoGet() {}
};


template<Persona persona>
class GetFromMetadataManager {
 public:
  GetFromMetadataManager(NfsResponseMapper& response_mapper,
                         routing::Routing& routing)
      : response_mapper_(response_mapper),
        routing_(routing),
        source_(PersonaId(persona, routing.kNodeId())) {}

  template<typename Data>
  std::vector<std::future<nfs::Reply>> Get(const typename Data::name_type& name) {
    DataMessage::Data data(Data::name_type::tag_type::kEnumValue, name.data,
                           NonEmptyString(), DataMessage::Action::kGet);
    DataMessage data_message(Persona::kMetadataManager, source_, data);
    Message message(DataMessage::message_type_identifier, data_message.Serialise());
    return NfsSendGroup(NodeId(name->string()), message,
                        nfs::IsCacheable<Data>(), response_mapper_, routing_);
  }

 protected:
  ~GetFromMetadataManager() {}

 private:
  NfsResponseMapper& response_mapper_;
  routing::Routing& routing_;
  PersonaId source_;
};


#ifdef TESTING

class GetFromKeyFile {
 public:
  explicit GetFromKeyFile(const std::vector<passport::PublicPmid>& all_pmids)
      : kAllPmids_(all_pmids) {}
  std::future<passport::PublicPmid> Get(const passport::PublicPmid::name_type& name);

 private:
  const std::vector<passport::PublicPmid> kAllPmids_;
};

#endif


}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_GET_POLICIES_H_
