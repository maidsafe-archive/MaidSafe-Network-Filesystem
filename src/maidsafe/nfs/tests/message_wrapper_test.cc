/* Copyright 2013 MaidSafe.net limited

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

#include "maidsafe/nfs/message_wrapper.h"

#include <string>

#include "boost/variant/static_visitor.hpp"
#include "boost/variant/variant.hpp"

#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/message_wrapper_variant.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace test {

namespace {

typedef MessageWrapper<MessageAction::kGetRequest,
                       DestinationPersona<Persona::kDataManager>,
                       SourcePersona<Persona::kMaidNode>> MaidNodeGet;
typedef MessageWrapper<MessageAction::kPutRequest,
                       DestinationPersona<Persona::kMaidManager>,
                       SourcePersona<Persona::kMaidNode>> MaidNodePut;
typedef MessageWrapper<MessageAction::kDeleteRequest,
                       DestinationPersona<Persona::kMaidManager>,
                       SourcePersona<Persona::kMaidNode>> MaidNodeDelete;

}  // unnamed namespace

typedef boost::variant<MaidNodePut, MaidNodeDelete> MaidManagerServiceMessages;
typedef boost::variant<MaidNodeGet> DataManagerServiceMessages;

struct MaidManagerService {
  std::string HandlePut(const MaidNodePut& message) {
    LOG(kInfo) << "MaidManager handling Put for message " << message.serialised_message;
    return message.serialised_message;
  }
  std::string HandleDelete(const MaidNodeDelete& message) {
    LOG(kInfo) << "MaidManager handling Delete for message " << message.serialised_message;
    return message.serialised_message;
  }
};

struct DataManagerService {
  std::string HandleGet(const MaidNodeGet& message) {
    LOG(kInfo) << "DataManager handling Get for message " << message.serialised_message;
    return message.serialised_message;
  }
};

class MaidManagerDemuxer : public boost::static_visitor<std::string> {
 public:
  explicit MaidManagerDemuxer(MaidManagerService& service) : service_(service) {}
  template<typename Message>
  std::string operator()(const Message&) const {
    ThrowError(CommonErrors::invalid_parameter);
    return "";
  }
  template<>
  std::string operator()(const MaidNodePut& message) const {
    return service_.HandlePut(message);
  }
  template<>
  std::string operator()(const MaidNodeDelete& message) const {
    return service_.HandleDelete(message);
  }

 private:
  MaidManagerService& service_;
};

class DataManagerDemuxer : public boost::static_visitor<std::string> {
 public:
  explicit DataManagerDemuxer(DataManagerService& service) : service_(service) {}
  template<typename Message>
  std::string operator()(const Message&) const {
    ThrowError(CommonErrors::invalid_parameter);
    return "";
  }
  template<>
  std::string operator()(const MaidNodeGet& message) const {
    return service_.HandleGet(message);
  }

 private:
  DataManagerService& service_;
};

TEST(MessageWrapperTest, BEH_CheckVariant) {
  MaidNodeGet get("m1");
  MaidNodePut put("m2");
  MaidNodeDelete del("m3");

  auto serialised_get(get.Serialise());
  auto serialised_put(put.Serialise());
  auto serialised_del(del.Serialise());

  MaidManagerService maid_manager_service;
  MaidManagerDemuxer maid_manager_demuxer(maid_manager_service);
  DataManagerService data_manager_service;
  DataManagerDemuxer data_manager_demuxer(data_manager_service);

  EXPECT_THROW(GetVariant<MaidManagerServiceMessages>(ParseMessageWrapper(serialised_get)),
               maidsafe_error);
  auto parsed_get(GetVariant<DataManagerServiceMessages>(ParseMessageWrapper(serialised_get)));
  EXPECT_THROW(boost::apply_visitor(maid_manager_demuxer, parsed_get), maidsafe_error);
  EXPECT_EQ(get.serialised_message, boost::apply_visitor(data_manager_demuxer, parsed_get));

  auto parsed_put(GetVariant<MaidManagerServiceMessages>(ParseMessageWrapper(serialised_put)));
  EXPECT_THROW(GetVariant<DataManagerServiceMessages>(ParseMessageWrapper(serialised_put)),
               maidsafe_error);
  EXPECT_EQ(put.serialised_message, boost::apply_visitor(maid_manager_demuxer, parsed_put));
  EXPECT_THROW(boost::apply_visitor(data_manager_demuxer, parsed_put), maidsafe_error);

  auto parsed_del(GetVariant<MaidManagerServiceMessages>(ParseMessageWrapper(serialised_del)));
  EXPECT_THROW(GetVariant<DataManagerServiceMessages>(ParseMessageWrapper(serialised_del)),
               maidsafe_error);
  EXPECT_EQ(del.serialised_message, boost::apply_visitor(maid_manager_demuxer, parsed_del));
  EXPECT_THROW(boost::apply_visitor(data_manager_demuxer, parsed_del), maidsafe_error);
}

//TEST_F(MessageWrapperTest, BEH_SerialiseThenParse) {
//  auto serialised_message(message_.Serialise());
//  Message recovered_message(serialised_message);
//
//  EXPECT_EQ(action_, recovered_message.data().action);
//  EXPECT_EQ(destination_persona_, recovered_message.destination_persona());
//  EXPECT_EQ(source_.persona, recovered_message.source().persona);
//  EXPECT_EQ(source_.node_id, recovered_message.source().node_id);
//  EXPECT_EQ(data_type_, recovered_message.data().type);
//  EXPECT_EQ(name_, recovered_message.data().name);
//  EXPECT_EQ(content_, recovered_message.data().content);
//  EXPECT_EQ(pmid_node_, recovered_message.pmid_node());
//}
//
//TEST_F(MessageWrapperTest, BEH_SerialiseParseReserialiseReparse) {
//  auto serialised_message(message_.Serialise());
//  Message recovered_message(serialised_message);
//
//  auto serialised_message2(recovered_message.Serialise());
//  Message recovered_message2(serialised_message2);
//
//  EXPECT_EQ(serialised_message, serialised_message2);
//  EXPECT_EQ(action_, recovered_message2.data().action);
//  EXPECT_EQ(destination_persona_, recovered_message2.destination_persona());
//  EXPECT_EQ(source_.persona, recovered_message2.source().persona);
//  EXPECT_EQ(source_.node_id, recovered_message2.source().node_id);
//  EXPECT_EQ(data_type_, recovered_message2.data().type);
//  EXPECT_EQ(name_, recovered_message2.data().name);
//  EXPECT_EQ(content_, recovered_message2.data().content);
//  EXPECT_EQ(pmid_node_, recovered_message2.pmid_node());
//}
//
//TEST_F(MessageWrapperTest, BEH_AssignMessage) {
//  Message message2 = message_;
//
//  EXPECT_EQ(action_, message2.data().action);
//  EXPECT_EQ(destination_persona_, message2.destination_persona());
//  EXPECT_EQ(source_.persona, message2.source().persona);
//  EXPECT_EQ(source_.node_id, message2.source().node_id);
//  EXPECT_EQ(data_type_, message2.data().type);
//  EXPECT_EQ(name_, message2.data().name);
//  EXPECT_EQ(content_, message2.data().content);
//  EXPECT_EQ(pmid_node_, message2.pmid_node());
//
//  Message message3(message_);
//
//  EXPECT_EQ(action_, message3.data().action);
//  EXPECT_EQ(destination_persona_, message3.destination_persona());
//  EXPECT_EQ(source_.persona, message3.source().persona);
//  EXPECT_EQ(source_.node_id, message3.source().node_id);
//  EXPECT_EQ(data_type_, message3.data().type);
//  EXPECT_EQ(name_, message3.data().name);
//  EXPECT_EQ(content_, message3.data().content);
//  EXPECT_EQ(pmid_node_, message3.pmid_node());
//
//  Message message4 = std::move(message3);
//
//  EXPECT_EQ(action_, message4.data().action);
//  EXPECT_EQ(destination_persona_, message4.destination_persona());
//  EXPECT_EQ(source_.persona, message4.source().persona);
//  EXPECT_EQ(source_.node_id, message4.source().node_id);
//  EXPECT_EQ(data_type_, message4.data().type);
//  EXPECT_EQ(name_, message4.data().name);
//  EXPECT_EQ(content_, message4.data().content);
//  EXPECT_EQ(pmid_node_, message4.pmid_node());
//
//  Message message5(std::move(message4));
//
//  EXPECT_EQ(action_, message5.data().action);
//  EXPECT_EQ(destination_persona_, message5.destination_persona());
//  EXPECT_EQ(source_.persona, message5.source().persona);
//  EXPECT_EQ(source_.node_id, message5.source().node_id);
//  EXPECT_EQ(data_type_, message5.data().type);
//  EXPECT_EQ(name_, message5.data().name);
//  EXPECT_EQ(content_, message5.data().content);
//  EXPECT_EQ(pmid_node_, message5.pmid_node());
//}
//
//TEST_F(MessageWrapperTest, BEH_DefaultValues) {
//  Message message(destination_persona_, source_, message_.data());
//  EXPECT_EQ(action_, message.data().action);
//  EXPECT_EQ(destination_persona_, message.destination_persona());
//  EXPECT_EQ(source_.persona, message.source().persona);
//  EXPECT_EQ(source_.node_id, message.source().node_id);
//  EXPECT_EQ(data_type_, message.data().type);
//  EXPECT_EQ(name_, message.data().name);
//  EXPECT_EQ(content_, message.data().content);
//  EXPECT_FALSE(message.HasDataHolder());
//}
//
//TEST_F(MessageWrapperTest, BEH_InvalidSource) {
//  PersonaId bad_source(GenerateSource());
//  bad_source.node_id = NodeId();
//  EXPECT_THROW(Message(destination_persona_, bad_source, message_.data()),
//               common_error);
//}
//
//TEST_F(MessageWrapperTest, BEH_InvalidName) {
//  EXPECT_THROW(Message(destination_persona_, source_,
//                           Message::Data(data_type_, Identity(), content_, action_)),
//               common_error);
//}
//
//TEST_F(MessageWrapperTest, BEH_SignData) {
//  EXPECT_FALSE(message_.client_validation().name.IsInitialised());
//  EXPECT_FALSE(message_.client_validation().data_signature.IsInitialised());
//  EXPECT_FALSE(message_.HasClientValidation());
//  asymm::Keys keys(asymm::GenerateKeyPair());
//
//  Message::Data stored_data(message_.data());
//  protobuf::Message::Data data;
//  data.set_type(static_cast<uint32_t>(*stored_data.type));
//  data.set_name(stored_data.name.string());
//  if (stored_data.content.IsInitialised())
//    data.set_content(stored_data.content.string());
//  data.set_action(static_cast<int32_t>(stored_data.action));
//  asymm::PlainText serialised_data(data.SerializeAsString());
//
//  message_.SignData(keys.private_key);
//  EXPECT_TRUE(message_.client_validation().name.IsInitialised());
//  EXPECT_TRUE(message_.client_validation().data_signature.IsInitialised());
//  Message::ClientValidation client_validation(message_.client_validation());
//  EXPECT_EQ(message_.source().node_id.string(), client_validation.name.string());
//  EXPECT_TRUE(asymm::CheckSignature(serialised_data, client_validation.data_signature,
//                                    keys.public_key));
//  EXPECT_TRUE(message_.HasClientValidation());
//}
//
//TEST_F(MessageWrapperTest, BEH_SerialiseAndSignThenValidate) {
//  Message::serialised_type serialised_message(message_.Serialise());
//  asymm::Keys keys(asymm::GenerateKeyPair());
//  auto result(message_.SerialiseAndSign(keys.private_key));
//  EXPECT_EQ(serialised_message.data.string(), result.first.data.string());
//  asymm::PlainText serialised_data(result.first.data.string());
//  EXPECT_TRUE(asymm::CheckSignature(serialised_data, result.second, keys.public_key));
//  serialised_data = asymm::PlainText(serialised_message.data.string());
//  EXPECT_TRUE(asymm::CheckSignature(serialised_data, result.second, keys.public_key));
//
//  EXPECT_TRUE(message_.Validate(result.second, keys.public_key));
//  EXPECT_FALSE(message_.Validate(asymm::Signature(RandomString(256)), keys.public_key));
//  asymm::Keys false_keys(asymm::GenerateKeyPair());
//  EXPECT_FALSE(message_.Validate(asymm::Signature(result.second), false_keys.public_key));
//}

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
