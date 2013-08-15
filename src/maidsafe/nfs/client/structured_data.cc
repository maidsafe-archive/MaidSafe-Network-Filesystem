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

#include "maidsafe/nfs/client/structured_data.h"

#include <utility>

#include "maidsafe/common/error.h"

#include "maidsafe/nfs/client/structured_data.pb.h"


namespace maidsafe {

namespace nfs_client {

StructuredData::StructuredData(const std::vector<StructuredDataVersions::VersionName>& versions_in)
    : versions(versions_in) {}

StructuredData::StructuredData() : versions() {}

StructuredData::StructuredData(const StructuredData& other) : versions(other.versions) {}

StructuredData::StructuredData(StructuredData&& other) : versions(std::move(other.versions)) {}

StructuredData& StructuredData::operator=(StructuredData other) {
  swap(*this, other);
  return *this;
}

StructuredData::StructuredData(const std::string& serialised_copy) : versions() {
  protobuf::StructuredData proto_structured_data;
  if (!proto_structured_data.ParseFromString(serialised_copy))
    ThrowError(CommonErrors::parsing_error);
  for (auto i(0); i < proto_structured_data.serialised_versions_size(); ++i)
    versions.emplace_back(proto_structured_data.serialised_versions(i));
}

std::string StructuredData::Serialise() const {
  protobuf::StructuredData proto_structured_data;
  for (const auto& version : versions)
    proto_structured_data.add_serialised_versions(version.Serialise());
  return proto_structured_data.SerializeAsString();
}

void swap(StructuredData& lhs, StructuredData& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.versions, rhs.versions);
}

}  // namespace nfs_client

}  // namespace maidsafe
