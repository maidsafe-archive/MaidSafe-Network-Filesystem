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

#include "maidsafe/nfs/data_message.h"

#include "maidsafe/common/error.h"

#include "maidsafe/nfs/message_pb.h"


namespace maidsafe {

namespace nfs {

DataMessage::DataMessage(ActionType action_type,
                         PersonaType destination_persona_type,
                         Source source,
                         maidsafe::detail::DataTagValue data_type,
                         const Identity& name,
                         const NonEmptyString& content,
                         const asymm::Signature& signature)
    : action_type_(action_type),
      destination_persona_type_(destination_persona_type),
      source_(source),
      data_type_(data_type),
      name_(name),
      content_(content),
      signature_(signature) {
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

DataMessage::DataMessage(const DataMessage& other)
    : action_type_(other.action_type_),
      destination_persona_type_(other.destination_persona_type_),
      source_(other.source_),
      data_type_(other.data_type_),
      name_(other.name_),
      content_(other.content_),
      signature_(other.signature_) {}

DataMessage& DataMessage::operator=(const DataMessage& other) {
  action_type_ = other.action_type_;
  destination_persona_type_ = other.destination_persona_type_;
  source_ = other.source_;
  data_type_ = other.data_type_;
  name_ = other.name_;
  content_ = other.content_;
  signature_ = other.signature_;
  return *this;
}

DataMessage::DataMessage(DataMessage&& other)
    : action_type_(std::move(other.action_type_)),
      destination_persona_type_(std::move(other.destination_persona_type_)),
      source_(std::move(other.source_)),
      data_type_(std::move(other.data_type_)),
      name_(std::move(other.name_)),
      content_(std::move(other.content_)),
      signature_(std::move(other.signature_)) {}

DataMessage& DataMessage::operator=(DataMessage&& other) {
  action_type_ = std::move(other.action_type_);
  destination_persona_type_ = std::move(other.destination_persona_type_);
  source_ = std::move(other.source_);
  data_type_ = std::move(other.data_type_);
  name_ = std::move(other.name_);
  content_ = std::move(other.content_);
  signature_ = std::move(other.signature_);
  return *this;
}

// TODO(Fraser#5#): 2012-12-24 - Once MSVC eventually handles delegating constructors, we can make
//                  this more efficient by using a lambda which returns the parsed protobuf
//                  inside a private constructor taking a single arg of type protobuf.
DataMessage::DataMessage(const serialised_type& serialised_message)
    : action_type_(),
      destination_persona_type_(),
      source_(),
      data_type_(),
      name_(),
      content_(),
      signature_() {
  protobuf::DataMessage proto_data_message;
  if (!proto_data_message.ParseFromString(serialised_message->string()))
    ThrowError(NfsErrors::message_parsing_error);
  action_type_ = static_cast<ActionType>(proto_data_message.action_type());
  destination_persona_type_ =
      static_cast<PersonaType>(proto_data_message.destination_persona_type());
  source_.persona_type = static_cast<PersonaType>(proto_data_message.source().persona_type());
  source_.node_id = NodeId(proto_data_message.source().node_id());
  data_type_ = static_cast<maidsafe::detail::DataTagValue>(proto_data_message.data_type());
  name_ = Identity(proto_data_message.name());
  if (proto_data_message.has_content())
    content_ = NonEmptyString(proto_data_message.content());
  if (proto_data_message.has_signature())
    signature_ = asymm::Signature(proto_data_message.signature());
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

bool DataMessage::ValidateInputs() const {
  return (static_cast<int32_t>(data_type_) >= 0) &&
         name_.IsInitialised() &&
         !source_.node_id.IsZero();
}

DataMessage::serialised_type DataMessage::Serialise() const {
  serialised_type serialised_message;
  try {
    protobuf::DataMessage proto_data_message;
    proto_data_message.set_action_type(static_cast<int32_t>(action_type_));
    proto_data_message.set_destination_persona_type(
        static_cast<int32_t>(destination_persona_type_));
    proto_data_message.mutable_source()->set_persona_type(
        static_cast<int32_t>(source_.persona_type));
    proto_data_message.mutable_source()->set_node_id(source_.node_id.string());
    proto_data_message.set_data_type(static_cast<int32_t>(data_type_));
    proto_data_message.set_name(name_.string());
    if (content_.IsInitialised())
      proto_data_message.set_content(content_.string());
    if (signature_.IsInitialised())
      proto_data_message.set_signature(signature_.string());

    serialised_message = serialised_type(NonEmptyString(proto_data_message.SerializeAsString()));
  }
  catch(const std::system_error&) {
    ThrowError(NfsErrors::invalid_parameter);
  }
  return serialised_message;
}

}  // namespace nfs

}  // namespace maidsafe
