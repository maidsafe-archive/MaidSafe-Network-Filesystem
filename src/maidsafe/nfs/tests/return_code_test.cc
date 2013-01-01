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
  ReturnCodeTest() : value_(RandomInt32()), info_() {
    if (GetParam())
      info_ = ReturnCode::Info(RandomString(1 + RandomUint32() % 4096));
  }

  int value_;
  ReturnCode::Info info_;
};

TEST_P(ReturnCodeTest, BEH_AllGood) {
  ReturnCode return_code(value_, info_);
  EXPECT_EQ(value_, return_code.value());
  EXPECT_EQ(info_, return_code.info());
  if (!GetParam())
    EXPECT_FALSE(return_code.info().IsInitialised());
  auto serialised_return_code(return_code.Serialise());

  ReturnCode return_code_copy(return_code);
  EXPECT_EQ(value_, return_code_copy.value());
  EXPECT_EQ(info_, return_code_copy.info());
  if (!GetParam())
    EXPECT_FALSE(return_code_copy.info().IsInitialised());
  EXPECT_EQ(serialised_return_code, return_code_copy.Serialise());

  ReturnCode return_code_assigned(9);
  return_code_assigned = return_code;
  EXPECT_EQ(value_, return_code_assigned.value());
  EXPECT_EQ(info_, return_code_assigned.info());
  if (!GetParam())
    EXPECT_FALSE(return_code_assigned.info().IsInitialised());
  EXPECT_EQ(serialised_return_code, return_code_assigned.Serialise());

  ReturnCode return_code_moved(std::move(return_code_copy));
  EXPECT_EQ(value_, return_code_moved.value());
  EXPECT_EQ(info_, return_code_moved.info());
  if (!GetParam())
    EXPECT_FALSE(return_code_moved.info().IsInitialised());
  EXPECT_EQ(serialised_return_code, return_code_moved.Serialise());

  return_code_assigned = std::move(return_code_moved);
  EXPECT_EQ(value_, return_code_assigned.value());
  EXPECT_EQ(info_, return_code_assigned.info());
  if (!GetParam())
    EXPECT_FALSE(return_code_assigned.info().IsInitialised());
  EXPECT_EQ(serialised_return_code, return_code_assigned.Serialise());

  ReturnCode return_code_parsed(serialised_return_code);
  EXPECT_EQ(value_, return_code_parsed.value());
  EXPECT_EQ(info_, return_code_parsed.info());
  if (!GetParam())
    EXPECT_FALSE(return_code_parsed.info().IsInitialised());
  EXPECT_EQ(serialised_return_code, return_code_parsed.Serialise());
}

TEST_P(ReturnCodeTest, BEH_ParseFromBadString) {
  ReturnCode return_code(value_, info_);

  std::vector<ReturnCode::serialised_type> bad_strings;
  bad_strings.push_back(ReturnCode::serialised_type(NonEmptyString(
      RandomString(1 + RandomUint32() % 100))));
  bad_strings.push_back(ReturnCode::serialised_type(NonEmptyString()));
  auto good_string(return_code.Serialise());
  bad_strings.push_back(
      ReturnCode::serialised_type(good_string.data + NonEmptyString(RandomString(1))));
  bad_strings.push_back(ReturnCode::serialised_type(
      NonEmptyString(good_string->string().substr(0, good_string->string().size() - 2))));

  for (auto& bad_string : bad_strings)
    EXPECT_THROW(ReturnCode parsed(bad_string), std::system_error);
}

INSTANTIATE_TEST_CASE_P(NonEmptyAndEmptyInfo, ReturnCodeTest, testing::Bool());

TEST(SingleReturnCodeTest, BEH_BadInfoSize) {
  EXPECT_THROW(ReturnCode(RandomInt32(), ReturnCode::Info("")), std::system_error);
  EXPECT_THROW(ReturnCode(RandomInt32(), ReturnCode::Info(std::string(4097, 0))),
               std::system_error);
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
