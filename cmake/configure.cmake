#==================================================================================================#
#                                                                                                  #
#  Copyright 2013 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or    #
#  later, and The General Public License (GPL), version 3. By contributing code to this project    #
#  You agree to the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in    #
#  the root directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also    #
#  available at:                                                                                   #
#                                                                                                  #
#    http://www.novinet.com/license                                                                #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, software distributed under the       #
#  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       #
#  either express or implied. See the License for the specific language governing permissions      #
#  and limitations under the License.                                                              #
#                                                                                                  #
#==================================================================================================#


function(EnumClassToList FileName EnumName)
  file(READ "${FileName}" Contents)
  string(REGEX REPLACE ".*enum[ ]+class[ ]+${EnumName}[ ]+:[ ]+[u]?int[1368][246]?_t[ ]+{([^}]*).*" "\\1" Values "${Contents}")
  string(REGEX REPLACE "[ ,\n]" ";" Values "${Values}")
  set(${EnumName} ${Values} PARENT_SCOPE)
endfunction()

EnumClassToList(types.h MessageAction)
EnumClassToList(types.h Persona)

foreach(Action ${MessageAction})
  list(APPEND ActionSwitch "    case MessageAction::${Action}:\n")
  list(APPEND ActionSwitch "      HandleAction<MessageWrapperVariant,\n")
  list(APPEND ActionSwitch "                   MessageAction::${Action}>(variant, message);\n")
  list(APPEND ActionSwitch "      break;\n")
endforeach()
string(REPLACE "\n;" "\n" ActionSwitch "${ActionSwitch}")

foreach(Destination ${Persona})
  list(APPEND DestinationSwitch "    case Persona::${Destination}:\n")
  list(APPEND DestinationSwitch "      HandleDestination<MessageWrapperVariant,\n")
  list(APPEND DestinationSwitch "                        action,\n")
  list(APPEND DestinationSwitch "                        DestinationPersona<Persona::${Destination}>>(variant, message);\n")
  list(APPEND DestinationSwitch "      break;\n")
endforeach()
string(REPLACE "\n;" "\n" DestinationSwitch "${DestinationSwitch}")

foreach(Source ${Persona})
  list(APPEND SourceSwitch "    case Persona::${Source}:\n")
  list(APPEND SourceSwitch "      variant = GetVariantFromMessage<\n")
  list(APPEND SourceSwitch "                    MessageWrapperVariant,\n")
  list(APPEND SourceSwitch "                    MessageWrapper<action, DestinationPersonaType,\n")
  list(APPEND SourceSwitch "                                   SourcePersona<Persona::${Source}>>>(message);\n")
  list(APPEND SourceSwitch "      break;\n")
endforeach()
string(REPLACE "\n;" "\n" SourceSwitch "${SourceSwitch}")

configure_file(${InputFile} ${OutputFile} @ONLY)
