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

#include "maidsafe/nfs/message_wrapper.h"

#include "maidsafe/common/error.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/message_wrapper.pb.h"


namespace maidsafe {

namespace nfs {

namespace detail {

MessageId GetNewMessageId() {
  static int32_t random_element(RandomInt32());
  return MessageId(random_element++);
}

std::string SerialiseMessageWrapper(const TypeErasedMessageWrapper& message_tuple) {
  protobuf::MessageWrapper proto_message_wrapper;
  proto_message_wrapper.set_action(static_cast<int32_t>(std::get<0>(message_tuple)));
  proto_message_wrapper.set_source_persona(static_cast<int32_t>(std::get<1>(message_tuple).data));
  proto_message_wrapper.set_destination_persona(
      static_cast<int32_t>(std::get<2>(message_tuple).data));
  proto_message_wrapper.set_message_id(std::get<3>(message_tuple));
  proto_message_wrapper.set_serialised_contents(std::get<4>(message_tuple));
  return proto_message_wrapper.SerializeAsString();
}

}  // namespace detail



TypeErasedMessageWrapper ParseMessageWrapper(const std::string& serialised_message_wrapper) {
  protobuf::MessageWrapper proto_message_wrapper;
  if (!proto_message_wrapper.ParseFromString(serialised_message_wrapper))
    ThrowError(CommonErrors::parsing_error);

  return std::make_tuple(
      static_cast<MessageAction>(proto_message_wrapper.action()),
      detail::SourceTaggedValue(static_cast<Persona>(proto_message_wrapper.source_persona())),
      detail::DestinationTaggedValue(
          static_cast<Persona>(proto_message_wrapper.destination_persona())),
      MessageId(proto_message_wrapper.message_id()),
      proto_message_wrapper.serialised_contents());
}

}  // namespace nfs

}  // namespace maidsafe
