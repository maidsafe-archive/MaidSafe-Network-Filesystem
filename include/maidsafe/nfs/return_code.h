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
#include <system_error>

#include "maidsafe/common/bounded_string.h"
#include "maidsafe/common/error.h"
#include "maidsafe/common/types.h"

#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

class ReturnCode {
 public:
  typedef TaggedValue<NonEmptyString, struct SerialisedReturnCodeTag> serialised_type;
  static const MessageCategory message_type_identifier;

  // Designed to be used with maidsafe-specific error enums (e.g. CommonErrors::success)
  template<typename ErrorCode>
  ReturnCode(ErrorCode error_code, const NonEmptyString& data = NonEmptyString())
      : error_(MakeError(error_code)),
        data_(data) {}
  ReturnCode(std::system_error error, const NonEmptyString& data = NonEmptyString());

  ReturnCode(const ReturnCode& other);
  ReturnCode& operator=(const ReturnCode& other);
  ReturnCode(ReturnCode&& other);
  ReturnCode& operator=(ReturnCode&& other);

  explicit ReturnCode(const serialised_type& serialised_return_code);
  serialised_type Serialise() const;

  std::system_error error() const { return error_; }
  NonEmptyString data() const { return data_; }
  bool IsSuccess() const;

 private:
  std::system_error error_;
  NonEmptyString data_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_RETURN_CODE_H_
