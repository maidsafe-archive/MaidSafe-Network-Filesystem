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

#include "maidsafe/nfs/utils.h"

#include <future>
#include <memory>
#include <string>
#include <vector>

#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/passport/passport.h"

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/nfs.h"


namespace maidsafe {

namespace nfs {

namespace test {

template<typename Data>
std::pair<Identity, NonEmptyString> GetNameAndContent();

template<typename Fob>
std::pair<Identity, NonEmptyString> MakeNameAndContentPair(const Fob& fob) {
  maidsafe::passport::detail::PublicFob<typename Fob::name_type::tag_type> public_fob(fob);
  return std::make_pair(public_fob.name().data, public_fob.Serialise().data);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<passport::PublicAnmid>() {
  passport::Anmid anmid;
  return MakeNameAndContentPair(anmid);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<passport::PublicAnsmid>() {
  passport::Ansmid ansmid;
  return MakeNameAndContentPair(ansmid);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<passport::PublicAntmid>() {
  passport::Antmid antmid;
  return MakeNameAndContentPair(antmid);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<passport::PublicAnmaid>() {
  passport::Anmaid anmaid;
  return MakeNameAndContentPair(anmaid);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<passport::PublicMaid>() {
  passport::Anmaid anmaid;
  passport::Maid maid(anmaid);
  return MakeNameAndContentPair(maid);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<passport::PublicPmid>() {
  passport::Anmaid anmaid;
  passport::Maid maid(anmaid);
  passport::Pmid pmid(maid);
  return MakeNameAndContentPair(pmid);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<passport::PublicAnmpid>() {
  passport::Anmpid anmpid;
  return MakeNameAndContentPair(anmpid);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<passport::PublicMpid>() {
  passport::Anmpid anmpid;
  passport::Mpid mpid(NonEmptyString("Test"), anmpid);
  return MakeNameAndContentPair(mpid);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<passport::Mid>() {
  const UserPassword kKeyword(RandomAlphaNumericString(20));
  const UserPassword kPassword(RandomAlphaNumericString(20));
  const uint32_t kPin(RandomUint32() % 9999 + 1);
  const NonEmptyString kMasterData(RandomString(34567));
  auto encrypted_session(passport::EncryptSession(kKeyword, kPin, kPassword, kMasterData));
  passport::Antmid antmid;
  passport::Tmid tmid(encrypted_session, antmid);

  passport::Anmid anmid;
  passport::Mid mid(passport::MidName(kKeyword, kPin),
                    passport::EncryptTmidName(kKeyword, kPin, tmid.name()),
                    anmid);
  return std::make_pair(mid.name().data, mid.Serialise().data);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<passport::Smid>() {
  const UserPassword kKeyword(RandomAlphaNumericString(20));
  const UserPassword kPassword(RandomAlphaNumericString(20));
  const uint32_t kPin(RandomUint32() % 9999 + 1);
  const NonEmptyString kMasterData(RandomString(34567));
  auto encrypted_session(passport::EncryptSession(kKeyword, kPin, kPassword, kMasterData));
  passport::Antmid antmid;
  passport::Tmid tmid(encrypted_session, antmid);

  passport::Ansmid ansmid;
  passport::Smid smid(passport::SmidName(kKeyword, kPin),
                      passport::EncryptTmidName(kKeyword, kPin, tmid.name()),
                      ansmid);
  return std::make_pair(smid.name().data, smid.Serialise().data);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<passport::Tmid>() {
  const UserPassword kKeyword(RandomAlphaNumericString(20));
  const UserPassword kPassword(RandomAlphaNumericString(20));
  const uint32_t kPin(RandomUint32() % 9999 + 1);
  const NonEmptyString kMasterData(RandomString(34567));
  auto encrypted_session(passport::EncryptSession(kKeyword, kPin, kPassword, kMasterData));
  passport::Antmid antmid;
  passport::Tmid tmid(encrypted_session, antmid);
  return std::make_pair(tmid.name().data, tmid.Serialise().data);
}

template<typename T>
class UtilsTest : public testing::Test {
};

TYPED_TEST_CASE_P(UtilsTest);

TYPED_TEST_P(UtilsTest, BEH_TestHandleGetResponse) {
  auto promise(std::make_shared<std::promise<TypeParam>>());  // NOLINT (Fraser)
  std::future<TypeParam> future1(promise->get_future());
  std::vector<std::string> serialised_messages;
  HandleGetResponse<TypeParam>(promise, serialised_messages);
  EXPECT_THROW(future1.get(), nfs_error);

  std::pair<Identity, NonEmptyString> name_and_content(GetNameAndContent<TypeParam>());

  PersonaType destination_persona_type(PersonaType::kDataHolder);
  DataMessage::Source source(PersonaType::kClientMaid, NodeId(NodeId::kRandomId));
  DataMessage data_message(ActionType::kGet, destination_persona_type, source,
                           TypeParam::name_type::tag_type::kEnumValue, name_and_content.first,
                           name_and_content.second, asymm::Signature());
  Message message(data_message);
  promise = std::make_shared<std::promise<TypeParam>>();  // NOLINT (Fraser)
  std::future<TypeParam> future2(promise->get_future());
  serialised_messages.push_back(message.Serialise()->string());
  HandleGetResponse<TypeParam>(promise, serialised_messages);
  auto data(future2.get());
  EXPECT_EQ(data.name().data, name_and_content.first);
}

REGISTER_TYPED_TEST_CASE_P(UtilsTest,
                           BEH_TestHandleGetResponse);

typedef testing::Types<passport::PublicAnmid,
                       passport::PublicAnsmid,
                       passport::PublicAntmid,
                       passport::PublicAnmaid,
                       passport::PublicMaid,
                       passport::PublicPmid,
                       passport::Mid,
                       passport::Smid,
                       passport::Tmid,
                       passport::PublicAnmpid,
                       passport::PublicMpid> DataTypes;
INSTANTIATE_TYPED_TEST_CASE_P(All, UtilsTest, DataTypes);

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
