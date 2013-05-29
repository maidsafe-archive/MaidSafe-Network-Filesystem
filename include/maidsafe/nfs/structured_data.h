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

#ifndef MAIDSAFE_NFS_STRUCTURED_DATA_H_
#define MAIDSAFE_NFS_STRUCTURED_DATA_H_

#include <cstdint>
#include <functional>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "maidsafe/data_types/structured_data_versions.h"

namespace maidsafe {

namespace nfs {

class StructuredData {
 public:
  typedef TaggedValue<NonEmptyString, struct SerialisedVersionsTag> serialised_type;
  StructuredData(const std::vector<StructuredDataVersions::VersionName> versions);
  StructuredData(const serialised_type& serialised_message);
  StructuredData(const StructuredData& other);
  StructuredData& operator=(const StructuredData& other);
  StructuredData(StructuredData&& other);
  StructuredData& operator=(StructuredData&& other);
  serialised_type Serialise() const;
  std::vector<StructuredDataVersions::VersionName> Versions() { return versions_; }
 private:
  std::vector<StructuredDataVersions::VersionName> versions_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_STRUCTURED_DATA_H_
