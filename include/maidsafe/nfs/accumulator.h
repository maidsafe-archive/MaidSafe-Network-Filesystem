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
  class AccumulatorTest_BEH_PushRequestThreaded_Test;
  class AccumulatorTest_BEH_CheckPendingRequestsLimit_Test;
  class AccumulatorTest_BEH_CheckHandled_Test;
  class AccumulatorTest_BEH_SetHandled_Test;
}

template <typename Name>
class Accumulator {
 public:
  struct PendingRequest {
    PendingRequest(const DataMessage& msg_in,
            const routing::ReplyFunctor& reply_functor_in,
            const Reply& reply_in);
    PendingRequest(const PendingRequest& other);
    PendingRequest& operator=(const PendingRequest& other);
    PendingRequest(PendingRequest&& other);
    PendingRequest& operator=(PendingRequest&& other);

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
                   const int32_t& size_in,
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
    int32_t size;
    Reply reply;
  };

  typedef TaggedValue<NonEmptyString, struct SerialisedRequestsTag> serialised_requests;

  Accumulator();

  // Returns true and populates <reply_out> if the message has already been set as handled.
  bool CheckHandled(const DataMessage& data_message, Reply& reply_out) const;
  // Adds a request pending the overall result of the operation.
  std::vector<Reply> PushRequest(const PendingRequest& pending_request);
  // Marks the message as handled and returns all pending requests held with the same ID
  std::vector<PendingRequest> SetHandled(const DataMessage& data_message, const Reply& reply);
  // Returns all handled requests for the given account name.
  std::vector<HandledRequest> Get(const Name& name) const;
  // Serialises all handled requests for the given account name.
  serialised_requests Serialise(const Name& name) const;
  // Parses the list of serialised handled requests.
  std::vector<HandledRequest> Parse(const serialised_requests& serialised_requests_in) const;

  friend class test::AccumulatorTest_BEH_PushRequest_Test;
  friend class test::AccumulatorTest_BEH_PushRequestThreaded_Test;
  friend class test::AccumulatorTest_BEH_CheckPendingRequestsLimit_Test;
  friend class test::AccumulatorTest_BEH_CheckHandled_Test;
  friend class test::AccumulatorTest_BEH_SetHandled_Test;

 private:
  Accumulator(const Accumulator&);
  Accumulator& operator=(const Accumulator&);
  Accumulator(Accumulator&&);
  Accumulator& operator=(Accumulator&&);

  std::deque<PendingRequest> pending_requests_;
  std::deque<HandledRequest> handled_requests_;
  const size_t kMaxPendingRequestsCount_, kMaxHandledRequestsCount_;
};

}  // namespace nfs

}  // namespace maidsafe

#include "maidsafe/nfs/accumulator-inl.h"

#endif  // MAIDSAFE_NFS_ACCUMULATOR_H_
