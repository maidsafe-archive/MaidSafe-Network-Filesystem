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

#ifndef MAIDSAFE_NFS_DATA_MESSAGE_H_
#define MAIDSAFE_NFS_DATA_MESSAGE_H_

#include <functional>
#include <ostream>
#include <string>
#include <utility>

#include "maidsafe/common/node_id.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"
#include "maidsafe/data_types/data_type_values.h"
#include "maidsafe/passport/types.h"

#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

class DataMessage {
 public:
  enum class Action : int { kGet, kPut, kDelete };
  struct Data {
    Data();
    Data(DataTagValue type_in,
         const Identity& name_in,
         const NonEmptyString& content_in,
         Action action_in);
    Data(const Data& other);
    Data& operator=(const Data& other);
    Data(Data&& other);
    Data& operator=(Data&& other);

    DataTagValue type;
    Identity name;
    NonEmptyString content;
    Action action;
  };
  struct Originator {
    Originator();
    Originator(const passport::PublicMaid::name_type& name_in,
               const asymm::Signature& data_signature_in);
    Originator(const Originator& other);
    Originator& operator=(const Originator& other);
    Originator(Originator&& other);
    Originator& operator=(Originator&& other);

    Identity name;
    asymm::Signature data_signature;
  };

  typedef TaggedValue<NonEmptyString, struct SerialisedDataMessageTag> serialised_type;
  typedef std::function<void(DataMessage message)> OnError;
  static const int32_t message_type_identifier;

  DataMessage(Persona next_persona,
              const MessageSource& this_persona,
              const Data& data,
              const passport::PublicPmid::name_type& data_holder_hint =
                  passport::PublicPmid::name_type());
  DataMessage(const DataMessage& other);
  DataMessage& operator=(const DataMessage& other);
  DataMessage(DataMessage&& other);
  DataMessage& operator=(DataMessage&& other);

  explicit DataMessage(const serialised_type& serialised_message);
  // This should only be called from client NFS.  It adds an Originator containing this_persona's
  // node ID, and a signature of the data serialised.
  void SignData(const asymm::PrivateKey& signer_private_key);
  serialised_type Serialise() const;
  std::pair<serialised_type, asymm::Signature> SerialiseAndSign(
      const asymm::PrivateKey& signer_private_key) const;
  bool Validate(const asymm::Signature& signature,
                const asymm::PublicKey& signer_public_key) const;

  MessageId message_id() const { return message_id_; }
  Persona next_persona() const { return next_persona_; }
  MessageSource this_persona() const { return this_persona_; }
  Data data() const { return data_; }
  Originator originator() const { return originator_; }
  bool HasOriginator() const { return originator_.name.IsInitialised(); }
  passport::PublicPmid::name_type data_holder_hint() const { return data_holder_hint_; }
  bool HasDataHolderHint() const { return data_holder_hint_->IsInitialised(); }

 private:
  bool ValidateInputs() const;

  MessageId message_id_;
  Persona next_persona_;
  MessageSource this_persona_;
  Data data_;
  Originator originator_;
  passport::PublicPmid::name_type data_holder_hint_;
};


template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& ostream,
                                             const DataMessage::Action &action) {
  std::string action_str;
  switch (action) {
    case DataMessage::Action::kGet:
      action_str = "Get";
      break;
    case DataMessage::Action::kPut:
      action_str = "Put";
      break;
    case DataMessage::Action::kDelete:
      action_str = "Delete";
      break;
    default:
      action_str = "Invalid DataMessage action type";
      break;
  }

  for (std::string::iterator itr(action_str.begin()); itr != action_str.end(); ++itr)
    ostream << ostream.widen(*itr);
  return ostream;
}



}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_DATA_MESSAGE_H_
