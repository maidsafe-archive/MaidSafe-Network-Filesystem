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

#ifndef MAIDSAFE_NFS_CLIENT_CLIENT_UTILS_H_
#define MAIDSAFE_NFS_CLIENT_CLIENT_UTILS_H_

#include <memory>
#include <vector>

#include "boost/exception/all.hpp"
#include "boost/thread/future.hpp"

#include "maidsafe/common/data_types/structured_data_versions.h"

#include "maidsafe/routing/timer.h"

#include "maidsafe/nfs/utils.h"
#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/client/maid_node_dispatcher.h"

namespace maidsafe {

namespace nfs_client {

template <typename Data>
struct HandleGetResult {
  explicit HandleGetResult(std::shared_ptr<boost::promise<Data>> promise_in)
      : promise(std::move(promise_in)) {}
  void operator()(const DataNameAndContentOrReturnCode& result) const;
  std::shared_ptr<boost::promise<Data>> promise;
};

void HandlePutResponseResult(const ReturnCode& result,
                             std::shared_ptr<boost::promise<void>> promise);

void HandleSendMessageResponseResult(const ReturnCode& result,
                                     std::shared_ptr<boost::promise<void>> promise);
void HandleGetMessageResponseResult(const MpidMessageOrReturnCode& result,
        std::shared_ptr<boost::promise<nfs_vault::MpidMessage>> promise);

void HandleGetVersionsOrBranchResult(
    const StructuredDataNameAndContentOrReturnCode& result,
    std::shared_ptr<boost::promise<std::vector<StructuredDataVersions::VersionName>>> promise);

void HandleCreateAccountResult(const ReturnCode& result,
                               std::shared_ptr<boost::promise<void>> promise);

void HandleCreateVersionTreeResult(const ReturnCode& result,
                                   std::shared_ptr<boost::promise<void>> promise);

void HandlePutVersionResult(
    const TipOfTreeAndReturnCode& result,
    std::shared_ptr<boost::promise<std::unique_ptr<StructuredDataVersions::VersionName>>> promise);

void HandleRegisterPmidResult(const ReturnCode& result,
                              std::shared_ptr<boost::promise<void>> promise);

// ==================== Implementation =============================================================
template <typename Data>
void HandleGetResult<Data>::operator()(const DataNameAndContentOrReturnCode& result) const {
  LOG(kVerbose) << "HandleGetResult<Data>::operator()";
  try {
    if (result.content) {
      if (result.name.type != Data::Tag::kValue) {
        LOG(kError) << "HandleGetResult incorrect returned data";
        BOOST_THROW_EXCEPTION(MakeError(CommonErrors::invalid_parameter));
      }
      LOG(kInfo) << "HandleGetResult fetched chunk has name : "
                 << HexSubstr(result.name.raw_name) << " and content : "
                 << HexSubstr(result.content->data);
      Data data(typename Data::Name(result.name.raw_name),
                typename Data::serialised_type(NonEmptyString(result.content->data)));
      promise->set_value(data);
    } else if (result.return_code) {
      LOG(kWarning) << "HandleGetResult don't have a result but having a return code "
                    << result.return_code->value.what();
      BOOST_THROW_EXCEPTION(result.return_code->value);
    } else {
      LOG(kError) << "HandleGetResult result uninitialised";
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::uninitialised));
    }
  }
  catch (...) {
    LOG(kError) << "processing error when try to parsing the result of get";
    promise->set_exception(boost::current_exception());
  }
}

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_CLIENT_UTILS_H_
