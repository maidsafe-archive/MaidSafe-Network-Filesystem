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

#include "maidsafe/common/tagged_value.h"


namespace maidsafe {

namespace nfs {

enum class MessageAction : int32_t {
  kGetRequest,
  kGetResponse,
  kGetCachedResponse,
  kPutRequest,
  kPutResponse,
  kDeleteRequest,
  kGetVersionsRequest,
  kGetVersionsResponse,
  kGetBranchRequest,
  kGetBranchResponse,
  kPutVersionRequest,
  kPutVersionResponse,
  kDeleteBranchUntilForkRequest,
  kDeleteBranchUntilForkResponse,
  kCreateAccountRequest,
  kCreateAccountResponse,
  kRemoveAccountRequest,
  kRemoveAccountResponse,
  kRegisterPmidRequest,
  kRegisterPmidResponse,
  kUnregisterPmidRequest,
  kUnregisterPmidResponse,
  kGetPmidHealthRequest,
  kGetPmidHealthResponse,
  kGetPmidTotalsRequest,
  kGetPmidTotalsResponse,
  kGetPmidAccountRequest,
  kGetPmidAccountResponse,
  kStateChange,
  kSynchronise,
  kAccountTransfer,
  kIncrementSubscribers,
  kDecrementSubscribers,
  kSetPmidOnline,
  kSetPmidOffline
};

enum class Persona : int32_t {
  kMaidNode,
  kMpidNode,
  kDataGetter,
  kMaidManager,
  kDataManager,
  kPmidManager,
  kPmidNode,
  kMpidManager,
  kVersionManager
};

template<Persona PersonaType>
struct DestinationPersona {
  static const Persona value = PersonaType;
};

template<Persona PersonaType>
const Persona DestinationPersona<PersonaType>::value;

template<Persona PersonaType>
struct SourcePersona {
  static const Persona value = PersonaType;
};

template<Persona PersonaType>
const Persona SourcePersona<PersonaType>::value;

template<Persona PersonaType>
struct PersonaTypes;

namespace detail { struct MessageIdTag; }
typedef TaggedValue<int32_t, detail::MessageIdTag> MessageId;

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_TYPES_H_
