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

#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/detail/data_type_values.h"

#include "maidsafe/nfs/message.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace test {

ActionType GenerateActionType() {
  return static_cast<ActionType>(RandomUint32() % 4);  // matches ActionType enum in types.h
}

PersonaType GeneratePersonaType() {
  return static_cast<PersonaType>(RandomUint32() % 4);  // matches PersonaType enum in types.h
}

class MessageTest : public testing::Test {
 protected:
  MessageTest()
      : action_type_(GenerateActionType()),
        dest_persona_type_(GeneratePersonaType()),
        src_persona_type_(GeneratePersonaType()),
        data_type_(static_cast<detail::DataTagValue>(RandomUint32() % 13)),
        dest_id_(NodeId(NodeId::kRandomId)),
        src_id_(NodeId(NodeId::kRandomId)),
        content_(RandomString(1 + RandomUint32() % 50)),
        signature_(RandomString(1 + RandomUint32() % 50)),
        message_(action_type_, dest_persona_type_, src_persona_type_, data_type_, dest_id_,
                 src_id_, content_, signature_) {}

  ActionType action_type_;
  PersonaType dest_persona_type_;
  PersonaType src_persona_type_;
  detail::DataTagValue data_type_;
  NodeId dest_id_;
  NodeId src_id_;
  NonEmptyString content_;
  asymm::Signature signature_;
  Message message_;
};

TEST_F(MessageTest, BEH_CheckGetters) {
  EXPECT_EQ(action_type_, message_.action_type());
  EXPECT_EQ(dest_persona_type_, message_.destination_persona_type());
  EXPECT_EQ(src_persona_type_, message_.source_persona_type());
  EXPECT_EQ(data_type_, message_.data_type());
  EXPECT_EQ(dest_id_, message_.destination());
  EXPECT_EQ(src_id_, message_.source());
  EXPECT_EQ(content_, message_.content());
  EXPECT_EQ(signature_, message_.signature());
}

TEST_F(MessageTest, BEH_SerialiseThenParse) {
  auto serialised_message(message_.Serialise());
  Message recovered_message(serialised_message);

  EXPECT_EQ(action_type_, recovered_message.action_type());
  EXPECT_EQ(dest_persona_type_, recovered_message.destination_persona_type());
  EXPECT_EQ(src_persona_type_, recovered_message.source_persona_type());
  EXPECT_EQ(data_type_, recovered_message.data_type());
  EXPECT_EQ(dest_id_, recovered_message.destination());
  EXPECT_EQ(src_id_, recovered_message.source());
  EXPECT_EQ(content_, recovered_message.content());
  EXPECT_EQ(signature_, recovered_message.signature());
}

TEST_F(MessageTest, BEH_ParseFromBadString) {
  std::vector<Message::serialised_type> bad_strings;
  bad_strings.push_back(Message::serialised_type(
      NonEmptyString(RandomString(1 + RandomUint32() % 100))));
  bad_strings.push_back(Message::serialised_type());
  auto good_string(message_.Serialise());
  bad_strings.push_back(Message::serialised_type(
      good_string.data + NonEmptyString(RandomString(1))));
  bad_strings.push_back(Message::serialised_type(
      NonEmptyString(good_string.data.string().substr(0, good_string.data.string().size() - 2))));

  for (auto& bad_string : bad_strings) {
    // TODO(Alison) - expect more specific value
    EXPECT_THROW(Message message(bad_string), std::system_error);
  }
}

TEST_F(MessageTest, BEH_SerialiseParseReserialiseReparse) {
  auto serialised_message(message_.Serialise());
  Message recovered_message(serialised_message);

  auto serialised_message2(recovered_message.Serialise());
  Message recovered_message2(serialised_message2);

  EXPECT_EQ(serialised_message, serialised_message2);
  EXPECT_EQ(action_type_, recovered_message2.action_type());
  EXPECT_EQ(dest_persona_type_, recovered_message2.destination_persona_type());
  EXPECT_EQ(src_persona_type_, recovered_message2.source_persona_type());
  EXPECT_EQ(data_type_, recovered_message2.data_type());
  EXPECT_EQ(dest_id_, recovered_message2.destination());
  EXPECT_EQ(src_id_, recovered_message2.source());
  EXPECT_EQ(content_, recovered_message2.content());
  EXPECT_EQ(signature_, recovered_message2.signature());
}

TEST_F(MessageTest, BEH_AssignMessage) {
  Message message2 = message_;

  EXPECT_EQ(action_type_, message2.action_type());
  EXPECT_EQ(dest_persona_type_, message2.destination_persona_type());
  EXPECT_EQ(src_persona_type_, message2.source_persona_type());
  EXPECT_EQ(data_type_, message2.data_type());
  EXPECT_EQ(dest_id_, message2.destination());
  EXPECT_EQ(src_id_, message2.source());
  EXPECT_EQ(content_, message2.content());
  EXPECT_EQ(signature_, message2.signature());

  Message message3(message_);

  EXPECT_EQ(action_type_, message3.action_type());
  EXPECT_EQ(dest_persona_type_, message3.destination_persona_type());
  EXPECT_EQ(src_persona_type_, message3.source_persona_type());
  EXPECT_EQ(data_type_, message3.data_type());
  EXPECT_EQ(dest_id_, message3.destination());
  EXPECT_EQ(src_id_, message3.source());
  EXPECT_EQ(content_, message3.content());
  EXPECT_EQ(signature_, message3.signature());

  Message message4 = std::move(message3);

  EXPECT_EQ(action_type_, message4.action_type());
  EXPECT_EQ(dest_persona_type_, message4.destination_persona_type());
  EXPECT_EQ(src_persona_type_, message4.source_persona_type());
  EXPECT_EQ(data_type_, message4.data_type());
  EXPECT_EQ(dest_id_, message4.destination());
  EXPECT_EQ(src_id_, message4.source());
  EXPECT_EQ(content_, message4.content());
  EXPECT_EQ(signature_, message4.signature());

  Message message5(std::move(message4));

  EXPECT_EQ(action_type_, message5.action_type());
  EXPECT_EQ(dest_persona_type_, message5.destination_persona_type());
  EXPECT_EQ(src_persona_type_, message5.source_persona_type());
  EXPECT_EQ(data_type_, message5.data_type());
  EXPECT_EQ(dest_id_, message5.destination());
  EXPECT_EQ(src_id_, message5.source());
  EXPECT_EQ(content_, message5.content());
  EXPECT_EQ(signature_, message5.signature());
}

TEST_F(MessageTest, BEH_BadDataType) {
  int32_t bad_data_type(RandomInt32());
  while (bad_data_type == 0)
    bad_data_type = RandomInt32();
  if (bad_data_type > 0)
    bad_data_type = -bad_data_type;
  // TODO(Alison) - expect CommonErrors::invalid_parameter
  EXPECT_THROW(Message(action_type_, dest_persona_type_, src_persona_type_,
                       static_cast<detail::DataTagValue>(bad_data_type), dest_id_, src_id_,
                       content_, signature_),
               std::system_error);
}

TEST_F(MessageTest, BEH_BadSource) {
  // TODO(Alison) - expect CommonErrors::invalid_parameter
  EXPECT_THROW(Message(action_type_, dest_persona_type_, src_persona_type_, data_type_, dest_id_,
                       NodeId(), content_, signature_),
               std::system_error);
}

TEST_F(MessageTest, BEH_BadDestination) {
  // TODO(Alison) - expect CommonErrors::invalid_parameter
  EXPECT_THROW(Message(action_type_, dest_persona_type_, src_persona_type_, data_type_, NodeId(),
                       src_id_, content_, signature_),
               std::system_error);
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
