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

#include "maidsafe/common/crypto.h"
#include "maidsafe/common/error.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/data_message_pb.h"
#include "maidsafe/nfs/utils.h"


namespace maidsafe {

namespace nfs {

const MessageCategory DataMessage::message_type_identifier = MessageCategory::kData;


DataMessage::Data::Data()
    : type(static_cast<DataTagValue>(-1)),
      name(),
      content(),
      action(static_cast<Action>(-1)) {}

DataMessage::Data::Data(DataTagValue type_in,
                        const Identity& name_in,
                        const NonEmptyString& content_in,
                        Action action_in)
    : type(type_in),
      name(name_in),
      content(content_in),
      action(action_in) {}

DataMessage::Data::Data(const Data& other)
    : type(other.type),
      name(other.name),
      content(other.content),
      action(other.action) {}

DataMessage::Data& DataMessage::Data::operator=(const Data& other) {
  type = other.type;
  name = other.name;
  content = other.content;
  action = other.action;
  return *this;
}

DataMessage::Data::Data(Data&& other)
    : type(std::move(other.type)),
      name(std::move(other.name)),
      content(std::move(other.content)),
      action(std::move(other.action)) {}

DataMessage::Data& DataMessage::Data::operator=(Data&& other) {
  type = std::move(other.type);
  name = std::move(other.name);
  content = std::move(other.content);
  action = std::move(other.action);
  return *this;
}



DataMessage::Originator::Originator() : name(), data_signature() {}

DataMessage::Originator::Originator(const passport::PublicMaid::name_type& name_in,
                                    const asymm::Signature& data_signature_in)
    : name(name_in),
      data_signature(data_signature_in) {}

DataMessage::Originator::Originator(const Originator& other)
    : name(other.name),
      data_signature(other.data_signature) {}

DataMessage::Originator& DataMessage::Originator::operator=(const Originator& other) {
  name = other.name;
  data_signature = other.data_signature;
  return *this;
}

DataMessage::Originator::Originator(Originator&& other)
    : name(std::move(other.name)),
      data_signature(std::move(other.data_signature)) {}

DataMessage::Originator& DataMessage::Originator::operator=(Originator&& other) {
  name = std::move(other.name);
  data_signature = std::move(other.data_signature);
  return *this;
}



DataMessage::DataMessage(Persona next_persona,
                         const PersonaId& this_persona,
                         const Data& data,
                         const passport::PublicPmid::name_type& data_holder_hint,
                         const Identity& target_id)
    : message_id_(detail::GetNewMessageId(this_persona.node_id)),
      next_persona_(next_persona),
      this_persona_(this_persona),
      data_(data),
      originator_(),
      data_holder_hint_(data_holder_hint),
      target_id_(target_id) {
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

DataMessage::DataMessage(const DataMessage& other)
    : message_id_(other.message_id_),
      next_persona_(other.next_persona_),
      this_persona_(other.this_persona_),
      data_(other.data_),
      originator_(other.originator_),
      data_holder_hint_(other.data_holder_hint_),
      target_id_(other.target_id_) {}

DataMessage& DataMessage::operator=(const DataMessage& other) {
  message_id_ = other.message_id_;
  next_persona_ = other.next_persona_;
  this_persona_ = other.this_persona_;
  data_ = other.data_;
  originator_ = other.originator_;
  data_holder_hint_ = other.data_holder_hint_;
  target_id_ = other.target_id_;
  return *this;
}

DataMessage::DataMessage(DataMessage&& other)
    : message_id_(std::move(other.message_id_)),
      next_persona_(std::move(other.next_persona_)),
      this_persona_(std::move(other.this_persona_)),
      data_(std::move(other.data_)),
      originator_(std::move(other.originator_)),
      data_holder_hint_(std::move(other.data_holder_hint_)),
      target_id_(std::move(other.target_id_)) {}

DataMessage& DataMessage::operator=(DataMessage&& other) {
  message_id_ = std::move(other.message_id_);
  next_persona_ = std::move(other.next_persona_);
  this_persona_ = std::move(other.this_persona_);
  data_ = std::move(other.data_);
  originator_ = std::move(other.originator_);
  data_holder_hint_ = std::move(other.data_holder_hint_);
  target_id_ = std::move(other.target_id_);
  return *this;
}

// TODO(Fraser#5#): 2012-12-24 - Once MSVC eventually handles delegating constructors, we can make
//                  this more efficient by using a lambda which returns the parsed protobuf
//                  inside a private constructor taking a single arg of type protobuf.
DataMessage::DataMessage(const serialised_type& serialised_message)
    : message_id_(),
      next_persona_(),
      this_persona_(),
      data_(),
      originator_(),
      data_holder_hint_(),
      target_id_() {
  protobuf::DataMessage proto_data_message;
  if (!proto_data_message.ParseFromString(serialised_message->string()))
    ThrowError(CommonErrors::parsing_error);

  message_id_ = MessageId(Identity(proto_data_message.message_id()));
  next_persona_ = static_cast<Persona>(proto_data_message.next_persona());
  this_persona_.persona = static_cast<Persona>(proto_data_message.this_persona().persona());
  this_persona_.node_id = NodeId(proto_data_message.this_persona().node_id());

  auto& proto_data(proto_data_message.data());
  data_.type = static_cast<DataTagValue>(proto_data.type());
  data_.name = Identity(proto_data.name());
  if (proto_data.has_content())
    data_.content = NonEmptyString(proto_data.content());
  data_.action = static_cast<Action>(proto_data.action());

  if (proto_data_message.has_originator()) {
    auto& proto_originator(proto_data_message.originator());
    originator_.name = Identity(proto_originator.name());
    originator_.data_signature = asymm::Signature(proto_originator.data_signature());
  }

  if (proto_data_message.has_data_holder_hint()) {
    data_holder_hint_ =
        passport::PublicPmid::name_type((Identity(proto_data_message.data_holder_hint())));
  }

  if (proto_data_message.has_target_id()) {
    target_id_ = Identity(proto_data_message.target_id());
  }

  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

bool DataMessage::ValidateInputs() const {
  return (static_cast<int32_t>(data_.type) >= 0) &&
         data_.name.IsInitialised() &&
         !this_persona_.node_id.IsZero();
}

void DataMessage::SignData(const asymm::PrivateKey& signer_private_key) {
  protobuf::DataMessage::Data data;
  data.set_type(static_cast<int32_t>(data_.type));
  data.set_name(data_.name.string());
  if (data_.content.IsInitialised())
    data.set_content(data_.content.string());
  data.set_action(static_cast<int32_t>(data_.action));
  asymm::PlainText serialised_data(data.SerializeAsString());
  originator_.name = Identity(this_persona_.node_id.string());
  originator_.data_signature = asymm::Sign(serialised_data, signer_private_key);
}

DataMessage::serialised_type DataMessage::Serialise() const {
  serialised_type serialised_message;
  try {
    protobuf::DataMessage proto_data_message;
    proto_data_message.set_message_id(message_id_->string());
    proto_data_message.set_next_persona(static_cast<int32_t>(next_persona_));
    proto_data_message.mutable_this_persona()->set_persona(
        static_cast<int32_t>(this_persona_.persona));
    proto_data_message.mutable_this_persona()->set_node_id(this_persona_.node_id.string());
    proto_data_message.mutable_data()->set_type(static_cast<int32_t>(data_.type));
    proto_data_message.mutable_data()->set_name(data_.name.string());
    if (data_.content.IsInitialised())
      proto_data_message.mutable_data()->set_content(data_.content.string());
    proto_data_message.mutable_data()->set_action(static_cast<int32_t>(data_.action));
    if (HasOriginator()) {
      proto_data_message.mutable_originator()->set_name(originator_.name.string());
      proto_data_message.mutable_originator()->set_data_signature(
          originator_.data_signature.string());
    }
    if (HasDataHolderHint())
      proto_data_message.set_data_holder_hint(data_holder_hint_->string());
    if (HasTargetId())
      proto_data_message.set_target_id(target_id_.string());
    serialised_message = serialised_type(NonEmptyString(proto_data_message.SerializeAsString()));
  }
  catch(const std::exception&) {
    ThrowError(NfsErrors::invalid_parameter);
  }
  return serialised_message;
}

std::pair<DataMessage::serialised_type, asymm::Signature> DataMessage::SerialiseAndSign(
    const asymm::PrivateKey& signer_private_key) const {
  auto serialised(Serialise());
  auto signature(asymm::Sign(asymm::PlainText(serialised.data), signer_private_key));
  return std::make_pair(serialised, signature);
}

bool DataMessage::Validate(const asymm::Signature& signature,
                           const asymm::PublicKey& signer_public_key) const {
  asymm::PlainText serialised(Serialise().data);
  return asymm::CheckSignature(serialised, signature, signer_public_key);
}

}  // namespace nfs

}  // namespace maidsafe
