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

#include "maidsafe/nfs/generic_message.h"

#include "maidsafe/common/error.h"

#include "maidsafe/nfs/generic_message_pb.h"
#include "maidsafe/nfs/utils.h"


namespace maidsafe {

namespace nfs {

const MessageCategory GenericMessage::message_type_identifier = MessageCategory::kGeneric;


GenericMessage::Batch::Batch() : size(-1), index(-1) {}

GenericMessage::Batch::Batch(int32_t size_in, int32_t index_in) : size(size_in), index(index_in) {}

GenericMessage::Batch::Batch(const Batch& other) : size(other.size), index(other.index) {}

GenericMessage::Batch& GenericMessage::Batch::operator=(const Batch& other) {
  size = other.size;
  index = other.index;
  return *this;
}

GenericMessage::Batch::Batch(Batch&& other)
    : size(std::move(other.size)),
      index(std::move(other.index)) {}

GenericMessage::Batch& GenericMessage::Batch::operator=(Batch&& other) {
  size = std::move(other.size);
  index = std::move(other.index);
  return *this;
}


GenericMessage::GenericMessage(Action action,
                               Persona destination_persona,
                               const PersonaId& source,
                               const Identity& name,
                               const NonEmptyString& content,
                               Batch batch)
    : message_id_(detail::GetNewMessageId(source.node_id)),
      batch_(batch),
      action_(action),
      destination_persona_(destination_persona),
      source_(source),
      name_(name),
      content_(content) {
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

GenericMessage::GenericMessage(const GenericMessage& other)
    : message_id_(other.message_id_),
      batch_(other.batch_),
      action_(other.action_),
      destination_persona_(other.destination_persona_),
      source_(other.source_),
      name_(other.name_),
      content_(other.content_) {}

GenericMessage& GenericMessage::operator=(const GenericMessage& other) {
  message_id_ = other.message_id_;
  batch_ = other.batch_;
  action_ = other.action_;
  destination_persona_ = other.destination_persona_;
  source_ = other.source_;
  name_ = other.name_;
  content_ = other.content_;
  return *this;
}

GenericMessage::GenericMessage(GenericMessage&& other)
    : message_id_(std::move(other.message_id_)),
      batch_(std::move(other.batch_)),
      action_(std::move(other.action_)),
      destination_persona_(std::move(other.destination_persona_)),
      source_(std::move(other.source_)),
      name_(std::move(other.name_)),
      content_(std::move(other.content_)) {}

GenericMessage& GenericMessage::operator=(GenericMessage&& other) {
  message_id_ = std::move(other.message_id_);
  batch_ = std::move(other.batch_);
  action_ = std::move(other.action_);
  destination_persona_ = std::move(other.destination_persona_);
  source_ = std::move(other.source_);
  name_ = std::move(other.name_);
  content_ = std::move(other.content_);
  return *this;
}

// TODO(Fraser#5#): 2012-12-24 - Once MSVC eventually handles delegating constructors, we can make
//                  this more efficient by using a lambda which returns the parsed protobuf
//                  inside a private constructor taking a single arg of type protobuf.
GenericMessage::GenericMessage(const serialised_type& serialised_message)
    : message_id_(),
      batch_(),
      action_(),
      destination_persona_(),
      source_(),
      name_(),
      content_() {
  protobuf::GenericMessage proto_generic_message;
  if (!proto_generic_message.ParseFromString(serialised_message->string()))
    ThrowError(CommonErrors::parsing_error);
  message_id_ = MessageId(Identity(proto_generic_message.message_id()));
  if (proto_generic_message.has_batch()) {
    batch_.size = proto_generic_message.batch().size();
    batch_.index = proto_generic_message.batch().index();
  }
  action_ = static_cast<GenericMessage::Action>(proto_generic_message.action());
  destination_persona_ = static_cast<Persona>(proto_generic_message.destination_persona());
  source_.persona = static_cast<Persona>(proto_generic_message.source().persona());
  source_.node_id = NodeId(proto_generic_message.source().node_id());
  name_ = Identity(proto_generic_message.name());
  if (proto_generic_message.has_content())
    content_ = NonEmptyString(proto_generic_message.content());
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

bool GenericMessage::ValidateInputs() const {
  if (batch_.size > 0 && batch_.index > 0 && batch_.size < batch_.index)
    return false;
  return name_.IsInitialised() && !source_.node_id.IsZero();
}

GenericMessage::serialised_type GenericMessage::Serialise() const {
  serialised_type serialised_message;
  try {
    protobuf::GenericMessage proto_generic_message;
    proto_generic_message.set_message_id(message_id_->string());
    if (batch_.size > 0) {
      proto_generic_message.mutable_batch()->set_size(batch_.size);
      proto_generic_message.mutable_batch()->set_index(batch_.index);
    }
    proto_generic_message.set_action(static_cast<int32_t>(action_));
    proto_generic_message.set_destination_persona(static_cast<int32_t>(destination_persona_));
    proto_generic_message.mutable_source()->set_persona(
        static_cast<int32_t>(source_.persona));
    proto_generic_message.mutable_source()->set_node_id(source_.node_id.string());
    proto_generic_message.set_name(name_.string());
    if (content_.IsInitialised())
      proto_generic_message.set_content(content_.string());
    serialised_message = serialised_type(NonEmptyString(proto_generic_message.SerializeAsString()));
  }
  catch(const std::exception&) {
    ThrowError(NfsErrors::invalid_parameter);
  }
  return serialised_message;
}

}  // namespace nfs

}  // namespace maidsafe
