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
  struct Source {
    Source(PersonaType persona_type_in, const NodeId& node_id_in)
        : persona_type(persona_type_in),
          node_id(node_id_in) {}
    Source() : persona_type(), node_id() {}
    PersonaType persona_type;
    NodeId node_id;
  };

  typedef TaggedValue<NonEmptyString, struct SerialisedMessageTag> serialised_type;

  Message(ActionType action_type,
          PersonaType destination_persona_type,
          Source source,
          maidsafe::detail::DataTagValue data_type,
          const Identity& name,
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
  Source source() const { return source_; }
  maidsafe::detail::DataTagValue data_type() const { return data_type_; }
  Identity name() const { return name_; }
  NonEmptyString content() const { return content_; }
  asymm::Signature signature() const { return signature_; }
  // TODO(Team) : implementation of id()
  NonEmptyString id() const { return NonEmptyString(); }

 private:
  bool ValidateInputs() const;

  ActionType action_type_;
  PersonaType destination_persona_type_;
  Source source_;
  maidsafe::detail::DataTagValue data_type_;
  Identity name_;
  NonEmptyString content_;
  asymm::Signature signature_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGE_H_
