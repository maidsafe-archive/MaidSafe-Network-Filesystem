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

#ifndef MAIDSAFE_NFS_MESSAGE_WRAPPER_H_
#define MAIDSAFE_NFS_MESSAGE_WRAPPER_H_

#include <string>
#include <tuple>
#include <utility>

#include "maidsafe/common/tagged_value.h"

#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace detail {

struct DestinationTag;
struct SourceTag;
typedef TaggedValue<Persona, struct detail::DestinationTag> DestinationTaggedValue;
typedef TaggedValue<Persona, struct detail::SourceTag> SourceTaggedValue;

}  // namespace detail



typedef std::tuple<MessageAction, detail::DestinationTaggedValue, detail::SourceTaggedValue,
                   MessageId, std::string> TypeErasedMessageWrapper;

template<MessageAction action, typename DestinationPersonaType, typename SourcePersonaType>
struct MessageWrapper {
  MessageWrapper();

  // For use with new messages (a new message_id is automatically applied).
  explicit MessageWrapper(const std::string& serialised_message_in);

  // For use when handling incoming messages where the sender has already set the message_id.
  explicit MessageWrapper(const TypeErasedMessageWrapper& parsed_message_wrapper);

  MessageWrapper(const MessageWrapper& other);
  MessageWrapper(MessageWrapper&& other);
  MessageWrapper& operator=(MessageWrapper other);

  std::string Serialise() const;

  friend void swap(MessageWrapper& lhs, MessageWrapper& rhs) {
    using std::swap;
    swap(lhs.message_id, rhs.message_id);
    swap(lhs.serialised_message, rhs.serialised_message);
  }

  MessageId message_id;
  std::string serialised_message;

 private:
  static const detail::DestinationTaggedValue kDestinationTaggedValue;
  static const detail::SourceTaggedValue kSourceTaggedValue;
};

TypeErasedMessageWrapper ParseMessageWrapper(const std::string& serialised_message_wrapper);



// ==================== Implementation =============================================================
namespace detail {

MessageId GetNewMessageId();

std::string SerialiseMessageWrapper(MessageAction action,
                                    DestinationTaggedValue destination_persona,
                                    SourceTaggedValue source_persona,
                                    MessageId message_id,
                                    const std::string& serialised_message);

}  // namespace detail

template<MessageAction action, typename DestinationPersonaType, typename SourcePersonaType>
const detail::DestinationTaggedValue
    MessageWrapper<action, DestinationPersonaType, SourcePersonaType>::kDestinationTaggedValue =
        detail::DestinationTaggedValue(typename DestinationPersonaType::value);

template<MessageAction action, typename DestinationPersonaType, typename SourcePersonaType>
const detail::SourceTaggedValue
    MessageWrapper<action, DestinationPersonaType, SourcePersonaType>::kSourceTaggedValue =
        detail::SourceTaggedValue(typename SourcePersonaType::value);

template<MessageAction action, typename DestinationPersonaType, typename SourcePersonaType>
MessageWrapper<action, DestinationPersonaType, SourcePersonaType>::MessageWrapper()
        : message_id(0),
          serialised_message() {}

template<MessageAction action, typename DestinationPersonaType, typename SourcePersonaType>
MessageWrapper<action, DestinationPersonaType, SourcePersonaType>::MessageWrapper(
    const std::string& serialised_message_in)
        : message_id(detail::GetNewMessageId()),
          serialised_message(serialised_message_in) {}

template<MessageAction action, typename DestinationPersonaType, typename SourcePersonaType>
MessageWrapper<action, DestinationPersonaType, SourcePersonaType>::MessageWrapper(
    const TypeErasedMessageWrapper& parsed_message_wrapper)
        : message_id(std::get<3>(parsed_message_wrapper)),
          serialised_message(std::get<4>(parsed_message_wrapper)) {}

template<MessageAction action, typename DestinationPersonaType, typename SourcePersonaType>
MessageWrapper<action, DestinationPersonaType, SourcePersonaType>::MessageWrapper(
    const MessageWrapper& other)
        : message_id(other.message_id),
          serialised_message(other.serialised_message) {}

template<MessageAction action, typename DestinationPersonaType, typename SourcePersonaType>
MessageWrapper<action, DestinationPersonaType, SourcePersonaType>::MessageWrapper(
    MessageWrapper&& other)
        : message_id(std::move(other.message_id)),
          serialised_message(std::move(other.serialised_message)) {}

template<MessageAction action, typename DestinationPersonaType, typename SourcePersonaType>
MessageWrapper<action, DestinationPersonaType, SourcePersonaType>&
    MessageWrapper<action, DestinationPersonaType, SourcePersonaType>::operator=(
        MessageWrapper other) {
  swap(*this, other);
  return *this;
}

template<MessageAction action, typename DestinationPersonaType, typename SourcePersonaType>
std::string MessageWrapper<action, DestinationPersonaType, SourcePersonaType>::Serialise() const {
  return detail::SerialiseMessageWrapper(action, kDestinationTaggedValue, kSourceTaggedValue,
                                         message_id, serialised_message);
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGE_WRAPPER_H_
