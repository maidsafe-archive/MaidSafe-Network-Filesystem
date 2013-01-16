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

#ifndef MAIDSAFE_NFS_ACCUMULATOR_H_
#define MAIDSAFE_NFS_ACCUMULATOR_H_

#include <deque>
#include <utility>
#include <vector>

#include "maidsafe/nfs/utils.h"

#include "maidsafe/routing/routing_api.h"

namespace maidsafe {

namespace nfs {

class Accumulator {
 public:
  struct Request {
    Request(Message msg_in, routing::ReplyFunctor reply_functor_in, ReturnCode ret_code_in)
      : msg(msg_in), reply_functor(reply_functor_in), ret_code(ret_code_in) {}
    Message msg;
    routing::ReplyFunctor reply_functor;
    ReturnCode ret_code;
  };
  // Identity part is id() and personal_type()
  typedef std::deque<std::tuple<NonEmptyString, PersonaType, Request>> Requests;
  typedef std::deque<std::tuple<NonEmptyString, PersonaType, ReturnCode>> HandledRequests;

  Accumulator();

  bool CheckHandled(const NonEmptyString& id,
                    const PersonaType& personal_type,
                    ReturnCode& ret_code);
  std::vector<ReturnCode> PushRequest(const Request& request);
  std::vector<Request> SetHandled(const NonEmptyString& msg_id,
                                  const PersonaType& personal_type,
                                  const ReturnCode& ret_code);

 private:
  Requests pending_requests_;
  HandledRequests handled_requests_;
  mutable std::mutex mutex_;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_ACCUMULATOR_H_
