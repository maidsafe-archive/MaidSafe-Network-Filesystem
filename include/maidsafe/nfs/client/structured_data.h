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

#ifndef MAIDSAFE_NFS_CLIENT_STRUCTURED_DATA_H_
#define MAIDSAFE_NFS_CLIENT_STRUCTURED_DATA_H_

#include <string>
#include <vector>

#include "maidsafe/common/config.h"
#include "maidsafe/data_types/structured_data_versions.h"


namespace maidsafe {

namespace nfs_client {

struct StructuredData {
 public:
  explicit StructuredData(const std::vector<StructuredDataVersions::VersionName>& versions);

  StructuredData();
  StructuredData(const StructuredData& other);
  StructuredData(StructuredData&& other);
  StructuredData& operator=(StructuredData other);

  explicit StructuredData(const std::string& serialised_copy);
  std::string Serialise() const;

  std::vector<StructuredDataVersions::VersionName> versions;
};

void swap(StructuredData& lhs, StructuredData& rhs) MAIDSAFE_NOEXCEPT;

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_STRUCTURED_DATA_H_
