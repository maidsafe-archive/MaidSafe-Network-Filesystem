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

#include "maidsafe/nfs/post_message.h"

#include "maidsafe/common/error.h"

#include "maidsafe/nfs/post_messages_pb.h"


namespace maidsafe {

namespace nfs {

PostMessage::PostMessage(PostActionType post_action_type,
                         PersonaType destination_persona_type,
                         DataMessage::Source source,
                         const Identity& name,
                         const NonEmptyString& content,
                         const asymm::Signature& signature)
    : post_action_type_(post_action_type),
      destination_persona_type_(destination_persona_type),
      source_(source),
      name_(name),
      content_(content),
      signature_(signature),
      message_id_(2) {  // FIXME get sequential message from a common function
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

PostMessage::PostMessage(const PostMessage& other)
    : post_action_type_(other.post_action_type_),
      destination_persona_type_(other.destination_persona_type_),
      source_(other.source_),
      name_(other.name_),
      content_(other.content_),
      signature_(other.signature_),
      message_id_(other.message_id_) {}

PostMessage& PostMessage::operator=(const PostMessage& other) {
  post_action_type_ = other.post_action_type_;
  destination_persona_type_ = other.destination_persona_type_;
  source_ = other.source_;
  name_ = other.name_;
  content_ = other.content_;
  signature_ = other.signature_;
  message_id_ = other.message_id_;
  return *this;
}

PostMessage::PostMessage(PostMessage&& other)
    : post_action_type_(std::move(other.post_action_type_)),
      destination_persona_type_(std::move(other.destination_persona_type_)),
      source_(std::move(other.source_)),
      name_(std::move(other.name_)),
      content_(std::move(other.content_)),
      signature_(std::move(other.signature_)),
      message_id_(std::move(other.message_id_)) {}

PostMessage& PostMessage::operator=(PostMessage&& other) {
  post_action_type_ = std::move(other.post_action_type_);
  destination_persona_type_ = std::move(other.destination_persona_type_);
  source_ = std::move(other.source_);
  name_ = std::move(other.name_);
  content_ = std::move(other.content_);
  signature_ = std::move(other.signature_);
  message_id_ = std::move(other.message_id_);
  return *this;
}

// TODO(Fraser#5#): 2012-12-24 - Once MSVC eventually handles delegating constructors, we can make
//                  this more efficient by using a lambda which returns the parsed protobuf
//                  inside a private constructor taking a single arg of type protobuf.
PostMessage::PostMessage(const serialised_type& serialised_message)
    : post_action_type_(),
      destination_persona_type_(),
      source_(),
      name_(),
      content_(),
      signature_(),
      message_id_() {
  protobuf::PostMessage_ proto_post_message;
  if (!proto_post_message.ParseFromString(serialised_message->string()))
    ThrowError(NfsErrors::message_parsing_error);
  post_action_type_ = static_cast<PostActionType>(proto_post_message.post_action_type());
  destination_persona_type_ =
      static_cast<PersonaType>(proto_post_message.destination_persona_type());
  source_.persona_type = static_cast<PersonaType>(proto_post_message.source().persona_type());
  source_.node_id = NodeId(proto_post_message.source().node_id());
  name_ = Identity(proto_post_message.name());
  content_ = NonEmptyString(proto_post_message.content());
  if (proto_post_message.has_signature())
    signature_ = asymm::Signature(proto_post_message.signature());
  message_id_ = proto_post_message.message_id();
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

bool PostMessage::ValidateInputs() const {
  return name_.IsInitialised() && !source_.node_id.IsZero();
}

PostMessage::serialised_type PostMessage::Serialise() const {
  serialised_type serialised_message;
  try {
    protobuf::PostMessage_ proto_post_message;
    proto_post_message.set_post_action_type(static_cast<int32_t>(post_action_type_));
    proto_post_message.set_destination_persona_type(
        static_cast<int32_t>(destination_persona_type_));
    proto_post_message.mutable_source()->set_persona_type(
        static_cast<int32_t>(source_.persona_type));
    proto_post_message.mutable_source()->set_node_id(source_.node_id.string());
    proto_post_message.set_name(name_.string());
    proto_post_message.set_content(content_.string());
    if (signature_.IsInitialised())
      proto_post_message.set_signature(signature_.string());
    proto_post_message.set_message_id(message_id_);
    serialised_message = serialised_type(NonEmptyString(proto_post_message.SerializeAsString()));
  }
  catch(const std::system_error&) {
    ThrowError(NfsErrors::invalid_parameter);
  }
  return serialised_message;
}

}  // namespace nfs

}  // namespace maidsafe
