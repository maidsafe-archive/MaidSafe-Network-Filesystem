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
#include "maidsafe/nfs/detail/network_backend.h"

namespace maidsafe {
namespace nfs {
namespace detail {

NetworkBackend::~NetworkBackend() {}

boost::future<void> NetworkBackend::DoCreateSDV(
    const ContainerId& container_id,
    const ContainerVersion& initial_version,
    std::uint32_t max_versions,
    std::uint32_t max_branches) {
  return backend_->CreateVersionTree(container_id.data, initial_version, max_versions, max_branches);
}

boost::future<void> NetworkBackend::DoPutSDVVersion(
    const ContainerId& container_id,
    const ContainerVersion& old_version,
    const ContainerVersion& new_version) {
  return backend_->PutVersion(container_id.data, old_version, new_version);
}

boost::future<std::vector<ContainerVersion>> NetworkBackend::DoGetBranches(
    const ContainerId& container_id) {
  return backend_->GetVersions(container_id.data);
}

boost::future<std::vector<ContainerVersion>> NetworkBackend::DoGetBranchVersions(
    const ContainerId& container_id, const ContainerVersion& tip) {
  return backend_->GetBranch(container_id.data, tip);
}

boost::future<void> NetworkBackend::DoPutChunk(const ImmutableData& data) {
  return backend_->Put(data);
}

boost::future<ImmutableData> NetworkBackend::DoGetChunk(const ImmutableData::Name& name) {
  return backend_->Get(name);
}

}  // detail
}  // nfs
}  // maidsafe
