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
#include "maidsafe/nfs/reply.h"

namespace maidsafe {

namespace nfs {

namespace test {
  typedef std::pair<std::future<std::string>, std::promise<Reply>> FuturePromisePair;

class ResponseMapperTest : public testing::Test {
 protected:
  typedef std::function<Reply(std::string&& input)> converter;
  ResponseMapperTest()
    : converter_([](std::string &&input)->Reply {
                        try {
                          Reply reply((Reply::serialised_type(NonEmptyString(input))));
                          return reply;
                        }
                        catch(std::error_code& ec) {
                          throw ec;
                        }
                    }),
      response_mapper_(converter_),
      mutex_(),
      exception_count_(0),
      no_exception_count_(0) {}

  std::string CreateInput() {
    Reply reply(CommonErrors::success, NonEmptyString(RandomAlphaNumericString(16)));
    Reply::serialised_type ser_type = reply.Serialise();
    return (ser_type.data).string();
  }

  void SetPromise(std::vector<std::promise<std::string>> &promise2, std::string input) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (promise2.empty())
      return;
    ++no_exception_count_;
    uint32_t index = (RandomUint32() % promise2.size());
    promise2.at(index).set_value(input);
    promise2.erase(promise2.begin() + index);
  }

  void SetExceptionPromise(std::vector<std::promise<std::string>> &promise2) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (promise2.empty())
      return;
    ++exception_count_;
    uint32_t index = (RandomUint32() % promise2.size());
    Reply reply(MakeError(
        NfsErrors::failed_to_get_data), NonEmptyString());
    Reply::serialised_type ser_type = reply.Serialise();
    promise2.at(index).set_exception(std::make_exception_ptr(MakeError(
        NfsErrors::failed_to_get_data)));
    promise2.erase(promise2.begin() + index);
  }

  bool CheckOutput(std::vector<std::string> &inputs, Reply::serialised_type output) {
    return (std::find_if(inputs.begin(),
                         inputs.end(),
                         [output] (const std::string& element) {
                             return output == Reply::serialised_type(NonEmptyString(element));
                         }) != inputs.end());
  }

  converter converter_;
  ResponseMapper<std::string, Reply, converter> response_mapper_;
  std::mutex mutex_;
  uint16_t exception_count_;
  uint16_t no_exception_count_;
};

TEST_F(ResponseMapperTest, BEH_push_back) {
  maidsafe::test::RunInParallel(10, [&] {
    uint16_t num_inputs(10);
    std::vector<std::string> inputs;
    std::vector<std::promise<Reply>> promise1(num_inputs);
    std::vector<std::promise<std::string>> promise2(num_inputs);
    std::vector<std::future<Reply>> future1;
    auto promise2_itr = promise2.begin();
    auto promise1_itr = promise1.begin();
    while (!promise1.empty()) {
      inputs.push_back(CreateInput());
      future1.emplace_back((*promise1_itr).get_future());
      std::future<std::string> future2 = (*promise2_itr).get_future();
      std::promise<Reply> promise(std::move(*promise1_itr));
      FuturePromisePair pair(std::make_pair(std::move(future2), std::move(promise)));
      response_mapper_.push_back(std::move(pair));
      promise1_itr = promise1.erase(promise1_itr);
      ++promise2_itr;
    }
    //  Set promises values
    while (!promise2.empty())
      SetPromise(promise2, inputs.at(RandomInt32() % inputs.size()));

    bool done(true);
    while (done) {
      auto future_itr = future1.begin();
      while (future_itr != future1.end()) {
        if (IsReady(*future_itr)) {
          try {
            Reply reply = (*future_itr).get();
            Reply::serialised_type serialised_resp = reply.Serialise();
            EXPECT_TRUE(CheckOutput(inputs, serialised_resp));
          }
          catch(...) {
            std::cout<< "Error Occurred..." <<std::endl;
          }
          future_itr = future1.erase(future_itr);
        } else {
          ++future_itr;
        }
      }
      if (future1.empty())
        done = false;
    }
  });
}

TEST_F(ResponseMapperTest, BEH_push_back_with_exception) {
  maidsafe::test::RunInParallel(10, [&] {
    uint16_t num_inputs(1);
    std::vector<std::promise<Reply>> promise1(num_inputs);
    std::vector<std::promise<std::string>> promise2(num_inputs);
    std::vector<std::future<Reply>> future1;
    auto promise2_itr = promise2.begin();
    auto promise1_itr = promise1.begin();
    while (!promise1.empty()) {
      future1.emplace_back((*promise1_itr).get_future());
      std::future<std::string> future2 = (*promise2_itr).get_future();
      std::promise<Reply> promise(std::move(*promise1_itr));
      FuturePromisePair pair(std::make_pair(std::move(future2), std::move(promise)));
      response_mapper_.push_back(std::move(pair));
      promise1_itr = promise1.erase(promise1_itr);
      ++promise2_itr;
    }
    //  Set promises values
    while (!promise2.empty())
      SetExceptionPromise(promise2);
    bool done(true);
    while (done) {
      auto future_itr = future1.begin();
      while (future_itr != future1.end()) {
        if (IsReady(*future_itr)) {
          try {
            Reply reply = (*future_itr).get();
            Reply::serialised_type serialised_resp = reply.Serialise();
          }
          catch(const std::exception&) {
            std::cout<<"\nError Occured\n";
          }
          future_itr = future1.erase(future_itr);
        } else {
          ++future_itr;
        }
      }
      if (future1.empty())
        done = false;
    }
  });
}

TEST_F(ResponseMapperTest, BEH_push_back_Random) {
  uint16_t num_inputs(5);
  std::vector<std::string> inputs;
  std::vector<std::promise<Reply>> promise1(num_inputs);
  std::vector<std::promise<std::string>> promise2;
  std::vector<std::future<Reply>> future1;
  for (auto& promise : promise1) {
    future1.emplace_back(promise.get_future());
    inputs.push_back(CreateInput());
  }

  auto promise1_itr = promise1.begin();
  bool pending(true);
  std::mutex mutex;
  std::vector<std::future<void>> result_futures;
  while (pending) {
    uint16_t operation = (RandomInt32() % 3);
    switch (operation) {
      case(0) : {
        if (promise1.empty())
          break;
        FuturePromisePair pair;
        {
          std::lock_guard<std::mutex> lock(mutex_);
          std::promise<std::string> prom2;
          std::future<std::string> future2 = (prom2).get_future();
          promise2.push_back(std::move(prom2));
          std::promise<Reply> prom1(std::move(*promise1_itr));
          pair = std::make_pair(std::move(future2), std::move(prom1));
          promise1_itr = promise1.erase(promise1_itr);
        }
        result_futures.push_back(std::async([this, &pair] {
                                            this->response_mapper_.push_back(std::move(pair));
                                           }));
        break;
      }
      case(1) : {
        std::string value = inputs.at(RandomInt32() % inputs.size());
        result_futures.push_back(std::async([this, &promise2, value] {
                                           this->SetPromise(promise2, value);
                                           }));
        break;
      }
      case(2) : {
        result_futures.push_back(std::async([this, &promise2] {
                                            this->SetExceptionPromise(promise2);
                                           }));
        break;
      }
      default : {
        break;
      }
    }
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if ((no_exception_count_ + exception_count_) == num_inputs) {
        pending = false;
      }
    }
  }
  for (auto& result : result_futures)
    result.get();
  uint16_t exception_count(0);
  uint16_t no_exception_count(0);
  bool done(true);
  while (done) {
    auto future_itr = future1.begin();
    while (future_itr != future1.end()) {
      if (IsReady(*future_itr)) {
        try {
          Reply return_code = (*future_itr).get();
          Reply::serialised_type serialised_resp = return_code.Serialise();
          EXPECT_TRUE(CheckOutput(inputs, serialised_resp));
          ++no_exception_count;
        }
        catch(...) {
          ++exception_count;
        }
        future_itr = future1.erase(future_itr);
      } else {
        ++future_itr;
      }
    }
    if (future1.empty())
      done = false;
  }
  EXPECT_EQ((no_exception_count + exception_count), num_inputs);
}

}  // namespace test

}  // namespace nfs
}  // namespace maidsafe
