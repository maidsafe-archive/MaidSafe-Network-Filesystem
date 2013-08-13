/* Copyright 2012 MaidSafe.net limited

This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or later,
and The General Public License (GPL), version 3. By contributing code to this project You agree to
the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in the root directory
of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available at:

http://www.novinet.com/license

Unless required by applicable law or agreed to in writing, software distributed under the License is
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. See the License for the specific language governing permissions and limitations under the
License.
*/

#include "maidsafe/nfs/pmid_registration.h"

#include "maidsafe/common/rsa.h"
#include "maidsafe/common/utils.h"

#include "maidsafe/nfs/messages.pb.h"


namespace maidsafe {

namespace nfs {

namespace {

asymm::PlainText GetSerialisedDetails(const passport::PublicMaid::Name& maid_name,
                                      const passport::PublicPmid::Name& pmid_name,
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

PmidRegistration::PmidRegistration()
    : maid_name_(),
      pmid_name_(),
      unregister_(false),
      maid_signature_(),
      pmid_signature_() {}

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

PmidRegistration::PmidRegistration(const std::string& serialised_copy)
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
  if (!proto_pmid_registration.ParseFromString(serialised_copy))
    fail();
  protobuf::PmidRegistration::SignedDetails signed_details;
  if (!signed_details.ParseFromString(proto_pmid_registration.serialised_signed_details()))
    fail();
  protobuf::PmidRegistration::SignedDetails::Details details;
  if (!details.ParseFromString(signed_details.serialised_details()))
    fail();

  maid_name_ = passport::PublicMaid::Name(Identity(details.maid_name()));
  pmid_name_ = passport::PublicPmid::Name(Identity(details.pmid_name()));
  unregister_ = details.unregister();
  maid_signature_ = asymm::Signature(proto_pmid_registration.maid_signature());
  pmid_signature_ = asymm::Signature(signed_details.pmid_signature());
}

PmidRegistration::PmidRegistration(const PmidRegistration& other)
    : maid_name_(other.maid_name_),
      pmid_name_(other.pmid_name_),
      unregister_(other.unregister_),
      maid_signature_(other.maid_signature_),
      pmid_signature_(other.pmid_signature_) {}

PmidRegistration::PmidRegistration(PmidRegistration&& other)
    : maid_name_(std::move(other.maid_name_)),
      pmid_name_(std::move(other.pmid_name_)),
      unregister_(std::move(other.unregister_)),
      maid_signature_(std::move(other.maid_signature_)),
      pmid_signature_(std::move(other.pmid_signature_)) {}

PmidRegistration& PmidRegistration::operator=(PmidRegistration other) {
  swap(*this, other);
  return *this;
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

std::string PmidRegistration::Serialise() const {
  protobuf::PmidRegistration proto_pmid_registration;
  proto_pmid_registration.set_serialised_signed_details(
      GetSerialisedSignedDetails(GetSerialisedDetails(maid_name_, pmid_name_, unregister_),
                                 pmid_signature_).string());
  proto_pmid_registration.set_maid_signature(maid_signature_.string());
  return proto_pmid_registration.SerializeAsString();
}

void swap(PmidRegistration& lhs, PmidRegistration& rhs) {
  using std::swap;
  swap(lhs.maid_name_, rhs.maid_name_);
  swap(lhs.pmid_name_, rhs.pmid_name_);
  swap(lhs.unregister_, rhs.unregister_);
  swap(lhs.maid_signature_, rhs.maid_signature_);
  swap(lhs.pmid_signature_, rhs.pmid_signature_);
}

}  // namespace nfs

}  // namespace maidsafe
