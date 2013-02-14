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

#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"
#include "maidsafe/data_types/data_type_values.h"

#include "maidsafe/nfs/data_message_pb.h"
#include "maidsafe/nfs/types.h"
#include "maidsafe/nfs/accumulator.h"


namespace maidsafe {

namespace nfs {

namespace test {

namespace {

DataMessage::Action GenerateAction() {
  return static_cast<DataMessage::Action>(RandomUint32() % 3);
}

PersonaId GenerateSource() {
  PersonaId source;
  // matches Persona enum in types.h
  source.persona = static_cast<Persona>(RandomUint32() % 7);
  source.node_id = NodeId(NodeId::kRandomId);
  return source;
}

}  // unnamed namespace

TEST(AccumulatorTest, BEH_PushRequest) {
  DataMessage::Data data(static_cast<DataTagValue>(RandomUint32() % 13),
                    Identity(RandomString(NodeId::kSize)),
                    NonEmptyString(RandomString(1 + RandomUint32() % 50)),
                    static_cast<DataMessage::Action>(RandomUint32() % 3));
  DataMessage data_message(
      static_cast<Persona>(RandomUint32() % 7),
      GenerateSource(),
      data,
      passport::PublicPmid::name_type(Identity(RandomString(NodeId::kSize))));
  Reply reply(CommonErrors::success);
  Accumulator<passport::PublicMaid::name_type> accumulator;
  Accumulator<passport::PublicMaid::name_type>::Request request(data_message,
                                                     [](const std::string&) {},
                                                     reply);
  accumulator.PushRequest(request);
  EXPECT_EQ(accumulator.pending_requests_.size(), 1);
  auto request_identity(accumulator.pending_requests_.at(0).first);
  EXPECT_FALSE(accumulator.CheckHandled(request_identity, reply));
  accumulator.SetHandled(request_identity, reply);
  EXPECT_EQ(accumulator.pending_requests_.size(), 0);
  EXPECT_TRUE(accumulator.CheckHandled(request_identity, reply));
  Accumulator<passport::PublicMaid::name_type>::serialised_requests serialised(
      accumulator.SerialiseHandledRequests(request_identity.second));
  auto parsed(accumulator.ParseHandledRequests(serialised));
  EXPECT_EQ(parsed.size(), 1);
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
