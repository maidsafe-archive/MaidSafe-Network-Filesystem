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
#include "maidsafe/nfs/tests/mock_backend.h"

#include "maidsafe/common/error.h"

namespace maidsafe {
namespace nfs {
namespace detail {
namespace test {

namespace {
template<typename Return>
Return SafeReturn(std::shared_ptr<Return> value) {
  if (value == nullptr) {
    return Return{};
  }

  return std::move(*value);
}
}  // namespace

MockBackend::MockBackend(std::shared_ptr<Network::Interface> real)
  : Network::Interface(),
    mock_(std::move(real)) {
}

MockBackend::Mock::Mock(std::shared_ptr<Network::Interface> real)
  : real_(std::move(real)) {
  if (real_ == nullptr) {
    BOOST_THROW_EXCEPTION(std::system_error(make_error_code(CommonErrors::null_pointer)));
  }
}

void MockBackend::Mock::SetDefaultDoCreateSDV() {
  using ::testing::_;
  using ::testing::Invoke;
  ON_CALL(*this, DoCreateSDV(_, _, _, _))
    .WillByDefault(Invoke(Real(&Network::Interface::DoCreateSDV)));
}

void MockBackend::Mock::SetDefaultDoPutSDVVersion() {
  using ::testing::_;
  using ::testing::Invoke;
  ON_CALL(*this, DoPutSDVVersion(_, _, _))
    .WillByDefault(Invoke(Real(&Network::Interface::DoPutSDVVersion)));
}

void MockBackend::Mock::SetDefaultDoGetBranches() {
  using ::testing::_;
  using ::testing::Invoke;
  ON_CALL(*this, DoGetBranches(_)).WillByDefault(Invoke(Real(&Network::Interface::DoGetBranches)));
}

void MockBackend::Mock::SetDefaultDoGetBranchVersions() {
  using ::testing::_;
  using ::testing::Invoke;
  ON_CALL(*this, DoGetBranchVersions(_, _))
    .WillByDefault(Invoke(Real(&Network::Interface::DoGetBranchVersions)));
}

void MockBackend::Mock::SetDefaultDoPutChunk() {
  using ::testing::_;
  using ::testing::Invoke;
  ON_CALL(*this, DoPutChunk(_)).WillByDefault(Invoke(Real(&Network::Interface::DoPutChunk)));
}

void MockBackend::Mock::SetDefaultDoGetChunk() {
  using ::testing::_;
  using ::testing::Invoke;
  ON_CALL(*this, DoGetChunk(_)).WillByDefault(Invoke(Real(&Network::Interface::DoGetChunk)));
}

void MockBackend::Mock::SetDefaults() {
  SetDefaultDoCreateSDV();
  SetDefaultDoPutSDVVersion();
  SetDefaultDoGetBranches();
  SetDefaultDoGetBranchVersions();
  SetDefaultDoPutChunk();
  SetDefaultDoGetChunk();
}

MockBackend::~MockBackend() {}

boost::future<void> MockBackend::DoCreateSDV(
    const ContainerId& container_id,
    const ContainerVersion& initial_version,
    std::uint32_t max_versions,
    std::uint32_t max_branches) {
  return SafeReturn(mock_.DoCreateSDV(container_id, initial_version, max_versions, max_branches));
}

boost::future<void> MockBackend::DoPutSDVVersion(
    const ContainerId& container_id,
    const ContainerVersion& old_version,
    const ContainerVersion& new_version) {
  return SafeReturn(mock_.DoPutSDVVersion(container_id, old_version, new_version));
}

boost::future<std::vector<ContainerVersion>> MockBackend::DoGetBranches(
    const ContainerId& container_id) {
  return SafeReturn(mock_.DoGetBranches(container_id));
}

boost::future<std::vector<ContainerVersion>> MockBackend::DoGetBranchVersions(
    const ContainerId& container_id, const ContainerVersion& tip) {
  return SafeReturn(mock_.DoGetBranchVersions(container_id, tip));
}

boost::future<void> MockBackend::DoPutChunk(const ImmutableData& data) {
  return SafeReturn(mock_.DoPutChunk(data));
}

boost::future<ImmutableData> MockBackend::DoGetChunk(const ImmutableData::Name& name) {
  return SafeReturn(mock_.DoGetChunk(name));
}

}  // namespace test
}  // namespace detail
}  // namespace nfs
}  // namespace maidsafe
