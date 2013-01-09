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

#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/maid_account.h"

#include "maidsafe/nfs/containers_pb.h"
#include "maidsafe/nfs/post_messages_pb.h"

namespace maidsafe {

namespace nfs {

void PmidRegistration::Parse(const NonEmptyString& serialised_pmidregistration) {
  nfs::protobuf::PmidRegistration proto_pmidregistration;
  if (!proto_pmidregistration.ParseFromString(serialised_pmidregistration.string()) ||
      !proto_pmidregistration.IsInitialized()) {
    LOG(kError) << "Failed to parse pmid_registration.";
    ThrowError(NfsErrors::pmid_registration_parsing_error);
  }

  maid_id_ = Identity(proto_pmidregistration.maid_id());
  pmid_id_ = Identity(proto_pmidregistration.pmid_id());
  register_ = proto_pmidregistration.register_();
  maid_signature_ = NonEmptyString(proto_pmidregistration.maid_signature());
  pmid_signature_ = NonEmptyString(proto_pmidregistration.pmid_signature());
}

NonEmptyString PmidRegistration::Serialise() {
  nfs::protobuf::PmidRegistration proto_pmidregistration;
  proto_pmidregistration.set_maid_id(maid_id_.string());
  proto_pmidregistration.set_pmid_id(pmid_id_.string());
  proto_pmidregistration.set_register_(register_);
  proto_pmidregistration.set_maid_signature(maid_signature_.string());
  proto_pmidregistration.set_pmid_signature(pmid_signature_.string());
  return NonEmptyString(proto_pmidregistration.SerializeAsString());
}

void PmidSize::Parse(const NonEmptyString& serialised_pmidsize) {
  nfs::protobuf::PmidSize proto_pmidsize;
  if (!proto_pmidsize.ParseFromString(serialised_pmidsize.string()) ||
      !proto_pmidsize.IsInitialized()) {
    LOG(kError) << "Failed to parse pmid_size.";
    ThrowError(NfsErrors::pmid_size_parsing_error);
  }

  pmid_id = Identity(proto_pmidsize.pmid_id());
  num_data_elements = proto_pmidsize.num_data_elements();
  total_size = proto_pmidsize.total_size();
  lost_size = proto_pmidsize.lost_size();
  lost_number_of_elements = proto_pmidsize.lost_number_of_elements();
}

NonEmptyString PmidSize::Serialise() {
  nfs::protobuf::PmidSize proto_pmidsize;
  proto_pmidsize.set_pmid_id(pmid_id.string());
  proto_pmidsize.set_num_data_elements(num_data_elements);
  proto_pmidsize.set_total_size(total_size);
  proto_pmidsize.set_lost_size(lost_size);
  proto_pmidsize.set_lost_number_of_elements(lost_number_of_elements);
  return NonEmptyString(proto_pmidsize.SerializeAsString());
}

NonEmptyString PmidTotal::Serialise() {
  nfs::protobuf::PmidSize proto_pmidsize;
  proto_pmidsize.ParseFromString(pmid_size.Serialise().string());
  nfs::protobuf::PmidRegistration proto_pmidregistration;
  proto_pmidregistration.ParseFromString(registration.Serialise().string());

  nfs::protobuf::PmidTotals proto_pmidtotal;
  *(proto_pmidtotal.mutable_pmid_size()) = proto_pmidsize;
  *(proto_pmidtotal.mutable_registration()) = proto_pmidregistration;
  return NonEmptyString(proto_pmidtotal.SerializeAsString());
}

NonEmptyString DataElement::Serialise() {
  nfs::protobuf::DataElements proto_dataelement;
  proto_dataelement.set_data_id(data_id.string());
  proto_dataelement.set_data_size(data_size);
  return NonEmptyString(proto_dataelement.SerializeAsString());
}

void MaidAccount::Parse(const NonEmptyString& serialised_maidaccount) {
  nfs::protobuf::MaidAccount proto_maidaccount;
  if (!proto_maidaccount.ParseFromString(serialised_maidaccount.string()) ||
      !proto_maidaccount.IsInitialized()) {
    LOG(kError) << "Failed to parse maid_account.";
    ThrowError(NfsErrors::maid_account_parsing_error);
  }

  maid_id_ = Identity(proto_maidaccount.maid_id());

  data_elements.clear();
  for (int i(0); i != proto_maidaccount.data_elements_size(); ++i) {
    data_elements.push_back(DataElement(Identity(proto_maidaccount.data_elements(i).data_id()),
                                        proto_maidaccount.data_elements(i).data_size()));
  }

  pmid_totals.clear();
  for (int i(0); i != proto_maidaccount.data_elements_size(); ++i) {
    pmid_totals.push_back(
        PmidTotal(PmidRegistration(NonEmptyString(
                      proto_maidaccount.pmid_totals(i).registration().SerializeAsString())),
                  PmidSize(NonEmptyString(
                      proto_maidaccount.pmid_totals(i).pmid_size().SerializeAsString()))));
  }
}

NonEmptyString MaidAccount::Serialise() {
  nfs::protobuf::MaidAccount proto_maidaccount;
  proto_maidaccount.set_maid_id(maid_id_.string());
  for (auto& pmid_total : pmid_totals) {
    nfs::protobuf::PmidTotals proto_pmidtotal;
    proto_pmidtotal.ParseFromString(pmid_total.Serialise().string());
    *(proto_maidaccount.add_pmid_totals()) = proto_pmidtotal;
  }
  for (auto& data_element : data_elements) {
    nfs::protobuf::DataElements proto_dataelements;
    proto_dataelements.ParseFromString(data_element.Serialise().string());
    *(proto_maidaccount.add_data_elements()) = proto_dataelements;
  }
  return NonEmptyString(proto_maidaccount.SerializeAsString());
}

}  // namespace nfs

}  // namespace maidsafe
