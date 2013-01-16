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

#include "maidsafe/nfs/generic_message.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace test {

namespace {

PostActionType GenerateActionType() {
  return static_cast<PostActionType>(RandomUint32() % 6);  // matches PostActionType enum in types.h
}

DataMessage::Source GenerateSource() {
  DataMessage::Source source;
  // matches PersonaType enum in types.h
  source.persona_type = static_cast<PersonaType>(RandomUint32() % 7);
  source.node_id = NodeId(NodeId::kRandomId);
  return source;
}

}  // unnamed namespace

class PostMessageTest : public testing::Test {
 protected:
  PostMessageTest()
      : post_action_type_(GenerateActionType()),
        destination_persona_type_(static_cast<PersonaType>(RandomUint32() % 7)),
        source_(GenerateSource()),
        name_(RandomString(NodeId::kSize)),
        content_(RandomString(1 + RandomUint32() % 50)),
//        signature_(RandomString(1 + RandomUint32() % 50)),
        generic_message_(post_action_type_, destination_persona_type_, source_, name_, content_/*,
                      signature_*/) {}

  PostActionType post_action_type_;
  PersonaType destination_persona_type_;
  DataMessage::Source source_;
  Identity name_;
  NonEmptyString content_;
//  asymm::Signature signature_;
  GenericMessage generic_message_;
};

TEST_F(PostMessageTest, BEH_CheckGetters) {
  EXPECT_EQ(post_action_type_, generic_message_.post_action_type());
  EXPECT_EQ(destination_persona_type_, generic_message_.destination_persona_type());
  EXPECT_EQ(source_.persona_type, generic_message_.source().persona_type);
  EXPECT_EQ(source_.node_id, generic_message_.source().node_id);
  EXPECT_EQ(name_, generic_message_.name());
  EXPECT_EQ(content_, generic_message_.content());
//  EXPECT_EQ(signature_, generic_message_.signature());
}

TEST_F(PostMessageTest, BEH_SerialiseThenParse) {
  auto serialised_message(generic_message_.Serialise());
  GenericMessage recovered_message(serialised_message);

  EXPECT_EQ(post_action_type_, recovered_message.post_action_type());
  EXPECT_EQ(destination_persona_type_, recovered_message.destination_persona_type());
  EXPECT_EQ(source_.persona_type, recovered_message.source().persona_type);
  EXPECT_EQ(source_.node_id, recovered_message.source().node_id);
  EXPECT_EQ(name_, recovered_message.name());
  EXPECT_EQ(content_, recovered_message.content());
//  EXPECT_EQ(signature_, recovered_message.signature());
}

TEST_F(PostMessageTest, BEH_SerialiseParseReserialiseReparse) {
  auto serialised_message(generic_message_.Serialise());
  GenericMessage recovered_message(serialised_message);

  auto serialised_message2(recovered_message.Serialise());
  GenericMessage recovered_message2(serialised_message2);

  EXPECT_EQ(serialised_message, serialised_message2);
  EXPECT_EQ(post_action_type_, recovered_message2.post_action_type());
  EXPECT_EQ(destination_persona_type_, recovered_message2.destination_persona_type());
  EXPECT_EQ(source_.persona_type, recovered_message2.source().persona_type);
  EXPECT_EQ(source_.node_id, recovered_message2.source().node_id);
  EXPECT_EQ(name_, recovered_message2.name());
  EXPECT_EQ(content_, recovered_message2.content());
//  EXPECT_EQ(signature_, recovered_message2.signature());
}

TEST_F(PostMessageTest, BEH_AssignMessage) {
  GenericMessage message2 = generic_message_;

  EXPECT_EQ(post_action_type_, message2.post_action_type());
  EXPECT_EQ(destination_persona_type_, message2.destination_persona_type());
  EXPECT_EQ(source_.persona_type, message2.source().persona_type);
  EXPECT_EQ(source_.node_id, message2.source().node_id);
  EXPECT_EQ(name_, message2.name());
  EXPECT_EQ(content_, message2.content());
//  EXPECT_EQ(signature_, message2.signature());

  GenericMessage message3(generic_message_);

  EXPECT_EQ(post_action_type_, message3.post_action_type());
  EXPECT_EQ(destination_persona_type_, message3.destination_persona_type());
  EXPECT_EQ(source_.persona_type, message3.source().persona_type);
  EXPECT_EQ(source_.node_id, message3.source().node_id);
  EXPECT_EQ(name_, message3.name());
  EXPECT_EQ(content_, message3.content());
//  EXPECT_EQ(signature_, message3.signature());

  GenericMessage message4 = std::move(message3);

  EXPECT_EQ(post_action_type_, message4.post_action_type());
  EXPECT_EQ(destination_persona_type_, message4.destination_persona_type());
  EXPECT_EQ(source_.persona_type, message4.source().persona_type);
  EXPECT_EQ(source_.node_id, message4.source().node_id);
  EXPECT_EQ(name_, message4.name());
  EXPECT_EQ(content_, message4.content());
//  EXPECT_EQ(signature_, message4.signature());

  GenericMessage message5(std::move(message4));

  EXPECT_EQ(post_action_type_, message5.post_action_type());
  EXPECT_EQ(destination_persona_type_, message5.destination_persona_type());
  EXPECT_EQ(source_.persona_type, message5.source().persona_type);
  EXPECT_EQ(source_.node_id, message5.source().node_id);
  EXPECT_EQ(name_, message5.name());
  EXPECT_EQ(content_, message5.content());
//  EXPECT_EQ(signature_, message5.signature());
}

TEST_F(PostMessageTest, BEH_InvalidSource) {
  DataMessage::Source bad_source(GenerateSource());
  bad_source.node_id = NodeId();
  EXPECT_THROW(GenericMessage(post_action_type_, destination_persona_type_, bad_source, name_,
                              content_/*, signature_*/),
               nfs_error);
}

TEST_F(PostMessageTest, BEH_InvalidName) {
  EXPECT_THROW(GenericMessage(post_action_type_, destination_persona_type_, source_, Identity(),
                              content_/*, signature_*/),
               nfs_error);
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
