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

#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <utility>

#include "maidsafe/common/node_id.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"
#include "maidsafe/data_types/data_type_values.h"
#include "maidsafe/passport/types.h"

#include "maidsafe/nfs/persona_id.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

class DataMessage {
 public:
  enum class Action : int32_t { kGet, kPut, kDelete };
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
  struct ClientValidation {
    ClientValidation();
    ClientValidation(const passport::PublicMaid::name_type& name_in,
                     const asymm::Signature& data_signature_in);
    ClientValidation(const ClientValidation& other);
    ClientValidation& operator=(const ClientValidation& other);
    ClientValidation(ClientValidation&& other);
    ClientValidation& operator=(ClientValidation&& other);

    Identity name;
    asymm::Signature data_signature;
  };

  typedef TaggedValue<NonEmptyString, struct SerialisedDataMessageTag> serialised_type;
  static const MessageCategory message_type_identifier;
  // In the context of a MaidClient doing a Put, the data_holder arg represents a hint to a
  // DataHolder the client would want to store the data (i.e. one of its own vaults).  In the
  // context of a MetadataManger sending to PmidAccountHolder, the data_holder represents the name
  // of the actual DataHolder chosen (the PmidAccountHolder should be managing this DataHolder's
  // account).
  DataMessage(Persona destination_persona,
              const PersonaId& source,
              const Data& data,
              const passport::PublicPmid::name_type& data_holder =
                  passport::PublicPmid::name_type());
  DataMessage(const DataMessage& other);
  DataMessage& operator=(const DataMessage& other);
  DataMessage(DataMessage&& other);
  DataMessage& operator=(DataMessage&& other);

  explicit DataMessage(const serialised_type& serialised_message);
  // This should only be called from client NFS.  It adds an ClientValidation containing
  // source's node ID, and a signature of the data serialised.
  void SignData(const asymm::PrivateKey& signer_private_key);
  serialised_type Serialise() const;
  std::pair<serialised_type, asymm::Signature> SerialiseAndSign(
      const asymm::PrivateKey& signer_private_key) const;
  bool Validate(const asymm::Signature& signature,
                const asymm::PublicKey& signer_public_key) const;

  MessageId message_id() const { return message_id_; }
  Persona destination_persona() const { return destination_persona_; }
  PersonaId source() const { return source_; }
  Data data() const { return data_; }
  ClientValidation client_validation() const { return client_validation_; }
  bool HasClientValidation() const { return client_validation_.name.IsInitialised(); }
  passport::PublicPmid::name_type data_holder() const { return data_holder_; }
  bool HasDataHolder() const { return data_holder_->IsInitialised(); }

 private:
  bool ValidateInputs() const;

  MessageId message_id_;
  Persona destination_persona_;
  PersonaId source_;
  Data data_;
  ClientValidation client_validation_;
  passport::PublicPmid::name_type data_holder_;
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
