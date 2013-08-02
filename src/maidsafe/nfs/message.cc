/* Copyright 2012 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

#include "maidsafe/nfs/message.h"

#include "maidsafe/common/crypto.h"
#include "maidsafe/common/error.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/message.pb.h"
#include "maidsafe/nfs/utils.h"


namespace maidsafe {

namespace nfs {

namespace {

maidsafe_error GetError(int error_value, const std::string& error_category_name) {
  if (error_category_name == std::string(GetCommonCategory().name()))
    return MakeError(static_cast<CommonErrors>(error_value));
  if (error_category_name == std::string(GetAsymmCategory().name()))
    return MakeError(static_cast<AsymmErrors>(error_value));
  if (error_category_name == std::string(GetPassportCategory().name()))
    return MakeError(static_cast<PassportErrors>(error_value));
  if (error_category_name == std::string(GetNfsCategory().name()))
    return MakeError(static_cast<NfsErrors>(error_value));
  if (error_category_name == std::string(GetRoutingCategory().name()))
    return MakeError(static_cast<RoutingErrors>(error_value));
  if (error_category_name == std::string(GetVaultCategory().name()))
    return MakeError(static_cast<VaultErrors>(error_value));
  if (error_category_name == std::string(GetLifeStuffCategory().name()))
    return MakeError(static_cast<LifeStuffErrors>(error_value));

  ThrowError(CommonErrors::parsing_error);
  return MakeError(CommonErrors::parsing_error);
}

}  // unnamed namespace



Message::Data::Data()
    : type(),
      name(),
      originator(),
      content(),
      action(static_cast<MessageAction>(-1)),
      error() {}

Message::Data::Data(DataTagValue type_in,
                    const Identity& name_in,
                    const NonEmptyString& content_in,
                    MessageAction action_in)
    : type(type_in),
      name(name_in),
      originator(),
      content(content_in),
      action(action_in),
      error() {}

Message::Data::Data(DataTagValue type_in,
                    const Identity& name_in,
                    const Identity& originator_in,
                    const NonEmptyString& content_in,
                    MessageAction action_in)
    : type(type_in),
      name(name_in),
      originator(originator_in),
      content(content_in),
      action(action_in),
      error() {}

Message::Data::Data(const Identity& name_in,
                    const NonEmptyString& content_in,
                    MessageAction action_in)
    : type(),
      name(name_in),
      originator(),
      content(content_in),
      action(action_in),
      error() {}


Message::Data::Data(const Data& other)
    : type(other.type),
      name(other.name),
      originator(other.originator),
      content(other.content),
      action(other.action),
      error(other.error) {}

Message::Data& Message::Data::operator=(const Data& other) {
  type = other.type;
  name = other.name;
  originator = other.originator;
  content = other.content;
  action = other.action;
  error = other.error;
  return *this;
}

Message::Data::Data(Data&& other)
    : type(std::move(other.type)),
      name(std::move(other.name)),
      originator(std::move(other.originator)),
      content(std::move(other.content)),
      action(std::move(other.action)),
      error(std::move(other.error)) {}

Message::Data& Message::Data::operator=(Data&& other) {
  type = std::move(other.type);
  name = std::move(other.name);
  originator = std::move(other.originator);
  content = std::move(other.content);
  action = std::move(other.action);
  error = std::move(other.error);
  return *this;
}

bool Message::Data::IsSuccess() const {
  static std::error_code success((MakeError(CommonErrors::success)).code());
  return error && error->code() == success;
}

Message::ClientValidation::ClientValidation() : name(), data_signature() {}

Message::ClientValidation::ClientValidation(const passport::PublicMaid::name_type& name_in,
                                            const asymm::Signature& data_signature_in)
    : name(name_in),
      data_signature(data_signature_in) {}

Message::ClientValidation::ClientValidation(const ClientValidation& other)
    : name(other.name),
      data_signature(other.data_signature) {}

Message::ClientValidation& Message::ClientValidation::operator=(const ClientValidation& other) {
  name = other.name;
  data_signature = other.data_signature;
  return *this;
}

Message::ClientValidation::ClientValidation(ClientValidation&& other)
    : name(std::move(other.name)),
      data_signature(std::move(other.data_signature)) {}

Message::ClientValidation& Message::ClientValidation::operator=(ClientValidation&& other) {
  name = std::move(other.name);
  data_signature = std::move(other.data_signature);
  return *this;
}



Message::Message(Persona destination_persona,
                 Persona source_persona,
                 const Data& data,
                 const passport::PublicPmid::name_type& pmid_node)
    : message_id_(detail::GetNewMessageId()),
      destination_persona_(destination_persona),
      source_persona_(source_persona),
      data_(data),
      client_validation_(),
      pmid_node_(pmid_node) {
  if (!ValidateInputs())
    ThrowError(CommonErrors::invalid_parameter);
}

Message::Message(const Message& other)
    : message_id_(other.message_id_),
      destination_persona_(other.destination_persona_),
      source_persona_(other.source_persona_),
      data_(other.data_),
      client_validation_(other.client_validation_),
      pmid_node_(other.pmid_node_) {}

Message& Message::operator=(const Message& other) {
  message_id_ = other.message_id_;
  destination_persona_ = other.destination_persona_;
  source_persona_ = other.source_persona_;
  data_ = other.data_;
  client_validation_ = other.client_validation_;
  pmid_node_ = other.pmid_node_;
  return *this;
}

Message::Message(Message&& other)
    : message_id_(std::move(other.message_id_)),
      destination_persona_(std::move(other.destination_persona_)),
      source_persona_(std::move(other.source_persona_)),
      data_(std::move(other.data_)),
      client_validation_(std::move(other.client_validation_)),
      pmid_node_(std::move(other.pmid_node_)) {}

Message& Message::operator=(Message&& other) {
  message_id_ = std::move(other.message_id_);
  destination_persona_ = std::move(other.destination_persona_);
  source_persona_ = std::move(other.source_persona_);
  data_ = std::move(other.data_);
  client_validation_ = std::move(other.client_validation_);
  pmid_node_ = std::move(other.pmid_node_);
  return *this;
}

// TODO(Fraser#5#): 2012-12-24 - Once MSVC eventually handles delegating constructors, we can make
//                  this more efficient by using a lambda which returns the parsed protobuf
//                  inside a private constructor taking a single arg of type protobuf.
Message::Message(const serialised_type& serialised_message)
    : message_id_(),
      destination_persona_(),
      source_persona_(),
      data_(),
      client_validation_(),
      pmid_node_() {
  protobuf::Message proto_message;
  if (!proto_message.ParseFromString(serialised_message->string()))
    ThrowError(CommonErrors::parsing_error);

  message_id_ = MessageId(proto_message.message_id());
  destination_persona_ = static_cast<Persona>(proto_message.destination_persona());
  source_persona_ = static_cast<Persona>(proto_message.source_persona());

  auto& proto_data(proto_message.data());
  if (proto_data.has_type())
    data_.type = static_cast<DataTagValue>(proto_data.type());
  data_.name = Identity(proto_data.name());
  if (proto_data.has_content())
    data_.content = NonEmptyString(proto_data.content());
  data_.action = static_cast<MessageAction>(proto_data.action());
  if (proto_data.has_error()) {
    data_.error = GetError(proto_data.error().error_value(),
                           proto_data.error().error_category_name());
  }

  if (proto_message.has_client_validation()) {
    auto& proto_client_validation(proto_message.client_validation());
    client_validation_.name = Identity(proto_client_validation.name());
    client_validation_.data_signature = asymm::Signature(proto_client_validation.data_signature());
  }

  if (proto_message.has_pmid_node())
    pmid_node_ = passport::PublicPmid::name_type((Identity(proto_message.pmid_node())));

  if (!ValidateInputs())
    ThrowError(CommonErrors::invalid_parameter);
}

bool Message::ValidateInputs() const {
  return data_.type && data_.name.IsInitialised();
}

void Message::SignData(const Identity& client_name, const asymm::PrivateKey& signer_private_key) {
  protobuf::Message::Data data;
  if (data_.type)
    data.set_type(static_cast<uint32_t>(*data_.type));
  data.set_name(data_.name.string());
  if (data_.content.IsInitialised())
    data.set_content(data_.content.string());
  data.set_action(static_cast<int32_t>(data_.action));
  asymm::PlainText serialised_data(data.SerializeAsString());
  client_validation_.name = client_name;
  client_validation_.data_signature = asymm::Sign(serialised_data, signer_private_key);
}

Message::serialised_type Message::Serialise() const {
  serialised_type serialised_message;
  try {
    protobuf::Message proto_message;
    proto_message.set_message_id(message_id_.data);
    proto_message.set_destination_persona(static_cast<int32_t>(destination_persona_));
    proto_message.set_source_persona(static_cast<int32_t>(source_persona_));
    if (data_.type)
      proto_message.mutable_data()->set_type(static_cast<uint32_t>(*data_.type));
    proto_message.mutable_data()->set_name(data_.name.string());
    if (data_.content.IsInitialised())
      proto_message.mutable_data()->set_content(data_.content.string());
    proto_message.mutable_data()->set_action(static_cast<int32_t>(data_.action));
    if (data_.error) {
      proto_message.mutable_data()->mutable_error()->set_error_value(data_.error->code().value());
      proto_message.mutable_data()->mutable_error()->set_error_category_name(
          data_.error->code().category().name());
    }
    if (HasClientValidation()) {
      proto_message.mutable_client_validation()->set_name(client_validation_.name.string());
      proto_message.mutable_client_validation()->set_data_signature(
          client_validation_.data_signature.string());
    }
    if (HasDataHolder())
      proto_message.set_pmid_node(pmid_node_->string());
    serialised_message = serialised_type(NonEmptyString(proto_message.SerializeAsString()));
  }
  catch(const std::exception&) {
    ThrowError(CommonErrors::invalid_parameter);
  }
  return serialised_message;
}

std::pair<Message::serialised_type, asymm::Signature> Message::SerialiseAndSign(
    const asymm::PrivateKey& signer_private_key) const {
  auto serialised(Serialise());
  auto signature(asymm::Sign(asymm::PlainText(serialised.data), signer_private_key));
  return std::make_pair(serialised, signature);
}

bool Message::Validate(const asymm::Signature& signature,
                       const asymm::PublicKey& signer_public_key) const {
  asymm::PlainText serialised(Serialise().data);
  return asymm::CheckSignature(serialised, signature, signer_public_key);
}

}  // namespace nfs

}  // namespace maidsafe
