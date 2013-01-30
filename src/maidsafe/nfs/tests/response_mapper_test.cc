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
#include "maidsafe/nfs/return_code.h"

namespace maidsafe {

namespace nfs {

namespace test {
  typedef std::pair<std::future<std::string>, std::promise<ReturnCode>> FuturePromisePair;

class ResponseMapperTest : public testing::Test {
 protected:
  typedef std::function<ReturnCode(std::string&& input)> converter;

  ResponseMapperTest()
    : converter_([](std::string &&input)->ReturnCode {
                        ReturnCode return_code(
                            (ReturnCode::serialised_type(NonEmptyString(input))));
                        return return_code;
                    }),
      response_mapper_(converter_) {}

  void SetPromise(std::vector<std::promise<std::string>> &promise2, std::string response) {
    if (promise2.empty())
      return;
    uint32_t index = (RandomUint32() % promise2.size());
    promise2.at(index).set_value(response);
    promise2.erase(promise2.begin() + index);
  }

  converter converter_;
  ResponseMapper<std::string, ReturnCode, converter> response_mapper_;
};

TEST_F(ResponseMapperTest, BEH_push_back) {
  uint16_t num_responses(1);
  std::vector<std::string> responses;
  std::vector<std::promise<ReturnCode>> promise1(num_responses);
  std::vector<std::promise<std::string>> promise2(num_responses);
  std::vector<std::future<ReturnCode>> future1;
  auto promise2_itr = promise2.begin();
  auto promise1_itr = promise1.begin();
  while (!promise1.empty()) {
    future1.emplace_back((*promise1_itr).get_future());
    std::future<std::string> future2 = (*promise2_itr).get_future();
    std::promise<ReturnCode> promise(std::move(*promise1_itr));
    FuturePromisePair pair(std::make_pair(std::move(future2), std::move(promise)));
    response_mapper_.push_back(std::move(pair));
    promise1.erase(promise1_itr);
  }
  //  Set promises values
  while (!promise2.empty())
    SetPromise(promise2, responses.at(RandomInt32() % responses.size()));

  auto future_itr = future1.begin();
  while (future_itr != future1.end()) {
    if (IsReady(*future_itr)) {
      ReturnCode return_code = (*future_itr).get();
      // std::string resp; Convert returncode into string
      // auto itr = std::find(responses.begin(), responses.end(), resp);
      // EXPECT_NE(responses.end(), itr);
      future_itr = future1.erase(future_itr);
    }
    ++future_itr;
  }
}

}  // namespace test

}  // namespace nfs
}  // namespace maidsafe
