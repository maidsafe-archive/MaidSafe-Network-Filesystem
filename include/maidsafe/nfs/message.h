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
#include "maidsafe/nfs/post_message.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace protobuf { class Message; }

class Message {
 public:
  typedef TaggedValue<NonEmptyString, struct SerialisedMessageTag> serialised_type;

  template<typename MessageType>
  Message(const MessageType& message);

  explicit Message(const serialised_type& serialised_message);

  Message(const Message& other);
  Message& operator=(const Message& other);
  Message(Message&& other);

  serialised_type Serialise() const;
  bool is_data_message() const { return is_data_message_; }

  template<typename MessageType>
  MessageType inner_message();

 private:
  bool is_data_message_;
  serialised_type serialised_message_;
};

template<typename MessageType>
Message::Message(const MessageType& message)
    : is_data_message_(false),
      serialised_message_(Message::serialised_type(NonEmptyString(message.Serialise()))) {}


template<typename MessageType>
MessageType Message::inner_message() {
  return MessageType(typename MessageType::serialised_type(
                       NonEmptyString(serialised_message_->string())));
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGE_H_
