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

#include "maidsafe/nfs/return_code.h"

#include "maidsafe/common/error.h"

#include "maidsafe/nfs/return_code_pb.h"


namespace maidsafe {

namespace nfs {

ReturnCode::ReturnCode(int value, const Info& info) : value_(value), info_(info) {}

ReturnCode::ReturnCode(const ReturnCode& other) : value_(other.value_), info_(other.info_) {}

ReturnCode& ReturnCode::operator=(const ReturnCode& other) {
  value_ = other.value_;
  info_ = other.info_;
  return *this;
}

ReturnCode::ReturnCode(ReturnCode&& other)
    : value_(std::move(other.value_)),
      info_(std::move(other.info_)) {}

ReturnCode& ReturnCode::operator=(ReturnCode&& other) {
  value_ = std::move(other.value_);
  info_ = std::move(other.info_);
  return *this;
}

// TODO(Fraser#5#): 2012-12-24 - Once MSVC eventually handles delegating constructors, we can make
//                  this more efficient by using a lambda which returns the parsed protobuf
//                  inside a private constructor taking a single arg of type protobuf.
ReturnCode::ReturnCode(const serialised_type& serialised_return_code) : value_(), info_() {
  protobuf::ReturnCode proto_return_code;
  if (!proto_return_code.ParseFromString(serialised_return_code.data.string()))
    ThrowError(NfsErrors::return_code_parsing_error);
  value_ = proto_return_code.value();
  if (proto_return_code.has_info())
    info_ = Info(proto_return_code.info());
}

ReturnCode::serialised_type ReturnCode::Serialise() const {
  protobuf::ReturnCode proto_return_code;
  proto_return_code.set_value(value_);
  if (info_.IsInitialised())
    proto_return_code.set_info(info_.string());
  return serialised_type(NonEmptyString(proto_return_code.SerializeAsString()));
}

}  // namespace nfs

}  // namespace maidsafe
