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

#include "maidsafe/nfs/return_code.h"

#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"


namespace maidsafe {

namespace nfs {

namespace test {

class ReturnCodeTest : public testing::TestWithParam<bool> {
 protected:
  ReturnCodeTest()
      : error_(MakeError(CommonErrors::success)),
        data_(),
        return_code_(CommonErrors::success) {
    if (GetParam())
      data_ = NonEmptyString(RandomString(1 + RandomUint32() % 4096));
    int type(RandomUint32() % 7), value(RandomUint32() % 5);
    switch (type) {
      case 0:
        error_ = MakeError(static_cast<CommonErrors>(value));
        return_code_ = ReturnCode(static_cast<CommonErrors>(value), data_);
        break;
      case 1:
        error_ = MakeError(static_cast<AsymmErrors>(value));
        return_code_ = ReturnCode(static_cast<AsymmErrors>(value), data_);
        break;
      case 2:
        error_ = MakeError(static_cast<PassportErrors>(value));
        return_code_ = ReturnCode(static_cast<PassportErrors>(value), data_);
        break;
      case 3:
        error_ = MakeError(static_cast<NfsErrors>(value));
        return_code_ = ReturnCode(static_cast<NfsErrors>(value), data_);
        break;
      case 4:
        error_ = MakeError(static_cast<RoutingErrors>(value));
        return_code_ = ReturnCode(static_cast<RoutingErrors>(value), data_);
        break;
      case 5:
        error_ = MakeError(static_cast<VaultErrors>(value));
        return_code_ = ReturnCode(static_cast<VaultErrors>(value), data_);
        break;
      default:
        error_ = MakeError(static_cast<LifeStuffErrors>(value));
        return_code_ = ReturnCode(static_cast<LifeStuffErrors>(value), data_);
    }
  }

  std::system_error error_;
  NonEmptyString data_;
  ReturnCode return_code_;
};

TEST_P(ReturnCodeTest, BEH_AllGood) {
  EXPECT_EQ(error_.code(), return_code_.error().code());
  EXPECT_EQ(error_.code().category(), return_code_.error().code().category());
  EXPECT_EQ(data_, return_code_.data());
  if (!GetParam())
    EXPECT_FALSE(return_code_.data().IsInitialised());
  auto serialised_return_code(return_code_.Serialise());

  ReturnCode return_code_from_error(error_, data_);
  EXPECT_EQ(error_.code(), return_code_from_error.error().code());
  EXPECT_EQ(error_.code().category(), return_code_from_error.error().code().category());
  EXPECT_EQ(data_, return_code_from_error.data());
  if (!GetParam())
    EXPECT_FALSE(return_code_from_error.data().IsInitialised());
  EXPECT_EQ(serialised_return_code, return_code_from_error.Serialise());

  ReturnCode return_code_copy(return_code_);
  EXPECT_EQ(error_.code(), return_code_copy.error().code());
  EXPECT_EQ(error_.code().category(), return_code_copy.error().code().category());
  EXPECT_EQ(data_, return_code_copy.data());
  if (!GetParam())
    EXPECT_FALSE(return_code_copy.data().IsInitialised());
  EXPECT_EQ(serialised_return_code, return_code_copy.Serialise());

  ReturnCode return_code_assigned(CommonErrors::success);
  return_code_assigned = return_code_;
  EXPECT_EQ(error_.code(), return_code_assigned.error().code());
  EXPECT_EQ(error_.code().category(), return_code_assigned.error().code().category());
  EXPECT_EQ(data_, return_code_assigned.data());
  if (!GetParam())
    EXPECT_FALSE(return_code_assigned.data().IsInitialised());
  EXPECT_EQ(serialised_return_code, return_code_assigned.Serialise());

  ReturnCode return_code_moved(std::move(return_code_copy));
  EXPECT_EQ(error_.code(), return_code_moved.error().code());
  EXPECT_EQ(error_.code().category(), return_code_moved.error().code().category());
  EXPECT_EQ(data_, return_code_moved.data());
  if (!GetParam())
    EXPECT_FALSE(return_code_moved.data().IsInitialised());
  EXPECT_EQ(serialised_return_code, return_code_moved.Serialise());

  return_code_assigned = std::move(return_code_moved);
  EXPECT_EQ(error_.code(), return_code_assigned.error().code());
  EXPECT_EQ(error_.code().category(), return_code_assigned.error().code().category());
  EXPECT_EQ(data_, return_code_assigned.data());
  if (!GetParam())
    EXPECT_FALSE(return_code_assigned.data().IsInitialised());
  EXPECT_EQ(serialised_return_code, return_code_assigned.Serialise());

  ReturnCode return_code_parsed(serialised_return_code);
  EXPECT_EQ(error_.code(), return_code_parsed.error().code());
  EXPECT_EQ(error_.code().category(), return_code_parsed.error().code().category());
  EXPECT_EQ(data_, return_code_parsed.data());
  if (!GetParam())
    EXPECT_FALSE(return_code_parsed.data().IsInitialised());
  EXPECT_EQ(serialised_return_code, return_code_parsed.Serialise());
}

TEST_P(ReturnCodeTest, BEH_ParseFromBadString) {
  std::vector<ReturnCode::serialised_type> bad_strings;
  bad_strings.push_back(ReturnCode::serialised_type(NonEmptyString(
      RandomString(1 + RandomUint32() % 100))));
  bad_strings.push_back(ReturnCode::serialised_type(NonEmptyString()));
  auto good_string(return_code_.Serialise());
  bad_strings.push_back(
      ReturnCode::serialised_type(good_string.data + NonEmptyString(RandomString(1))));
  bad_strings.push_back(ReturnCode::serialised_type(
      NonEmptyString(good_string->string().substr(0, good_string->string().size() - 2))));

  for (auto& bad_string : bad_strings)
    EXPECT_THROW(ReturnCode parsed(bad_string), std::system_error);
}

INSTANTIATE_TEST_CASE_P(NonEmptyAndEmptyInfo, ReturnCodeTest, testing::Bool());

TEST(SingleReturnCodeTest, BEH_BadDataSize) {
  EXPECT_THROW(ReturnCode(CommonErrors::success, NonEmptyString("")), std::system_error);
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
