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

#include "maidsafe/detail/data_type_values.h"

#include "maidsafe/nfs/data_message.h"
#include "maidsafe/nfs/generic_message.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace protobuf { class Message; }

enum class MessageType : int32_t {
  kDataMessage = 1,
  kGenericMessage = 2
};

class Message {
 public:
  typedef TaggedValue<NonEmptyString, struct SerialisedMessageTag> serialised_type;

  template<typename SerialisedType>
  Message(const SerialisedType& serialised_inner_message);
  template<typename SerialisedType>
  Message(const SerialisedType& serialised_inner_message, const asymm::Signature& signature);
  explicit Message(const serialised_type& serialised_message);
  Message(const Message& other);
  Message& operator=(const Message& other);
  Message(Message&& other);

  serialised_type Serialise() const;
  MessageType message_type() const { return message_type_; }

  template<typename SerialisedType>
  SerialisedType inner_message();

  asymm::Signature signature() const { return signature_; }

 private:
  MessageType message_type_;
  NonEmptyString serialised_inner_message_;
  asymm::Signature signature_;
};

template<typename SerialisedType>
Message::Message(const SerialisedType& serialised_inner_message,
                 const asymm::Signature& signature)
    : message_type_(MessageType::kDataMessage), // FIXME Fraser
      serialised_inner_message_(NonEmptyString(serialised_inner_message->string())),
      signature_(signature) {}

template<typename SerialisedType>
SerialisedType Message::inner_message() {
  return SerialisedType(serialised_inner_message_);
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGE_H_
