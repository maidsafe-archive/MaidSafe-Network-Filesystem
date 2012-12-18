/*******************************************************************************
 *  Copyright 2012 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ******************************************************************************/

#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/message.h"

namespace maidsafe {

namespace nfs {

namespace test {

ActionType GenerateActionType() {
  return static_cast<ActionType>(RandomUint32() % 4);  // matches ActionType enum in message.h
}

PersonaType GeneratePersonaType() {
  return static_cast<PersonaType>(RandomUint32() % 4);  // matches PersonaType enum in message.h
}

class MessageTest : public testing::Test {
 public:
  MessageTest()
    : action_type_(GenerateActionType()),
      dest_persona_type_(GeneratePersonaType()),
      src_persona_type_(GeneratePersonaType()),
      data_type_(RandomInt32()),
      dest_id_(NodeId(NodeId::kRandomId)),
      src_id_(NodeId(NodeId::kRandomId)),
      content_(RandomAlphaNumericString(1 + RandomUint32() % 50)),
      signature_(RandomAlphaNumericString(1 + RandomUint32() % 50)) {}

  ActionType action_type_;
  PersonaType dest_persona_type_;
  PersonaType src_persona_type_;
  int data_type_;
  NodeId dest_id_;
  NodeId src_id_;
  std::string content_;
  std::string signature_;
};

TEST_F(MessageTest, BEH_CheckGetters) {
  Message message(action_type_,
                  dest_persona_type_,
                  src_persona_type_,
                  data_type_,
                  dest_id_,
                  src_id_,
                  content_,
                  signature_);
  EXPECT_EQ(action_type_, message.action_type());
  EXPECT_EQ(dest_persona_type_, message.destination_persona_type());
  EXPECT_EQ(src_persona_type_, message.source_persona_type());
  EXPECT_EQ(data_type_, message.data_type());
  EXPECT_EQ(dest_id_, message.destination());
  EXPECT_EQ(src_id_, message.source());
  EXPECT_EQ(content_, message.content());
  EXPECT_EQ(signature_, message.signature());
}

TEST_F(MessageTest, BEH_SerialiseThenParse) {
  Message message(action_type_,
                  dest_persona_type_,
                  src_persona_type_,
                  data_type_,
                  dest_id_,
                  src_id_,
                  content_,
                  signature_);
  std::string string(SerialiseAsString(message));
  Message message2(ParseFromString(string));

  EXPECT_EQ(action_type_, message2.action_type());
  EXPECT_EQ(dest_persona_type_, message2.destination_persona_type());
  EXPECT_EQ(src_persona_type_, message2.source_persona_type());
  EXPECT_EQ(data_type_, message2.data_type());
  EXPECT_EQ(dest_id_, message2.destination());
  EXPECT_EQ(src_id_, message2.source());
  EXPECT_EQ(content_, message2.content());
  EXPECT_EQ(signature_, message2.signature());
}

TEST_F(MessageTest, BEH_ParseFromBadString) {
  Message message(action_type_,
                  dest_persona_type_,
                  src_persona_type_,
                  data_type_,
                  dest_id_,
                  src_id_,
                  content_,
                  signature_);
  std::vector<std::string> bad_strings;
  bad_strings.push_back(RandomAlphaNumericString(1 + RandomUint32() % 100));
  bad_strings.push_back("");
  std::string good_string(SerialiseAsString(message));
  bad_strings.push_back(good_string.append(RandomAlphaNumericString(1)));
  good_string.resize(good_string.size() - 2);
  bad_strings.push_back(good_string);

  for (auto bad_string : bad_strings) {
    // TODO(Alison) - expect more specific value
    EXPECT_THROW(ParseFromString(bad_string), std::exception);
  }
}

TEST_F(MessageTest, BEH_SerialiseParseReserialiseReparse) {
  Message message(action_type_,
                  dest_persona_type_,
                  src_persona_type_,
                  data_type_,
                  dest_id_,
                  src_id_,
                  content_,
                  signature_);
  std::string string(SerialiseAsString(message));
  Message message2(ParseFromString(string));

  EXPECT_EQ(action_type_, message2.action_type());
  EXPECT_EQ(dest_persona_type_, message2.destination_persona_type());
  EXPECT_EQ(src_persona_type_, message2.source_persona_type());
  EXPECT_EQ(data_type_, message2.data_type());
  EXPECT_EQ(dest_id_, message2.destination());
  EXPECT_EQ(src_id_, message2.source());
  EXPECT_EQ(content_, message2.content());
  EXPECT_EQ(signature_, message2.signature());

  std::string string2(SerialiseAsString(message2));
  Message message3(ParseFromString(string2));

  EXPECT_EQ(action_type_, message3.action_type());
  EXPECT_EQ(dest_persona_type_, message3.destination_persona_type());
  EXPECT_EQ(src_persona_type_, message3.source_persona_type());
  EXPECT_EQ(data_type_, message3.data_type());
  EXPECT_EQ(dest_id_, message3.destination());
  EXPECT_EQ(src_id_, message3.source());
  EXPECT_EQ(content_, message3.content());
  EXPECT_EQ(signature_, message3.signature());
}

TEST_F(MessageTest, BEH_AssignMessage) {
  Message message(action_type_,
                  dest_persona_type_,
                  src_persona_type_,
                  data_type_,
                  dest_id_,
                  src_id_,
                  content_,
                  signature_);
  Message message2 = message;

  EXPECT_EQ(action_type_, message2.action_type());
  EXPECT_EQ(dest_persona_type_, message2.destination_persona_type());
  EXPECT_EQ(src_persona_type_, message2.source_persona_type());
  EXPECT_EQ(data_type_, message2.data_type());
  EXPECT_EQ(dest_id_, message2.destination());
  EXPECT_EQ(src_id_, message2.source());
  EXPECT_EQ(content_, message2.content());
  EXPECT_EQ(signature_, message2.signature());

  Message message3(message);

  EXPECT_EQ(action_type_, message3.action_type());
  EXPECT_EQ(dest_persona_type_, message3.destination_persona_type());
  EXPECT_EQ(src_persona_type_, message3.source_persona_type());
  EXPECT_EQ(data_type_, message3.data_type());
  EXPECT_EQ(dest_id_, message3.destination());
  EXPECT_EQ(src_id_, message3.source());
  EXPECT_EQ(content_, message3.content());
  EXPECT_EQ(signature_, message3.signature());
}



TEST_F(MessageTest, BEH_BadDataType) {
  int32_t bad_data_type(RandomInt32());
  while (bad_data_type == 0)
    bad_data_type = RandomInt32();
  if (bad_data_type > 0)
    bad_data_type = -bad_data_type;
  // TODO(Alison) - expect CommonErrors::invalid_parameter
  EXPECT_THROW(Message(action_type_,
                       dest_persona_type_,
                       src_persona_type_,
                       bad_data_type,
                       dest_id_,
                       src_id_,
                       content_,
                       signature_),
               std::exception);
}

TEST_F(MessageTest, BEH_BadSource) {
  // TODO(Alison) - expect CommonErrors::invalid_parameter
  EXPECT_THROW(Message(action_type_,
                       dest_persona_type_,
                       src_persona_type_,
                       data_type_,
                       dest_id_,
                       NodeId(),
                       content_,
                       signature_),
               std::exception);
}

TEST_F(MessageTest, BEH_BadDestination) {
  // TODO(Alison) - expect CommonErrors::invalid_parameter
  EXPECT_THROW(Message(action_type_,
                       dest_persona_type_,
                       src_persona_type_,
                       data_type_,
                       NodeId(),
                       src_id_,
                       content_,
                       signature_),
               std::exception);
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
