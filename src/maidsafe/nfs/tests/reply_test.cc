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

#include "maidsafe/nfs/reply.h"

#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"


namespace maidsafe {

namespace nfs {

namespace test {

class ReplyTest : public testing::TestWithParam<bool> {
 protected:
  ReplyTest()
      : error_(MakeError(CommonErrors::success)),
        data_(),
        reply_(CommonErrors::success) {
    if (GetParam())
      data_ = NonEmptyString(RandomString(1 + RandomUint32() % 4096));
    int type(RandomUint32() % 7), value(RandomUint32() % 5);
    switch (type) {
      case 0:
        error_ = MakeError(static_cast<CommonErrors>(value));
        reply_ = Reply(static_cast<CommonErrors>(value), data_);
        break;
      case 1:
        error_ = MakeError(static_cast<AsymmErrors>(value));
        reply_ = Reply(static_cast<AsymmErrors>(value), data_);
        break;
      case 2:
        error_ = MakeError(static_cast<PassportErrors>(value));
        reply_ = Reply(static_cast<PassportErrors>(value), data_);
        break;
      case 3:
        error_ = MakeError(static_cast<NfsErrors>(value));
        reply_ = Reply(static_cast<NfsErrors>(value), data_);
        break;
      case 4:
        error_ = MakeError(static_cast<RoutingErrors>(value));
        reply_ = Reply(static_cast<RoutingErrors>(value), data_);
        break;
      case 5:
        error_ = MakeError(static_cast<VaultErrors>(value));
        reply_ = Reply(static_cast<VaultErrors>(value), data_);
        break;
      default:
        error_ = MakeError(static_cast<LifeStuffErrors>(value));
        reply_ = Reply(static_cast<LifeStuffErrors>(value), data_);
    }
  }

  maidsafe_error error_;
  NonEmptyString data_;
  Reply reply_;
};

TEST_P(ReplyTest, BEH_AllGood) {
  EXPECT_EQ(error_.code(), reply_.error().code());
  EXPECT_EQ(error_.code().category(), reply_.error().code().category());
  EXPECT_EQ(data_, reply_.data());
  if (!GetParam())
    EXPECT_FALSE(reply_.data().IsInitialised());
  auto serialised_reply(reply_.Serialise());

  Reply reply_from_error(error_, data_);
  EXPECT_EQ(error_.code(), reply_from_error.error().code());
  EXPECT_EQ(error_.code().category(), reply_from_error.error().code().category());
  EXPECT_EQ(data_, reply_from_error.data());
  if (!GetParam())
    EXPECT_FALSE(reply_from_error.data().IsInitialised());
  EXPECT_EQ(serialised_reply, reply_from_error.Serialise());

  Reply reply_copy(reply_);
  EXPECT_EQ(error_.code(), reply_copy.error().code());
  EXPECT_EQ(error_.code().category(), reply_copy.error().code().category());
  EXPECT_EQ(data_, reply_copy.data());
  if (!GetParam())
    EXPECT_FALSE(reply_copy.data().IsInitialised());
  EXPECT_EQ(serialised_reply, reply_copy.Serialise());

  Reply reply_assigned(CommonErrors::success);
  reply_assigned = reply_;
  EXPECT_EQ(error_.code(), reply_assigned.error().code());
  EXPECT_EQ(error_.code().category(), reply_assigned.error().code().category());
  EXPECT_EQ(data_, reply_assigned.data());
  if (!GetParam())
    EXPECT_FALSE(reply_assigned.data().IsInitialised());
  EXPECT_EQ(serialised_reply, reply_assigned.Serialise());

  Reply reply_moved(std::move(reply_copy));
  EXPECT_EQ(error_.code(), reply_moved.error().code());
  EXPECT_EQ(error_.code().category(), reply_moved.error().code().category());
  EXPECT_EQ(data_, reply_moved.data());
  if (!GetParam())
    EXPECT_FALSE(reply_moved.data().IsInitialised());
  EXPECT_EQ(serialised_reply, reply_moved.Serialise());

  reply_assigned = std::move(reply_moved);
  EXPECT_EQ(error_.code(), reply_assigned.error().code());
  EXPECT_EQ(error_.code().category(), reply_assigned.error().code().category());
  EXPECT_EQ(data_, reply_assigned.data());
  if (!GetParam())
    EXPECT_FALSE(reply_assigned.data().IsInitialised());
  EXPECT_EQ(serialised_reply, reply_assigned.Serialise());

  Reply reply_parsed(serialised_reply);
  EXPECT_EQ(error_.code(), reply_parsed.error().code());
  EXPECT_EQ(error_.code().category(), reply_parsed.error().code().category());
  EXPECT_EQ(data_, reply_parsed.data());
  if (!GetParam())
    EXPECT_FALSE(reply_parsed.data().IsInitialised());
  EXPECT_EQ(serialised_reply, reply_parsed.Serialise());
}

TEST_P(ReplyTest, BEH_ParseFromBadString) {
  std::vector<Reply::serialised_type> bad_strings;
  bad_strings.push_back(Reply::serialised_type(NonEmptyString(
      RandomString(1 + RandomUint32() % 100))));
  bad_strings.push_back(Reply::serialised_type(NonEmptyString()));
  auto good_string(reply_.Serialise());
  bad_strings.push_back(Reply::serialised_type(good_string.data + NonEmptyString(RandomString(1))));
  bad_strings.push_back(Reply::serialised_type(
      NonEmptyString(good_string->string().substr(0, good_string->string().size() - 2))));

  for (auto& bad_string : bad_strings)
    EXPECT_THROW(Reply parsed(bad_string), maidsafe_error);
}

INSTANTIATE_TEST_CASE_P(NonEmptyAndEmptyInfo, ReplyTest, testing::Bool());

TEST(SingleReplyTest, BEH_BadDataSize) {
  EXPECT_THROW(Reply(CommonErrors::success, NonEmptyString("")), maidsafe_error);
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
