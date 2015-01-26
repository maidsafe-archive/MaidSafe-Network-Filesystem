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
#ifndef MAIDSAFE_NFS_DETAIL_NETWORK_BACKEND_H_
#define MAIDSAFE_NFS_DETAIL_NETWORK_BACKEND_H_

#include <cstdint>
#include <stdexcept>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)
#endif
#include "boost/thread/future.hpp"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "boost/throw_exception.hpp"

#include "maidsafe/common/data_types/immutable_data.h"
#include "maidsafe/common/error.h"
#include "maidsafe/nfs/client/maid_node_nfs.h"
#include "maidsafe/nfs/container_version.h"
#include "maidsafe/nfs/detail/container_id.h"
#include "maidsafe/nfs/detail/network.h"

namespace maidsafe {
namespace nfs {
namespace detail {

// For legacy reasons, the network and disk versions are not using virtual dispatch
class NetworkBackend : public Network::Interface {
 public:
  template<typename... Args>
  explicit NetworkBackend(Args... args)
    : Network::Interface(),
      backend_(nfs_client::MaidNodeNfs::MakeShared(std::forward<Args>(args)...)) {
    if (backend_ == nullptr) {
      BOOST_THROW_EXCEPTION(std::system_error(make_error_code(CommonErrors::null_pointer)));
    }
  }

  virtual ~NetworkBackend();

 private:
  NetworkBackend(const NetworkBackend&) = delete;
  NetworkBackend(NetworkBackend&&) = delete;

  NetworkBackend& operator=(const NetworkBackend&) = delete;
  NetworkBackend& operator=(NetworkBackend&&) = delete;

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

 private:
  const std::shared_ptr<nfs_client::MaidNodeNfs> backend_;
};

}  // namespace detail
}  // namespace nfs
}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_DETAIL_NETWORK_BACKEND_H_
