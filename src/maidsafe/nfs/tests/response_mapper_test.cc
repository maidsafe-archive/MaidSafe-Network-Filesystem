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

TEST_F(ResponseMapperTest, BEH_Simple) {
  std::promise<std::string> promise_nfs1, promise_nfs2, promise_routing1, promise_routing2;
  std::future<std::string> future_nfs1(promise_nfs1.get_future());
  std::future<std::string> future_nfs2(promise_nfs2.get_future());
  std::future<std::string> future_routing1(promise_routing1.get_future());
  std::future<std::string> future_routing2(promise_routing2.get_future());
  std::pair<std::future<std::string>, std::promise<std::string>>
      request1(std::move(future_routing1), std::move(promise_nfs1));
  response_mapper_.push_back(std::move(request1));
  std::pair<std::future<std::string>, std::promise<std::string>>
      request2(std::move(future_routing2), std::move(promise_nfs2));
  response_mapper_.push_back(std::move(request2));
  LOG(kInfo) << "Sleep(boost::posix_time::seconds(4))";
  Sleep(boost::posix_time::seconds(4));
  promise_routing1.set_value("routing response 1");
  LOG(kInfo) << "set first value for promise_routing1";
  promise_routing2.set_value("routing response 2");
  LOG(kInfo) << "set first value for promise_routing2";
  std::string response_nfs_string1(future_nfs1.get());
  std::string response_nfs_string2(future_nfs2.get());
  LOG(kInfo) << "got NFS response -- " << response_nfs_string1;
  LOG(kInfo) << "got NFS response -- " << response_nfs_string2;
}


TEST_F(ResponseMapperTest, BEH_push_back) {
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
  SetPromise(promise2, "First Attempt");
  while (!future1.empty()) {
    for (auto& future : future1) {
      if (IsReady(future)) {
        std::string resp = future.get();
        auto itr = std::find(responses.begin(), responses.end(), resp);
        EXPECT_NE(responses.end(), itr);
      }
    }
  }
}

}  // namespace test

}  // namespace nfs
}  // namespace maidsafe
