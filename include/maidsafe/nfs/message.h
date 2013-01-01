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

#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace protobuf { class Message; }

class Message {
 public:
  struct Peer {
    Peer(PersonaType persona_type_in, const NodeId& node_id_in)
        : persona_type(persona_type_in),
          node_id(node_id_in) {}
    Peer() : persona_type(), node_id() {}
    PersonaType persona_type;
    NodeId node_id;
  };
  typedef TaggedValue<Peer, struct DestinationTag> Destination;
  typedef TaggedValue<Peer, struct SourceTag> Source;

  typedef TaggedValue<NonEmptyString, struct SerialisedMessageTag> serialised_type;

  Message(ActionType action_type,
          Destination destination,
          Source source,
          maidsafe::detail::DataTagValue data_type,
          const NonEmptyString& content,
          const asymm::Signature& signature);
  Message(const Message& other);
  Message& operator=(const Message& other);
  Message(Message&& other);
  Message& operator=(Message&& other);

  explicit Message(const serialised_type& serialised_message);
  serialised_type Serialise() const;

  ActionType action_type() const { return action_type_; }
  Destination destination() const { return destination_; }
  Source source() const { return source_; }
  maidsafe::detail::DataTagValue data_type() const { return data_type_; }
  NonEmptyString content() const { return content_; }
  asymm::Signature signature() const { return signature_; }

  void set_content(const NonEmptyString& content) { content_ = content; }
  void set_signature(const asymm::Signature& signature) { signature_ = signature; }

 private:
  bool ValidateInputs() const;

  ActionType action_type_;
  Destination destination_;
  Source source_;
  maidsafe::detail::DataTagValue data_type_;
  NonEmptyString content_;
  asymm::Signature signature_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGE_H_
