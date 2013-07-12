/* Copyright 2012 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

#ifndef MAIDSAFE_NFS_DATA_POLICIES_H_
#define MAIDSAFE_NFS_DATA_POLICIES_H_

#include <memory>
#include <string>
#include <vector>

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/message.h"
#include "maidsafe/nfs/message_wrapper.h"
#include "maidsafe/nfs/persona_id.h"


namespace maidsafe {

namespace nfs {

template<typename SigningFob, Persona source_persona, MessageAction action>
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
                     const passport::PublicPmid::name_type& pmid_node_name =
      passport::PublicPmid::name_type());

 private:
  template<typename Data>
  PersonaId GetDestination(const typename Data::name_type& name,
                           const passport::PublicPmid::name_type& pmid_node_name) const;
  template<typename Data>
  Message ConstructMessage(Persona destination_persona,
                           const typename Data::name_type& name,
                           const NonEmptyString& content,
                           const passport::PublicPmid::name_type& pmid_node_name) const;
  MessageWrapper ConstructMessageWrapper(const Message& message) const;
  // Default no-op.
  void SignDataIfRequired(Message& /*message*/) const {}

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
class PutDataPolicy : public DataPolicy<SigningFob, source_persona, MessageAction::kPut> {
 public:
  PutDataPolicy(routing::Routing& routing, const SigningFob& signing_fob)
      : DataPolicy<SigningFob, source_persona, MessageAction::kPut>(routing, signing_fob) {}
  template<typename Data>
  void Put(const Data& data,
           const passport::PublicPmid::name_type& pmid_node_hint,
           const routing::ResponseFunctor& callback) {
    this->template ExecuteAction<Data>(callback, data.name(), data.Serialise().data,
                                       pmid_node_hint);
  }
};

template<typename SigningFob, Persona source_persona>
class GetDataPolicy : public DataPolicy<SigningFob, source_persona, MessageAction::kGet> {
 public:
  explicit GetDataPolicy(routing::Routing& routing)
      : DataPolicy<SigningFob, source_persona, MessageAction::kGet>(routing) {}

  template<typename Data>
  void Get(const typename Data::name_type& name, const routing::ResponseFunctor& callback) {
    this->template ExecuteAction<Data>(callback, name);
  }
};

template<typename SigningFob, Persona source_persona>
class DeleteDataPolicy : public DataPolicy<SigningFob, source_persona, MessageAction::kDelete> {
 public:
  DeleteDataPolicy(routing::Routing& routing, const SigningFob& signing_fob)
      : DataPolicy<SigningFob, source_persona, MessageAction::kDelete>(routing, signing_fob) {}
  template<typename Data>
  void Delete(const typename Data::name_type& name, const routing::ResponseFunctor& callback) {
    this->template ExecuteAction<Data>(callback, name);
  }
};


template<typename SigningFob, Persona source_persona>
class PutReducerDataPolicy : public DataPolicy<SigningFob, source_persona, MessageAction::kPut> {
 public:
  PutReducerDataPolicy(routing::Routing& routing, const SigningFob& signing_fob)
      : DataPolicy<SigningFob, source_persona, MessageAction::kPut>(routing, signing_fob) {}
  template<typename Data>
  void Put(const passport::PublicPmid::name_type& pmid_node_name,
           const Data& data,
           const routing::ResponseFunctor& callback) {
    this->template ExecuteAction<Data>(callback, data.name(), data.Serialise().data,
                                       pmid_node_name);
  }
};

template<typename SigningFob, Persona source_persona>
class GetReducerDataPolicy : public DataPolicy<SigningFob, source_persona, MessageAction::kGet> {
 public:
  explicit GetReducerDataPolicy(routing::Routing& routing)
      : DataPolicy<SigningFob, source_persona, MessageAction::kGet>(routing) {}

  template<typename Data>
  void Get(const passport::PublicPmid::name_type& pmid_node_name,
           const typename Data::name_type& name,
           const routing::ResponseFunctor& callback) {
    this->template ExecuteAction<Data>(callback, name, NonEmptyString(), pmid_node_name);
  }
};

template<typename SigningFob, Persona source_persona>
class DeleteReducerDataPolicy : public DataPolicy<SigningFob,
                                                  source_persona,
                                                  MessageAction::kDelete> {
 public:
  DeleteReducerDataPolicy(routing::Routing& routing, const SigningFob& signing_fob)
      : DataPolicy<SigningFob,
                   source_persona,
                   MessageAction::kDelete>(routing, signing_fob) {}
  template<typename Data>
  void Delete(const passport::PublicPmid::name_type& pmid_node_name,
              const typename Data::name_type& name,
              const routing::ResponseFunctor& callback) {
    this->template ExecuteAction<Data>(callback, name, NonEmptyString(), pmid_node_name);
  }
};

class DataGetterPolicy : public DataPolicy<passport::Maid,
                                           Persona::kDataGetter,
                                           MessageAction::kGet> {
 public:
  explicit DataGetterPolicy(routing::Routing& routing)
      : DataPolicy<passport::Maid,
                   Persona::kDataGetter,
                   MessageAction::kGet>(routing) {}
  template<typename Data>
  void Get(const typename Data::name_type& name, const routing::ResponseFunctor& callback) {
    this->template ExecuteAction<Data>(callback, name);
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
  explicit NoGet(routing::Routing&) {}
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


typedef PutDataPolicy<passport::Maid, Persona::kMaidNode> ClientMaidPutPolicy;
typedef GetDataPolicy<passport::Maid, Persona::kMaidNode> ClientMaidGetPolicy;
typedef DeleteDataPolicy<passport::Maid, Persona::kMaidNode> ClientMaidDeletePolicy;

typedef PutDataPolicy<passport::Pmid, Persona::kMaidManager> MaidManagerPutPolicy;
typedef GetDataPolicy<passport::Pmid, Persona::kMaidManager> MaidManagerGetPolicy;
typedef DeleteDataPolicy<passport::Pmid, Persona::kMaidManager> MaidManagerDeletePolicy;

typedef NoPut<passport::Pmid> VersionManagerPutPolicy;
typedef NoGet<passport::Pmid> VersionManagerGetPolicy;
typedef NoDelete<passport::Pmid>VersionManagerDeletePolicy;


typedef PutReducerDataPolicy<passport::Pmid, Persona::kDataManager> DataManagerPutPolicy;
typedef GetReducerDataPolicy<passport::Pmid, Persona::kDataManager> DataManagerGetPolicy;
typedef DeleteReducerDataPolicy<passport::Pmid,
                                Persona::kDataManager> DataManagerDeletePolicy;

typedef PutReducerDataPolicy<passport::Pmid,
                             Persona::kPmidManager> PmidManagerPutPolicy;
typedef GetReducerDataPolicy<passport::Pmid,
                             Persona::kPmidManager> PmidManagerGetPolicy;
typedef DeleteReducerDataPolicy<passport::Pmid,
                                Persona::kPmidManager> PmidManagerDeletePolicy;

}  // namespace nfs

}  // namespace maidsafe

#include "maidsafe/nfs/data_policies-inl.h"

#endif  // MAIDSAFE_NFS_DATA_POLICIES_H_
