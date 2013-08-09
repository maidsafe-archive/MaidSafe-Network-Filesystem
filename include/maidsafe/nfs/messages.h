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

#ifndef MAIDSAFE_NFS_MESSAGES_H_
#define MAIDSAFE_NFS_MESSAGES_H_

//#include <cstdint>
//#include <functional>
//#include <ostream>
#include <string>
//#include <system_error>
//#include <type_traits>
//#include <utility>
//#include <vector>

#include "maidsafe/common/error.h"
//#include "maidsafe/common/node_id.h"
//#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"
#include "maidsafe/data_types/data_type_values.h"
//#include "maidsafe/passport/types.h"
//
//#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

struct DataName {
  template<typename Data>
  explicit DataName(const typename Data::name_type& name)
      : type(Data::name_type::tag_type::kEnumValue),
        raw_name(name_in.data) {}

  DataName(DataTagValue type_in, const Identity& raw_name_in);

  DataName();
  DataName(const DataName& other);
  DataName(DataName&& other);
  DataName& operator=(DataName other);

  explicit DataName(const std::string& serialised_copy);
  std::string Serialise() const;

  DataTagValue type;
  Identity raw_name;
};

void swap(DataName& lhs, DataName& rhs);



struct ReturnCode {
  // This c'tor designed to be used with maidsafe-specific error enums (e.g. CommonErrors::success)
  template<typename ErrorCode>
  explicit ReturnCode(ErrorCode error_code,
                      typename std::enable_if<std::is_error_code_enum<ErrorCode>::value>::type* = 0)
      : value(MakeError(error_code)) {}

  template<typename Error>
  explicit ReturnCode(Error error,
                      typename std::enable_if<!std::is_error_code_enum<Error>::value>::type* = 0)
      : value(error) {}

  ReturnCode();
  ReturnCode(const ReturnCode& other);
  ReturnCode(ReturnCode&& other);
  ReturnCode& operator=(ReturnCode other);

  explicit ReturnCode(const std::string& serialised_copy);
  std::string Serialise() const;

  maidsafe_error value;
};

void swap(ReturnCode& lhs, ReturnCode& rhs);



struct DataNameAndContent {
  template<typename Data>
  explicit DataNameAndContent(const Data& data)
      : name<Data>(Data.name()),
        content(data.Serialise().data) {}

  DataNameAndContent(DataTagValue type_in,
                     const Identity& name_in,
                     const NonEmptyString& content_in);

  DataNameAndContent();
  DataNameAndContent(const DataNameAndContent& other);
  DataNameAndContent(DataNameAndContent&& other);
  DataNameAndContent& operator=(DataNameAndContent other);

  explicit DataNameAndContent(const std::string& serialised_copy);
  std::string Serialise() const;

  DataName name;
  NonEmptyString content;
};

void swap(DataNameAndContent& lhs, DataNameAndContent& rhs);



struct DataNameContentAndHolder {
  template<typename Data>
  DataNameContentAndHolder(const Data& data, const Identity& holder_id_in)
      : name_and_content<Data>(data),
        holder_id(holder_id_in) {}

  DataNameContentAndHolder(DataTagValue type_in,
                           const Identity& name_in,
                           const NonEmptyString& content_in,
                           const Identity& holder_id_in);

  DataNameContentAndHolder();
  DataNameContentAndHolder(const DataNameContentAndHolder& other);
  DataNameContentAndHolder(DataNameContentAndHolder&& other);
  DataNameContentAndHolder& operator=(DataNameContentAndHolder other);

  explicit DataNameContentAndHolder(const std::string& serialised_copy);
  std::string Serialise() const;

  DataNameAndContent name_and_content;
  Identity holder_id;
};

void swap(DataNameContentAndHolder& lhs, DataNameContentAndHolder& rhs);




typedef DataName MaidNodeGetRequest, MaidNodeGetVersionRequest, MaidNodeDeleteRequest;
typedef DataNameContentAndHolder MaidNodePutRequest;




/*
  struct Data {
    Data();
    Data(DataTagValue type_in,
         const Identity& name_in,
         const NonEmptyString& content_in);
    Data(DataTagValue type_in,
         const Identity& name_in,
         const Identity& originator_in,
         const NonEmptyString& content_in);
    Data(const Identity& name_in,
         const NonEmptyString& content_in);
    template<typename ErrorCode>
    explicit Data(ErrorCode error_code,
                  typename std::enable_if<std::is_error_code_enum<ErrorCode>::value>::type* = 0)
        : type(),
          name(),
          originator(),
          content(),
          error(MakeError(error_code)) {}
    template<typename Error>
    explicit Data(Error error,
                  typename std::enable_if<!std::is_error_code_enum<Error>::value>::type* = 0)
        : type(),
          name(),
          originator(),
          content(),
          error(error) {
      static_assert(std::is_same<Error, maidsafe_error>::value ||
                    std::is_base_of<maidsafe_error, Error>::value,
                    "Error type must be a MaidSafe-specific type");
    }
    Data(const Data& other);
    Data& operator=(const Data& other);
    Data(Data&& other);
    Data& operator=(Data&& other);

    bool IsSuccess() const;

    boost::optional<DataTagValue> type;
    Identity name;
    Identity originator;
    NonEmptyString content;
    boost::optional<maidsafe_error> error;
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
          Persona source_persona,
          const Data& data,
          const passport::PublicPmid::name_type& pmid_node = passport::PublicPmid::name_type());
  Message(const Message& other);
  Message& operator=(const Message& other);
  Message(Message&& other);
  Message& operator=(Message&& other);

  explicit Message(const serialised_type& serialised_message);
  // This should only be called from client NFS.  It adds a ClientValidation containing
  // source's node ID, and a signature of the data serialised.
  void SignData(const Identity& client_name, const asymm::PrivateKey& signer_private_key);
  serialised_type Serialise() const;
  std::pair<serialised_type, asymm::Signature> SerialiseAndSign(
      const asymm::PrivateKey& signer_private_key) const;
  bool Validate(const asymm::Signature& signature, const asymm::PublicKey& signer_public_key) const;

  MessageId message_id() const { return message_id_; }
  Persona destination_persona() const { return destination_persona_; }
  Persona source_persona() const { return source_persona_; }
  Data data() const { return data_; }
  ClientValidation client_validation() const { return client_validation_; }
  bool HasClientValidation() const { return client_validation_.name.IsInitialised(); }
  passport::PublicPmid::name_type pmid_node() const { return pmid_node_; }
  bool HasDataHolder() const { return pmid_node_->IsInitialised(); }

 private:
  bool ValidateInputs() const;

  MessageId message_id_;
  Persona destination_persona_;
  Persona source_persona_;
  Data data_;
  ClientValidation client_validation_;
  passport::PublicPmid::name_type pmid_node_;
};*/

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGES_H_
