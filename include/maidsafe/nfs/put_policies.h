/*
* ============================================================================
*
* Copyright [2011] maidsafe.net limited
*
* The following source code is property of maidsafe.net limited and is not
* meant for external use.  The use of this code is governed by the license
* file licence.txt found in the root of this directory and also on
* www.maidsafe.net.
*
* You are not free to copy, amend or otherwise use this source code without
* the explicit written permission of the board of directors of maidsafe.net.
*
* ============================================================================
*/

#ifndef MAIDSAFE_NFS_PUT_POLICIES_H_
#define MAIDSAFE_NFS_PUT_POLICIES_H_

#include "maidsafe/common/types.h"
#include "maidsafe/private/data_types/fob.h"
#include "maidsafe/private/data_types/data_types.h"
#include "maidsafe/routing/routing_api.h"

namespace maidsafe {

template <typename T>
class PutToMaidAccountHolder {
 public:
  static void  PutPolicy<>(name, callback, routing, fob) {
  }
  static void  PutPolicy<MutableData>(name, callback, routing, fob) {
    // could use T.Fob() T.Routing() here rather than passing routing fob parameters.
    // need to get result of edit or store may be +ve or -Ve or fail after it
    // goes to MAIDAccountHandler
  }

 protected:
  ~PutToMaidAccountHolder() {}
};

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_PUT_POLICIES_H_

