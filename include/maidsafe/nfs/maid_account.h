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

#ifndef MAIDSAFE_NFS_MAID_ACCOUNT_H_
#define MAIDSAFE_NFS_MAID_ACCOUNT_H_

#include <vector>

#include "maidsafe/common/types.h"

namespace maidsafe {

namespace nfs {

class PmidRegistration {
 public:
  PmidRegistration(Identity maid_id_in, Identity pmid_id_in)
    : maid_id(maid_id_in),
      pmid_id(pmid_id_in),
      register_(false),
      maid_signature(NonEmptyString()),
      pmid_signature(NonEmptyString()) {}
  explicit PmidRegistration(const NonEmptyString& serialised_pmidregistration)
    : maid_id(),
      pmid_id(),
      register_(false),
      maid_signature(NonEmptyString()),
      pmid_signature(NonEmptyString()) {
    Parse(serialised_pmidregistration);
  }

  void Parse(const NonEmptyString& serialised_pmidregistration);

  NonEmptyString Serialise();

  Identity maid_id;
  Identity pmid_id;
  bool register_;
  NonEmptyString maid_signature;
  NonEmptyString pmid_signature;
};

class PmidSize {
 public:
  explicit PmidSize(Identity pmid_id_in)
    : pmid_id(pmid_id_in),
      num_data_elements(0),
      total_size(0),
      lost_size(0),
      lost_number_of_elements(0) {}
  explicit PmidSize(const NonEmptyString& serialised_pmidsize)
    : pmid_id(),
      num_data_elements(0),
      total_size(0),
      lost_size(0),
      lost_number_of_elements(0) {
    Parse(serialised_pmidsize);
  }

  void Parse(const NonEmptyString& serialised_pmidsize);
  NonEmptyString Serialise();

  Identity pmid_id;
  int32_t num_data_elements;
  int64_t total_size;
  int64_t lost_size;
  int64_t lost_number_of_elements;
};

class PmidTotal {
 public:
  PmidTotal(Identity maid_id_in, Identity pmid_id_in)
    : registration(maid_id_in, pmid_id_in), pmid_size(pmid_id_in) {}
  PmidTotal(PmidRegistration registration_in, PmidSize pmid_size_in)
    : registration(registration_in), pmid_size(pmid_size_in) {}

  NonEmptyString Serialise();

  PmidRegistration registration;
  PmidSize pmid_size;
};

class DataElement {
 public:
  DataElement() : data_id(), data_size(0) {}

  DataElement(Identity data_id_in, int32_t data_size_in)
    : data_id(data_id_in), data_size(data_size_in) {}

  NonEmptyString Serialise();

  Identity data_id;
  int32_t data_size;
};

class MaidAccount {
 public:
  MaidAccount()
    : maid_id(),
      pmid_totals(),
      data_elements() {}

  explicit MaidAccount(Identity maid_id_in)
    : maid_id(maid_id_in),
      pmid_totals(),
      data_elements() {}

  explicit MaidAccount(const NonEmptyString& serialised_maidaccount)
    : maid_id(),
      pmid_totals(),
      data_elements() {
    Parse(serialised_maidaccount);
  }

  void Parse(const NonEmptyString& serialised_maidaccount);
  NonEmptyString Serialise();

  Identity maid_id;
  std::vector<PmidTotal> pmid_totals;
  std::vector<DataElement> data_elements;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MAID_ACCOUNT_H_
