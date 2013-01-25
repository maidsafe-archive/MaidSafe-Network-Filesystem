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

#include "maidsafe/data_types/detail/data_type_values.h"

#include "maidsafe/nfs/data_message.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace test {

namespace {

DataMessage::Action GenerateAction() {
  return static_cast<DataMessage::Action>(RandomUint32() % 3);
}

MessageSource GenerateSource() {
  MessageSource source;
  // matches Persona enum in types.h
  source.persona = static_cast<Persona>(RandomUint32() % 7);
  source.node_id = NodeId(NodeId::kRandomId);
  return source;
}

}  // unnamed namespace

class DataMessageTest : public testing::Test {
 protected:
  DataMessageTest()
      : action_(GenerateAction()),
        destination_persona_(static_cast<Persona>(RandomUint32() % 7)),
        source_(GenerateSource()),
        data_type_(static_cast<detail::DataTagValue>(RandomUint32() % 13)),
        name_(RandomString(NodeId::kSize)),
        content_(RandomString(1 + RandomUint32() % 50)),
        data_message_(action_, destination_persona_, source_,
                      DataMessage::Data(data_type_, name_, content_)) {}

  DataMessage::Action action_;
  Persona destination_persona_;
  MessageSource source_;
  DataMessage::Data data_;
  detail::DataTagValue data_type_;
  Identity name_;
  NonEmptyString content_;
  DataMessage data_message_;
};

TEST_F(DataMessageTest, BEH_CheckGetters) {
  EXPECT_EQ(action_, data_message_.action());
  EXPECT_EQ(destination_persona_, data_message_.destination_persona());
  EXPECT_EQ(source_.persona, data_message_.source().persona);
  EXPECT_EQ(source_.node_id, data_message_.source().node_id);
  EXPECT_EQ(data_type_, data_message_.data().type);
  EXPECT_EQ(name_, data_message_.data().name);
  EXPECT_EQ(content_, data_message_.data().content);
}

TEST_F(DataMessageTest, BEH_SerialiseThenParse) {
  auto serialised_message(data_message_.Serialise());
  DataMessage recovered_message(serialised_message);

  EXPECT_EQ(action_, recovered_message.action());
  EXPECT_EQ(destination_persona_, recovered_message.destination_persona());
  EXPECT_EQ(source_.persona, recovered_message.source().persona);
  EXPECT_EQ(source_.node_id, recovered_message.source().node_id);
  EXPECT_EQ(data_type_, recovered_message.data().type);
  EXPECT_EQ(name_, recovered_message.data().name);
  EXPECT_EQ(content_, recovered_message.data().content);
}

TEST_F(DataMessageTest, BEH_SerialiseParseReserialiseReparse) {
  auto serialised_message(data_message_.Serialise());
  DataMessage recovered_message(serialised_message);

  auto serialised_message2(recovered_message.Serialise());
  DataMessage recovered_message2(serialised_message2);

  EXPECT_EQ(serialised_message, serialised_message2);
  EXPECT_EQ(action_, recovered_message2.action());
  EXPECT_EQ(destination_persona_, recovered_message2.destination_persona());
  EXPECT_EQ(source_.persona, recovered_message2.source().persona);
  EXPECT_EQ(source_.node_id, recovered_message2.source().node_id);
  EXPECT_EQ(data_type_, recovered_message2.data().type);
  EXPECT_EQ(name_, recovered_message2.data().name);
  EXPECT_EQ(content_, recovered_message2.data().content);
}

TEST_F(DataMessageTest, BEH_AssignMessage) {
  DataMessage message2 = data_message_;

  EXPECT_EQ(action_, message2.action());
  EXPECT_EQ(destination_persona_, message2.destination_persona());
  EXPECT_EQ(source_.persona, message2.source().persona);
  EXPECT_EQ(source_.node_id, message2.source().node_id);
  EXPECT_EQ(data_type_, message2.data().type);
  EXPECT_EQ(name_, message2.data().name);
  EXPECT_EQ(content_, message2.data().content);

  DataMessage message3(data_message_);

  EXPECT_EQ(action_, message3.action());
  EXPECT_EQ(destination_persona_, message3.destination_persona());
  EXPECT_EQ(source_.persona, message3.source().persona);
  EXPECT_EQ(source_.node_id, message3.source().node_id);
  EXPECT_EQ(data_type_, message3.data().type);
  EXPECT_EQ(name_, message3.data().name);
  EXPECT_EQ(content_, message3.data().content);

  DataMessage message4 = std::move(message3);

  EXPECT_EQ(action_, message4.action());
  EXPECT_EQ(destination_persona_, message4.destination_persona());
  EXPECT_EQ(source_.persona, message4.source().persona);
  EXPECT_EQ(source_.node_id, message4.source().node_id);
  EXPECT_EQ(data_type_, message4.data().type);
  EXPECT_EQ(name_, message4.data().name);
  EXPECT_EQ(content_, message4.data().content);

  DataMessage message5(std::move(message4));

  EXPECT_EQ(action_, message5.action());
  EXPECT_EQ(destination_persona_, message5.destination_persona());
  EXPECT_EQ(source_.persona, message5.source().persona);
  EXPECT_EQ(source_.node_id, message5.source().node_id);
  EXPECT_EQ(data_type_, message5.data().type);
  EXPECT_EQ(name_, message5.data().name);
  EXPECT_EQ(content_, message5.data().content);
}

TEST_F(DataMessageTest, BEH_InvalidDataType) {
  int32_t bad_data_type(RandomInt32());
  while (bad_data_type == 0)
    bad_data_type = RandomInt32();
  if (bad_data_type > 0)
    bad_data_type = -bad_data_type;
  EXPECT_THROW(DataMessage(action_, destination_persona_, source_,
                           DataMessage::Data(static_cast<detail::DataTagValue>(bad_data_type),
                                             name_, content_)),
               nfs_error);
}

TEST_F(DataMessageTest, BEH_InvalidSource) {
  MessageSource bad_source(GenerateSource());
  bad_source.node_id = NodeId();
  EXPECT_THROW(DataMessage(action_, destination_persona_, bad_source, data_message_.data()),
               nfs_error);
}

TEST_F(DataMessageTest, BEH_InvalidName) {
  EXPECT_THROW(DataMessage(action_, destination_persona_, source_,
                           DataMessage::Data(data_type_, Identity(), content_)),
               nfs_error);
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
