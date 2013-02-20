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

#ifndef MAIDSAFE_NFS_PUBLIC_KEY_GETTER_H_
#define MAIDSAFE_NFS_PUBLIC_KEY_GETTER_H_

#include <string>
#include <vector>

#include "maidsafe/common/utils.h"
#include "maidsafe/passport/types.h"
#include "maidsafe/routing/routing_api.h"

#include "maidsafe/nfs/nfs.h"


namespace maidsafe {

namespace nfs {

class PublicKeyGetter {
 public:
  // all_pmids_from_file should only be non-empty if TESTING is defined
  PublicKeyGetter(routing::Routing& routing,
                  const std::vector<passport::PublicPmid>& public_pmids_from_file =
                      std::vector<passport::PublicPmid>());

  template<typename Data>
  void HandleGetKey(const typename Data::name_type& key_name,
                    const routing::ResponseFunctor& get_key_functor);

 private:
  std::unique_ptr<KeyGetterNfs> key_getter_nfs_;
  std::unique_ptr<FakeKeyGetterNfs> fake_key_getter_nfs_;
};

template<typename Data>
void PublicKeyGetter::HandleGetKey(const typename Data::name_type& key_name,
                                   const routing::ResponseFunctor& get_key_functor) {
#ifdef TESTING
  if (key_getter_nfs_) {
#endif
    key_getter_nfs_->Get<Data>(key_name, get_key_functor);
#ifdef TESTING
  } else {
    fake_key_getter_nfs_->Get<passport::PublicPmid>(key_name, get_key_functor);
  }
#endif
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_PUBLIC_KEY_GETTER_H_
