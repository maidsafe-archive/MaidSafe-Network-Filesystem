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

#ifndef MAIDSAFE_NFS_TYPES_H_
#define MAIDSAFE_NFS_TYPES_H_


namespace maidsafe {

namespace nfs {

enum class PersonaType : int {
  kMaidAccountHolder = 0,
  kMetaDataManager = 1,
  kPmidAccountHolder = 2,
  kDataHolder = 3
  kClientMaid = 4,
  kClientMpid = 5
};

enum class ActionType : int { kGet = 0, kPut = 1, kPost = 2, kDelete = 3 };

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_TYPES_H_
