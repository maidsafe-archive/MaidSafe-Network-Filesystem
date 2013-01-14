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

#include "maidsafe/nfs/message.h"

#include "maidsafe/common/error.h"

#include "maidsafe/nfs/message_pb.h"


namespace maidsafe {

namespace nfs {

Message::Message(const Message& other)
    : is_data_message_(other.is_data_message_),
      serialised_message_(other.serialised_message_) {}

Message& Message::operator=(const Message& other) {
  is_data_message_ = other.is_data_message_;
  serialised_message_ = other.serialised_message_;
  return *this;
}

Message::Message(Message&& other)
    : is_data_message_(std::move(other.is_data_message_)),
      serialised_message_(std::move(other.serialised_message_)) {}

Message::Message(const serialised_type& serialised_message)
    : is_data_message_(false),
      serialised_message_() {
  protobuf::Message proto_message;
  if (!proto_message.ParseFromString(serialised_message->string()))
    ThrowError(NfsErrors::message_parsing_error);
  is_data_message_ = (proto_message.has_data_message() ? true : false);
  if (is_data_message_) {
    serialised_message_ = serialised_type(NonEmptyString(proto_message.data_message().SerializeAsString()));
  } else {
    serialised_message_ = serialised_type(NonEmptyString(proto_message.post_message().SerializeAsString()));
  }
}

Message::serialised_type Message::Serialise() const {
  serialised_type serialised_message;
  try {
    protobuf::Message proto_message;
    if (is_data_message()) {
      protobuf::DataMessage *data_message = proto_message.mutable_data_message();
      if (!data_message->ParseFromString(serialised_message_->string()))
        ThrowError(NfsErrors::message_parsing_error);
    } else {
      protobuf::PostMessage_ *post_message = proto_message.mutable_post_message();
      if (!post_message->ParseFromString(serialised_message_->string()))
        ThrowError(NfsErrors::message_parsing_error);
    }
    serialised_message = serialised_type(NonEmptyString(proto_message.SerializeAsString()));
  }
  catch(const std::system_error&) {
    ThrowError(NfsErrors::invalid_parameter);
  }
  return serialised_message;
}

}  // namespace nfs

}  // namespace maidsafe
