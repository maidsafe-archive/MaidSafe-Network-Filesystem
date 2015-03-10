/*  Copyright 2012 MaidSafe.net limited

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
#include <iostream>
#include <memory>

#include "boost/program_options/parsers.hpp"
#include "boost/program_options/variables_map.hpp"

#include "maidsafe/common/config.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/passport/passport.h"
#include "maidsafe/nfs/detail/disk_backend.h"
#include "maidsafe/nfs/detail/network_backend.h"
#include "maidsafe/nfs/tests/network_fixture.h"

namespace {
const maidsafe::DiskUsage kDefaultMaxDiskUsage(2000);

const auto create_network_backend = []() {
  return std::make_shared<maidsafe::nfs::detail::NetworkBackend>(
      maidsafe::passport::CreateMaidAndSigner());
};

const auto create_disk_backend = []() {
  // shared_ptr that erases folder when refcount == 0
  const auto disk_space = ::maidsafe::test::CreateTestPath("MaidSafe_Test_FakeStore");

  const auto delete_disk_backend =
  [disk_space](maidsafe::nfs::detail::Network::Interface* interface) {
    const std::unique_ptr<maidsafe::nfs::detail::Network::Interface> ptr(interface);
  };

  return std::shared_ptr<maidsafe::nfs::detail::Network::Interface>(
      new maidsafe::nfs::detail::DiskBackend(*disk_space, kDefaultMaxDiskUsage),
      delete_disk_backend);
};

}  // namespace

int main(int argc, char** argv) {
  {
    namespace po = boost::program_options;
    po::options_description description("NFS Test Options");
    description.add_options()
      ("local", "Use local disk for tests")
      ("network", "Use Local Network Controller for tests");

    try {
      po::variables_map options;
      po::store(
          po::command_line_parser(argc, argv).options(description).allow_unregistered().run(),
          options);
      po::notify(options);

      const bool local_test = (options.count("local") != 0);
      const bool network_test = (options.count("network") != 0);
      if (local_test && network_test) {
        throw po::error("Cannot specify --local and --network");
      }

      if (network_test) {
        maidsafe::nfs::detail::test::NetworkFixture::SetCreator(create_network_backend);
      } else {  // default to local
        maidsafe::nfs::detail::test::NetworkFixture::SetCreator(create_disk_backend);
      }
    } catch (const po::error& e) {
      // SystemLog hasn't been initialised yet
      std::cerr << e.what() << std::endl;
      std::cerr << description << std::endl;
      return EXIT_FAILURE;
    }
  }
  return maidsafe::test::ExecuteMain(argc, argv);
}
