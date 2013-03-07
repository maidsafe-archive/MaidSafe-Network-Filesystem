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

#include "maidsafe/data_types/data_type_values.h"

#include "maidsafe/nfs/generic_message.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace test {

namespace {

GenericMessage::Action GenerateAction() {
  return static_cast<GenericMessage::Action>(RandomUint32() % 6);
}

PersonaId GenerateSource() {
  PersonaId source;
  // matches Persona enum in types.h
  source.persona = static_cast<Persona>(RandomUint32() % 7);
  source.node_id = NodeId(NodeId::kRandomId);
  return source;
}

}  // unnamed namespace

class GenericMessageTest : public testing::Test {
 protected:
  GenericMessageTest()
      : action_(GenerateAction()),
        destination_persona_(static_cast<Persona>(RandomUint32() % 7)),
        source_(GenerateSource()),
        name_(RandomString(NodeId::kSize)),
        content_(RandomString(1 + RandomUint32() % 50)),
        batch_(GenericMessage::Batch(16, 4)),
//        signature_(RandomString(1 + RandomUint32() % 50)),
        generic_message_(action_, destination_persona_, source_, name_, content_, batch_) {}

  GenericMessage::Action action_;
  Persona destination_persona_;
  PersonaId source_;
  Identity name_;
  NonEmptyString content_;
  GenericMessage::Batch batch_;
//  asymm::Signature signature_;
  GenericMessage generic_message_;
};

TEST_F(GenericMessageTest, BEH_CheckGetters) {
  EXPECT_EQ(action_, generic_message_.action());
  EXPECT_EQ(destination_persona_, generic_message_.destination_persona());
  EXPECT_EQ(source_.persona, generic_message_.source().persona);
  EXPECT_EQ(source_.node_id, generic_message_.source().node_id);
  EXPECT_EQ(name_, generic_message_.name());
  EXPECT_EQ(content_, generic_message_.content());
  EXPECT_EQ(batch_.index, generic_message_.batch().index);
  EXPECT_EQ(batch_.size, generic_message_.batch().size);
//  EXPECT_EQ(signature_, generic_message_.signature());
}

TEST_F(GenericMessageTest, BEH_SerialiseThenParse) {
  auto serialised_message(generic_message_.Serialise());
  GenericMessage recovered_message(serialised_message);

  EXPECT_EQ(action_, recovered_message.action());
  EXPECT_EQ(destination_persona_, recovered_message.destination_persona());
  EXPECT_EQ(source_.persona, recovered_message.source().persona);
  EXPECT_EQ(source_.node_id, recovered_message.source().node_id);
  EXPECT_EQ(name_, recovered_message.name());
  EXPECT_EQ(content_, recovered_message.content());
  EXPECT_EQ(batch_.index, recovered_message.batch().index);
  EXPECT_EQ(batch_.size, recovered_message.batch().size);
//  EXPECT_EQ(signature_, recovered_message.signature());
}

TEST_F(GenericMessageTest, BEH_SerialiseParseReserialiseReparse) {
  auto serialised_message(generic_message_.Serialise());
  GenericMessage recovered_message(serialised_message);

  auto serialised_message2(recovered_message.Serialise());
  GenericMessage recovered_message2(serialised_message2);

  EXPECT_EQ(serialised_message, serialised_message2);
  EXPECT_EQ(action_, recovered_message2.action());
  EXPECT_EQ(destination_persona_, recovered_message2.destination_persona());
  EXPECT_EQ(source_.persona, recovered_message2.source().persona);
  EXPECT_EQ(source_.node_id, recovered_message2.source().node_id);
  EXPECT_EQ(name_, recovered_message2.name());
  EXPECT_EQ(content_, recovered_message2.content());
  EXPECT_EQ(batch_.index, recovered_message2.batch().index);
  EXPECT_EQ(batch_.size, recovered_message2.batch().size);
//  EXPECT_EQ(signature_, recovered_message2.signature());
}

TEST_F(GenericMessageTest, BEH_AssignMessage) {
  GenericMessage message2 = generic_message_;

  EXPECT_EQ(action_, message2.action());
  EXPECT_EQ(destination_persona_, message2.destination_persona());
  EXPECT_EQ(source_.persona, message2.source().persona);
  EXPECT_EQ(source_.node_id, message2.source().node_id);
  EXPECT_EQ(name_, message2.name());
  EXPECT_EQ(content_, message2.content());
  EXPECT_EQ(batch_.index, message2.batch().index);
  EXPECT_EQ(batch_.size, message2.batch().size);
//  EXPECT_EQ(signature_, message2.signature());

  GenericMessage message3(generic_message_);

  EXPECT_EQ(action_, message3.action());
  EXPECT_EQ(destination_persona_, message3.destination_persona());
  EXPECT_EQ(source_.persona, message3.source().persona);
  EXPECT_EQ(source_.node_id, message3.source().node_id);
  EXPECT_EQ(name_, message3.name());
  EXPECT_EQ(content_, message3.content());
  EXPECT_EQ(batch_.index, message3.batch().index);
  EXPECT_EQ(batch_.size, message3.batch().size);
//  EXPECT_EQ(signature_, message3.signature());

  GenericMessage message4 = std::move(message3);

  EXPECT_EQ(action_, message4.action());
  EXPECT_EQ(destination_persona_, message4.destination_persona());
  EXPECT_EQ(source_.persona, message4.source().persona);
  EXPECT_EQ(source_.node_id, message4.source().node_id);
  EXPECT_EQ(name_, message4.name());
  EXPECT_EQ(content_, message4.content());
  EXPECT_EQ(batch_.index, message4.batch().index);
  EXPECT_EQ(batch_.size, message4.batch().size);
//  EXPECT_EQ(signature_, message4.signature());

  GenericMessage message5(std::move(message4));

  EXPECT_EQ(action_, message5.action());
  EXPECT_EQ(destination_persona_, message5.destination_persona());
  EXPECT_EQ(source_.persona, message5.source().persona);
  EXPECT_EQ(source_.node_id, message5.source().node_id);
  EXPECT_EQ(name_, message5.name());
  EXPECT_EQ(content_, message5.content());
  EXPECT_EQ(batch_.index, message5.batch().index);
  EXPECT_EQ(batch_.size, message5.batch().size);
//  EXPECT_EQ(signature_, message5.signature());
}

TEST_F(GenericMessageTest, BEH_DefaultValue) {
  GenericMessage generic_message(action_, destination_persona_, source_, name_, content_);
  EXPECT_EQ(action_, generic_message.action());
  EXPECT_EQ(destination_persona_, generic_message.destination_persona());
  EXPECT_EQ(source_.persona, generic_message.source().persona);
  EXPECT_EQ(source_.node_id, generic_message.source().node_id);
  EXPECT_EQ(name_, generic_message.name());
  EXPECT_EQ(content_, generic_message.content());
  EXPECT_NE(batch_.index, generic_message.batch().index);
  EXPECT_NE(batch_.size, generic_message.batch().size);
  EXPECT_EQ(int32_t(-1), generic_message.batch().index);
  EXPECT_EQ(int32_t(-1), generic_message.batch().size);
}

TEST_F(GenericMessageTest, BEH_InvalidSource) {
  PersonaId bad_source(GenerateSource());
  bad_source.node_id = NodeId();
  EXPECT_THROW(GenericMessage(action_, destination_persona_, bad_source, name_, content_),
               nfs_error);
}

TEST_F(GenericMessageTest, BEH_InvalidName) {
  EXPECT_THROW(GenericMessage(action_, destination_persona_, source_, Identity(), content_),
               nfs_error);
}

TEST_F(GenericMessageTest, BEH_InvalidBatch) {
  EXPECT_THROW(GenericMessage(action_, destination_persona_, source_, name_, content_,
                              GenericMessage::Batch(10, 11)), nfs_error);
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
