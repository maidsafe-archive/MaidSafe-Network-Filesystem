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

#include "maidsafe/nfs/accumulator.h"

#include <algorithm>

#include "maidsafe/nfs/handled_request_pb.h"


namespace maidsafe {

namespace nfs {

template <>
typename Accumulator<passport::PublicMaid::name_type>::serialised_requests
Accumulator<passport::PublicMaid::name_type>::SerialiseHandledRequests(
    const passport::PublicMaid::name_type& name) const {
  Accumulator<passport::PublicMaid::name_type>::serialised_requests ser;
  protobuf::HandledRequests handled_requests;
  protobuf::HandledRequest* handled_request;
  handled_requests.set_name((name->string()));
  std::lock_guard<std::mutex> lock(mutex_);
  for (auto& request : handled_requests_) {
    if (request.first.second == name) {
      handled_request = handled_requests.add_request();
      handled_request->set_message_id(request.first.first->string());
      handled_request->set_reply(request.second.Serialise()->string());
    }
  }
  return serialised_requests(NonEmptyString(handled_requests.SerializeAsString()));
}

template <>
std::vector<typename Accumulator<passport::PublicMaid::name_type>::HandledRequest>
Accumulator<passport::PublicMaid::name_type>::ParseHandledRequests(
    const typename Accumulator<passport::PublicMaid::name_type>::serialised_requests&
        serialised_message) const {
  std::vector<typename Accumulator<passport::PublicMaid::name_type>::HandledRequest>
      ret_handled_requests;
  protobuf::HandledRequests proto_handled_requests;
  if (!proto_handled_requests.ParseFromString(serialised_message->string()))
    ThrowError(CommonErrors::parsing_error);
  try {
    for (auto index(0); index < proto_handled_requests.request_size(); ++index) {
      ret_handled_requests.push_back(
          std::make_pair(
              std::make_pair(
                  MessageId(Identity(proto_handled_requests.request(index).message_id())),
                  passport::PublicMaid::name_type(Identity(proto_handled_requests.name()))),
              Reply(Reply::serialised_type(
                  NonEmptyString(proto_handled_requests.request(index).reply())))));
    }
  }
  catch(const std::exception&) {
    ThrowError(CommonErrors::parsing_error);
  }
  return ret_handled_requests;
}

}  // namespace nfs

}  // namespace maidsafe
