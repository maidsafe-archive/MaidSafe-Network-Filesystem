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

#include "maidsafe/nfs/client_utils.h"


namespace maidsafe {

namespace nfs {

std::error_code GetMostFrequentError(const std::vector<maidsafe_error>& errors) {
  typedef std::map<std::error_code, int> Count;
  Count count;
  for (auto& error : errors)
    ++count[error.code()];
  auto itr(std::max_element(
      count.begin(),
      count.end(),
      [](const Count::value_type& lhs, const Count::value_type& rhs)->bool {
          return lhs.second < rhs.second;
      }));
  return (*itr).first;
}

}  // namespace nfs

}  // namespace maidsafe
