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

#include "maidsafe/common/node_id.h"
#include "maidsafe/common/types.h"


namespace maidsafe {

namespace nfs {

enum class MessageCategory : int32_t { kData, kGeneric, kReply };

enum class MessageAction : int32_t {
  kGet,
  kPut,
  kDelete,
  kRegisterPmid,
  kUnregisterPmid,
  kConnect,
  kGetPmidHealth,
  kNodeDown,
  kNodeUp,
  kPmidStatus,
  kGetElementList,
  kSynchronise
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
  case MessageAction::kDelete:
    action_str = "Delete";
    break;
  case MessageAction::kRegisterPmid:
    action_str = "Register PMID";
    break;
  case MessageAction::kConnect:
    action_str = "Connect";
    break;
  case MessageAction::kGetPmidHealth:
    action_str = "Get PMID Size";
    break;
  case MessageAction::kNodeDown:
    action_str = "Node Down";
    break;
  case MessageAction::kNodeUp:
    action_str = "Node Up";
    break;
  case MessageAction::kSynchronise:
    action_str = "Synchronise";
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
  kMaidAccountHolder,
  kMetadataManager,
  kPmidAccountHolder,
  kDataHolder,
  kClientMaid,
  kClientMpid,
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
    case Persona::kMaidAccountHolder:
      persona_str = "MAID Account Holder Persona";
      break;
    case Persona::kMetadataManager:
      persona_str = "Metadata Manager Persona";
      break;
    case Persona::kPmidAccountHolder:
      persona_str = "PMID Account Holder Persona";
      break;
    case Persona::kDataHolder:
      persona_str = "Data Holder Persona";
      break;
    case Persona::kClientMaid:
      persona_str = "Client MAID Persona";
      break;
    case Persona::kClientMpid:
      persona_str = "Client MPID Persona";
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

typedef TaggedValue<Identity, struct MessageIdTag> MessageId;

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_TYPES_H_
