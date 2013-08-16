/* Copyright 2013 MaidSafe.net limited

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

#ifndef MAIDSAFE_NFS_SERVICE_H_
#define MAIDSAFE_NFS_SERVICE_H_

#include <type_traits>

#include "boost/variant/static_visitor.hpp"
#include "boost/variant/variant.hpp"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/message_wrapper.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace detail {

template<typename PersonaService, typename Sender, typename Receiver>
class PersonaDemuxer : public boost::static_visitor<> {
 public:
  PersonaDemuxer(PersonaService& persona_service, const Sender& sender, const Receiver& receiver)
    : persona_service_(persona_service),
      sender_(sender),
      receiver_(receiver) {}
  template<typename Message>
  void operator()(const Message& message) const {
    persona_service_.HandleMessage(message, sender_, receiver_);
  }

 private:
  PersonaService& persona_service_;
  const Sender& sender_;
  const Receiver& receiver_;
};

}  // namespace detail


template<typename PersonaService>
class Service {
 public:
  typedef typename PersonaService::PublicMessages PublicMessages;
  typedef typename PersonaService::VaultMessages VaultMessages;

  explicit Service(routing::Routing& routing) : impl_(routing) {}

  template<typename Sender, typename Receiver>
  void HandleMessage(const nfs::TypeErasedMessageWrapper& message,
                     const Sender& sender,
                     const Receiver& receiver) {
    if (!HandlePublicMessage(message, sender, receiver, std::is_void<PublicMessages>::type) &&
        !HandleVaultMessage(message, sender, receiver, std::is_void<VaultMessages>::type)) {
      LOG(kError) << "Invalid request.";
      ThrowError(CommonErrors::invalid_parameter);
    }
  }

 private:
  template<typename Sender, typename Receiver>
  bool HandlePublicMessage(const nfs::TypeErasedMessageWrapper& message,
                           const Sender& sender,
                           const Receiver& receiver,
                           std::true_type) {
    return false;
  }

  template<typename Sender, typename Receiver>
  bool HandlePublicMessage(const nfs::TypeErasedMessageWrapper& message,
                           const Sender& sender,
                           const Receiver& receiver,
                           std::false_type) {
    static const detail::PersonaDemuxer<PersonaService, Sender, Receiver> demuxer(impl_, sender,
                                                                                  receiver);
    PublicMessages public_variant_message;
    return nfs::GetVariant(message, public_variant_message) &&
           boost::apply_visitor(demuxer, public_variant_message);
  }

  template<typename Sender, typename Receiver>
  bool HandleVaultMessage(const nfs::TypeErasedMessageWrapper& message,
                          const Sender& sender,
                          const Receiver& receiver,
                          std::true_type) {
    return false;
  }

  template<typename Sender, typename Receiver>
  bool HandleVaultMessage(const nfs::TypeErasedMessageWrapper& message,
                          const Sender& sender,
                          const Receiver& receiver,
                          std::false_type) {
    static const detail::PersonaDemuxer<PersonaService, Sender, Receiver> demuxer(impl_, sender,
                                                                                  receiver);
    VaultMessages vault_variant_message;
    if (vault::GetVariant(message, vault_variant_message))
      return boost::apply_visitor(demuxer, vault_variant_message);
  }
  PersonaService impl_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_SERVICE_H_
