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

namespace {

std::system_error GetError(int error_value, const std::string& error_category_name) {
  if (error_category_name == std::string(GetCommonCategory().name()))
    return MakeError(static_cast<CommonErrors>(error_value));
  if (error_category_name == std::string(GetAsymmCategory().name()))
    return MakeError(static_cast<AsymmErrors>(error_value));
  if (error_category_name == std::string(GetPassportCategory().name()))
    return MakeError(static_cast<PassportErrors>(error_value));
  if (error_category_name == std::string(GetNfsCategory().name()))
    return MakeError(static_cast<NfsErrors>(error_value));
  if (error_category_name == std::string(GetRoutingCategory().name()))
    return MakeError(static_cast<RoutingErrors>(error_value));
  if (error_category_name == std::string(GetVaultCategory().name()))
    return MakeError(static_cast<VaultErrors>(error_value));
  if (error_category_name == std::string(GetLifeStuffCategory().name()))
    return MakeError(static_cast<LifeStuffErrors>(error_value));
  ThrowError(CommonErrors::parsing_error);
  return MakeError(CommonErrors::parsing_error);
}

}  // unnamed namespace

const MessageCategory ReturnCode::message_type_identifier = MessageCategory::kReply;


ReturnCode::ReturnCode(std::system_error error, const NonEmptyString& data)
    : error_(error),
      data_(data) {}

ReturnCode::ReturnCode(const ReturnCode& other) : error_(other.error_), data_(other.data_) {}

ReturnCode& ReturnCode::operator=(const ReturnCode& other) {
  error_ = other.error_;
  data_ = other.data_;
  return *this;
}

ReturnCode::ReturnCode(ReturnCode&& other)
    : error_(std::move(other.error_)),
      data_(std::move(other.data_)) {}

ReturnCode& ReturnCode::operator=(ReturnCode&& other) {
  error_ = std::move(other.error_);
  data_ = std::move(other.data_);
  return *this;
}

// TODO(Fraser#5#): 2012-12-24 - Once MSVC eventually handles delegating constructors, we can make
//                  this more efficient by using a lambda which returns the parsed protobuf
//                  inside a private constructor taking a single arg of type protobuf.
ReturnCode::ReturnCode(const serialised_type& serialised_return_code)
    : error_(CommonErrors::success),
      data_() {
  protobuf::ReturnCode proto_return_code;
  if (!proto_return_code.ParseFromString(serialised_return_code->string()))
    ThrowError(CommonErrors::parsing_error);

  error_ = GetError(proto_return_code.error_value(), proto_return_code.error_category_name());

  if (proto_return_code.has_data())
    data_ = NonEmptyString(proto_return_code.data());
}

ReturnCode::serialised_type ReturnCode::Serialise() const {
  protobuf::ReturnCode proto_return_code;
  proto_return_code.set_error_value(error_.code().value());
  proto_return_code.set_error_category_name(error_.code().category().name());
  if (data_.IsInitialised())
    proto_return_code.set_data(data_.string());
  return serialised_type(NonEmptyString(proto_return_code.SerializeAsString()));
}

bool ReturnCode::IsSuccess() const {
  return error_.code().category().name() == std::string(GetCommonCategory().name()) &&
         static_cast<CommonErrors>(error_.code().value()) == CommonErrors::success;
}

}  // namespace nfs

}  // namespace maidsafe
