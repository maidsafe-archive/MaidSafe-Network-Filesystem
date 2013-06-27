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

#include "maidsafe/nfs/structured_data.h"

#include <utility>

#include "maidsafe/common/error.h"

#include "maidsafe/nfs/structured_data.pb.h"


namespace maidsafe {

namespace nfs {

StructuredData::StructuredData(const std::vector<StructuredDataVersions::VersionName>& versions)
    : versions_(versions) {}

StructuredData::StructuredData(const StructuredData& other) : versions_(other.versions_) {}

StructuredData::StructuredData(StructuredData&& other) : versions_(std::move(other.versions_)) {}

StructuredData& StructuredData::operator=(StructuredData other) {
  swap(*this, other);
  return *this;
}

StructuredData::StructuredData(const serialised_type& serialised_message) : versions_() {
  protobuf::StructuredData proto_structured_data;
  if (!proto_structured_data.ParseFromString(serialised_message->string()))
    ThrowError(CommonErrors::parsing_error);
  for (auto i(0); i < proto_structured_data.versions_size(); ++i) {
    versions_.emplace_back(
        proto_structured_data.versions(i).index(),
        ImmutableData::name_type(Identity(proto_structured_data.versions(i).id())));
  }
}

StructuredData::serialised_type StructuredData::Serialise() const {
  protobuf::StructuredData proto_structured_data;
  for (const auto& version : versions_) {
    auto proto_version = proto_structured_data.add_versions();
    proto_version->set_index(version.index);
    proto_version->set_id(version.id->string());
  }
  return serialised_type(NonEmptyString(proto_structured_data.SerializeAsString()));
}

void swap(StructuredData& lhs, StructuredData& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.versions_, rhs.versions_);
}

}  // namespace nfs

}  // namespace maidsafe
