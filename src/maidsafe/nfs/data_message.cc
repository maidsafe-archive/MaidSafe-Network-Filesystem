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



DataMessage::ClientValidation::ClientValidation() : name(), data_signature() {}

DataMessage::ClientValidation::ClientValidation(const passport::PublicMaid::name_type& name_in,
                                                const asymm::Signature& data_signature_in)
    : name(name_in),
      data_signature(data_signature_in) {}

DataMessage::ClientValidation::ClientValidation(const ClientValidation& other)
    : name(other.name),
      data_signature(other.data_signature) {}

DataMessage::ClientValidation& DataMessage::ClientValidation::operator=(
    const ClientValidation& other) {
  name = other.name;
  data_signature = other.data_signature;
  return *this;
}

DataMessage::ClientValidation::ClientValidation(ClientValidation&& other)
    : name(std::move(other.name)),
      data_signature(std::move(other.data_signature)) {}

DataMessage::ClientValidation& DataMessage::ClientValidation::operator=(ClientValidation&& other) {
  name = std::move(other.name);
  data_signature = std::move(other.data_signature);
  return *this;
}



DataMessage::DataMessage(Persona destination_persona,
                         const PersonaId& source,
                         const Data& data,
                         const passport::PublicPmid::name_type& data_holder)
    : message_id_(detail::GetNewMessageId(source.node_id)),
      destination_persona_(destination_persona),
      source_(source),
      data_(data),
      client_validation_(),
      data_holder_(data_holder) {
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

DataMessage::DataMessage(const DataMessage& other)
    : message_id_(other.message_id_),
      destination_persona_(other.destination_persona_),
      source_(other.source_),
      data_(other.data_),
      client_validation_(other.client_validation_),
      data_holder_(other.data_holder_) {}

DataMessage& DataMessage::operator=(const DataMessage& other) {
  message_id_ = other.message_id_;
  destination_persona_ = other.destination_persona_;
  source_ = other.source_;
  data_ = other.data_;
  client_validation_ = other.client_validation_;
  data_holder_ = other.data_holder_;
  return *this;
}

DataMessage::DataMessage(DataMessage&& other)
    : message_id_(std::move(other.message_id_)),
      destination_persona_(std::move(other.destination_persona_)),
      source_(std::move(other.source_)),
      data_(std::move(other.data_)),
      client_validation_(std::move(other.client_validation_)),
      data_holder_(std::move(other.data_holder_)) {}

DataMessage& DataMessage::operator=(DataMessage&& other) {
  message_id_ = std::move(other.message_id_);
  destination_persona_ = std::move(other.destination_persona_);
  source_ = std::move(other.source_);
  data_ = std::move(other.data_);
  client_validation_ = std::move(other.client_validation_);
  data_holder_ = std::move(other.data_holder_);
  return *this;
}

// TODO(Fraser#5#): 2012-12-24 - Once MSVC eventually handles delegating constructors, we can make
//                  this more efficient by using a lambda which returns the parsed protobuf
//                  inside a private constructor taking a single arg of type protobuf.
DataMessage::DataMessage(const serialised_type& serialised_message)
    : message_id_(),
      destination_persona_(),
      source_(),
      data_(),
      client_validation_(),
      data_holder_() {
  protobuf::DataMessage proto_data_message;
  if (!proto_data_message.ParseFromString(serialised_message->string()))
    ThrowError(CommonErrors::parsing_error);

  message_id_ = MessageId(Identity(proto_data_message.message_id()));
  destination_persona_ = static_cast<Persona>(proto_data_message.destination_persona());
  source_.persona = static_cast<Persona>(proto_data_message.source().persona());
  source_.node_id = NodeId(proto_data_message.source().node_id());

  auto& proto_data(proto_data_message.data());
  data_.type = static_cast<DataTagValue>(proto_data.type());
  data_.name = Identity(proto_data.name());
  if (proto_data.has_content())
    data_.content = NonEmptyString(proto_data.content());
  data_.action = static_cast<Action>(proto_data.action());

  if (proto_data_message.has_client_validation()) {
    auto& proto_client_validation(proto_data_message.client_validation());
    client_validation_.name = Identity(proto_client_validation.name());
    client_validation_.data_signature = asymm::Signature(proto_client_validation.data_signature());
  }

  if (proto_data_message.has_data_holder())
    data_holder_ = passport::PublicPmid::name_type((Identity(proto_data_message.data_holder())));

  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

bool DataMessage::ValidateInputs() const {
  return (static_cast<int32_t>(data_.type) >= 0) &&
         data_.name.IsInitialised() &&
         !source_.node_id.IsZero();
}

void DataMessage::SignData(const asymm::PrivateKey& signer_private_key) {
  protobuf::DataMessage::Data data;
  data.set_type(static_cast<int32_t>(data_.type));
  data.set_name(data_.name.string());
  if (data_.content.IsInitialised())
    data.set_content(data_.content.string());
  data.set_action(static_cast<int32_t>(data_.action));
  asymm::PlainText serialised_data(data.SerializeAsString());
  client_validation_.name = Identity(source_.node_id.string());
  client_validation_.data_signature = asymm::Sign(serialised_data, signer_private_key);
}

DataMessage::serialised_type DataMessage::Serialise() const {
  serialised_type serialised_message;
  try {
    protobuf::DataMessage proto_data_message;
    proto_data_message.set_message_id(message_id_->string());
    proto_data_message.set_destination_persona(static_cast<int32_t>(destination_persona_));
    proto_data_message.mutable_source()->set_persona(
        static_cast<int32_t>(source_.persona));
    proto_data_message.mutable_source()->set_node_id(source_.node_id.string());
    proto_data_message.mutable_data()->set_type(static_cast<int32_t>(data_.type));
    proto_data_message.mutable_data()->set_name(data_.name.string());
    if (data_.content.IsInitialised())
      proto_data_message.mutable_data()->set_content(data_.content.string());
    proto_data_message.mutable_data()->set_action(static_cast<int32_t>(data_.action));
    if (HasClientValidation()) {
      proto_data_message.mutable_client_validation()->set_name(client_validation_.name.string());
      proto_data_message.mutable_client_validation()->set_data_signature(
          client_validation_.data_signature.string());
    }
    if (HasDataHolder())
      proto_data_message.set_data_holder(data_holder_->string());
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
