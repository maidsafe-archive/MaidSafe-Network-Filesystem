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

#ifndef MAIDSAFE_NFS_TYPES_H_
#define MAIDSAFE_NFS_TYPES_H_

#include <cstdint>
#include <functional>
#include <ostream>
#include <string>

#include "maidsafe/data_types/structured_data_versions.h"
#include "maidsafe/data_types/data_name_variant.h"
#include "maidsafe/common/node_id.h"
#include "maidsafe/common/types.h"


namespace maidsafe {

namespace nfs {

enum class MessageAction : int32_t {
  kGetRequest,
  kGetResponse,
  kPutRequest,
  kPutResponse,
  kDeleteRequest,
  kRegisterPmidRequest,
  kRegisterPmidResponse,
  kUnregisterPmidRequest,
  kUnregisterPmidResponse,
  kDeleteBranchUntilForkRequest,
  kDeleteBranchUntilForkResponse,
  kGetBranchRequest,
  kGetBranchResponse,
  kCreateAccountRequest,
  kCreateAccountResponse,
  kRemoveAccountRequest,
  kRemoveAccountResponse,
  kGetPmidHealthRequest,
  kGetPmidHealthResponse,
  kGetPmidTotalsRequest,
  kGetPmidTotalsResponse,
  kGetPmidAccountRequest,
  kGetPmidAccountResponse,
  kStateChange,
  kSynchronise,
  kAccountTransfer
};

enum class Persona : int32_t {
  kMaidManager,
  kDataManager,
  kPmidManager,
  kPmidNode,
  kMaidNode,
  kMpidManager,
  kMpidNode,
  kVersionManager,
  kOwnerDirectoryManager,
  kGroupDirectoryManager,
  kWorldDirectoryManager,
  kDataGetter
};

template<Persona PersonaType>
struct PersonaTypes;

typedef TaggedValue<int32_t, struct MessageIdTag> MessageId;

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_TYPES_H_
