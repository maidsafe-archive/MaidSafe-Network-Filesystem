/*  Copyright 2014 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */
#ifndef MAIDSAFE_NFS_DETAIL_NETWORK_H_
#define MAIDSAFE_NFS_DETAIL_NETWORK_H_

#include <condition_variable>
#include <mutex>
#include <utility>
#include <thread>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)
#endif
#include "boost/thread/future.hpp"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "maidsafe/common/config.h"
#include "maidsafe/common/data_types/immutable_data.h"
#include "maidsafe/nfs/container_version.h"
#include "maidsafe/nfs/detail/async_result.h"
#include "maidsafe/nfs/detail/container_id.h"
#include "maidsafe/nfs/expected.h"

namespace maidsafe {
namespace nfs {
namespace detail {

class Network {
 private:
  MAIDSAFE_CONSTEXPR_OR_CONST static std::uint32_t kMaxBranches = 1;
  MAIDSAFE_CONSTEXPR_OR_CONST static std::uint32_t kMaxVersions = 100;

 public:
  /* This inner class exists so that on destruction the interface can be
     destroyed first, and then the internal thread can wait for all of the
     outstanding requests to be canceled. If routing_v2 provides an async_result
     interface, the extra thread here can be dropped and these interface
     functions can be moved to the outter class. */
  class Interface {
   public:
    Interface();
    virtual ~Interface() = 0;

    virtual boost::future<void> DoCreateSDV(
        const ContainerId& container_id,
        const ContainerVersion& initial_version,
        std::uint32_t max_versions,
        std::uint32_t max_branches) = 0;
    virtual boost::future<void> DoPutSDVVersion(
        const ContainerId& container_id,
        const ContainerVersion& old_version,
        const ContainerVersion& new_version) = 0;
    virtual boost::future<std::vector<ContainerVersion>> DoGetBranches(
        const ContainerId& container_id) = 0;
    virtual boost::future<std::vector<ContainerVersion>> DoGetBranchVersions(
        const ContainerId& container_id, const ContainerVersion& tip) = 0;

    virtual boost::future<void> DoPutChunk(const ImmutableData& data) = 0;
    virtual boost::future<ImmutableData> DoGetChunk(const ImmutableData::Name& name) = 0;

   private:
    Interface(const Interface&) = delete;
    Interface(Interface&&) = delete;

    Interface& operator=(const Interface&) = delete;
    Interface& operator=(Interface&&) = delete;
  };

  explicit Network(std::shared_ptr<Interface> interface);
  ~Network();

  // Return max number of SDV versions stored on the network
  static MAIDSAFE_CONSTEXPR std::uint32_t GetMaxVersions() { return kMaxVersions; }

  template<typename Token>
  AsyncResultReturn<Token, void> CreateSDV(
      const ContainerId& container_id,
      const ContainerVersion& initial_version,
      Token token) {
    assert(interface_ != nullptr);
    using Handler = AsyncHandler<Token, void>;

    Handler handler(std::move(token));
    asio::async_result<Handler> result(handler);

    AddToWaitList(
        interface_->DoCreateSDV(container_id, initial_version, kMaxVersions, kMaxBranches)
        .then(boost::launch::async, Bridge<Handler>{std::move(handler)}));

    return result.get();
  }

  template<typename Token>
  AsyncResultReturn<Token, void> PutSDVVersion(
      const ContainerId& container_id,
      const ContainerVersion& previous_version,
      const ContainerVersion& new_version,
      Token token) {
    assert(interface_ != nullptr);
    using Handler = AsyncHandler<Token, void>;

    Handler handler{std::move(token)};
    asio::async_result<Handler> result{handler};

    AddToWaitList(
        interface_->DoPutSDVVersion(container_id, previous_version, new_version)
        .then(boost::launch::async, Bridge<Handler>{std::move(handler)}));

    return result.get();
  }

  template<typename Token>
  AsyncResultReturn<Token, std::vector<ContainerVersion>> GetSDVVersions(
      const ContainerId& container_id, Token token) {
    assert(interface_ != nullptr);
    using Handler = AsyncHandler<Token, std::vector<ContainerVersion>>;

    Handler handler{std::move(token)};
    asio::async_result<Handler> result{handler};

    std::weak_ptr<Interface> weak_interface{interface_};

    AddToWaitList(
        interface_->DoGetBranches(container_id)
        .then(boost::launch::async,
              [weak_interface, container_id]
              (boost::future<std::vector<ContainerVersion>> future) mutable {
                const auto branches = future.get();

                const std::shared_ptr<Interface> interface{weak_interface.lock()};
                if (interface == nullptr) {
                  BOOST_THROW_EXCEPTION(
                      std::system_error(std::make_error_code(std::errc::operation_canceled)));
                }

                if (branches.size() == 1) {
                  return interface->DoGetBranchVersions(
                      std::move(container_id), branches.front()).get();
                } else {
                  /* A fork in the SDV. A bug in the code, or someone using rogue
                     software. Do not alert via Expected, this should never
                     happen currently. */
                  BOOST_THROW_EXCEPTION(std::runtime_error("Unexpected fork in NFS SDV history"));
                }
              })
        .then(boost::launch::async, Bridge<Handler>{std::move(handler)}));

    return result.get();
  }

  template<typename Token>
  AsyncResultReturn<Token, void> PutChunk(const ImmutableData& data, Token token) {
    assert(interface_ != nullptr);
    using Handler = AsyncHandler<Token, void>;

    Handler handler{std::move(token)};
    asio::async_result<Handler> result{handler};

    AddToWaitList(
        interface_->DoPutChunk(data).then(
            boost::launch::async, Bridge<Handler>{std::move(handler)}));

    return result.get();
  }

  template<typename Token>
  AsyncResultReturn<Token, ImmutableData> GetChunk(const ImmutableData::Name& name, Token token) {
    assert(interface_ != nullptr);
    using Handler = AsyncHandler<Token, ImmutableData>;

    Handler handler{std::move(token)};
    asio::async_result<Handler> result{handler};

    AddToWaitList(
        interface_->DoGetChunk(name).then(
            boost::launch::async, Bridge<Handler>{std::move(handler)}));

    return result.get();
  }

 private:
  void WaitForTokens();
  void AddToWaitList(boost::future<void> completion_token);

  template<typename Result>
  static Expected<Result> ConvertToExpected(boost::future<Result> result) {
    try {
      return result.get();
    } catch (const std::system_error& error) {
      return boost::make_unexpected(error.code());
    }
  }

  static Expected<void> ConvertToExpected(boost::future<void> result) {
    try {
      result.get();
      return Expected<void>(boost::expect);
    } catch (const std::system_error& error) {
      return boost::make_unexpected(error.code());
    }
  }

  /* After routing_v2 settles, the SAFE and disk backends can be
     re-written to support async_result. */
  template<typename Handler>
  class Bridge {
   public:
    explicit Bridge(Handler handler) : handler_(std::move(handler)) {}

    Bridge(const Bridge&) = default;
    Bridge(Bridge&& other) : handler_(std::move(other.handler_)) {}

    Bridge& operator=(const Bridge&) = delete;
    Bridge& operator=(Bridge&&) = delete;

    template<typename Result>
    void operator()(boost::future<Result> result) {
      handler_(ConvertToExpected(std::move(result)));
    }

   private:
    Handler handler_;
  };

 private:
  Network(const Network&) = delete;
  Network(Network&&) = delete;
  Network& operator=(const Network&) = delete;
  Network& operator=(Network&&) = delete;

  std::shared_ptr<Interface> interface_;
  std::vector<boost::future<void>> waiting_tokens_;
  std::thread waiting_thread_;
  std::condition_variable waiting_notification_;
  std::mutex waiting_mutex_;
  std::atomic<bool> continue_waiting_;
};

}  // namespace detail
}  // namespace nfs
}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_DETAIL_NETWORK_H_
