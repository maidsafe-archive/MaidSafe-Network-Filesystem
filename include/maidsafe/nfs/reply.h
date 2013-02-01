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

#ifndef MAIDSAFE_NFS_REPLY_H_
#define MAIDSAFE_NFS_REPLY_H_

#include <string>
#include <system_error>
#include <type_traits>

#include "maidsafe/common/bounded_string.h"
#include "maidsafe/common/error.h"
#include "maidsafe/common/types.h"

#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

class Reply {
 public:
  typedef TaggedValue<NonEmptyString, struct SerialisedReplyTag> serialised_type;
  static const MessageCategory message_type_identifier;

  // Designed to be used with maidsafe-specific error enums (e.g. CommonErrors::success)
  template<typename ErrorCode>
  Reply(ErrorCode error_code,
        const NonEmptyString& data = NonEmptyString(),
        typename std::enable_if<std::is_error_code_enum<ErrorCode>::value>::type* = 0)
      : error_(MakeError(error_code)),
        data_(data) {}
  template<typename Error>
  Reply(Error error,
        const NonEmptyString& data = NonEmptyString(),
        typename std::enable_if<!std::is_error_code_enum<Error>::value>::type* = 0)
      : error_(error),
        data_(data) {
    static_assert(std::is_same<Error, maidsafe_error>::value ||
                  std::is_base_of<maidsafe_error, Error>::value,
                  "Error type must be a MaidSafe-specific type");
  }

  Reply(const Reply& other);
  Reply& operator=(const Reply& other);
  Reply(Reply&& other);
  Reply& operator=(Reply&& other);

  explicit Reply(const serialised_type& serialised_reply);
  serialised_type Serialise() const;

  maidsafe_error error() const { return error_; }
  NonEmptyString data() const { return data_; }
  bool IsSuccess() const;

 private:
  maidsafe_error error_;
  NonEmptyString data_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_REPLY_H_
