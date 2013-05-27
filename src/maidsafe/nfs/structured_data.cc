/***************************************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use.  The use of this code is governed by the licence file licence.txt found in the root of    *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit         *
 *  written permission of the board of directors of MaidSafe.net.                                  *
 **************************************************************************************************/

#include "maidsafe/nfs/structured_data.h"

#include <vector>
#include "maidsafe/common/error.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/structured_data.pb.h"
#include "maidsafe/nfs/utils.h"


namespace maidsafe {

namespace nfs {



StructuredData::StructuredData(const std::vector<StructuredDataVersions::VersionName> versions)
    :  versions_(versions){}


StructuredData::StructuredData(const StructuredData& other)
 :  versions_(other.versions_){}

StructuredData& StructuredData::operator=(const StructuredData& other) {
  versions_ = other.versions_;
  return *this;
}

StructuredData::StructuredData(StructuredData&& other)
    : versions_(std::move(other.versions_)) {}


StructuredData::StructuredData(const serialised_type& serialised_message)
    : versions_() {
  protobuf::StructuredData proto_structured_data;
  if (!proto_structured_data.ParseFromString(serialised_message->string()))
    ThrowError(CommonErrors::parsing_error);

}

StructuredData::serialised_type StructuredData::Serialise() const {
  serialised_type serialised_message;
  try {
    protobuf::StructuredData proto_structured_data;
    for (size_t i(0); i < versions_.size() ; ++i) {
      proto_structured_data.add_versions(versions_.at(i).id->string());
     }

    auto serialised_message =
      serialised_type(NonEmptyString(proto_structured_data.SerializeAsString()));
  }
  catch(const std::exception&) {
    ThrowError(CommonErrors::invalid_parameter);
  }
  return serialised_message;
}

}  // namespace nfs

}  // namespace maidsafe
