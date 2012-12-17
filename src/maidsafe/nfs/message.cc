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

#include "maidsafe/nfs/message_types/messages.pb.h"

namespace maidsafe {

namespace nfs {

Message::Message(const ActionType& action_type,
                 const PersonaType& destination_persona_type,
                 const PersonaType& source_persona_type,
                 const int& data_type,
                 const NodeId& destination,
                 const NodeId& source,
                 const std::string& content,
                 const std::string& signature)
    : action_type_(action_type),
      destination_persona_type_(destination_persona_type),
      source_persona_type_(source_persona_type),
      data_type_(data_type),
      destination_(destination),
      source_(source),
      content_(content),
      signature_(signature) {
  if ((data_type_ < 0) ||
     (destination_.IsZero()) ||
     (source_.IsZero()) ||
     content.empty()) {
    ThrowError(CommonErrors::invalid_parameter);
  }
}

// throws
Message ParseFromString(const std::string& message_string) {
  proto::Nfs proto_message;
  if (!proto_message.ParseFromString(message_string)) {
    ThrowError(CommonErrors::unknown);  // TODO(Prakash) : is new error type for this case?
  }

  ActionType action_type = static_cast<ActionType>(proto_message.action_type());
  PersonaType destination_persona_type =
      static_cast<PersonaType>(proto_message.destination_persona_type());
  PersonaType source_persona_type = static_cast<PersonaType>(proto_message.source_persona_type());
  int data_type = proto_message.data_type();
  NodeId destination(proto_message.destination());
  NodeId source(proto_message.source());
  std::string content(proto_message.content());
  std::string signature(proto_message.signature());
  return (Message(action_type, destination_persona_type, source_persona_type, data_type,
                  destination, source, content, signature));
}

// throws
std::string SerialiseAsString(const Message& message) {
  proto::Nfs proto_message;
  proto_message.set_action_type(static_cast<int32_t>(message.action_type()));
  proto_message.set_destination_persona_type(
      static_cast<int32_t>(message.destination_persona_type()));
  proto_message.set_source_persona_type(static_cast<int32_t>(message.source_persona_type()));
  proto_message.set_data_type(message.data_type());
  proto_message.set_destination(message.destination().string());
  proto_message.set_source(message.source().string());
  proto_message.set_content(message.content());
  if (!message.signature().empty())
    proto_message.set_signature(message.signature());
  if (!proto_message.IsInitialized()) {
    ThrowError(CommonErrors::unknown);  // TODO(Prakash) : is new error type for this case?
  }
  return proto_message.SerializeAsString();
}

}  // namespace vault

}  // namespace maidsafe
