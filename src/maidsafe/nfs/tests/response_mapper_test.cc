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

#include "maidsafe/nfs/response_mapper.h"

#include <memory>
#include <vector>

#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

namespace maidsafe {

namespace nfs {

namespace test {

class ResponseMapperTest : public testing::Test {
 protected:
  typedef std::function<std::string(std::string&& input)> converter;

  ResponseMapperTest()
    : converter_([](std::string &&input)->std::string {
                     return input;}),
      response_mapper_(converter_) {}

  converter converter_;
  ResponseMapper<std::string, std::string, converter> response_mapper_;
};

TEST_F(ResponseMapperTest, BEH_push_back) {
}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
