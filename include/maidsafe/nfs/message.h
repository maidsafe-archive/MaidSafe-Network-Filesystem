/* Copyright 2012 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

#ifndef MAIDSAFE_NFS_MESSAGE_H_
#define MAIDSAFE_NFS_MESSAGE_H_

#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "boost/optional/optional.hpp"

#include "maidsafe/common/node_id.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"
#include "maidsafe/data_types/data_type_values.h"
#include "maidsafe/passport/types.h"

#include "maidsafe/nfs/persona_id.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

class Message {
 public:
  struct Data {
    Data();
    Data(DataTagValue type_in,
         const Identity& name_in,
         const NonEmptyString& content_in,
         MessageAction action_in);
    Data(DataTagValue type_in,
         const Identity& name_in,
         const Identity& originator_in,
         const NonEmptyString& content_in,
         MessageAction action_in);
    Data(const Identity& name_in,
         const NonEmptyString& content_in,
         MessageAction action_in);
    Data(const Data& other);
    Data& operator=(const Data& other);
    Data(Data&& other);
    Data& operator=(Data&& other);

    boost::optional<DataTagValue> type;
    Identity name;
    Identity originator;
    NonEmptyString content;
    MessageAction action;
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

  typedef TaggedValue<NonEmptyString, struct SerialisedMessageTag> serialised_type;
  // In the context of a MaidClient doing a Put, the pmid_node arg represents a hint to a
  // DataHolder the client would want to store the data (i.e. one of its own vaults).  In the
  // context of a MetadataManger sending to PmidManager, the pmid_node represents the name
  // of the actual DataHolder chosen (the PmidManager should be managing this DataHolder's
  // account).
  Message(Persona destination_persona,
          const PersonaId& source,
          const Data& data,
          const passport::PublicPmid::name_type& pmid_node = passport::PublicPmid::name_type());
  Message(const Message& other);
  Message& operator=(const Message& other);
  Message(Message&& other);
  Message& operator=(Message&& other);

  explicit Message(const serialised_type& serialised_message);
  // This should only be called from client NFS.  It adds an ClientValidation containing
  // source's node ID, and a signature of the data serialised.
  void SignData(const asymm::PrivateKey& signer_private_key);
  serialised_type Serialise() const;
  std::pair<serialised_type, asymm::Signature> SerialiseAndSign(
      const asymm::PrivateKey& signer_private_key) const;
  bool Validate(const asymm::Signature& signature, const asymm::PublicKey& signer_public_key) const;

  MessageId message_id() const { return message_id_; }
  Persona destination_persona() const { return destination_persona_; }
  PersonaId source() const { return source_; }
  Data data() const { return data_; }
  ClientValidation client_validation() const { return client_validation_; }
  bool HasClientValidation() const { return client_validation_.name.IsInitialised(); }
  passport::PublicPmid::name_type pmid_node() const { return pmid_node_; }
  bool HasDataHolder() const { return pmid_node_->IsInitialised(); }

 private:
  bool ValidateInputs() const;

  MessageId message_id_;
  Persona destination_persona_;
  PersonaId source_;
  Data data_;
  ClientValidation client_validation_;
  passport::PublicPmid::name_type pmid_node_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGE_H_
