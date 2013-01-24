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

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/generic_message_pb.h"


namespace maidsafe {

namespace nfs {

namespace {

asymm::PlainText GetSerialisedDetails(const passport::PublicMaid::name_type& maid_name,
                                      const passport::PublicPmid::name_type& pmid_name,
                                      bool unregister) {
  protobuf::PmidRegistration::SignedDetails::Details details;
  details.set_maid_name(maid_name->string());
  details.set_pmid_name(pmid_name->string());
  details.set_unregister(unregister);
  return asymm::PlainText(details.SerializeAsString());
}

asymm::PlainText GetSerialisedSignedDetails(const asymm::PlainText& serialised_details,
                                            const asymm::Signature& pmid_signature) {
  protobuf::PmidRegistration::SignedDetails signed_details;
  signed_details.set_serialised_details(serialised_details.string());
  signed_details.set_pmid_signature(pmid_signature.string());
  return asymm::PlainText(signed_details.SerializeAsString());
}

}  //  unnamed namespace

PmidRegistration::PmidRegistration(const passport::Maid& maid,
                                   const passport::Pmid& pmid,
                                   bool unregister)
    : maid_name_(maid.name()),
      pmid_name_(pmid.name()),
      unregister_(unregister),
      maid_signature_(),
      pmid_signature_() {
  auto serialised_details(GetSerialisedDetails(maid_name_, pmid_name_, unregister_));
  pmid_signature_ = asymm::Sign(serialised_details, pmid.private_key());

  auto serialised_signed_details(GetSerialisedSignedDetails(serialised_details, pmid_signature_));
  maid_signature_ = asymm::Sign(serialised_signed_details, maid.private_key());
}

PmidRegistration::PmidRegistration(const serialised_type& serialised_pmid_registration)
    : maid_name_(),
      pmid_name_(),
      unregister_(),
      maid_signature_(),
      pmid_signature_() {
  auto fail([]() {
    LOG(kError) << "Failed to parse pmid_registration.";
    ThrowError(CommonErrors::parsing_error);
  });
  protobuf::PmidRegistration proto_pmid_registration;
  if (!proto_pmid_registration.ParseFromString(serialised_pmid_registration->string()))
    fail();
  protobuf::PmidRegistration::SignedDetails signed_details;
  if (!signed_details.ParseFromString(proto_pmid_registration.serialised_signed_details()))
    fail();
  protobuf::PmidRegistration::SignedDetails::Details details;
  if (!details.ParseFromString(signed_details.serialised_details()))
    fail();

  maid_name_ = passport::PublicMaid::name_type(Identity(details.maid_name()));
  pmid_name_ = passport::PublicPmid::name_type(Identity(details.pmid_name()));
  unregister_ = details.unregister();
  maid_signature_ = asymm::Signature(proto_pmid_registration.maid_signature());
  pmid_signature_ = asymm::Signature(signed_details.pmid_signature());
}

bool PmidRegistration::Validate(const passport::PublicMaid& public_maid,
                                const passport::PublicPmid& public_pmid) const {
  auto serialised_details(GetSerialisedDetails(maid_name_, pmid_name_, unregister_));
  if (!asymm::CheckSignature(serialised_details, pmid_signature_, public_pmid.public_key())) {
    LOG(kWarning) << "Failed to validate PMID signature.";
    return false;
  }
  auto serialised_signed_details(GetSerialisedSignedDetails(serialised_details, pmid_signature_));
  if (!asymm::CheckSignature(serialised_signed_details, maid_signature_,
                             public_maid.public_key())) {
    LOG(kWarning) << "Failed to validate MAID signature.";
    return false;
  }
  return true;
}

PmidRegistration::serialised_type PmidRegistration::Serialise() const {
  protobuf::PmidRegistration proto_pmid_registration;
  proto_pmid_registration.set_serialised_signed_details(
      GetSerialisedSignedDetails(GetSerialisedDetails(maid_name_, pmid_name_, unregister_),
                                 pmid_signature_).string());
  proto_pmid_registration.set_maid_signature(maid_signature_.string());
  return serialised_type(NonEmptyString(proto_pmid_registration.SerializeAsString()));
}

}  // namespace nfs

}  // namespace maidsafe
