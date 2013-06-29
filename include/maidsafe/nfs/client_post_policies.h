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

#ifndef MAIDSAFE_NFS_CLIENT_POST_POLICIES_H_
#define MAIDSAFE_NFS_CLIENT_POST_POLICIES_H_

#include <string>

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/message.h"
#include "maidsafe/nfs/message_wrapper.h"
#include "maidsafe/nfs/persona_id.h"

namespace maidsafe {

namespace nfs {

template<typename SigningFob>
class NoPost {
 public:
  NoPost() {}
  NoPost(routing::Routing&, const SigningFob&) {}
  template<typename Data>
  void Post(const typename Data::name_type& /*name*/) {}

 protected:
  ~NoPost() {}
};

template <typename SigningFob, Persona source_persona>
class ClientPostPolicy {
 public:
  ClientPostPolicy(routing::Routing& routing, const SigningFob& signing_fob)
      : routing_(routing),
        kSigningFob_(new SigningFob(signing_fob)),
        kSource_(source_persona, routing_.kNodeId()) {}

  void RegisterPmid(const NonEmptyString& serialised_pmid_registration,
                    const routing::ResponseFunctor& callback) {
    Message message(Persona::kMaidManager, kSource_,
                    Message::Data(kSigningFob_->name().data, serialised_pmid_registration,
                                  MessageAction::kRegisterPmid));
    MessageWrapper message_wrapper(message.Serialise());
    routing_.SendGroup(NodeId(message.data().name.string()),
                       message_wrapper.Serialise()->string(), false, callback);
  }

  void UnregisterPmid(const NonEmptyString& serialised_pmid_unregistration,
                      const routing::ResponseFunctor& callback) {
    Message message(Persona::kMaidManager, kSource_,
                    Message::Data(kSigningFob_->name().data, serialised_pmid_unregistration,
                                  MessageAction::kUnregisterPmid));
    MessageWrapper message_wrapper(message.Serialise());
    routing_.SendGroup(NodeId(message.data().name.string()),
                       message_wrapper.Serialise()->string(), false, callback);
  }

 private:
  routing::Routing& routing_;
  const std::unique_ptr<const SigningFob> kSigningFob_;
  const PersonaId kSource_;
};

typedef ClientPostPolicy<passport::Maid, Persona::kMaidNode> ClientMaidPostPolicy;

}  // namespace nfs

}  // namespace maidsafe

#include "maidsafe/nfs/client_post_policies-inl.h"

#endif  // MAIDSAFE_NFS_CLIENT_POST_POLICIES_H_
