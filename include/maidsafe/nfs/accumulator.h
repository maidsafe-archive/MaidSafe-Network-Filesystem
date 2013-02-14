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

namespace maidsafe {

namespace nfs {

namespace test {
  class AccumulatorTest_BEH_PushRequest_Test;
}

template <typename Name>
class Accumulator {
 public:
  struct Request {
    Request(const DataMessage& msg_in,
            const routing::ReplyFunctor& reply_functor_in,
            const Reply& reply_in);
    Request(const Request& other);
    Request& operator=(const Request& other);
    Request(Request&& other);
    Request& operator=(Request&& other);

    DataMessage msg;
    routing::ReplyFunctor reply_functor;
    Reply reply;
  };

  struct HandledRequest {
    HandledRequest(const MessageId& msg_id_in,
                   const Name& source_name_in,
                   const DataMessage::Action& action_type_in,
                   const Identity& data_name,
                   const DataTagValue& data_type,
                   const uint64_t& size_in,
                   const uint32_t& replication_in,
                   const Reply& reply_in);
    HandledRequest(const HandledRequest& other);
    HandledRequest& operator=(const HandledRequest& other);
    HandledRequest(HandledRequest&& other);
    HandledRequest& operator=(HandledRequest&& other);

    MessageId msg_id;
    Name source_name;
    DataMessage::Action action;
    Identity data_name;
    DataTagValue data_type;
    uint64_t size;
    int32_t replication;
    Reply reply;
  };

  typedef TaggedValue<NonEmptyString, struct SerialisedRequestsTag> serialised_requests;
  typedef std::pair<MessageId, Name> RequestIdentity;
  typedef std::pair<RequestIdentity, Request> PendingRequest;

  Accumulator();

  bool CheckHandled(const RequestIdentity& request_identity, Reply& reply) const;
  std::vector<Reply> PushRequest(const Request& request);
  std::vector<Request> SetHandled(const RequestIdentity& request_identity,
                                  const Reply& reply);
  std::vector<HandledRequest> GetHandledRequests(const Name& name) const;
  serialised_requests SerialiseHandledRequests(const Name& name) const;
  std::vector<HandledRequest> ParseHandledRequests(
      const serialised_requests& serialised_requests_in) const;

  friend class test::AccumulatorTest_BEH_PushRequest_Test;

 private:
  typedef std::deque<PendingRequest> Requests;
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
