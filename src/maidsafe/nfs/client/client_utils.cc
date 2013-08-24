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

#include "maidsafe/nfs/client/client_utils.h"


namespace maidsafe {

namespace nfs_client {

void HandleGetVersionsOrBranchResult(const StructuredDataNameAndContentOrReturnCode& result,
    std::shared_ptr<boost::promise<std::vector<StructuredDataVersions::VersionName>>> promise) {
  try {
    if (result.structured_data) {
      promise->set_value(result.structured_data->versions);
    } else if (result.data_name_and_return_code) {
      boost::throw_exception(result.data_name_and_return_code->return_code.value);
    } else {
      ThrowError(CommonErrors::uninitialised);
    }
  }
  catch(...) {
    promise->set_exception(boost::current_exception());
  }
}

}  // namespace nfs_client

}  // namespace maidsafe
