/*  Copyright 2013 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.novinet.com/license

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#ifndef MAIDSAFE_NFS_SERVICE_H_
#define MAIDSAFE_NFS_SERVICE_H_

#include <memory>
#include <type_traits>

#include "boost/variant/static_visitor.hpp"
#include "boost/variant/variant.hpp"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/message_wrapper.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace vault {

template<typename T>
inline bool GetVariant(const maidsafe::nfs::TypeErasedMessageWrapper& message, T& variant);

}  // namespace vault

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

  explicit Service(std::unique_ptr<PersonaService>&& impl) : impl_(std::move(impl)) {
    static_assert(!std::is_void<PublicMessages>::value || !std::is_void<VaultMessages>::value,
                  "Both Message types cannot be 'void'.");
    static_assert(!std::is_same<PublicMessages, VaultMessages>::value,
                  "Both Message types cannot be the same.");
  }

  template<typename Sender, typename Receiver>
  void HandleMessage(const nfs::TypeErasedMessageWrapper& message,
                     const Sender& sender,
                     const Receiver& receiver) {
    const detail::PersonaDemuxer<PersonaService, Sender, Receiver> demuxer(*impl_, sender,
                                                                           receiver);
    static std::is_void<PublicMessages> public_messages_void_state;
    static std::is_void<VaultMessages> vault_messages_void_state;
    if (!HandleMessage(message, demuxer, public_messages_void_state, vault_messages_void_state)) {
      LOG(kError) << "Invalid request.";
      ThrowError(CommonErrors::invalid_parameter);
    }
  }

 private:
  typedef std::true_type IsVoid;
  typedef std::false_type IsNotVoid;

  template<typename Demuxer>
  bool HandlePublicMessage(const nfs::TypeErasedMessageWrapper& message, const Demuxer& demuxer) {
    PublicMessages public_variant_message;
    if (!nfs::GetVariant(message, public_variant_message))
      return false;
    boost::apply_visitor(demuxer, public_variant_message);
    return true;
  }

  template<typename Demuxer>
  bool HandleVaultMessage(const nfs::TypeErasedMessageWrapper& message, const Demuxer& demuxer) {
    VaultMessages vault_variant_message;
    if (vault::GetVariant(message, vault_variant_message))
      return false;
    boost::apply_visitor(demuxer, vault_variant_message);
    return true;
  }

  // Public messages only are non-void.
  template<typename Demuxer>
  bool HandleMessage(const nfs::TypeErasedMessageWrapper& message,
                     const Demuxer& demuxer,
                     IsNotVoid,
                     IsVoid) {
    return HandlePublicMessage(message, demuxer);
  }

  // Vault messages only are non-void.
  template<typename Demuxer>
  bool HandleMessage(const nfs::TypeErasedMessageWrapper& message,
                     const Demuxer& demuxer,
                     IsVoid,
                     IsNotVoid) {
    return HandleVaultMessage(message, demuxer);
  }

  // Both types of message are non-void.
  template<typename Demuxer>
  bool HandleMessage(const nfs::TypeErasedMessageWrapper& message,
                     const Demuxer& demuxer,
                     IsNotVoid,
                     IsNotVoid) {
    return HandlePublicMessage(message, demuxer) && HandleVaultMessage(message, demuxer);
  }

  std::unique_ptr<PersonaService> impl_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_SERVICE_H_
