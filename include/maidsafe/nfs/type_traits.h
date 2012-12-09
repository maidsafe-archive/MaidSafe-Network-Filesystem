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

#ifndef MAIDSAFE_NFS_TYPE_TRAITS_H_
#define MAIDSAFE_NFS_TYPE_TRAITS_H_

#include "maidsafe/common/types.h"

// traits
//
template <typename T>
struct is_nfs_id : std::false_type {};

template <>
struct is_nfs_id<PMID> : std::true_type {};

template <>
struct is_nfs_id<MPID> : std::true_type{};

template <>
struct is_nfs_id<MAID> : std::true_type {};

#endif  // MAIDSAFE_NFS_TYPE_TRAITS_H_
