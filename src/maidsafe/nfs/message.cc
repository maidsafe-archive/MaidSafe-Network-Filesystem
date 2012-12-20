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

Message::Message(ActionType action_type,
                 PersonaType destination_persona_type,
                 PersonaType source_persona_type,
                 int data_type,
                 const NodeId& destination,
                 const NodeId& source,
                 const NonEmptyString& content,
                 const asymm::Signature& signature)
    : action_type_(action_type),
      destination_persona_type_(destination_persona_type),
      source_persona_type_(source_persona_type),
      data_type_(data_type),
      destination_(destination),
      source_(source),
      content_(content),
      signature_(signature) {
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

Message::Message(const protobuf::Message& proto_message)
    : action_type_(static_cast<ActionType>(proto_message.action_type())),
      destination_persona_type_(static_cast<PersonaType>(proto_message.destination_persona_type())),
      source_persona_type_(static_cast<PersonaType>(proto_message.source_persona_type())),
      data_type_(proto_message.data_type()),
      destination_(proto_message.destination()),
      source_(proto_message.source()),
      content_(proto_message.content()),
      signature_(proto_message.signature()) {
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

bool Message::ValidateInputs() const {
  return (data_type_ >= 0) && !destination_.IsZero() && !source_.IsZero();
}

Message Parse(const NonEmptyString& serialised_message) {
  protobuf::Message proto_message;
  if (!proto_message.ParseFromString(serialised_message.string()))
    ThrowError(NfsErrors::message_parsing_error);

  return Message(proto_message);
}

NonEmptyString Serialise(const Message& message) {
  NonEmptyString serialised_message;
  try {
    protobuf::Message proto_message;
    proto_message.set_action_type(static_cast<int32_t>(message.action_type()));
    proto_message.set_destination_persona_type(
        static_cast<int32_t>(message.destination_persona_type()));
    proto_message.set_source_persona_type(static_cast<int32_t>(message.source_persona_type()));
    proto_message.set_data_type(message.data_type());
    proto_message.set_destination(message.destination().string());
    proto_message.set_source(message.source().string());
    proto_message.set_content(message.content().string());
    if (message.signature().IsInitialised())
      proto_message.set_signature(message.signature().string());

    serialised_message = NonEmptyString(proto_message.SerializeAsString());
  }
  catch(const std::system_error&) {
    ThrowError(NfsErrors::invalid_parameter);
  }
  return serialised_message;
}

}  // namespace nfs

}  // namespace maidsafe
