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
