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

#include <cstdint>
#include <ostream>
#include <string>

#include "maidsafe/common/node_id.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"
#include "maidsafe/data_types/data_type_values.h"
#include "maidsafe/nfs/persona_id.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

class GenericMessage {
 public:
  typedef MessageAction Action;
  struct Batch {
    Batch();
    Batch(int32_t size_in, int32_t index_in);
    Batch(const Batch& other);
    Batch& operator=(const Batch& other);
    Batch(Batch&& other);
    Batch& operator=(Batch&& other);

    int32_t size, index;
  };
  typedef TaggedValue<NonEmptyString, struct SerialisedGenericMessageTag> serialised_type;
  static const MessageCategory message_type_identifier;

  GenericMessage(Action action,
                 Persona destination_persona,
                 const PersonaId& source,
                 const Identity& name,
                 const NonEmptyString& content,
                 Batch batch = Batch());
  GenericMessage(const GenericMessage& other);
  GenericMessage& operator=(const GenericMessage& other);
  GenericMessage(GenericMessage&& other);
  GenericMessage& operator=(GenericMessage&& other);

  explicit GenericMessage(const serialised_type& serialised_message);
  serialised_type Serialise() const;

  MessageId message_id() const { return message_id_; }
  Batch batch() const { return batch_; }
  Action action() const { return action_; }
  Persona destination_persona() const { return destination_persona_; }
  PersonaId source() const { return source_; }
  Identity name() const { return name_; }
  NonEmptyString content() const { return content_; }

 private:
  bool ValidateInputs() const;

  MessageId message_id_;
  Batch batch_;
  Action action_;
  Persona destination_persona_;
  PersonaId source_;
  Identity name_;
  NonEmptyString content_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_GENERIC_MESSAGE_H_
