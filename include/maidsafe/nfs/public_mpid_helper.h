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

#ifndef MAIDSAFE_NFS_PUBLIC_MPID_HELPER_H_
#define MAIDSAFE_NFS_PUBLIC_MPID_HELPER_H_

#include <functional>
#include <memory>
#include <vector>

#include "boost/thread/future.hpp"

#include "maidsafe/passport/types.h"
#include "maidsafe/routing/api_config.h"

namespace maidsafe {

namespace nfs {

namespace detail {

class PublicMpidHelper {
 public :
  PublicMpidHelper();
  ~PublicMpidHelper();
  void AddEntry(boost::future<passport::PublicMpid>&& future,
                routing::GivePublicKeyFunctor give_key_functors);

 private:
  void Poll();
  void Run();

  std::mutex mutex_;
  bool running_;
  std::vector<routing::GivePublicKeyFunctor> new_functors_;
  std::vector<boost::future<passport::PublicMpid>> new_futures_;
  std::future<void> worker_future_;
};

}  // namespace detail

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_PUBLIC_MPID_HELPER_H_
