/***************************************************************************************************
 *  Copyright 2013 MaidSafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use.  The use of this code is governed by the licence file licence.txt found in the root of    *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit         *
 *  written permission of the board of directors of MaidSafe.net.                                  *
 **************************************************************************************************/

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
