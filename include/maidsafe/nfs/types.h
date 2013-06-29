/***************************************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use.  The use of this code is governed by the licence file licence.txt found in the root of    *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit         *
 *  written permission of the board of directors of MaidSafe.net.                                  *
 **************************************************************************************************/

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
  kGet,
  kPut,
  kPutResult,
  kDelete,
  kRegisterPmid,
  kUnregisterPmid,
  kDeleteBranchUntilFork,
  kGetBranch,
  kConnect,
  kGetPmidHealth,
  kStateChange,
  kGetPmidTotals,
  kPmidTotals,
  kGetElementList,
  kSynchronise,
  kAccountTransfer,
  kCreatePmidAccount
};

template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ostream,
                                             const MessageAction &action) {
  std::string action_str;
  switch (action) {
  case MessageAction::kGet:
    action_str = "Get";
    break;
  case MessageAction::kPut:
    action_str = "Put";
    break;
  case MessageAction::kPutResult:
    action_str = "PutResult";
    break;
  case MessageAction::kDelete:
    action_str = "Delete";
    break;
  case MessageAction::kRegisterPmid:
    action_str = "Register PMID";
    break;
  case MessageAction::kUnregisterPmid:
    action_str = "Unregister PMID";
    break;
  case MessageAction::kDeleteBranchUntilFork:
    action_str = "Delete BranchUntil Fork";
    break;
  case MessageAction::kGetBranch:
    action_str = "Get Branch";
    break;
  case MessageAction::kConnect:
    action_str = "Connect";
    break;
  case MessageAction::kGetPmidHealth:
    action_str = "Get PMID Size";
    break;
  case MessageAction::kStateChange:
    action_str = "State Change";
    break;
  case MessageAction::kGetPmidTotals:
    action_str = "Get PMID Totals";
    break;
  case MessageAction::kGetElementList:
    action_str = "Get element list";
    break;
  case MessageAction::kSynchronise:
    action_str = "Synchronise";
    break;
  case MessageAction::kAccountTransfer:
    action_str = "Account Transfer";
    break;
  default:
    action_str = "Invalid message action type";
    break;
  }

  for (std::string::iterator itr(action_str.begin()); itr != action_str.end(); ++itr)
    ostream << ostream.widen(*itr);
  return ostream;
}

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

template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ostream,
                                             const Persona &persona) {
  std::string persona_str;
  switch (persona) {
    case Persona::kMaidManager:
      persona_str = "MAID Account Holder Persona";
      break;
    case Persona::kDataManager:
      persona_str = "Metadata Manager Persona";
      break;
    case Persona::kPmidManager:
      persona_str = "PMID Account Holder Persona";
      break;
    case Persona::kPmidNode:
      persona_str = "Data Holder Persona";
      break;
    case Persona::kMaidNode:
      persona_str = "Client MAID Persona";
      break;
    case Persona::kMpidNode:
      persona_str = "Client MPID Persona";
      break;
    case Persona::kVersionManager:
      persona_str = "Structured Data Manager Persona";
      break;
    case Persona::kOwnerDirectoryManager:
      persona_str = "Owner Directory Manager Persona";
      break;
    case Persona::kGroupDirectoryManager:
      persona_str = "Group Directory Manager Persona";
      break;
    case Persona::kWorldDirectoryManager:
      persona_str = "World Directory Manager Persona";
      break;
    case Persona::kDataGetter:
      persona_str = "Data Getter Persona";
      break;
    default:
      persona_str = "Invalid persona type";
      break;
  }

  for (std::string::iterator itr(persona_str.begin()); itr != persona_str.end(); ++itr)
    ostream << ostream.widen(*itr);
  return ostream;
}

template<Persona PersonaType>
struct PersonaTypes;

typedef TaggedValue<Identity, struct MessageIdTag> MessageId;

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_TYPES_H_
