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

#ifndef MAIDSAFE_NFS_GENERIC_MESSAGE_H_
#define MAIDSAFE_NFS_GENERIC_MESSAGE_H_

#include <string>

#include "maidsafe/common/node_id.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"

#include "maidsafe/detail/data_type_values.h"

#include "maidsafe/nfs/data_message.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace protobuf { class GenericMessage; }

class GenericMessage {
 public:
  enum class ActionType : int {
    kRegisterPmid,
    kConnect,
    kGetPmidSize,
    kNodeDown,
    kNodeUp,
    kSynchronise
  };
  typedef TaggedValue<NonEmptyString, struct SerialisedGenericMessageTag> serialised_type;
  typedef std::function<void(GenericMessage message)> OnError;
  static const int32_t message_type_identifier = 1;

  GenericMessage(ActionType action_type,
                 PersonaType destination_persona_type,
                 const MessageSource& source,
                 const Identity& name,
                 const NonEmptyString& content);
  GenericMessage(const GenericMessage& other);
  GenericMessage& operator=(const GenericMessage& other);
  GenericMessage(GenericMessage&& other);
  GenericMessage& operator=(GenericMessage&& other);

  explicit GenericMessage(const serialised_type& serialised_message);
  serialised_type Serialise() const;

  MessageIdType message_id() const { return message_id_; }
  ActionType action_type() const { return action_type_; }
  PersonaType destination_persona_type() const { return destination_persona_type_; }
  MessageSource source() const { return source_; }
  Identity name() const { return name_; }
  NonEmptyString content() const { return content_; }

 private:
  bool ValidateInputs() const;

  MessageIdType message_id_;
  ActionType action_type_;
  PersonaType destination_persona_type_;
  MessageSource source_;
  Identity name_;
  NonEmptyString content_;
};


template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ostream,
                                             const GenericMessage::ActionType &action_type) {
  std::string action_type_str;
  switch (action_type) {
    case GenericMessage::ActionType::kRegisterPmid:
      action_type_str = "Register PMID";
      break;
    case GenericMessage::ActionType::kConnect:
      action_type_str = "Connect";
      break;
    case GenericMessage::ActionType::kGetPmidSize:
      action_type_str = "Get PMID Size";
      break;
    case GenericMessage::ActionType::kNodeDown:
      action_type_str = "Node Down";
      break;
    case GenericMessage::ActionType::kNodeUp:
      action_type_str = "Node Up";
      break;
    case GenericMessage::ActionType::kSynchronise:
      action_type_str = "Synchronise";
      break;
    default:
      action_type_str = "Invalid GenericMessage action type";
      break;
  }

  for (std::string::iterator itr(action_type_str.begin()); itr != action_type_str.end(); ++itr)
    ostream << ostream.widen(*itr);
  return ostream;
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_GENERIC_MESSAGE_H_
