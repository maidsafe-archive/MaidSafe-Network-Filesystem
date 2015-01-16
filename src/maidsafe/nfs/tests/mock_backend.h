/*  Copyright 2013 MaidSafe.net limited

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
#ifndef MAIDSAFE_NFS_TESTS_MOCK_BACKEND_H_
#define MAIDSAFE_NFS_TESTS_MOCK_BACKEND_H_

#include <memory>

#include "gmock/gmock.h"

#include "maidsafe/common/test.h"
#include "maidsafe/nfs/detail/network.h"

namespace maidsafe {
namespace nfs {
namespace detail {
namespace test {

class MockBackend : public Network::Interface {
 public:
  explicit MockBackend(std::shared_ptr<Network::Interface> real);
  virtual ~MockBackend();

  /* GoogleMock 1.7 doesn't support move only return values. A bridge
     was created to get around this issue. */
  class Mock {
   public:
    explicit Mock(std::shared_ptr<Network::Interface> real);

    // Set mock interface behavior to invoke real version
    void SetDefaultDoCreateSDV();
    void SetDefaultDoPutSDVVersion();
    void SetDefaultDoGetBranches();
    void SetDefaultDoGetBranchVersions();
    void SetDefaultDoPutChunk();
    void SetDefaultDoGetChunk();
    void SetDefaults();

    MOCK_METHOD4(
        DoCreateSDV,
        std::shared_ptr<boost::future<void>>(
            const ContainerId& container_id,
            const ContainerVersion& initial_version,
            std::uint32_t max_versions,
            std::uint32_t max_branches));
    MOCK_METHOD3(
        DoPutSDVVersion,
        std::shared_ptr<boost::future<void>>(
            const ContainerId& container_id,
            const ContainerVersion& old_version,
            const ContainerVersion& new_version));
    MOCK_METHOD1(
        DoGetBranches,
        std::shared_ptr<boost::future<std::vector<ContainerVersion>>>(
            const ContainerId& container_id));
    MOCK_METHOD2(
        DoGetBranchVersions,
        std::shared_ptr<boost::future<std::vector<ContainerVersion>>>(
            const ContainerId& container_id,
            const ContainerVersion& tip));
    MOCK_METHOD1(
        DoPutChunk,
        std::shared_ptr<boost::future<void>>(const ImmutableData& data));
    MOCK_METHOD1(
        DoGetChunk,
        std::shared_ptr<boost::future<ImmutableData>>(const ImmutableData::Name& name));

   private:
    template<typename Function>
    class Redirect {
     private:
      struct Invoke {
        template<typename... Args>
        auto operator()(const Function& function, Network::Interface& real, Args&&... args) const
            -> decltype((real.*function)(std::forward<Args>(args)...)) {
          return (real.*function)(std::forward<Args>(args)...);
        }
      };

      template<typename... Args>
      using RedirectResult =
        typename std::result_of<Invoke(const Function&, Network::Interface&, Args...)>::type;

     public:
      Redirect(std::shared_ptr<Network::Interface> real, Function function)
        : real_(std::move(real)),
          function(std::move(function)) {
      }

      template<typename... Args>
      std::shared_ptr<RedirectResult<Args...>> operator()(Args&&... args) const {
        const auto val(std::make_shared<RedirectResult<Args...>>());
        *val = Invoke{}(function, *real_, std::forward<Args>(args)...);
        return val;
      }

    private:
      const std::shared_ptr<Network::Interface> real_;
      const Function function;
    };

    template<typename Function>
    Redirect<Function> Real(Function function) const {
      return {real_, std::move(function)};
    }

   private:
    Mock(const Mock&) = delete;
    Mock(Mock&&) = delete;

    Mock& operator=(const Mock&) = delete;
    Mock& operator=(Mock&&) = delete;

   private:
    const std::shared_ptr<Network::Interface> real_;
  } mock_;

 private:
  MockBackend(const MockBackend&) = delete;
  MockBackend(MockBackend&&) = delete;

  MockBackend& operator=(const MockBackend&) = delete;
  MockBackend& operator=(MockBackend&&) = delete;

  virtual boost::future<void> DoCreateSDV(
      const ContainerId& container_id,
      const ContainerVersion& initial_version,
      std::uint32_t max_versions,
      std::uint32_t max_branches) override final;
  virtual boost::future<void> DoPutSDVVersion(
      const ContainerId& container_id,
      const ContainerVersion& old_version,
      const ContainerVersion& new_version) override final;
  virtual boost::future<std::vector<ContainerVersion>> DoGetBranches(
      const ContainerId& container_id) override final;
  virtual boost::future<std::vector<ContainerVersion>> DoGetBranchVersions(
      const ContainerId& container_id, const ContainerVersion& tip) override final;

  virtual boost::future<void> DoPutChunk(const ImmutableData& data) override final;
  virtual boost::future<ImmutableData> DoGetChunk(const ImmutableData::Name& name) override final;
};

}  // namespace test
}  // namespace detail
}  // namespace nfs
}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_TESTS_MOCK_BACKEND_H_
