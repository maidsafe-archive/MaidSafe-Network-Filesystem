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

#ifndef MAIDSAFE_NFS_TYPES_H_
#define MAIDSAFE_NFS_TYPES_H_

#include <cstdint>

#include "maidsafe/common/tagged_value.h"
#include "maidsafe/common/type_macros.h"

namespace maidsafe {

namespace nfs {

// Defines:
//     enum class MessageAction : int32_t { kGetRequest, kGetResponse, ... };
// Also defines a std::ostream operator<< for the MessageAction.
DEFINE_OSTREAMABLE_ENUM_VALUES(MessageAction, int32_t,
    (GetRequest)
    (GetResponse)
    (GetCachedResponse)
    (PutRequest)
    (PutResponse)
    (PutFailure)
    (DeleteRequest)
    (GetVersionsRequest)
    (GetVersionsResponse)
    (GetBranchRequest)
    (GetBranchResponse)
    (PutVersionRequest)
    (PutVersionResponse)
    (DeleteBranchUntilForkRequest)
    (DeleteBranchUntilForkResponse)
    (CreateAccountRequest)
    (CreateAccountResponse)
    (RemoveAccountRequest)
    (RemoveAccountResponse)
    (Synchronise)
    (AccountTransfer)
    (AccountQuery)
    (AccountQueryResponse)
    (AddPmid)
    (RemovePmid)
    (IntegrityCheckRequest)
    (IntegrityCheckResponse)
    (CreatePmidAccountRequest)
    (CreateVersionTreeRequest)
    (CreateVersionTreeResponse)
    (UpdateAccount)
    (SendMessageRequest)
    (SendMessageResponse)
    (GetMessageRequest)
    (GetMessageResponse)
    (SendAlert)
    (DeleteAlert)
    (NoOperation))  // NoOperation is added to avoid re-definition of types error in
                    // vault::message_types.
// Defines:
//     enum class Persona : int32_t { kMaidNode, kMpidNode, ... };
// Also defines a std::ostream operator<< for the Persona.
DEFINE_OSTREAMABLE_ENUM_VALUES(Persona, int32_t,
    (MaidNode)
    (MpidNode)
    (DataGetter)
    (MaidManager)
    (DataManager)
    (PmidManager)
    (PmidNode)
    (MpidManager)
    (VersionHandler)
    (CacheHandler)
    (NA))

template <Persona PersonaType>
struct DestinationPersona {
  static const Persona value = PersonaType;
};

template <Persona PersonaType>
const Persona DestinationPersona<PersonaType>::value;

template <Persona PersonaType>
struct SourcePersona {
  static const Persona value = PersonaType;
};

template <Persona PersonaType>
const Persona SourcePersona<PersonaType>::value;

template <Persona PersonaType>
struct PersonaTypes;

namespace detail {
struct MessageIdTag;
}
typedef TaggedValue<int32_t, detail::MessageIdTag> MessageId;

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_TYPES_H_
