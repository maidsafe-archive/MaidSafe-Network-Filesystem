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
  typedef std::pair<std::future<std::string>, std::promise<std::string>> FuturePromisePair;

class ResponseMapperTest : public testing::Test {
 protected:
  typedef std::function<std::string(std::string&& input)> converter;

  ResponseMapperTest()
    : converter_([](std::string &&input)->std::string {
                     return input;}),
      response_mapper_(converter_) {}

  void SetPromise(std::vector<std::promise<std::string>> &promise2, std::string response) {
    if (promise2.empty())
      return;
    uint32_t index = (RandomUint32() % promise2.size());
    promise2.at(index).set_value(response);
    promise2.erase(promise2.begin() + index);
  }

  converter converter_;
  ResponseMapper<std::string, std::string, converter> response_mapper_;
};

TEST_F(ResponseMapperTest, BEH_push_back) {
  std::cout<<"\nResponseMapperTest\n";
  uint16_t num_responses(1);
  std::vector<std::string> responses;
  std::vector<std::promise<std::string>> promise1(num_responses);
  std::vector<std::promise<std::string>> promise2(num_responses);
  std::vector<std::future<std::string>> future1;
  // std::vector<std::future<std::string>> future2;
  // for (auto index(0); index < num_responses; ++index) {
  auto promise2_itr = promise2.begin();
  auto promise1_itr = promise1.begin();
  while (!promise1.empty()) {
    responses.push_back(RandomString(16));
    future1.emplace_back((*promise1_itr).get_future());
    std::future<std::string> future2 = (*promise2_itr).get_future(); 
    std::promise<std::string> promise(std::move(*promise1_itr));
    FuturePromisePair pair(std::make_pair(std::move(future2), std::move(promise)));
    response_mapper_.push_back(std::move(pair));
    promise1.erase(promise1_itr);
  }
  std::cout<<"\nResponseMapper 0000000000000\n";
  SetPromise(promise2, "First Attempt");
  while (!future1.empty()) {
    std::cout<<"\nResponseMapper 1111111111111\n";
    for (auto& future : future1) {
      if (IsReady(future)) {
        std::string resp = future.get();
        auto itr = std::find(responses.begin(), responses.end(), resp);
        EXPECT_NE(responses.end(), itr);
      }
    }
  }
  std::cout<<"\nResponseMapper 22222222222222\n";

}

}  // namespace test

}  // namespace nfs
}  // namespace maidsafe
