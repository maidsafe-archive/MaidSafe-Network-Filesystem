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

#include "maidsafe/common/node_id.h"

#include "maidsafe/nfs/message.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace detail {

MessageId GetNewMessageId(const NodeId& source_node_id);

}  // namespace detail

template<typename Data>
bool IsCacheable();

}  // namespace nfs

}  // namespace maidsafe

#include "maidsafe/nfs/utils-inl.h"

#endif  // MAIDSAFE_NFS_UTILS_H_
