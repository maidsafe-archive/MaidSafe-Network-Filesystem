/*  Copyright 2013 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

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

#include "maidsafe/routing/api_config.h"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/message_types_partial.h"

#include "maidsafe/nfs/message_wrapper.h"
#include "maidsafe/nfs/types.h"

namespace maidsafe {

namespace vault {

template <typename T>
inline bool GetVariant(const maidsafe::nfs::TypeErasedMessageWrapper& message, T& variant);

}  // namespace vault

namespace nfs {

namespace detail {

template <typename PersonaService, typename Sender, typename Receiver>
class PersonaDemuxer : public boost::static_visitor<
                           typename PersonaService::HandleMessageReturnType> {
 public:
  PersonaDemuxer(PersonaService& persona_service, const Sender& sender, const Receiver& receiver)
      : persona_service_(persona_service), sender_(sender), receiver_(receiver) {}
  template <typename Message>
  typename std::enable_if<std::is_same<Sender, typename Message::Sender>::value &&
                              std::is_same<Receiver, typename Message::Receiver>::value,
                          typename PersonaService::HandleMessageReturnType>::type
  operator()(const Message& message) const {
//     LOG(kVerbose) << "PersonaDemuxer calling persona_service HandleMessage";
    // If you have a compiler error leading here, you probably haven't implemented HandleMessage for
    // *every* type of message in the PublicMessages and VaultMessages variants of PersonaService.
    return persona_service_.HandleMessage(message, sender_, receiver_);
  }
  template <typename Message>
  typename std::enable_if<!std::is_same<Sender, typename Message::Sender>::value ||
                              !std::is_same<Receiver, typename Message::Receiver>::value,
                          typename PersonaService::HandleMessageReturnType>::type
  operator()(const Message& /*message*/) const {
    LOG(kError) << "invalid function call because of un-specialised templated method";
    // Should never come here.  Ideally this specialisation shouldn't exist, but the static visitor
    // requires all types in the variant to be handled.
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::invalid_parameter));
  }

 private:
  PersonaService& persona_service_;
  const Sender& sender_;
  const Receiver& receiver_;
};

}  // namespace detail

template <typename PersonaService>
class Service {
 public:
  typedef typename PersonaService::PublicMessages PublicMessages;
  typedef typename PersonaService::VaultMessages VaultMessages;
  typedef typename PersonaService::HandleMessageReturnType ReturnType;

  explicit Service(std::unique_ptr<PersonaService>&& impl) : impl_(std::move(impl)) {
    static_assert(!std::is_same<PublicMessages, VaultMessages>::value,
                  "Both Message types cannot be the same.");
  }

  template <typename Sender, typename Receiver>
  ReturnType HandleMessage(
      const nfs::TypeErasedMessageWrapper& message, const Sender& sender,
      const Receiver& receiver) {
    const detail::PersonaDemuxer<PersonaService, Sender, Receiver> demuxer(*impl_, sender,
                                                                           receiver);
    static std::is_void<PublicMessages> public_messages_void_state;
    static std::is_void<VaultMessages> vault_messages_void_state;
    try {
      return HandleMessage(message, demuxer, public_messages_void_state, vault_messages_void_state);
    }
    catch (const maidsafe_error& error) {
      LOG(kError) << "Invalid request. " << boost::diagnostic_information(error);
      throw;
    }
  }

  void HandleChurnEvent(std::shared_ptr<routing::CloseNodesChange> close_nodes_change) {
//     LOG(kVerbose) << "NFS service calling persona_service HandleChurnEvent";
    return impl_->HandleChurnEvent(close_nodes_change);
  }

  // Special case to handle relay messages from data getter to data manager
  void HandleMessage(
      const nfs::GetRequestFromDataGetterPartialToDataManager& message,
      const typename nfs::GetRequestFromDataGetterPartialToDataManager::Sender& sender,
      const typename nfs::GetRequestFromDataGetterPartialToDataManager::Receiver& receiver) {
    impl_->HandleMessage(message, sender, receiver);
  }

  // Special case to handle relay messages from node to data manager
  void HandleMessage(
      const nfs::GetRequestFromMaidNodePartialToDataManager& message,
      const typename nfs::GetRequestFromMaidNodePartialToDataManager::Sender& sender,
      const typename nfs::GetRequestFromMaidNodePartialToDataManager::Receiver& receiver) {
    impl_->HandleMessage(message, sender, receiver);
  }

  void Stop() {
    impl_->Stop();
  }

 private:
  typedef std::true_type IsVoid;
  typedef std::false_type IsNotVoid;

  template <typename Demuxer>
  ReturnType HandlePublicMessage(const nfs::TypeErasedMessageWrapper& message,
                                 const Demuxer& demuxer) {
//     LOG(kVerbose) << "nfs HandlePublicMessage";
    PublicMessages public_variant_message;
    if (!nfs::GetVariant(message, public_variant_message)) {
//       LOG(kError) << "Not a valid public message";
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::invalid_parameter));
    }
    return boost::apply_visitor(demuxer, public_variant_message);
  }
  template <typename Demuxer>
  ReturnType HandleVaultMessage(const nfs::TypeErasedMessageWrapper& message,
                                const Demuxer& demuxer) {
//     LOG(kVerbose) << "nfs HandleVaultMessage";
    VaultMessages vault_variant_message;
    if (!vault::GetVariant(message, vault_variant_message)) {
      LOG(kError) << "Not a valid vault message" << vault_variant_message;
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::invalid_parameter));
    } else {
      LOG(kVerbose) << "processing: " << vault_variant_message;
    }
    return boost::apply_visitor(demuxer, vault_variant_message);
  }

  // Public messages only are non-void.
  template <typename Demuxer>
  ReturnType HandleMessage(const nfs::TypeErasedMessageWrapper& message, const Demuxer& demuxer,
                           IsNotVoid, IsVoid) {
    return HandlePublicMessage(message, demuxer);
  }

  // Vault messages only are non-void.
  template <typename Demuxer>
  ReturnType HandleMessage(const nfs::TypeErasedMessageWrapper& message, const Demuxer& demuxer,
                           IsVoid, IsNotVoid) {
    return HandleVaultMessage(message, demuxer);
  }

  // Both types of message are non-void.
  template <typename Demuxer>
  ReturnType HandleMessage(const nfs::TypeErasedMessageWrapper& message, const Demuxer& demuxer,
                           IsNotVoid, IsNotVoid) {
    try {
      return HandlePublicMessage(message, demuxer);
    }
    catch (const maidsafe_error& /*error*/) {
      return HandleVaultMessage(message, demuxer);
    }
  }

  std::unique_ptr<PersonaService> impl_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_SERVICE_H_
