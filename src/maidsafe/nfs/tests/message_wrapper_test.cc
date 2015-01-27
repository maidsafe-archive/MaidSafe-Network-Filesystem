/*  Copyright 2013 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#include "maidsafe/nfs/message_wrapper.h"

#include <string>

#include "boost/variant/static_visitor.hpp"
#include "boost/variant/variant.hpp"

#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/message_types.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"
#include "maidsafe/nfs/types.h"

namespace maidsafe {

namespace nfs {

namespace test {

namespace {

typedef PutRequestFromMaidNodeToMaidManager PutRequest;
typedef DeleteRequestFromMaidNodeToMaidManager DeleteRequest;

}  // unnamed namespace

template <typename ServiceImpl>
class PersonaDemuxer : public boost::static_visitor<std::string> {
 public:
  explicit PersonaDemuxer(ServiceImpl& service_impl) : service_impl_(service_impl) {}
  template <typename Message>
  std::string operator()(const Message& message) const {
    return service_impl_.Handle(message);
  }

 private:
  ServiceImpl& service_impl_;
};

template <typename ServiceImpl>
class Service {
 public:
  typedef typename ServiceImpl::Messages Messages;

  Service() : impl_(), demuxer_(impl_) {}

  std::string HandleMessage(const TypeErasedMessageWrapper& message) {
    Messages variant_message;
    GetVariant(message, variant_message);
    return boost::apply_visitor(demuxer_, variant_message);
  }

 private:
  ServiceImpl impl_;
  PersonaDemuxer<ServiceImpl> demuxer_;
};

class MaidManagerServiceImpl {
 public:
  typedef MaidManagerServiceMessages Messages;

  template <typename T>
  std::string Handle(const T& /*message*/) {
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::invalid_parameter));
#ifdef __GNUC__
    return std::string();
#endif
  }
};

template <>
std::string MaidManagerServiceImpl::Handle(const PutRequest& message) {
  LOG(kInfo) << "MaidManager handling Put";
  return message.contents->name.raw_name.string();
}

template <>
std::string MaidManagerServiceImpl::Handle(const DeleteRequestFromMaidNodeToMaidManager& message) {
  LOG(kInfo) << "MaidManager handling Delete";
  return message.contents->raw_name.string();
}

class DataManagerServiceImpl {
 public:
  typedef DataManagerServiceMessages Messages;

  template <typename T>
  std::string Handle(const T& /*message*/) {
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::invalid_parameter));
#ifdef __GNUC__
    return std::string();
#endif
  }
};

TEST(MessageWrapperTest, BEH_CheckVariant) {
  LOG(kInfo) << Persona::kMaidNode;
  LOG(kInfo) << Persona::kMpidNode;
  LOG(kInfo) << Persona::kDataGetter;
  LOG(kInfo) << Persona::kMaidManager;
  LOG(kInfo) << Persona::kDataManager;
  LOG(kInfo) << Persona::kPmidManager;
  LOG(kInfo) << Persona::kPmidNode;
  LOG(kInfo) << Persona::kMpidManager;
  LOG(kInfo) << Persona::kVersionHandler;
  LOG(kInfo) << Persona::kCacheHandler;
  LOG(kInfo) << static_cast<Persona>(-9);

  ImmutableData data2(NonEmptyString("data 2"));
  ImmutableData data3(NonEmptyString("data 3"));

  PutRequest::Contents put_contents;
  put_contents = nfs_vault::DataNameAndContent(data2);
  PutRequest put(put_contents);
  DeleteRequest del(DeleteRequest::Contents(data3.name()));

  auto serialised_put(put.Serialise());
  auto serialised_del(del.Serialise());

  Service<MaidManagerServiceImpl> maid_manager_service;
  Service<DataManagerServiceImpl> data_manager_service;

  auto tuple_put(ParseMessageWrapper(serialised_put));
  auto tuple_del(ParseMessageWrapper(serialised_del));

  EXPECT_EQ(data2.name()->string(), maid_manager_service.HandleMessage(tuple_put));
  EXPECT_THROW(data_manager_service.HandleMessage(tuple_put), maidsafe_error);

  EXPECT_EQ(data3.name()->string(), maid_manager_service.HandleMessage(tuple_del));
  EXPECT_THROW(data_manager_service.HandleMessage(tuple_del), maidsafe_error);
}

/*
 TEST_F(MessageWrapperTest, BEH_SerialiseThenParse) {
  auto serialised_message(message_.Serialise());
  Message recovered_message(serialised_message);

  EXPECT_EQ(action_, recovered_message.data().action);
  EXPECT_EQ(destination_persona_, recovered_message.destination_persona());
  EXPECT_EQ(source_.persona, recovered_message.source().persona);
  EXPECT_EQ(source_.node_id, recovered_message.source().node_id);
  EXPECT_EQ(data_type_, recovered_message.data().type);
  EXPECT_EQ(name_, recovered_message.data().name);
  EXPECT_EQ(content_, recovered_message.data().content);
  EXPECT_EQ(pmid_node_, recovered_message.pmid_node());
}

 TEST_F(MessageWrapperTest, BEH_SerialiseParseReserialiseReparse) {
  auto serialised_message(message_.Serialise());
  Message recovered_message(serialised_message);

  auto serialised_message2(recovered_message.Serialise());
  Message recovered_message2(serialised_message2);

  EXPECT_EQ(serialised_message, serialised_message2);
  EXPECT_EQ(action_, recovered_message2.data().action);
  EXPECT_EQ(destination_persona_, recovered_message2.destination_persona());
  EXPECT_EQ(source_.persona, recovered_message2.source().persona);
  EXPECT_EQ(source_.node_id, recovered_message2.source().node_id);
  EXPECT_EQ(data_type_, recovered_message2.data().type);
  EXPECT_EQ(name_, recovered_message2.data().name);
  EXPECT_EQ(content_, recovered_message2.data().content);
  EXPECT_EQ(pmid_node_, recovered_message2.pmid_node());
}

 TEST_F(MessageWrapperTest, BEH_AssignMessage) {
  Message message2 = message_;

  EXPECT_EQ(action_, message2.data().action);
  EXPECT_EQ(destination_persona_, message2.destination_persona());
  EXPECT_EQ(source_.persona, message2.source().persona);
  EXPECT_EQ(source_.node_id, message2.source().node_id);
  EXPECT_EQ(data_type_, message2.data().type);
  EXPECT_EQ(name_, message2.data().name);
  EXPECT_EQ(content_, message2.data().content);
  EXPECT_EQ(pmid_node_, message2.pmid_node());

  Message message3(message_);

  EXPECT_EQ(action_, message3.data().action);
  EXPECT_EQ(destination_persona_, message3.destination_persona());
  EXPECT_EQ(source_.persona, message3.source().persona);
  EXPECT_EQ(source_.node_id, message3.source().node_id);
  EXPECT_EQ(data_type_, message3.data().type);
  EXPECT_EQ(name_, message3.data().name);
  EXPECT_EQ(content_, message3.data().content);
  EXPECT_EQ(pmid_node_, message3.pmid_node());

  Message message4 = std::move(message3);

  EXPECT_EQ(action_, message4.data().action);
  EXPECT_EQ(destination_persona_, message4.destination_persona());
  EXPECT_EQ(source_.persona, message4.source().persona);
  EXPECT_EQ(source_.node_id, message4.source().node_id);
  EXPECT_EQ(data_type_, message4.data().type);
  EXPECT_EQ(name_, message4.data().name);
  EXPECT_EQ(content_, message4.data().content);
  EXPECT_EQ(pmid_node_, message4.pmid_node());

  Message message5(std::move(message4));

  EXPECT_EQ(action_, message5.data().action);
  EXPECT_EQ(destination_persona_, message5.destination_persona());
  EXPECT_EQ(source_.persona, message5.source().persona);
  EXPECT_EQ(source_.node_id, message5.source().node_id);
  EXPECT_EQ(data_type_, message5.data().type);
  EXPECT_EQ(name_, message5.data().name);
  EXPECT_EQ(content_, message5.data().content);
  EXPECT_EQ(pmid_node_, message5.pmid_node());
}

 TEST_F(MessageWrapperTest, BEH_DefaultValues) {
  Message message(destination_persona_, source_, message_.data());
  EXPECT_EQ(action_, message.data().action);
  EXPECT_EQ(destination_persona_, message.destination_persona());
  EXPECT_EQ(source_.persona, message.source().persona);
  EXPECT_EQ(source_.node_id, message.source().node_id);
  EXPECT_EQ(data_type_, message.data().type);
  EXPECT_EQ(name_, message.data().name);
  EXPECT_EQ(content_, message.data().content);
  EXPECT_FALSE(message.HasDataHolder());
}

 TEST_F(MessageWrapperTest, BEH_InvalidSource) {
  PersonaId bad_source(GenerateSource());
  bad_source.node_id = NodeId();
  EXPECT_THROW(Message(destination_persona_, bad_source, message_.data()),
               common_error);
}

 TEST_F(MessageWrapperTest, BEH_InvalidName) {
  EXPECT_THROW(Message(destination_persona_, source_,
                           Message::Data(data_type_, Identity(), content_, action_)),
               common_error);
}

 TEST_F(MessageWrapperTest, BEH_SignData) {
  EXPECT_FALSE(message_.client_validation().name.IsInitialised());
  EXPECT_FALSE(message_.client_validation().data_signature.IsInitialised());
  EXPECT_FALSE(message_.HasClientValidation());
  asymm::Keys keys(asymm::GenerateKeyPair());

  Message::Data stored_data(message_.data());
  protobuf::Message::Data data;
  data.set_type(static_cast<uint32_t>(*stored_data.type));
  data.set_name(stored_data.name.string());
  if (stored_data.content.IsInitialised())
    data.set_content(stored_data.content.string());
  data.set_action(static_cast<int32_t>(stored_data.action));
  asymm::PlainText serialised_data(data.SerializeAsString());

  message_.SignData(keys.private_key);
  EXPECT_TRUE(message_.client_validation().name.IsInitialised());
  EXPECT_TRUE(message_.client_validation().data_signature.IsInitialised());
  Message::ClientValidation client_validation(message_.client_validation());
  EXPECT_EQ(message_.source().node_id.string(), client_validation.name.string());
  EXPECT_TRUE(asymm::CheckSignature(serialised_data, client_validation.data_signature,
                                    keys.public_key));
  EXPECT_TRUE(message_.HasClientValidation());
}

 TEST_F(MessageWrapperTest, BEH_SerialiseAndSignThenValidate) {
  Message::serialised_type serialised_message(message_.Serialise());
  asymm::Keys keys(asymm::GenerateKeyPair());
  auto result(message_.SerialiseAndSign(keys.private_key));
  EXPECT_EQ(serialised_message->string(), result.first->string());
  asymm::PlainText serialised_data(result.first->string());
  EXPECT_TRUE(asymm::CheckSignature(serialised_data, result.second, keys.public_key));
  serialised_data = asymm::PlainText(serialised_message->string());
  EXPECT_TRUE(asymm::CheckSignature(serialised_data, result.second, keys.public_key));

  EXPECT_TRUE(message_.Validate(result.second, keys.public_key));
  EXPECT_FALSE(message_.Validate(asymm::Signature(RandomString(256)), keys.public_key));
  asymm::Keys false_keys(asymm::GenerateKeyPair());
  EXPECT_FALSE(message_.Validate(asymm::Signature(result.second), false_keys.public_key));
}
*/
}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
