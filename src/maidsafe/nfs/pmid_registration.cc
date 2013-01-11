/***************************************************************************************************
 *  Copyright 2012 maidsafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use. The use of this code is governed by the licence file licence.txt found in the root of     *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit written *
 *  permission of the board of directors of MaidSafe.net.                                          *
 **************************************************************************************************/

#include "maidsafe/nfs/pmid_registration.h"

#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/post_messages_pb.h"


namespace maidsafe {

namespace nfs {

void PmidRegistration::Parse(const NonEmptyString& serialised_pmid_registration) {
  nfs::protobuf::PmidRegistration proto_pmid_registration;
  if (!proto_pmid_registration.ParseFromString(serialised_pmid_registration.string())) {
    LOG(kError) << "Failed to parse pmid_registration.";
    ThrowError(NfsErrors::pmid_registration_parsing_error);
  }

  nfs::protobuf::PmidRegistration::Details details;
  if (!details.ParseFromString(proto_pmid_registration.serialised_details())) {
    LOG(kError) << "Failed to parse pmid_registration.";
    ThrowError(NfsErrors::pmid_registration_parsing_error);
  }

  maid_id_ = Identity(details.maid_name());
  pmid_id_ = Identity(details.pmid_name());
  register_ = details.register_();
  maid_signature_ = NonEmptyString(proto_pmid_registration.maid_signature());
  pmid_signature_ = NonEmptyString(proto_pmid_registration.pmid_signature());
}

NonEmptyString PmidRegistration::Serialise() {
  nfs::protobuf::PmidRegistration proto_pmid_registration;
  nfs::protobuf::PmidRegistration::Details details;
  details.set_maid_name(maid_id_.string());
  details.set_pmid_name(pmid_id_.string());
  details.set_register_(register_);

  proto_pmid_registration.set_serialised_details(details.SerializeAsString());
  proto_pmid_registration.set_maid_signature(maid_signature_.string());
  proto_pmid_registration.set_pmid_signature(pmid_signature_.string());
  return NonEmptyString(proto_pmid_registration.SerializeAsString());
}

}  // namespace nfs

}  // namespace maidsafe
