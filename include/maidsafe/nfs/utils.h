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

#ifndef MAIDSAFE_NFS_UTILS_H_
#define MAIDSAFE_NFS_UTILS_H_

#include <exception>
#include <future>
#include <string>
#include <vector>

#include "maidsafe/common/log.h"
#include "maidsafe/common/node_id.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"

#include "maidsafe/nfs/message.h"
#include "maidsafe/nfs/utils.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

template<typename Data>

Data ValidateAndParse(const Message& 1essage) {
///  ...
}

// TODO(Fraser#5#): 2012-12-20 - This is executed on one of Routing's io_service threads.  If we
//                  expect this to be slow (e.g. validate and parse message), we should take this
//                  off Routing's thread.
template<typename Data>
void HandleGetResponse(std::shared_ptr<std::promise<Data>> promise,
                       const std::vector<std::string>& serialised_messages) {
  try {
    if (serialised_messages.empty())
      ThrowError(NfsErrors::failed_to_get_data);

    for (auto& serialised_message : serialised_messages) {
      try {
        Message message(Parse(NonEmptyString(serialised_message)));
        Data data(ValidateAndParse<Data>(message));
        promise->set_value(std::move(data));
        return;
      }
      catch(const std::system_error& error) {
        LOG(kError) << error.code() << " - " << error.what();
      }
    }
    // Failed to get a usable result from any of the messages
    ThrowError(NfsErrors::failed_to_get_data);
  }
  catch(...) {
    promise->set_exception(std::current_exception());
  }
}


}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_UTILS_H_
