///* Copyright 2012 MaidSafe.net limited
//
//This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
//and The General Public License (GPL), version 3. By contributing code to this project You agree to
//the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
//of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:
//
//http://www.novinet.com/license
//
//Unless required by applicable law or agreed to in writing, software distributed under the License is
//distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//implied. See the License for the specific language governing permissions and limitations under the
//License.
//*/
//
//#include "maidsafe/nfs/public_key_getter.h"
//
//#include "maidsafe/common/error.h"
//#include "maidsafe/common/log.h"
//
//
//namespace maidsafe {
//
//namespace nfs {
//
//PublicKeyGetter::PublicKeyGetter(routing::Routing& routing,
//                                 const std::vector<passport::PublicPmid>& public_pmids_from_file)
//    : key_getter_nfs_(public_pmids_from_file.empty() ? new KeyGetterNfs(routing) : nullptr)
//#ifdef TESTING
//      , kAllPmids_(public_pmids_from_file)
//#endif
//{
//#ifndef TESTING
//  if (!public_pmids_from_file.empty()) {
//    LOG(kError) << "Cannot use fake key getter if TESTING is not defined";
//    ThrowError(CommonErrors::invalid_parameter);
//  }
//#endif
//}
//
//template<>
//void PublicKeyGetter::GetKey<passport::PublicPmid>(
//    const typename passport::PublicPmid::Name& key_name,
//    std::function<void(Message)> get_key_functor) {
//#ifdef TESTING
//  if (key_getter_nfs_) {
//#endif
//    auto get_op(std::make_shared<OperationOp>(1, get_key_functor));
//    key_getter_nfs_->Get<passport::PublicPmid>(key_name,
//                                               [get_op](std::string serialised_reply) {
//                                                 HandleOperationReply(get_op, serialised_reply);
//                                               });
//#ifdef TESTING
//  } else {
//    auto itr(std::find_if(kAllPmids_.begin(), kAllPmids_.end(),
//        [&key_name](const passport::PublicPmid& pmid) { return pmid.name() == key_name; }));
//    if (itr == kAllPmids_.end()) {
//      Message reply(Persona::kDataGetter, Persona::kDataGetter,
//                    Message::Data(NfsErrors::failed_to_get_data));
//      return get_key_functor(reply);
//    }
//    Message::Data data(CommonErrors::success);
//    data.name = key_name;
//    data.content = (*itr).Serialise().data;
//    Message reply(Persona::kDataGetter, Persona::kDataGetter, data);
//    get_key_functor(reply);
//  }
//#endif
//}
//
//}  // namespace nfs
//
//}  // namespace maidsafe
