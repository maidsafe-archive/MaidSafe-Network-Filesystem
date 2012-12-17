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

#ifndef MAIDSAFE_NFS_MESSAGE_H_
#define MAIDSAFE_NFS_MESSAGE_H_

#include <string>

#include "maidsafe/common/error.h"
#include "maidsafe/common/node_id.h"

namespace maidsafe {

namespace nfs {

enum class PersonaType : int {
  kMaidAccountHolder = 0,
  kMetaDataManager = 1,
  kPmidAccountHolder = 2,
  kDataHolder = 3
};

enum class ActionType : int {
  kGet = 0,
  kPut = 1,
  kPost = 2,
  kDelete = 3
};

class Message {
 public:
  Message(const ActionType& action_type,
          const PersonaType& source_persona_type,
          const PersonaType& destination_persona_type,
          const int& data_type,
          const NodeId& destination,
          const NodeId& source,
          const std::string& content,
          const std::string& signature);

  ActionType action_type() const { return action_type_; }
  PersonaType source_persona_type() const { return source_persona_type_; }
  PersonaType destination_persona_type() const { return destination_persona_type_; }
  int data_type() const { return data_type_; }
  NodeId destination() const { return destination_; }
  NodeId source() const { return source_; }
  std::string content() const {return content_; }
  std::string signature() const { return signature_; }

 private:
  ActionType action_type_;
  PersonaType source_persona_type_;
  PersonaType destination_persona_type_;
  int data_type_;
  NodeId destination_;
  NodeId source_;
  std::string content_;
  std::string signature_;
};

// throws
Message ParseFromString(const std::string& message_string);
std::string SerialiseAsString(const Message& message);

}  // namespace vault

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGE_H_
