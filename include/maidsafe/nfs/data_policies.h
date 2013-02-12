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

#ifndef MAIDSAFE_NFS_DATA_POLICIES_H_
#define MAIDSAFE_NFS_DATA_POLICIES_H_

#include <memory>
#include <string>
#include <vector>

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/data_message.h"
#include "maidsafe/nfs/message.h"
#include "maidsafe/nfs/persona_id.h"


namespace maidsafe {

namespace nfs {

template<typename SigningFob, Persona source_persona, DataMessage::Action action>
class DataPolicy {
 public:
  explicit DataPolicy(routing::Routing& routing);
  DataPolicy(routing::Routing& routing, const SigningFob& signing_fob);

 protected:
  ~DataPolicy() {}
  template<typename Data>
  void ExecuteAction(routing::ResponseFunctor callback,
                     const typename Data::name_type& name,
                     const NonEmptyString& content = NonEmptyString(),
                     const passport::PublicPmid::name_type& data_holder_name =
                         passport::PublicPmid::name_type());

 private:
  template<typename Data>
  PersonaId GetDestination(const typename Data::name_type& name,
                           const passport::PublicPmid::name_type& data_holder_name) const;
  template<typename Data>
  DataMessage ConstructDataMessage(Persona destination_persona,
                                   const typename Data::name_type& name,
                                   const NonEmptyString& content,
                                   const passport::PublicPmid::name_type& data_holder_name) const;
  // Default no-op.
  void SignDataIfRequired(DataMessage& /*data_message*/) const {}
  Message ConstructMessage(const DataMessage& data_message) const;
  // Default uses SendGroup with cacheable = false
  template<typename Data>
  void Send(const NodeId& destination,
            const std::string& serialised_message,
            const routing::ResponseFunctor& callback) {
    routing_.SendGroup(destination, serialised_message, false, callback);
  }

  routing::Routing& routing_;
  const std::unique_ptr<const SigningFob> kSigningFob_;
  const PersonaId kSource_;
};


template<typename SigningFob, Persona source_persona>
class PutDataPolicy : public DataPolicy<SigningFob, source_persona, DataMessage::Action::kPut> {
 public:
  PutDataPolicy(routing::Routing& routing, const SigningFob& signing_fob)
      : DataPolicy<SigningFob, source_persona, DataMessage::Action::kPut>(routing, signing_fob) {}
  template<typename Data>
  void Put(const Data& data,
           const passport::PublicPmid::name_type& data_holder_hint,
           const routing::ResponseFunctor& callback) {
    ExecuteAction(callback, data.name(), data.data(), data_holder_hint);
  }
};

template<typename SigningFob, Persona source_persona>
class GetDataPolicy : public DataPolicy<SigningFob, source_persona, DataMessage::Action::kGet> {
 public:
  explicit GetDataPolicy(routing::Routing& routing)
      : DataPolicy<SigningFob, source_persona, DataMessage::Action::kGet>(routing) {}

  template<typename Data>
  void Get(const typename Data::name_type& name, const routing::ResponseFunctor& callback) {
    ExecuteAction(callback, name);
  }
};

template<typename SigningFob, Persona source_persona>
class DeleteDataPolicy : public DataPolicy<SigningFob,
                                           source_persona,
                                           DataMessage::Action::kDelete> {
 public:
  DeleteDataPolicy(routing::Routing& routing, const SigningFob& signing_fob)
      : DataPolicy<SigningFob,
                   source_persona,
                   DataMessage::Action::kDelete>(routing, signing_fob) {}
  template<typename Data>
  void Delete(const typename Data::name_type& name, const routing::ResponseFunctor& callback) {
    ExecuteAction(callback, name);
  }
};


template<typename SigningFob, Persona source_persona>
class PutReducerDataPolicy : public DataPolicy<SigningFob,
                                               source_persona,
                                               DataMessage::Action::kPut> {
 public:
  PutReducerDataPolicy(routing::Routing& routing, const SigningFob& signing_fob)
      : DataPolicy<SigningFob, source_persona, DataMessage::Action::kPut>(routing, signing_fob) {}
  template<typename Data>
  void Put(const passport::PublicPmid::name_type& data_holder_name,
           const Data& data,
           const routing::ResponseFunctor& callback) {
    ExecuteAction(callback, data.name(), data.data(), data_holder_name);
  }
};

template<typename SigningFob, Persona source_persona>
class GetReducerDataPolicy : public DataPolicy<SigningFob,
                                               source_persona,
                                               DataMessage::Action::kGet> {
 public:
  explicit GetReducerDataPolicy(routing::Routing& routing)
      : DataPolicy<SigningFob, source_persona, DataMessage::Action::kGet>(routing) {}

  template<typename Data>
  void Get(const passport::PublicPmid::name_type& data_holder_name,
           const typename Data::name_type& name,
           const routing::ResponseFunctor& callback) {
    ExecuteAction(callback, name, NonEmptyString(), data_holder_name);
  }
};

template<typename SigningFob, Persona source_persona>
class DeleteReducerDataPolicy : public DataPolicy<SigningFob,
                                                  source_persona,
                                                  DataMessage::Action::kDelete> {
 public:
  DeleteReducerDataPolicy(routing::Routing& routing, const SigningFob& signing_fob)
      : DataPolicy<SigningFob,
                   source_persona,
                   DataMessage::Action::kDelete>(routing, signing_fob) {}
  template<typename Data>
  void Delete(const passport::PublicPmid::name_type& data_holder_name,
              const typename Data::name_type& name,
              const routing::ResponseFunctor& callback) {
    ExecuteAction(callback, name, NonEmptyString(), data_holder_name);
  }
};

class DataGetterPolicy : public DataPolicy<passport::Maid,
                                           Persona::kDataGetter,
                                           DataMessage::Action::kGet> {
 public:
  explicit DataGetterPolicy(routing::Routing& routing)
      : DataPolicy<passport::Maid,
                   Persona::kDataGetter,
                   DataMessage::Action::kGet>(routing) {}
  template<typename Data>
  void Get(const typename Data::name_type& name, const routing::ResponseFunctor& callback) {
    ExecuteAction(callback, name);
  }
};


template<typename SigningFob>
class NoPut {
 public:
  NoPut() {}
  NoPut(routing::Routing&, const SigningFob&) {}
  template<typename Data>
  void Put(const Data&, const routing::ResponseFunctor&) {}
  template<typename Data>
  void Put(const NodeId&, const Data&, const routing::ResponseFunctor&) {}
 protected:
  ~NoPut() {}
};

template<typename SigningFob>
class NoGet {
 public:
  NoGet() {}
  NoGet(routing::Routing&, const SigningFob&) {}
  template<typename Data>
  void Get(const typename Data::name_type&, const routing::ResponseFunctor&) {}
  template<typename Data>
  void Get(const NodeId&, const typename Data::name_type&, const routing::ResponseFunctor&) {}
 protected:
  ~NoGet() {}
};

template<typename SigningFob>
class NoDelete {
 public:
  NoDelete() {}
  NoDelete(routing::Routing&, const SigningFob&) {}
  template<typename Data>
  void Delete(const typename Data::name_type&, const routing::ResponseFunctor&) {}
  template<typename Data>
  void Delete(const NodeId&, const typename Data::name_type&, const routing::ResponseFunctor&) {}
 protected:
  ~NoDelete() {}
};


#ifdef TESTING

class GetFromKeyFile {
 public:
  explicit GetFromKeyFile(const std::vector<passport::PublicPmid>& all_pmids)
      : kAllPmids_(all_pmids) {}
  template<typename Data>
  void Get(const typename Data::name_type& name, const routing::ResponseFunctor& callback);

 private:
  const std::vector<passport::PublicPmid> kAllPmids_;
};

#endif


typedef PutDataPolicy<passport::Maid, Persona::kClientMaid> ClientMaidPutPolicy;
typedef GetDataPolicy<passport::Maid, Persona::kClientMaid> ClientMaidGetPolicy;
typedef DeleteDataPolicy<passport::Maid, Persona::kClientMaid> ClientMaidDeletePolicy;

typedef PutDataPolicy<passport::Pmid, Persona::kMaidAccountHolder> MaidAccountHolderPutPolicy;
typedef GetDataPolicy<passport::Pmid, Persona::kMaidAccountHolder> MaidAccountHolderGetPolicy;
typedef DeleteDataPolicy<passport::Pmid, Persona::kMaidAccountHolder> MaidAccountHolderDeletePolicy;

typedef PutReducerDataPolicy<passport::Pmid, Persona::kMetadataManager> MetadataManagerPutPolicy;
typedef GetReducerDataPolicy<passport::Pmid, Persona::kMetadataManager> MetadataManagerGetPolicy;
typedef DeleteReducerDataPolicy<passport::Pmid,
                                Persona::kMetadataManager> MetadataManagerDeletePolicy;

typedef PutReducerDataPolicy<passport::Pmid,
                             Persona::kPmidAccountHolder> PmidAccountHolderPutPolicy;
typedef GetReducerDataPolicy<passport::Pmid,
                             Persona::kPmidAccountHolder> PmidAccountHolderGetPolicy;
typedef DeleteReducerDataPolicy<passport::Pmid,
                                Persona::kPmidAccountHolder> PmidAccountHolderDeletePolicy;

}  // namespace nfs

}  // namespace maidsafe

#include "maidsafe/nfs/data_policies-inl.h"

#endif  // MAIDSAFE_NFS_DATA_POLICIES_H_
