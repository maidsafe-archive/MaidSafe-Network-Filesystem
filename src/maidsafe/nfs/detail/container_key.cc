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
#include "maidsafe/nfs/detail/container_key.h"

#include <array>
#include <string>

#include "maidsafe/common/config.h"
#include "maidsafe/common/crypto.h"

namespace maidsafe {
namespace nfs {
namespace detail {

namespace {
std::string MakeContainerKey() {
  std::array<byte, 64> new_id{{}};

  auto& random = maidsafe::crypto::random_number_generator();
  random.GenerateBlock(&new_id[0], new_id.size());

  return std::string(new_id.begin(), new_id.end());
}
}  // namespace

ContainerKey::ContainerKey() : key_(Identity(MakeContainerKey())) {}

ContainerId ContainerKey::GetId() const {
  return ContainerId(MutableData::Name(crypto::Hash<crypto::SHA512>(key_)));
}

}  // namespace detail
}  // namespace nfs
}  // namespace maidsafe
