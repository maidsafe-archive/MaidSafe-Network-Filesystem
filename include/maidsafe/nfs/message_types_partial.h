/*  Copyright 2013 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#ifndef MAIDSAFE_NFS_MESSAGE_TYPES_PARTIAL_H_
#define MAIDSAFE_NFS_MESSAGE_TYPES_PARTIAL_H_

// #include <string>

// #include "boost/variant/variant.hpp"

#include "maidsafe/nfs/message_wrapper.h"
#include "maidsafe/routing/message.h"

// #include "maidsafe/common/error.h"

#include "maidsafe/nfs/client/messages.h"
#include "maidsafe/nfs/vault/messages.h"

namespace maidsafe { namespace nfs_client { struct DataNameAndContentOrReturnCode; } }
namespace maidsafe { namespace nfs_vault { struct DataName; } }

namespace maidsafe {

namespace nfs {

typedef maidsafe::nfs::MessageWrapper<
    maidsafe::nfs::MessageAction::kGetRequest,
    maidsafe::nfs::SourcePersona<maidsafe::nfs::Persona::kDataGetter>,
    maidsafe::routing::SingleRelaySource,
    maidsafe::nfs::DestinationPersona<maidsafe::nfs::Persona::kDataManager>,
    maidsafe::routing::GroupId,
    maidsafe::nfs_vault::DataName>
        GetRequestFromDataGetterPartialToDataManager;

typedef maidsafe::nfs::MessageWrapper<
    maidsafe::nfs::MessageAction::kGetResponse,
    maidsafe::nfs::SourcePersona<maidsafe::nfs::Persona::kDataManager>,
    maidsafe::routing::GroupSource,
    maidsafe::nfs::DestinationPersona<maidsafe::nfs::Persona::kDataGetter>,
    maidsafe::routing::SingleIdRelay,
    maidsafe::nfs_client::DataNameAndContentOrReturnCode>
        GetResponseFromDataManagerToDataGetterPartial;

typedef maidsafe::nfs::MessageWrapper<
    maidsafe::nfs::MessageAction::kGetResponse,
    maidsafe::nfs::SourcePersona<maidsafe::nfs::Persona::kDataManager>,
    maidsafe::routing::GroupSource,
    maidsafe::nfs::DestinationPersona<maidsafe::nfs::Persona::kMaidNode>,
    maidsafe::routing::SingleIdRelay,
    maidsafe::nfs_client::DataNameAndContentOrReturnCode>
        GetResponseFromDataManagerToMaidNodePartial;

typedef maidsafe::nfs::MessageWrapper<
    maidsafe::nfs::MessageAction::kGetRequest,
    maidsafe::nfs::SourcePersona<maidsafe::nfs::Persona::kMpidNode>,
    maidsafe::routing::SingleRelaySource,
    maidsafe::nfs::DestinationPersona<maidsafe::nfs::Persona::kDataManager>,
    maidsafe::routing::GroupId,
    maidsafe::nfs_vault::DataName>
        GetRequestFromMpidNodePartialToDataManager;

typedef maidsafe::nfs::MessageWrapper<
    maidsafe::nfs::MessageAction::kGetResponse,
    maidsafe::nfs::SourcePersona<maidsafe::nfs::Persona::kDataManager>,
    maidsafe::routing::GroupSource,
    maidsafe::nfs::DestinationPersona<maidsafe::nfs::Persona::kMpidNode>,
    maidsafe::routing::SingleIdRelay,
    maidsafe::nfs_client::DataNameAndContentOrReturnCode>
        GetResponseFromDataManagerToMpidNodePartial;

typedef maidsafe::nfs::MessageWrapper<
    maidsafe::nfs::MessageAction::kGetRequest,
    maidsafe::nfs::SourcePersona<maidsafe::nfs::Persona::kMpidNode>,
    maidsafe::routing::SingleRelaySource,
    maidsafe::nfs::DestinationPersona<maidsafe::nfs::Persona::kDataManager>,
    maidsafe::routing::GroupId,
    maidsafe::nfs_vault::DataName>
        GetRequestFromMpidNodePartialToDataManager;

typedef maidsafe::nfs::MessageWrapper<
    maidsafe::nfs::MessageAction::kGetResponse,
    maidsafe::nfs::SourcePersona<maidsafe::nfs::Persona::kDataManager>,
    maidsafe::routing::GroupSource,
    maidsafe::nfs::DestinationPersona<maidsafe::nfs::Persona::kMpidNode>,
    maidsafe::routing::SingleIdRelay,
    maidsafe::nfs_client::DataNameAndContentOrReturnCode>
        GetResponseFromDataManagerToMpidNodePartial;

}  // namespace nfs

}  // namespace maidsafe


#endif  // MAIDSAFE_NFS_MESSAGE_TYPES_PARTIAL_H_
