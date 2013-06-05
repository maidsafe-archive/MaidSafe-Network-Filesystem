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

#ifndef MAIDSAFE_NFS_STRUCTURED_DATA_H_
#define MAIDSAFE_NFS_STRUCTURED_DATA_H_

#include <vector>

#include "maidsafe/common/types.h"
#include "maidsafe/common/tagged_value.h"
#include "maidsafe/data_types/structured_data_versions.h"


namespace maidsafe {

namespace nfs {

class StructuredData {
 private:
  struct StructuredDataTag;

 public:
  typedef TaggedValue<NonEmptyString, StructuredDataTag> serialised_type;

  explicit StructuredData(const std::vector<StructuredDataVersions::VersionName>& versions);

  StructuredData(const StructuredData& other);
  StructuredData(StructuredData&& other);
  StructuredData& operator=(StructuredData other);
  friend void swap(StructuredData& lhs, StructuredData& rhs) MAIDSAFE_NOEXCEPT;

  explicit StructuredData(const serialised_type& serialised_message);
  serialised_type Serialise() const;

  std::vector<StructuredDataVersions::VersionName> versions() const { return versions_; }

 private:
  std::vector<StructuredDataVersions::VersionName> versions_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_STRUCTURED_DATA_H_
