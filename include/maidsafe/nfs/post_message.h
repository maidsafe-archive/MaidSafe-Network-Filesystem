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

#ifndef MAIDSAFE_NFS_POST_MESSAGE_H_
#define MAIDSAFE_NFS_POST_MESSAGE_H_

#include <string>

#include "maidsafe/common/node_id.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"

#include "maidsafe/detail/data_type_values.h"

#include "maidsafe/nfs/message.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace protobuf { class PostMessage; }

class PostMessage {
 public:
  typedef TaggedValue<NonEmptyString, struct SerialisedPostMessageTag> serialised_type;

  PostMessage(PostActionType post_action_type,
              PersonaType destination_persona_type,
              Message::Source source,
              const Identity& name,
              const NonEmptyString& content,
              const asymm::Signature& signature);
  PostMessage(const PostMessage& other);
  PostMessage& operator=(const PostMessage& other);
  PostMessage(PostMessage&& other);
  PostMessage& operator=(PostMessage&& other);

  explicit PostMessage(const serialised_type& serialised_message);
  serialised_type Serialise() const;

  PostActionType post_action_type() const { return post_action_type_; }
  PersonaType destination_persona_type() const { return destination_persona_type_; }
  Message::Source source() const { return source_; }
  Identity name() const { return name_; }
  NonEmptyString content() const { return content_; }
  asymm::Signature signature() const { return signature_; }

 private:
  bool ValidateInputs() const;

  PostActionType post_action_type_;
  PersonaType destination_persona_type_;
  Message::Source source_;
  Identity name_;
  NonEmptyString content_;
  asymm::Signature signature_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_POST_MESSAGE_H_
