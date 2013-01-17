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

DataMessage::Data::Data()
    : type(static_cast<maidsafe::detail::DataTagValue>(-1)),
      name(),
      content(),
      version(NoVersion()) {}

DataMessage::Data::Data(maidsafe::detail::DataTagValue type_in,
                        const Identity& name_in,
                        const NonEmptyString& content_in,
                        int32_t version_in)
    : type(type_in),
      name(name_in),
      content(content_in),
      version(version_in) {}

DataMessage::Data::Data(const Data& other)
    : type(other.type),
      name(other.name),
      content(other.content),
      version(other.version) {}

DataMessage::Data& DataMessage::Data::operator=(const Data& other) {
  type = other.type;
  name = other.name;
  content = other.content;
  version = other.version;
  return *this;
}

DataMessage::Data::Data(Data&& other)
    : type(std::move(other.type)),
      name(std::move(other.name)),
      content(std::move(other.content)),
      version(std::move(other.version)) {}

DataMessage::Data& DataMessage::Data::operator=(Data&& other) {
  type = std::move(other.type);
  name = std::move(other.name);
  content = std::move(other.content);
  version = std::move(other.version);
  return *this;
}



DataMessage::DataMessage(ActionType action_type,
                         PersonaType destination_persona_type,
                         const MessageSource& source,
                         const Data& data)
    : message_id_(detail::GetNewMessageId(source.node_id)),
      action_type_(action_type),
      destination_persona_type_(destination_persona_type),
      source_(source),
      data_(data) {
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

DataMessage::DataMessage(const DataMessage& other)
    : message_id_(other.message_id_),
      action_type_(other.action_type_),
      destination_persona_type_(other.destination_persona_type_),
      source_(other.source_),
      data_(other.data_) {}

DataMessage& DataMessage::operator=(const DataMessage& other) {
  message_id_ = other.message_id_;
  action_type_ = other.action_type_;
  destination_persona_type_ = other.destination_persona_type_;
  source_ = other.source_;
  data_ = other.data_;
  return *this;
}

DataMessage::DataMessage(DataMessage&& other)
    : message_id_(std::move(other.message_id_)),
      action_type_(std::move(other.action_type_)),
      destination_persona_type_(std::move(other.destination_persona_type_)),
      source_(std::move(other.source_)),
      data_(std::move(other.data_)) {}

DataMessage& DataMessage::operator=(DataMessage&& other) {
  message_id_ = std::move(other.message_id_);
  action_type_ = std::move(other.action_type_);
  destination_persona_type_ = std::move(other.destination_persona_type_);
  source_ = std::move(other.source_);
  data_ = std::move(other.data_);
  return *this;
}

// TODO(Fraser#5#): 2012-12-24 - Once MSVC eventually handles delegating constructors, we can make
//                  this more efficient by using a lambda which returns the parsed protobuf
//                  inside a private constructor taking a single arg of type protobuf.
DataMessage::DataMessage(const serialised_type& serialised_message)
    : message_id_(),
      action_type_(),
      destination_persona_type_(),
      source_(),
      data_() {
  protobuf::DataMessage proto_data_message;
  if (!proto_data_message.ParseFromString(serialised_message->string()))
    ThrowError(CommonErrors::parsing_error);

  message_id_ = MessageId(Identity(proto_data_message.message_id()));
  action_type_ = static_cast<ActionType>(proto_data_message.action_type());
  destination_persona_type_ =
      static_cast<PersonaType>(proto_data_message.destination_persona_type());
  source_.persona_type = static_cast<PersonaType>(proto_data_message.source().persona_type());
  source_.node_id = NodeId(proto_data_message.source().node_id());

  auto& data(proto_data_message.data());
  data_.type = static_cast<maidsafe::detail::DataTagValue>(data.type());
  data_.name = Identity(data.name());
  if (data.has_content())
    data_.content = NonEmptyString(data.content());
  if (data.has_version())
    data_.version = data.version();

  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

bool DataMessage::ValidateInputs() const {
  return (static_cast<int32_t>(data_.type) >= 0) &&
         data_.name.IsInitialised() &&
         !source_.node_id.IsZero();
}

DataMessage::serialised_type DataMessage::Serialise() const {
  serialised_type serialised_message;
  try {
    protobuf::DataMessage proto_data_message;
    proto_data_message.set_message_id(message_id_->string());
    proto_data_message.set_action_type(static_cast<int32_t>(action_type_));
    proto_data_message.set_destination_persona_type(
        static_cast<int32_t>(destination_persona_type_));
    proto_data_message.mutable_source()->set_persona_type(
        static_cast<int32_t>(source_.persona_type));
    proto_data_message.mutable_source()->set_node_id(source_.node_id.string());
    proto_data_message.mutable_data()->set_type(static_cast<int32_t>(data_.type));
    proto_data_message.mutable_data()->set_name(data_.name.string());
    if (data_.content.IsInitialised())
      proto_data_message.mutable_data()->set_content(data_.content.string());
    if (data_.version != Data::NoVersion())
      proto_data_message.mutable_data()->set_version(data_.version);
    serialised_message = serialised_type(NonEmptyString(proto_data_message.SerializeAsString()));
  }
  catch(const std::system_error&) {
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
