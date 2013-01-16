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
  typedef TaggedValue<NonEmptyString, struct SerialisedGenericMessageTag> serialised_type;

  GenericMessage(PostActionType post_action_type,
                 PersonaType destination_persona_type,
                 DataMessage::Source source,
                 const Identity& name,
                 const NonEmptyString& content);
  GenericMessage(const GenericMessage& other);
  GenericMessage& operator=(const GenericMessage& other);
  GenericMessage(GenericMessage&& other);
  GenericMessage& operator=(GenericMessage&& other);

  explicit GenericMessage(const serialised_type& serialised_message);
  serialised_type Serialise() const;

  PostActionType post_action_type() const { return post_action_type_; }
  PersonaType destination_persona_type() const { return destination_persona_type_; }
  DataMessage::Source source() const { return source_; }
  Identity name() const { return name_; }
  NonEmptyString content() const { return content_; }

 private:
  bool ValidateInputs() const;

  PostActionType post_action_type_;
  PersonaType destination_persona_type_;
  DataMessage::Source source_;
  Identity name_;
  NonEmptyString content_;
  int32_t message_id_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_GENERIC_MESSAGE_H_
