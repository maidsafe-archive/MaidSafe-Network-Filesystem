/* Copyright 2012 MaidSafe.net limited

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

  // Designed to be used with maidsafe-specific error enums (e.g. CommonErrors::success)
  template<typename ErrorCode>
  explicit Reply(ErrorCode error_code,
                 const NonEmptyString& data = NonEmptyString(),
                 typename std::enable_if<std::is_error_code_enum<ErrorCode>::value>::type* = 0)
      : error_(MakeError(error_code)),
        data_(data) {}
  template<typename Error>
  explicit Reply(Error error,
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
