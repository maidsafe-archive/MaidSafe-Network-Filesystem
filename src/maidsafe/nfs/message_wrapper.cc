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

#include "maidsafe/nfs/message_wrapper.pb.h"


namespace maidsafe {

namespace nfs {

MessageWrapper::MessageWrapper(const NonEmptyString& serialised_inner_message,
                               const asymm::Signature& signature)
    : serialised_inner_message_(serialised_inner_message),
      signature_(signature) {}

MessageWrapper::MessageWrapper(NonEmptyString&& serialised_inner_message,
                               const asymm::Signature& signature)
    : serialised_inner_message_(std::move(serialised_inner_message)),
      signature_(signature) {}

MessageWrapper::MessageWrapper(const MessageWrapper& other)
    : serialised_inner_message_(other.serialised_inner_message_),
      signature_(other.signature_) {}

MessageWrapper& MessageWrapper::operator=(const MessageWrapper& other) {
  serialised_inner_message_ = other.serialised_inner_message_;
  signature_ = other.signature_;
  return *this;
}

MessageWrapper::MessageWrapper(MessageWrapper&& other)
    : serialised_inner_message_(std::move(other.serialised_inner_message_)),
      signature_(std::move(other.signature_)) {}

MessageWrapper& MessageWrapper::operator=(MessageWrapper&& other) {
  serialised_inner_message_ = std::move(other.serialised_inner_message_);
  signature_ = std::move(other.signature_);
  return *this;
}

MessageWrapper::MessageWrapper(const serialised_type& serialised_message)
    : serialised_inner_message_(),
      signature_() {
  protobuf::MessageWrapper proto_message;
  if (!proto_message.ParseFromString(serialised_message->string()))
    ThrowError(CommonErrors::parsing_error);
  serialised_inner_message_ = NonEmptyString(proto_message.serialised_message());
  if (proto_message.has_signature())
    signature_ = asymm::Signature(proto_message.signature());
}

MessageWrapper::serialised_type MessageWrapper::Serialise() const {
  serialised_type serialised_message;
  try {
    protobuf::MessageWrapper proto_message;
    proto_message.set_serialised_message(serialised_inner_message_.string());
    serialised_message = serialised_type(NonEmptyString(proto_message.SerializeAsString()));
    if (signature_.IsInitialised())
      proto_message.set_signature(signature_.string());
  }
  catch(const std::exception&) {
    ThrowError(CommonErrors::invalid_parameter);
  }
  return serialised_message;
}

}  // namespace nfs

}  // namespace maidsafe
