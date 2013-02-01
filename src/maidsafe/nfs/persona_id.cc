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

#include "maidsafe/nfs/persona_id.h"


namespace maidsafe {

namespace nfs {

PersonaId::PersonaId() : persona(), node_id() {}

PersonaId::PersonaId(Persona persona_in, const NodeId& node_id_in)
    : persona(persona_in),
      node_id(node_id_in) {}

PersonaId::PersonaId(const PersonaId& other) : persona(other.persona), node_id(other.node_id) {}

PersonaId& PersonaId::operator=(const PersonaId& other) {
  persona = other.persona;
  node_id = other.node_id;
  return *this;
}

PersonaId::PersonaId(PersonaId&& other)
    : persona(std::move(other.persona)),
      node_id(std::move(other.node_id)) {}

PersonaId& PersonaId::operator=(PersonaId&& other) {
  persona = std::move(other.persona);
  node_id = std::move(other.node_id);
  return *this;
}

}  // namespace nfs

}  // namespace maidsafe
