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

#ifndef MAIDSAFE_NFS_GET_POLICIES_H_
#define MAIDSAFE_NFS_GET_POLICIES_H_

#include "maidsafe/common/types.h"
#include "maidsafe/private/data_types/fob.h"
#include "maidsafe/private/data_types/data_types.h"
#include "maidsafe/routing/routing_api.h"

namespace maidsafe {

class GetFromMetaDataManager {
 public:
  template <typename T>
  static void  Get<>(name, callback, routing, fob) {
  }
 protected:
  ~GetFromDataManager() {}
};


class GetFromDataHolder {
 public:
  template <typename T>
  static std::future<T>  Get<>(T::name_type name, routing::Routing& routing) {
    std::async([] (T::name_type name, std::vector<std::string> messages) {
    routing.Send(name, name, nfs::Message message ....  
    }); 
  }
 protected:
  ~GetFromDataHolder() {}
};





}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_GET_POLICIES_H_

