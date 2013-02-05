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

#include "maidsafe/nfs/public_key_getter.h"

#include <memory>
#include <vector>

#include "maidsafe/common/log.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/test.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/passport/types.h"


namespace maidsafe {

namespace nfs {

namespace test {

//  class PublicKeyGetterTest : public testing::Test {
//   protected:
//  //  typedef PublicKeyGetter::PublicDataType PublicDataType;
//    typedef std::vector<passport::PublicPmid> PublicPmidVector;
//    typedef std::vector<asymm::PublicKey> PublicKeyVector;
//
//  //  struct GetPublicKey : public boost::static_visitor<asymm::PublicKey> {
//  //    template<typename Data>
//  //    asymm::PublicKey operator()(Data& data) {
//  //      return data.public_key();
//  //    }
//  //  };
//
//    PublicKeyGetterTest()
//        : routing_(nullptr),
//          public_key_getter_(new PublicKeyGetter(routing_, PublicPmidVector())) {}
//
//  //  PublicDataType GetRandomPublicData() {
//  //    uint32_t number_of_types = boost::mpl::size<typename PublicDataType::types>::type::value,
//  //             type_number;
//  //    type_number = RandomUint32() % number_of_types;
//  //    switch (type_number) {
//  //      case 0: return passport::PublicAnmid(passport::Anmid());
//  //      case 1: return passport::PublicAnsmid(passport::Ansmid());
//  //      case 2: return passport::PublicAntmid(passport::Antmid());
//  //      case 3: return passport::PublicAnmaid(passport::Anmaid());
//  //      case 4: {
//  //        passport::Anmaid anmaid;
//  //        passport::Maid maid(anmaid);
//  //        return passport::PublicMaid(maid);
//  //      }
//  //      case 5: {
//  //        passport::Anmaid anmaid;
//  //        passport::Maid maid(anmaid);
//  //        passport::Pmid pmid(maid);
//  //        return passport::PublicPmid(pmid);
//  //      }
//  //      case 6: return passport::PublicAnmpid(passport::Anmpid());
//  //      case 7: {
//  //        passport::Anmpid anmpid;
//  //        passport::Mpid mpid(NonEmptyString(RandomAlphaNumericString(1 +RandomUint32() % 100)),
//  //                            anmpid);
//  //        return passport::PublicMpid(mpid);
//  //      }
//  //    }
//  //  }
//
//    PublicPmidVector GeneratePublicPmids() {
//      uint32_t number = RandomUint32() % 20;
//      PublicPmidVector public_pmids;
//      public_pmids.reserve(number);
//      for (uint32_t i = 0; i != number; ++i) {
//        passport::Anmaid anmaid;
//        passport::Maid maid(anmaid);
//        passport::Pmid pmid(maid);
//        public_pmids.push_back(passport::PublicPmid(pmid));
//      }
//      return public_pmids;
//    }
//
//    PublicKeyVector FillPublicKeyVector(const PublicPmidVector& public_pmids) {
//      PublicKeyVector public_keys;
//      public_keys.reserve(public_pmids.size());
//      for (auto& public_pmid : public_pmids)
//        public_keys.push_back(public_pmid.public_key());
//      return public_keys;
//    }
//
//    routing::Routing routing_;
//    std::unique_ptr<PublicKeyGetter> public_key_getter_;
//  };
//
//  TEST_F(PublicKeyGetterTest, BEH_GetPublicPmidKeys) {
//    PublicPmidVector public_pmids(GeneratePublicPmids());
//    public_key_getter_.reset(new PublicKeyGetter(routing_, public_pmids));
//    PublicKeyVector public_keys(FillPublicKeyVector(public_pmids));
//    auto get_key_future([public_keys](std::future<passport::PublicPmid> future) {
//        try {
//          passport::PublicPmid public_pmid(future.get());
//          asymm::PublicKey public_key(public_pmid.public_key());
//          auto it = std::find_if(public_keys.begin(),
//                                 public_keys.end(),
//                                 [&public_key](const asymm::PublicKey& saved_public_key) {
//                                   return asymm::MatchingKeys(saved_public_key, public_key);
//                                 });
//          EXPECT_NE(public_keys.end(), it);
//        }
//        catch(...) {
//          LOG(kError) << "Exception thrown getting public key.";
//        }
//    });
//    for (auto& pmid : public_pmids)
//      public_key_getter_->HandleGetKey<passport::PublicPmid>(pmid.name(), get_key_future);
//  }
//
//  TEST_F(PublicKeyGetterTest, BEH_GetPublicKeys) {
//    auto get_key_future([this](std::future<PublicDataType> future) {
//                          try {
//                            GetPublicKey get_public_key;
//                            PublicDataType public_data(future.get());
//                            asymm::PublicKey public_key(boost::apply_visitor(get_public_key,
//                                                                             public_data));
//                          }
//                          catch(...) {
//                            LOG(kError) << "Exception thrown getting public key.";
//                          }
//                       });
//  }

}  // namespace test

}  // namespace nfs

}  // namespace maidsafe
