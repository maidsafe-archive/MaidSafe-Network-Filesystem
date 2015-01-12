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

#include "maidsafe/nfs/client/structured_data.h"

#include <utility>

#include "maidsafe/common/error.h"
#include "maidsafe/common/serialisation/serialisation.h"

#include "maidsafe/nfs/client/structured_data.pb.h"

namespace maidsafe {

namespace nfs_client {

StructuredData::StructuredData(std::vector<StructuredDataVersions::VersionName> versions_in)
    : versions(std::move(versions_in)) {}

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
    BOOST_THROW_EXCEPTION(MakeError(CommonErrors::parsing_error));
  for (auto i(0); i < proto_structured_data.serialised_versions_size(); ++i) {
    versions.push_back(ConvertFromString<StructuredDataVersions::VersionName>(
        proto_structured_data.serialised_versions(i)));
  }
}

std::string StructuredData::Serialise() const {
  protobuf::StructuredData proto_structured_data;
  for (const auto& version : versions)
    proto_structured_data.add_serialised_versions(ConvertToString(version));
  return proto_structured_data.SerializeAsString();
}

bool operator==(const StructuredData& lhs, const StructuredData& rhs) {
  if (lhs.versions.size() != rhs.versions.size())
    return false;
  for (const auto& version : lhs.versions) {
    if (std::find(rhs.versions.begin(), rhs.versions.end(), version) == rhs.versions.end())
      return false;
  }
  return true;
}

void swap(StructuredData& lhs, StructuredData& rhs) MAIDSAFE_NOEXCEPT {
  using std::swap;
  swap(lhs.versions, rhs.versions);
}

}  // namespace nfs_client

}  // namespace maidsafe
