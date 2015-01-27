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

#ifndef MAIDSAFE_NFS_TESTS_MPID_CLIENT_TEST_H_
#define MAIDSAFE_NFS_TESTS_MPID_CLIENT_TEST_H_

#include <string>
#include <utility>
#include <vector>

#include "boost/filesystem/path.hpp"

#include "maidsafe/common/test.h"
#include "maidsafe/passport/passport.h"
#include "maidsafe/routing/parameters.h"

#include "maidsafe/nfs/client/mpid_client.h"

namespace maidsafe {

namespace nfs {

namespace test {

class MpidClientTest : public testing::Test {
 public:
  MpidClientTest() {}

  ~MpidClientTest() {
    for (auto& client : clients_)
      client->Stop();
  }

 protected:
  void AddClient() {
    auto mpid_and_signer(passport::CreateMpidAndSigner());
    clients_.emplace_back(nfs_client::MpidClient::MakeShared(mpid_and_signer));
  }

  std::vector<nfs_vault::MpidMessage> messages_;
  std::vector<std::shared_ptr<nfs_client::MpidClient>> clients_;
};

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_TESTS_MPID_CLIENT_TEST_H_
