/* Copyright 2012 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

#ifndef MAIDSAFE_NFS_NFS_H_
#define MAIDSAFE_NFS_NFS_H_

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/passport/types.h"

#include "maidsafe/nfs/client_post_policies.h"
#include "maidsafe/nfs/data_policies.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

template<typename PutPolicy, typename GetPolicy, typename DeletePolicy, typename PostPolicy>
class NetworkFileSystem : public PutPolicy,
                          public GetPolicy,
                          public DeletePolicy,
                          public PostPolicy {
 public:
  NetworkFileSystem() : PutPolicy(), GetPolicy(), DeletePolicy(), PostPolicy() {}

  template<typename SigningFob>
  NetworkFileSystem(routing::Routing& routing, const SigningFob& signing_fob)
      : PutPolicy(routing, signing_fob),
        GetPolicy(routing),
        DeletePolicy(routing, signing_fob),
        PostPolicy(routing, signing_fob) {}
};

typedef NetworkFileSystem<ClientMaidPutPolicy,
                          ClientMaidGetPolicy,
                          ClientMaidDeletePolicy,
                          ClientMaidPostPolicy> ClientMaidNfs;


typedef DataGetterPolicy KeyGetterNfs;

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_NFS_H_
