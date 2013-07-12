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

#include "maidsafe/nfs/utils.h"

#include <future>
#include <memory>
#include <string>
#include <vector>

#include "maidsafe/common/crypto.h"
#include "maidsafe/common/error.h"
#include "maidsafe/common/log.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/passport/passport.h"
#include "maidsafe/passport/detail/secure_string.h"

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/data_types/immutable_data.h"
#include "maidsafe/data_types/owner_directory.h"
#include "maidsafe/data_types/group_directory.h"
#include "maidsafe/data_types/world_directory.h"

#include "maidsafe/nfs/nfs.h"
#include "maidsafe/nfs/message.h"


namespace maidsafe {

namespace nfs {

namespace test {

typedef std::vector<std::future<std::string>> RoutingFutures;

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
  const passport::detail::Keyword kKeyword(RandomAlphaNumericString(20));
  const passport::detail::Password kPassword(RandomAlphaNumericString(20));
  const passport::detail::Pin kPin(std::to_string(RandomUint32() % 9999 + 1));
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
  const passport::detail::Keyword kKeyword(RandomAlphaNumericString(20));
  const passport::detail::Password kPassword(RandomAlphaNumericString(20));
  const passport::detail::Pin kPin(std::to_string(RandomUint32() % 9999 + 1));
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
  const passport::detail::Keyword kKeyword(RandomAlphaNumericString(20));
  const passport::detail::Password kPassword(RandomAlphaNumericString(20));
  const passport::detail::Pin kPin(std::to_string(RandomUint32() % 9999 + 1));
  const NonEmptyString kMasterData(RandomString(34567));
  auto encrypted_session(passport::EncryptSession(kKeyword, kPin, kPassword, kMasterData));
  passport::Antmid antmid;
  passport::Tmid tmid(encrypted_session, antmid);
  return std::make_pair(tmid.name().data, tmid.Serialise().data);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<ImmutableData>() {
  NonEmptyString value(RandomString(RandomUint32() % 10000 + 10));
  ImmutableData immutable(value);
  return std::make_pair(immutable.name().data, immutable.Serialise().data);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<OwnerDirectory>() {
  NonEmptyString value(RandomString(RandomUint32() % 10000 + 10));
  Identity name(crypto::Hash<crypto::SHA512>(value));
  OwnerDirectory owner_directory(OwnerDirectory::name_type(name), value);
  return std::make_pair(owner_directory.name().data, owner_directory.Serialise().data);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<GroupDirectory>() {
  NonEmptyString value(RandomString(RandomUint32() % 10000 + 10));
  Identity name(crypto::Hash<crypto::SHA512>(value));
  GroupDirectory group_directory(GroupDirectory::name_type(name), value);
  return std::make_pair(group_directory.name().data, group_directory.Serialise().data);
}

template<>
std::pair<Identity, NonEmptyString> GetNameAndContent<WorldDirectory>() {
  NonEmptyString value(RandomString(RandomUint32() % 10000 + 10));
  Identity name(crypto::Hash<crypto::SHA512>(value));
  WorldDirectory world_directory(WorldDirectory::name_type(name), value);
  return std::make_pair(world_directory.name().data, world_directory.Serialise().data);
}

template<typename T>
class UtilsTest : public testing::Test {
 protected:
  std::string MakeSerialisedMessage(const std::pair<Identity, NonEmptyString>& name_and_content) {
    Persona destination_persona(Persona::kDataManager);
    PersonaId source(Persona::kMaidNode, NodeId(NodeId::kRandomId));
    Message::Data data(T::name_type::tag_type::kEnumValue, name_and_content.first,
                           name_and_content.second, MessageAction::kGet);
    Message message(destination_persona, source, data);
    MessageWrapper message_wrapper(message.Serialise());
    return message_wrapper.Serialise()->string();
  }

  RoutingFutures SendReturnsAllFailed() const {
    RoutingFutures futures;
    for (int i(0); i != 4; ++i) {
      auto promise(std::make_shared<std::promise<std::string>>());
      futures.push_back(promise->get_future());
      std::thread worker([promise, i] {
          std::this_thread::sleep_for(std::chrono::milliseconds(i * 100));
          promise->set_exception(std::make_exception_ptr(MakeError(RoutingErrors::timed_out)));
      });
      worker.detach();
    }
    return std::move(futures);
  }

  RoutingFutures SendReturnsOneSuccess(const std::string& serialised_message) const {
    RoutingFutures futures;
    int succeed_index(RandomUint32() % 4);
    for (int i(0); i != 4; ++i) {
      auto promise(std::make_shared<std::promise<std::string>>());
      futures.push_back(promise->get_future());
      std::thread worker([promise, i, succeed_index, serialised_message] {
          std::this_thread::sleep_for(std::chrono::milliseconds(i * 100));
          if (i == succeed_index) {
//            LOG(kSuccess) << "Setting value for i == " << i;
            promise->set_value(serialised_message);
          } else {
//            LOG(kWarning) << "Setting exception for i == " << i;
            promise->set_exception(std::make_exception_ptr(MakeError(RoutingErrors::timed_out)));
          }
      });
      worker.detach();
    }
    return std::move(futures);
  }

  RoutingFutures SendReturnsAllSuccesses(const std::string& serialised_message) const {
    RoutingFutures futures;
    for (int i(0); i != 4; ++i) {
      auto promise(std::make_shared<std::promise<std::string>>());
      futures.push_back(promise->get_future());
      std::thread worker([promise, i, serialised_message] {
          std::this_thread::sleep_for(std::chrono::milliseconds(i * 100));
          promise->set_value(serialised_message);
      });
      worker.detach();
    }
    return std::move(futures);
  }
};

TYPED_TEST_CASE_P(UtilsTest);

TYPED_TEST_P(UtilsTest, BEH_HandleGetFuturesAllFail) {
//  auto promise(std::make_shared<std::promise<TypeParam>>());
//  std::future<TypeParam> future(promise->get_future());
//  auto routing_futures(std::make_shared<RoutingFutures>(this->SendReturnsAllFailed()));
//
//  HandleGetFutures<TypeParam>(promise, routing_futures);
//  EXPECT_THROW(future.get(), nfs_error);
}

TYPED_TEST_P(UtilsTest, BEH_HandleGetFuturesOneSucceeds) {
//  auto promise(std::make_shared<std::promise<TypeParam>>());
//  std::future<TypeParam> future(promise->get_future());
//
//  std::pair<Identity, NonEmptyString> name_and_content(GetNameAndContent<TypeParam>());
//  auto serialised_message(this->MakeSerialisedMessage(name_and_content));
//  auto routing_futures(std::make_shared<RoutingFutures>(
//                           this->SendReturnsOneSuccess(serialised_message)));
//
//  HandleGetFutures<TypeParam>(promise, routing_futures);
//  auto data(future.get());
//  EXPECT_EQ(data.name().data, name_and_content.first);
}

TYPED_TEST_P(UtilsTest, BEH_HandleGetFuturesAllSucceed) {
//  auto promise(std::make_shared<std::promise<TypeParam>>());
//  std::future<TypeParam> future(promise->get_future());
//
//  std::pair<Identity, NonEmptyString> name_and_content(GetNameAndContent<TypeParam>());
//  auto serialised_message(this->MakeSerialisedMessage(name_and_content));
//  auto routing_futures(std::make_shared<RoutingFutures>(
//                           this->SendReturnsOneSuccess(serialised_message)));
//
//  HandleGetFutures<TypeParam>(promise, routing_futures);
//  auto data(future.get());
//  EXPECT_EQ(data.name().data, name_and_content.first);
}

REGISTER_TYPED_TEST_CASE_P(UtilsTest,
                           BEH_HandleGetFuturesAllFail,
                           BEH_HandleGetFuturesOneSucceeds,
                           BEH_HandleGetFuturesAllSucceed);

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
                       passport::PublicMpid,
                       ImmutableData,
                       OwnerDirectory,
                       GroupDirectory,
                       WorldDirectory> DataTypes;
INSTANTIATE_TYPED_TEST_CASE_P(All, UtilsTest, DataTypes);

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
