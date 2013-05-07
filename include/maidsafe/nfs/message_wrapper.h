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

#ifndef MAIDSAFE_NFS_MESSAGE_WRAPPER_H_
#define MAIDSAFE_NFS_MESSAGE_WRAPPER_H_

#include <cstdint>

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"

#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

class MessageWrapper {
 public:
  typedef TaggedValue<NonEmptyString, struct SerialisedMessageTag> serialised_type;

  MessageWrapper(MessageCategory inner_message_type,
                 const NonEmptyString& serialised_inner_message,
                 const asymm::Signature& signature = asymm::Signature());
  explicit MessageWrapper(const serialised_type& serialised_message);
  MessageWrapper(const MessageWrapper& other);
  MessageWrapper& operator=(const MessageWrapper& other);
  MessageWrapper(MessageWrapper&& other);
  MessageWrapper& operator=(MessageWrapper&& other);

  serialised_type Serialise() const;

  MessageCategory inner_message_type() const { return inner_message_type_; }
  template<typename InnerMessageType>
  typename InnerMessageType::serialised_type serialised_inner_message() const;
  asymm::Signature signature() const { return signature_; }

 private:
  MessageCategory inner_message_type_;
  NonEmptyString serialised_inner_message_;
  asymm::Signature signature_;
};

template<typename InnerMessageType>
typename InnerMessageType::serialised_type MessageWrapper::serialised_inner_message() const {
  return typename InnerMessageType::serialised_type(serialised_inner_message_);
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGE_WRAPPER_H_
