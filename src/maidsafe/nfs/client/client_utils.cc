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

#include "maidsafe/nfs/client/client_utils.h"

namespace maidsafe {

namespace nfs_client {

void HandleGetVersionsOrBranchResult(
    const StructuredDataNameAndContentOrReturnCode& result,
    std::shared_ptr<boost::promise<std::vector<StructuredDataVersions::VersionName>>> promise) {
  LOG(kVerbose) << "nfs_client::HandleGetVersionsOrBranchResult";
  try {
    if (result.structured_data) {
      promise->set_value(result.structured_data->versions);
    } else if (result.data_name_and_return_code) {
      LOG(kInfo) << "nfs_client::HandleGetVersionsOrBranchResult"
                 << " error during get version or branch";
      boost::throw_exception(result.data_name_and_return_code->return_code.value);
    } else {
      LOG(kInfo) << "nfs_client::HandleGetVersionsOrBranchResult"
                 << " uninitialised during get version or branch";
      BOOST_THROW_EXCEPTION(MakeError(CommonErrors::uninitialised));
    }
  }
  catch (...) {
    LOG(kError) << "nfs_client::HandleGetVersionsOrBranchResult"
                << " exception during get version or branch";
    promise->set_exception(boost::current_exception());
  }
}

void HandleCreateAccountResult(const ReturnCode& result,
                               std::shared_ptr<boost::promise<void>> promise) {
  LOG(kVerbose) << "nfs_client::HandleCreateAccountResult";
  try {
    if (nfs::IsSuccess(result)) {
      LOG(kInfo) << "Create Account succeeded";
      promise->set_value();
    } else {
      LOG(kWarning) << "nfs_client::HandleCreateAccountResult error during create account";
      boost::throw_exception(result.value);
    }
  }
  catch (...) {
    LOG(kError) << "nfs_client::HandleCreateAccountResult exception during create account";
    promise->set_exception(boost::current_exception());
  }
}

void HandlePmidHealthResult(const AvailableSizeAndReturnCode& result,
                            std::shared_ptr<boost::promise<uint64_t>> promise) {
  LOG(kVerbose) << "nfs_client::HandlePmidHealthResult";
  try {
    if (nfs::IsSuccess(result)) {
      LOG(kInfo) << "Get PmidHealth succeeded, returned available_size : "
                 << result.available_size.available_size;
      promise->set_value(result.available_size.available_size);
    } else {
      LOG(kWarning) << "nfs_client::HandlePmidHealthResult error during getPmidHealth";
      boost::throw_exception(result.return_code.value);
    }
  }
  catch (...) {
    LOG(kError) << "nfs_client::HandlePmidHealthResult exception during getPmidHealth";
    promise->set_exception(boost::current_exception());
  }
}

}  // namespace nfs_client

}  // namespace maidsafe
