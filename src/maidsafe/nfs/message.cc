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
      serialised_inner_message_(other.serialised_inner_message_) {}

Message& Message::operator=(const Message& other) {
  is_data_message_ = other.is_data_message_;
  serialised_inner_message_ = other.serialised_inner_message_;
  return *this;
}

Message::Message(Message&& other)
    : is_data_message_(std::move(other.is_data_message_)),
      serialised_inner_message_(std::move(other.serialised_inner_message_)) {}

Message::Message(const serialised_type& serialised_message)
    : is_data_message_(false),
      serialised_inner_message_() {
  protobuf::Message proto_message;
  if (!proto_message.ParseFromString(serialised_message->string()))
    ThrowError(NfsErrors::message_parsing_error);
  is_data_message_ = (proto_message.message_type() == 0);
  serialised_inner_message_ = NonEmptyString(proto_message.serialised_message());
}

Message::serialised_type Message::Serialise() const {
  serialised_type serialised_message;
  try {
    protobuf::Message proto_message;
    proto_message.set_message_type(static_cast<protobuf::Message::MessageType>(!is_data_message_)); // FIXME
    proto_message.set_serialised_message(serialised_inner_message_.string());
    serialised_message = serialised_type(NonEmptyString(proto_message.SerializeAsString()));
  }
  catch(const std::system_error&) {
    ThrowError(NfsErrors::invalid_parameter);
  }
  return serialised_message;
}

}  // namespace nfs

}  // namespace maidsafe
