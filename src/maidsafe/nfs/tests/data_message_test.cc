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

namespace {

DataMessage::ActionType GenerateActionType() {
  return static_cast<DataMessage::ActionType>(RandomUint32() % 3);
}

MessageSource GenerateSource() {
  MessageSource source;
  // matches PersonaType enum in types.h
  source.persona_type = static_cast<PersonaType>(RandomUint32() % 7);
  source.node_id = NodeId(NodeId::kRandomId);
  return source;
}

}  // unnamed namespace

class DataMessageTest : public testing::Test {
 protected:
  DataMessageTest()
      : action_type_(GenerateActionType()),
        destination_persona_type_(static_cast<PersonaType>(RandomUint32() % 7)),
        source_(GenerateSource()),
        data_type_(static_cast<detail::DataTagValue>(RandomUint32() % 13)),
        name_(RandomString(NodeId::kSize)),
        content_(RandomString(1 + RandomUint32() % 50)),
        version_(RandomInt32() % 100),
//        signature_(RandomString(1 + RandomUint32() % 50)),
        data_message_(action_type_, destination_persona_type_, source_,
                      DataMessage::Data(data_type_, name_, content_, version_)) {}

  DataMessage::ActionType action_type_;
  PersonaType destination_persona_type_;
  MessageSource source_;
  DataMessage::Data data_;
  detail::DataTagValue data_type_;
  Identity name_;
  NonEmptyString content_;
  int32_t version_;
//  asymm::Signature signature_;
  DataMessage data_message_;
};

TEST_F(DataMessageTest, BEH_CheckGetters) {
  EXPECT_EQ(action_type_, data_message_.action_type());
  EXPECT_EQ(destination_persona_type_, data_message_.destination_persona_type());
  EXPECT_EQ(source_.persona_type, data_message_.source().persona_type);
  EXPECT_EQ(source_.node_id, data_message_.source().node_id);
  EXPECT_EQ(data_type_, data_message_.data().type);
  EXPECT_EQ(name_, data_message_.data().name);
  EXPECT_EQ(content_, data_message_.data().content);
//  EXPECT_EQ(signature_, data_message_.signature());
}

TEST_F(DataMessageTest, BEH_SerialiseThenParse) {
  auto serialised_message(data_message_.Serialise());
  DataMessage recovered_message(serialised_message);

  EXPECT_EQ(action_type_, recovered_message.action_type());
  EXPECT_EQ(destination_persona_type_, recovered_message.destination_persona_type());
  EXPECT_EQ(source_.persona_type, recovered_message.source().persona_type);
  EXPECT_EQ(source_.node_id, recovered_message.source().node_id);
  EXPECT_EQ(data_type_, recovered_message.data().type);
  EXPECT_EQ(name_, recovered_message.data().name);
  EXPECT_EQ(content_, recovered_message.data().content);
//  EXPECT_EQ(signature_, recovered_message.signature());
}

TEST_F(DataMessageTest, BEH_SerialiseParseReserialiseReparse) {
  auto serialised_message(data_message_.Serialise());
  DataMessage recovered_message(serialised_message);

  auto serialised_message2(recovered_message.Serialise());
  DataMessage recovered_message2(serialised_message2);

  EXPECT_EQ(serialised_message, serialised_message2);
  EXPECT_EQ(action_type_, recovered_message2.action_type());
  EXPECT_EQ(destination_persona_type_, recovered_message2.destination_persona_type());
  EXPECT_EQ(source_.persona_type, recovered_message2.source().persona_type);
  EXPECT_EQ(source_.node_id, recovered_message2.source().node_id);
  EXPECT_EQ(data_type_, recovered_message2.data().type);
  EXPECT_EQ(name_, recovered_message2.data().name);
  EXPECT_EQ(content_, recovered_message2.data().content);
//  EXPECT_EQ(signature_, recovered_message2.signature());
}

TEST_F(DataMessageTest, BEH_AssignMessage) {
  DataMessage message2 = data_message_;

  EXPECT_EQ(action_type_, message2.action_type());
  EXPECT_EQ(destination_persona_type_, message2.destination_persona_type());
  EXPECT_EQ(source_.persona_type, message2.source().persona_type);
  EXPECT_EQ(source_.node_id, message2.source().node_id);
  EXPECT_EQ(data_type_, message2.data().type);
  EXPECT_EQ(name_, message2.data().name);
  EXPECT_EQ(content_, message2.data().content);
//  EXPECT_EQ(signature_, message2.signature());

  DataMessage message3(data_message_);

  EXPECT_EQ(action_type_, message3.action_type());
  EXPECT_EQ(destination_persona_type_, message3.destination_persona_type());
  EXPECT_EQ(source_.persona_type, message3.source().persona_type);
  EXPECT_EQ(source_.node_id, message3.source().node_id);
  EXPECT_EQ(data_type_, message3.data().type);
  EXPECT_EQ(name_, message3.data().name);
  EXPECT_EQ(content_, message3.data().content);
//  EXPECT_EQ(signature_, message3.signature());

  DataMessage message4 = std::move(message3);

  EXPECT_EQ(action_type_, message4.action_type());
  EXPECT_EQ(destination_persona_type_, message4.destination_persona_type());
  EXPECT_EQ(source_.persona_type, message4.source().persona_type);
  EXPECT_EQ(source_.node_id, message4.source().node_id);
  EXPECT_EQ(data_type_, message4.data().type);
  EXPECT_EQ(name_, message4.data().name);
  EXPECT_EQ(content_, message4.data().content);
//  EXPECT_EQ(signature_, message4.signature());

  DataMessage message5(std::move(message4));

  EXPECT_EQ(action_type_, message5.action_type());
  EXPECT_EQ(destination_persona_type_, message5.destination_persona_type());
  EXPECT_EQ(source_.persona_type, message5.source().persona_type);
  EXPECT_EQ(source_.node_id, message5.source().node_id);
  EXPECT_EQ(data_type_, message5.data().type);
  EXPECT_EQ(name_, message5.data().name);
  EXPECT_EQ(content_, message5.data().content);
//  EXPECT_EQ(signature_, message5.signature());
}

TEST_F(DataMessageTest, BEH_InvalidDataType) {
  int32_t bad_data_type(RandomInt32());
  while (bad_data_type == 0)
    bad_data_type = RandomInt32();
  if (bad_data_type > 0)
    bad_data_type = -bad_data_type;
  EXPECT_THROW(DataMessage(action_type_, destination_persona_type_, source_,
                           DataMessage::Data(static_cast<detail::DataTagValue>(bad_data_type),
                                             name_, content_, version_)),
               nfs_error);
}

TEST_F(DataMessageTest, BEH_InvalidSource) {
  MessageSource bad_source(GenerateSource());
  bad_source.node_id = NodeId();
  EXPECT_THROW(DataMessage(action_type_, destination_persona_type_, bad_source,
                           data_message_.data()),
               nfs_error);
}

TEST_F(DataMessageTest, BEH_InvalidName) {
  EXPECT_THROW(DataMessage(action_type_, destination_persona_type_, source_,
                           DataMessage::Data(data_type_, Identity(), content_, version_)),
               nfs_error);
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
