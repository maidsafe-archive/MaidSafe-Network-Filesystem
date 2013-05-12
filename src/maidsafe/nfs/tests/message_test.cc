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

#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"
#include "maidsafe/data_types/data_type_values.h"

#include "maidsafe/nfs/message.pb.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace test {

namespace {

MessageAction GenerateAction() {
  return static_cast<MessageAction>(RandomUint32() % 3);
}

PersonaId GenerateSource() {
  PersonaId source;
  // matches Persona enum in types.h
  source.persona = static_cast<Persona>(RandomUint32() % 7);
  source.node_id = NodeId(NodeId::kRandomId);
  return source;
}

}  // unnamed namespace

class MessageTest : public testing::Test {
 protected:
  MessageTest()
      : action_(GenerateAction()),
        destination_persona_(static_cast<Persona>(RandomUint32() % 7)),
        source_(GenerateSource()),
        data_type_(static_cast<DataTagValue>(RandomUint32() % 13)),
        name_(RandomString(NodeId::kSize)),
        content_(RandomString(1 + RandomUint32() % 50)),
        data_holder_(passport::PublicPmid::name_type(Identity(RandomString(NodeId::kSize)))),
        message_(destination_persona_, source_,
                      Message::Data(data_type_, name_, content_, action_), data_holder_) {}

  MessageAction action_;
  Persona destination_persona_;
  PersonaId source_;
  Message::Data data_;
  DataTagValue data_type_;
  Identity name_;
  NonEmptyString content_;
  passport::PublicPmid::name_type data_holder_;
  Message message_;
};

TEST_F(MessageTest, BEH_CheckGetters) {
  EXPECT_EQ(action_, message_.data().action);
  EXPECT_EQ(destination_persona_, message_.destination_persona());
  EXPECT_EQ(source_.persona, message_.source().persona);
  EXPECT_EQ(source_.node_id, message_.source().node_id);
  EXPECT_EQ(data_type_, message_.data().type);
  EXPECT_EQ(name_, message_.data().name);
  EXPECT_EQ(content_, message_.data().content);
  EXPECT_EQ(data_holder_, message_.data_holder());
}

TEST_F(MessageTest, BEH_SerialiseThenParse) {
  auto serialised_message(message_.Serialise());
  Message recovered_message(serialised_message);

  EXPECT_EQ(action_, recovered_message.data().action);
  EXPECT_EQ(destination_persona_, recovered_message.destination_persona());
  EXPECT_EQ(source_.persona, recovered_message.source().persona);
  EXPECT_EQ(source_.node_id, recovered_message.source().node_id);
  EXPECT_EQ(data_type_, recovered_message.data().type);
  EXPECT_EQ(name_, recovered_message.data().name);
  EXPECT_EQ(content_, recovered_message.data().content);
  EXPECT_EQ(data_holder_, recovered_message.data_holder());
}

TEST_F(MessageTest, BEH_SerialiseParseReserialiseReparse) {
  auto serialised_message(message_.Serialise());
  Message recovered_message(serialised_message);

  auto serialised_message2(recovered_message.Serialise());
  Message recovered_message2(serialised_message2);

  EXPECT_EQ(serialised_message, serialised_message2);
  EXPECT_EQ(action_, recovered_message2.data().action);
  EXPECT_EQ(destination_persona_, recovered_message2.destination_persona());
  EXPECT_EQ(source_.persona, recovered_message2.source().persona);
  EXPECT_EQ(source_.node_id, recovered_message2.source().node_id);
  EXPECT_EQ(data_type_, recovered_message2.data().type);
  EXPECT_EQ(name_, recovered_message2.data().name);
  EXPECT_EQ(content_, recovered_message2.data().content);
  EXPECT_EQ(data_holder_, recovered_message2.data_holder());
}

TEST_F(MessageTest, BEH_AssignMessage) {
  Message message2 = message_;

  EXPECT_EQ(action_, message2.data().action);
  EXPECT_EQ(destination_persona_, message2.destination_persona());
  EXPECT_EQ(source_.persona, message2.source().persona);
  EXPECT_EQ(source_.node_id, message2.source().node_id);
  EXPECT_EQ(data_type_, message2.data().type);
  EXPECT_EQ(name_, message2.data().name);
  EXPECT_EQ(content_, message2.data().content);
  EXPECT_EQ(data_holder_, message2.data_holder());

  Message message3(message_);

  EXPECT_EQ(action_, message3.data().action);
  EXPECT_EQ(destination_persona_, message3.destination_persona());
  EXPECT_EQ(source_.persona, message3.source().persona);
  EXPECT_EQ(source_.node_id, message3.source().node_id);
  EXPECT_EQ(data_type_, message3.data().type);
  EXPECT_EQ(name_, message3.data().name);
  EXPECT_EQ(content_, message3.data().content);
  EXPECT_EQ(data_holder_, message3.data_holder());

  Message message4 = std::move(message3);

  EXPECT_EQ(action_, message4.data().action);
  EXPECT_EQ(destination_persona_, message4.destination_persona());
  EXPECT_EQ(source_.persona, message4.source().persona);
  EXPECT_EQ(source_.node_id, message4.source().node_id);
  EXPECT_EQ(data_type_, message4.data().type);
  EXPECT_EQ(name_, message4.data().name);
  EXPECT_EQ(content_, message4.data().content);
  EXPECT_EQ(data_holder_, message4.data_holder());

  Message message5(std::move(message4));

  EXPECT_EQ(action_, message5.data().action);
  EXPECT_EQ(destination_persona_, message5.destination_persona());
  EXPECT_EQ(source_.persona, message5.source().persona);
  EXPECT_EQ(source_.node_id, message5.source().node_id);
  EXPECT_EQ(data_type_, message5.data().type);
  EXPECT_EQ(name_, message5.data().name);
  EXPECT_EQ(content_, message5.data().content);
  EXPECT_EQ(data_holder_, message5.data_holder());
}

TEST_F(MessageTest, BEH_InvalidDataType) {
  int32_t bad_data_type(RandomInt32());
  while (bad_data_type == 0)
    bad_data_type = RandomInt32();
  if (bad_data_type > 0)
    bad_data_type = -bad_data_type;
  EXPECT_THROW(Message(destination_persona_, source_,
                           Message::Data(static_cast<DataTagValue>(bad_data_type),
                                             name_, content_, action_)),
               nfs_error);
}

TEST_F(MessageTest, BEH_DefaultValues) {
  Message message(destination_persona_, source_, message_.data());
  EXPECT_EQ(action_, message.data().action);
  EXPECT_EQ(destination_persona_, message.destination_persona());
  EXPECT_EQ(source_.persona, message.source().persona);
  EXPECT_EQ(source_.node_id, message.source().node_id);
  EXPECT_EQ(data_type_, message.data().type);
  EXPECT_EQ(name_, message.data().name);
  EXPECT_EQ(content_, message.data().content);
  EXPECT_FALSE(message.HasDataHolder());
}

TEST_F(MessageTest, BEH_InvalidSource) {
  PersonaId bad_source(GenerateSource());
  bad_source.node_id = NodeId();
  EXPECT_THROW(Message(destination_persona_, bad_source, message_.data()),
               nfs_error);
}

TEST_F(MessageTest, BEH_InvalidName) {
  EXPECT_THROW(Message(destination_persona_, source_,
                           Message::Data(data_type_, Identity(), content_, action_)),
               nfs_error);
}

TEST_F(MessageTest, BEH_SignData) {
  EXPECT_FALSE(message_.client_validation().name.IsInitialised());
  EXPECT_FALSE(message_.client_validation().data_signature.IsInitialised());
  EXPECT_FALSE(message_.HasClientValidation());
  asymm::Keys keys(asymm::GenerateKeyPair());

  Message::Data stored_data(message_.data());
  protobuf::Message::Data data;
  data.set_type(static_cast<int32_t>(stored_data.type));
  data.set_name(stored_data.name.string());
  if (stored_data.content.IsInitialised())
    data.set_content(stored_data.content.string());
  data.set_action(static_cast<int32_t>(stored_data.action));
  asymm::PlainText serialised_data(data.SerializeAsString());

  message_.SignData(keys.private_key);
  EXPECT_TRUE(message_.client_validation().name.IsInitialised());
  EXPECT_TRUE(message_.client_validation().data_signature.IsInitialised());
  Message::ClientValidation client_validation(message_.client_validation());
  EXPECT_EQ(message_.source().node_id.string(), client_validation.name.string());
  EXPECT_TRUE(asymm::CheckSignature(serialised_data, client_validation.data_signature,
                                    keys.public_key));
  EXPECT_TRUE(message_.HasClientValidation());
}

TEST_F(MessageTest, BEH_SerialiseAndSignThenValidate) {
  Message::serialised_type serialised_message(message_.Serialise());
  asymm::Keys keys(asymm::GenerateKeyPair());
  auto result(message_.SerialiseAndSign(keys.private_key));
  EXPECT_EQ(serialised_message.data.string(), result.first.data.string());
  asymm::PlainText serialised_data(result.first.data.string());
  EXPECT_TRUE(asymm::CheckSignature(serialised_data, result.second, keys.public_key));
  serialised_data = asymm::PlainText(serialised_message.data.string());
  EXPECT_TRUE(asymm::CheckSignature(serialised_data, result.second, keys.public_key));

  EXPECT_TRUE(message_.Validate(result.second, keys.public_key));
  EXPECT_FALSE(message_.Validate(asymm::Signature(RandomString(256)), keys.public_key));
  asymm::Keys false_keys(asymm::GenerateKeyPair());
  EXPECT_FALSE(message_.Validate(asymm::Signature(result.second), false_keys.public_key));
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
