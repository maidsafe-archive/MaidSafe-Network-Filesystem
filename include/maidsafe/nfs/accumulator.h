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

#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/data_message.h"
#include "maidsafe/nfs/reply.h"
#include "maidsafe/nfs/types.h"
#include "maidsafe/nfs/utils.h"
#include "maidsafe/nfs/handled_request_pb.h"


namespace maidsafe {

namespace nfs {

template <typename Name>
class Accumulator {
 public:
  struct Request {
    Request(DataMessage msg_in, routing::ReplyFunctor reply_functor_in, Reply reply_in)
        : msg(msg_in), reply_functor(reply_functor_in), reply(reply_in) {}
    DataMessage msg;
    routing::ReplyFunctor reply_functor;
    Reply reply;
  };

  typedef TaggedValue<NonEmptyString, struct SerialisedAccumulatorTag> serialised_type;
  typedef std::pair<MessageId, typename Name::name_type> RequestIdentity;
  typedef std::pair<RequestIdentity, Reply> HandledRequest;

  Accumulator();

  bool CheckHandled(const RequestIdentity& request_identity, Reply& reply) const;
  std::vector<Reply> PushRequest(const Request& request);
  std::vector<Request> SetHandled(const RequestIdentity& request_identity, const Reply& reply);
  serialised_type GetSerialisedHandledRequests(const Name& name) const;
  std::vector<HandledRequest> ParseSerailisedHandledRequest(
      const typename Accumulator::serialised_type& serialised_message);

 private:
  typedef std::deque<std::pair<RequestIdentity, Request>> Requests;
  typedef std::deque<HandledRequest> HandledRequests;

  Requests pending_requests_;
  HandledRequests handled_requests_;
  const size_t kMaxPendingRequestsCount_, kMaxHandledRequestsCount_;
  mutable std::mutex mutex_;
};

}  // namespace nfs

}  // namespace maidsafe

#include "maidsafe/nfs/accumulator-inl.h"

#endif  // MAIDSAFE_NFS_ACCUMULATOR_H_
