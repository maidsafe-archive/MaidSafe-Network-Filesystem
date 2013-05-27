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
