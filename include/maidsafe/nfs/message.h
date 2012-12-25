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

#include "maidsafe/common/node_id.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"

#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace protobuf { class Message; }

class Message {
 public:
  typedef TaggedValue<NonEmptyString, struct SerialisedMessageTag> serialised_type;
  Message(ActionType action_type,
          PersonaType destination_persona_type,
          PersonaType source_persona_type,
          int data_type,
          const NodeId& destination,
          const NodeId& source,
          const NonEmptyString& content,
          const asymm::Signature& signature);
  Message(const Message& other);
  Message& operator=(const Message& other);
  Message(Message&& other);
  Message& operator=(Message&& other);

  explicit Message(const serialised_type& serialised_message);
  serialised_type Serialise() const;

  ActionType action_type() const { return action_type_; }
  PersonaType destination_persona_type() const { return destination_persona_type_; }
  PersonaType source_persona_type() const { return source_persona_type_; }
  int data_type() const { return data_type_; }
  NodeId destination() const { return destination_; }
  NodeId source() const { return source_; }
  NonEmptyString content() const { return content_; }
  asymm::Signature signature() const { return signature_; }

  void set_content(const NonEmptyString& content) { content_ = content; }
  void set_signature(const asymm::Signature& signature) { signature_ = signature; }

 private:
  bool ValidateInputs() const;

  ActionType action_type_;
  PersonaType destination_persona_type_;
  PersonaType source_persona_type_;
  int data_type_;
  NodeId destination_;
  NodeId source_;
  NonEmptyString content_;
  asymm::Signature signature_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGE_H_
