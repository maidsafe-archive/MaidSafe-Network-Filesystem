/*  Copyright 2012 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.novinet.com/license

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

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
  kAddPmid,
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
