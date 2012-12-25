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

#ifndef MAIDSAFE_NFS_RETURN_CODE_H_
#define MAIDSAFE_NFS_RETURN_CODE_H_

#include <string>

#include "maidsafe/common/bounded_string.h"
#include "maidsafe/common/types.h"


namespace maidsafe {

namespace nfs {

namespace protobuf { class ReturnCode; }

class ReturnCode {
 public:
  typedef detail::BoundedString<1, 4096> Info;
  typedef TaggedValue<NonEmptyString, struct SerialisedReturnCodeTag> serialised_type;
  ReturnCode(int value, const Info& info = Info());
  ReturnCode(const ReturnCode& other);
  ReturnCode& operator=(const ReturnCode& other);
  ReturnCode(ReturnCode&& other);
  ReturnCode& operator=(ReturnCode&& other);

  explicit ReturnCode(const serialised_type& serialised_return_code);
  serialised_type Serialise() const;

  int value() const { return value_; }
  Info info() const { return info_; }

 private:
  int value_;
  Info info_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_RETURN_CODE_H_
