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

#include <functional>
#include <ostream>
#include <string>

#include "maidsafe/common/node_id.h"
#include "maidsafe/common/types.h"


namespace maidsafe {

namespace nfs {

enum class PersonaType : int {
  kMaidAccountHolder,
  kMetadataManager,
  kPmidAccountHolder,
  kDataHolder,
  kClientMaid,
  kClientMpid,
  kDataGetter
};

template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ostream,
                                             const PersonaType &persona_type) {
  std::string persona_type_str;
  switch (persona_type) {
    case PersonaType::kMaidAccountHolder:
      persona_type_str = "MAID Account Holder Persona";
      break;
    case PersonaType::kMetadataManager:
      persona_type_str = "Metadata Manager Persona";
      break;
    case PersonaType::kPmidAccountHolder:
      persona_type_str = "PMID Account Holder Persona";
      break;
    case PersonaType::kDataHolder:
      persona_type_str = "Data Holder Persona";
      break;
    case PersonaType::kClientMaid:
      persona_type_str = "Client MAID Persona";
      break;
    case PersonaType::kClientMpid:
      persona_type_str = "Client MPID Persona";
      break;
    case PersonaType::kDataGetter:
      persona_type_str = "Data Getter Persona";
      break;
    default:
      persona_type_str = "Invalid persona type";
      break;
  }

  for (std::string::iterator itr(persona_type_str.begin()); itr != persona_type_str.end(); ++itr)
    ostream << ostream.widen(*itr);
  return ostream;
}

struct MessageSource {
  MessageSource(PersonaType persona_type_in, const NodeId& node_id_in)
      : persona_type(persona_type_in),
        node_id(node_id_in) {}
  MessageSource() : persona_type(), node_id() {}
  PersonaType persona_type;
  NodeId node_id;
};

typedef TaggedValue<Identity, struct MessageIdTag> MessageId;


}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_TYPES_H_
