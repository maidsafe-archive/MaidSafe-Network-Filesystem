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
  explicit StructuredData(std::vector<StructuredDataVersions::VersionName> versions_in);

  StructuredData();
  StructuredData(const StructuredData& other);
  StructuredData(StructuredData&& other);
  StructuredData& operator=(StructuredData other);

  explicit StructuredData(const std::string& serialised_copy);
  std::string Serialise() const;

  std::vector<StructuredDataVersions::VersionName> versions;
};

bool operator==(const StructuredData& lhs, const StructuredData& rhs);
void swap(StructuredData& lhs, StructuredData& rhs) MAIDSAFE_NOEXCEPT;

}  // namespace nfs_client

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_CLIENT_STRUCTURED_DATA_H_
