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
