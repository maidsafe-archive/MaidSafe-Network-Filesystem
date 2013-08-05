/* Copyright 2013 MaidSafe.net limited

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

#ifndef MAIDSAFE_NFS_MAID_NODE_DISPATCHER_H_
#define MAIDSAFE_NFS_MAID_NODE_DISPATCHER_H_

#include "maidsafe/common/error.h"
#include "maidsafe/common/types.h"
#include "maidsafe/passport/types.h"
#include "maidsafe/data_types/data_type_values.h"
#include "maidsafe/routing/message.h"
#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/types.h"


namespace maidsafe {

class OwnerDirectory;
class GroupDirectory;
class WorldDirectory;

namespace nfs {

class MaidNodeDispatcher {
 public:
  MaidNodeDispatcher(routing::Routing& routing, const passport::Maid& signing_fob);

  template<typename Data>
  void SendGetRequest(const typename Data::name_type& data_name);

  template<typename Data>
  void SendGetVersionRequest(const typename Data::name_type& data_name);

  template<typename Data>
  void SendPutRequest(const Data& data, const passport::Pmid::name_type& pmid_node_hint);

  template<typename Data>
  void SendDeleteRequest(const typename Data::name_type& data_name);

  void SendCreateAccountRequest();

  void SendRemoveAccountResponse();

  void SendRegisterPmidResponse(const passport::Pmid& pmid);

  void SendUnregisterPmidResponse(const passport::Pmid& pmid);

 private:
  MaidNodeDispatcher();
  MaidNodeDispatcher(const MaidNodeDispatcher&);
  MaidNodeDispatcher(MaidNodeDispatcher&&);
  MaidNodeDispatcher& operator=(MaidNodeDispatcher);

  routing::SingleSource Sender() const;

  routing::Routing& routing_;
  const passport::Pmid kSigningFob_;
  static const nfs::Persona kSourcePersona_;
};



// ==================== Implementation =============================================================


}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MAID_NODE_DISPATCHER_H_
