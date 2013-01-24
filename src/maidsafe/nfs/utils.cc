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

#include "maidsafe/nfs/utils.h"

#include <cstdint>

#include "maidsafe/common/utils.h"


namespace maidsafe {

namespace nfs {

namespace detail {

MessageId GetNewMessageId(const NodeId& source_node_id) {
  static int32_t random_element(RandomInt32());
  std::string id(source_node_id.string() + std::to_string(random_element++));
  return MessageId(Identity(crypto::Hash<crypto::SHA512>(source_node_id.string() +
                                                         std::to_string(random_element++))));
}

void GetReturnCode(int& success_count, int& failure_count, std::future<std::string>& future) {
  try {
    // Need '((' when constructing ReturnCode to avoid most vexing parse.
    ReturnCode return_code((ReturnCode::serialised_type(NonEmptyString(future.get()))));
    if (static_cast<CommonErrors>(return_code.value()) == CommonErrors::success) {
      ++success_count;
    } else {
      LOG(kWarning) << "Received an error " << return_code.value();
      ++failure_count;
    }
  }
  catch(const std::exception& e) {
    ++failure_count;
    LOG(kError) <<  e.what();
  }
}

}  // namespace detail

std::vector<StringFuture>::iterator FindNextReadyFuture(const StringFutureVector::iterator& begin,
                                                        StringFutureVector& routing_futures) {
 return std::find_if(begin,
                     routing_futures.end(),
                     [] (StringFuture& future) { return IsReady(future); });  // NOLINT (Dan)
}

void HandleGenericResponse(GenericMessage::OnError /*on_error_functor*/,
                           GenericMessage /*original_generic_message*/,
                           const std::vector<std::string>& /*serialised_messages*/) {
  // TODO(Team): BEFORE_RELEASE implement
}

}  // namespace nfs

}  // namespace maidsafe
