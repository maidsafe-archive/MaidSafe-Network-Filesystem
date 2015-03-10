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
#include "maidsafe/nfs/tests/network_fixture.h"

#include "maidsafe/common/error.h"

namespace maidsafe {
namespace nfs {
namespace detail {
namespace test {

namespace {
std::function<std::shared_ptr<Network::Interface>()>& GetCreator() {
  static std::function<std::shared_ptr<Network::Interface>()> instance;
  return instance;
}
}  // namespace

NetworkFixture::NetworkFixture()
  : mock_(std::make_shared<MockBackend>(Create())),
    network_(std::make_shared<Network>(mock_)) {
  GetNetworkMock().SetDefaults();
}

void NetworkFixture::SetCreator(std::function<std::shared_ptr<Network::Interface>()> creator) {
  GetCreator() = std::move(creator);
}

std::shared_ptr<Network::Interface> NetworkFixture::Create() {
  const auto& creator = GetCreator();
  if (!creator) {
    BOOST_THROW_EXCEPTION(std::system_error(make_error_code(CommonErrors::null_pointer)));
  }

  return creator();
}

NetworkFixture::~NetworkFixture() {}

}  // namespace test
}  // namespace detail
}  // namespace nfs
}  // namespace maidsafe
