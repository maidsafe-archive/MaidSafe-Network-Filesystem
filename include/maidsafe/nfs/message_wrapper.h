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

#ifndef MAIDSAFE_NFS_MESSAGE_WRAPPER_H_
#define MAIDSAFE_NFS_MESSAGE_WRAPPER_H_

#include <cstdint>

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"


namespace maidsafe {

namespace nfs {

class MessageWrapper {
 public:
  typedef TaggedValue<NonEmptyString, struct SerialisedMessageWrapperTag> serialised_type;

  explicit MessageWrapper(const NonEmptyString& serialised_inner_message,
                          const asymm::Signature& signature = asymm::Signature());
  explicit MessageWrapper(NonEmptyString&& serialised_inner_message,
                          const asymm::Signature& signature = asymm::Signature());
  explicit MessageWrapper(const serialised_type& serialised_message);
  MessageWrapper(const MessageWrapper& other);
  MessageWrapper& operator=(const MessageWrapper& other);
  MessageWrapper(MessageWrapper&& other);
  MessageWrapper& operator=(MessageWrapper&& other);

  serialised_type Serialise() const;

  template<typename InnerMessageType>
  typename InnerMessageType::serialised_type serialised_inner_message() const;
  asymm::Signature signature() const { return signature_; }

 private:
  NonEmptyString serialised_inner_message_;
  asymm::Signature signature_;
};

template<typename InnerMessageType>
typename InnerMessageType::serialised_type MessageWrapper::serialised_inner_message() const {
  return typename InnerMessageType::serialised_type(serialised_inner_message_);
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGE_WRAPPER_H_
