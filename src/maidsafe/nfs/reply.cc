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

#include "maidsafe/nfs/reply.h"

#include "maidsafe/common/error.h"

#include "maidsafe/nfs/reply_pb.h"


namespace maidsafe {

namespace nfs {

namespace {

maidsafe_error GetError(int error_value, const std::string& error_category_name) {
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

const MessageCategory Reply::message_type_identifier = MessageCategory::kReply;


Reply::Reply(const Reply& other) : error_(other.error_), data_(other.data_) {}

Reply& Reply::operator=(const Reply& other) {
  error_ = other.error_;
  data_ = other.data_;
  return *this;
}

Reply::Reply(Reply&& other) : error_(std::move(other.error_)), data_(std::move(other.data_)) {}

Reply& Reply::operator=(Reply&& other) {
  error_ = std::move(other.error_);
  data_ = std::move(other.data_);
  return *this;
}

// TODO(Fraser#5#): 2012-12-24 - Once MSVC eventually handles delegating constructors, we can make
//                  this more efficient by using a lambda which returns the parsed protobuf
//                  inside a private constructor taking a single arg of type protobuf.
Reply::Reply(const serialised_type& serialised_reply)
    : error_(CommonErrors::success),
      data_() {
  protobuf::Reply proto_reply;
  if (!proto_reply.ParseFromString(serialised_reply->string()))
    ThrowError(CommonErrors::parsing_error);

  error_ = GetError(proto_reply.error_value(), proto_reply.error_category_name());

  if (proto_reply.has_data())
    data_ = NonEmptyString(proto_reply.data());
}

Reply::serialised_type Reply::Serialise() const {
  protobuf::Reply proto_reply;
  proto_reply.set_error_value(error_.code().value());
  proto_reply.set_error_category_name(error_.code().category().name());
  if (data_.IsInitialised())
    proto_reply.set_data(data_.string());
  return serialised_type(NonEmptyString(proto_reply.SerializeAsString()));
}

bool Reply::IsSuccess() const {
  static std::error_code success((MakeError(CommonErrors::success)).code());
  return error_.code() == success;
}

}  // namespace nfs

}  // namespace maidsafe
