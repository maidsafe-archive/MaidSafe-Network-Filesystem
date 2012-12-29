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

passport::PublicAnmid CreatePublicAnmid() {
  passport::Anmid anmid;
  return passport::PublicAnmid(anmid);
}

passport::PublicAnsmid CreatePublicAnsmid() {
  passport::Ansmid ansmid;
  return passport::PublicAnsmid(ansmid);
}

passport::PublicAntmid CreatePublicAntmid() {
  passport::Antmid antmid;
  return passport::PublicAntmid(antmid);
}

passport::PublicAnmaid CreatePublicAnmaid() {
  passport::Anmaid anmaid;
  return passport::PublicAnmaid(anmaid);
}

passport::PublicMaid CreatePublicMaid() {
  passport::Anmaid anmaid;
  passport::Maid maid(anmaid);
  return passport::PublicMaid(maid);
}

passport::PublicPmid CreatePublicPmid() {
  passport::Anmaid anmaid;
  passport::Maid maid(anmaid);
  passport::Pmid pmid(maid);
  return passport::PublicPmid(pmid);
}

passport::PublicAnmpid CreatePublicAnmpid() {
  passport::Anmpid anmpid;
  return passport::PublicAnmpid(anmpid);
}

passport::PublicMpid CreatePublicMpid() {
  passport::Anmpid anmpid;
  passport::Mpid mpid(NonEmptyString("Test"), anmpid);
  return passport::PublicMpid(mpid);
}

template<typename Data>
std::pair<Identity, NonEmptyString> GetNameAndContent() {
  switch (Data::name_type::tag_type::kEnumValue) {
    case detail::DataTagValue::kAnmidValue: {
      auto anmid(CreatePublicAnmid());
      return std::make_pair(anmid.name().data, anmid.Serialise().data);
    }
    case detail::DataTagValue::kAnsmidValue: {
      auto ansmid(CreatePublicAnsmid());
      return std::make_pair(ansmid.name().data, ansmid.Serialise().data);
    }
    case detail::DataTagValue::kAntmidValue: {
      auto antmid(CreatePublicAntmid());
      return std::make_pair(antmid.name().data, antmid.Serialise().data);
    }
    case detail::DataTagValue::kAnmaidValue: {
      auto anmaid(CreatePublicAnmaid());
      return std::make_pair(anmaid.name().data, anmaid.Serialise().data);
    }
    case detail::DataTagValue::kMaidValue: {
      auto maid(CreatePublicMaid());
      return std::make_pair(maid.name().data, maid.Serialise().data);
    }
    case detail::DataTagValue::kPmidValue: {
      auto pmid(CreatePublicPmid());
      return std::make_pair(pmid.name().data, pmid.Serialise().data);
    }
    case detail::DataTagValue::kMidValue: {
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
    case detail::DataTagValue::kSmidValue: {
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
    case detail::DataTagValue::kTmidValue: {
      const UserPassword kKeyword(RandomAlphaNumericString(20));
      const UserPassword kPassword(RandomAlphaNumericString(20));
      const uint32_t kPin(RandomUint32() % 9999 + 1);
      const NonEmptyString kMasterData(RandomString(34567));
      auto encrypted_session(passport::EncryptSession(kKeyword, kPin, kPassword, kMasterData));
      passport::Antmid antmid;
      passport::Tmid tmid(encrypted_session, antmid);
      return std::make_pair(tmid.name().data, tmid.Serialise().data);
    }
    case detail::DataTagValue::kAnmpidValue: {
      auto anmpid(CreatePublicAnmpid());
      return std::make_pair(anmpid.name().data, anmpid.Serialise().data);
    }
    case detail::DataTagValue::kMpidValue: {
      auto mpid(CreatePublicMpid());
      return std::make_pair(mpid.name().data, mpid.Serialise().data);
    }
    default:
      return std::make_pair(Identity(), NonEmptyString());
  }
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

  Message message(ActionType::kGet, PersonaType::kDataHolder, PersonaType::kClientMaid,
                  TypeParam::name_type::tag_type::kEnumValue,
                  NodeId(name_and_content.first.string()), NodeId(NodeId::kRandomId),
                  name_and_content.second, asymm::Signature());

  promise = std::make_shared<std::promise<TypeParam>>();  // NOLINT (Fraser)
  std::future<TypeParam> future2(promise->get_future());
  serialised_messages.push_back(message.Serialise().data.string());
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
