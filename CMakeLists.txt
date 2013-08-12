#==================================================================================================#
#                                                                                                  #
#  Copyright 2012 MaidSafe.net limited                                                             #
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


set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)
project(nfs)

include(../../cmake_modules/standard_setup.cmake)

include_directories(${common_SOURCE_DIR}/include)
include_directories(${passport_SOURCE_DIR}/include)
include_directories(${private_SOURCE_DIR}/include)
include_directories(${rudp_SOURCE_DIR}/include)
include_directories(${routing_SOURCE_DIR}/include)


#==================================================================================================#
# Set up all files as GLOBs                                                                        #
#==================================================================================================#
set(OutputFile ${PROJECT_SOURCE_DIR}/include/maidsafe/nfs/message_types.h)
set(InputFile ${PROJECT_SOURCE_DIR}/cmake/message_types.h.in)
set(MessageTypes ${PROJECT_SOURCE_DIR}/cmake/message_types.cmake)
# An apparent bug in CMake means that file(READ...) can only be done from within
# CMAKE_CURRENT_BINARY_DIR.  Hence message_types.cmake is copied here to allow configure.cmake to work.
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/message_types.cmake
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MessageTypes} ${CMAKE_CURRENT_BINARY_DIR}/message_types.cmake
                   DEPENDS ${MessageTypes}
                   COMMENT "Copying message_types.cmake to ${CMAKE_CURRENT_BINARY_DIR}")
set(CMAKE_DISABLE_SOURCE_CHANGES OFF)
add_custom_command(OUTPUT ${OutputFile}
                   COMMAND ${CMAKE_COMMAND} -DOutputFile="${OutputFile}"
                                            -DInputFile="${InputFile}"
                                            -P ${PROJECT_SOURCE_DIR}/cmake/configure.cmake
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/message_types.cmake
                           ${InputFile}
                           ${PROJECT_SOURCE_DIR}/cmake/configure.cmake
                   COMMENT "Configuring message_types.h")
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set_source_files_properties(${OutputFile} PROPERTIES GENERATED TRUE)
source_group("Nfs API Files" FILES ${OutputFile})
source_group("Nfs CMake Files" FILES ${CMAKE_CURRENT_LIST_FILE}
                                     ${InputFile}
                                     ${PROJECT_SOURCE_DIR}/cmake/configure.cmake
                                     ${PROJECT_SOURCE_DIR}/cmake/message_types.cmake)

glob_dir(Nfs ${PROJECT_SOURCE_DIR}/src/maidsafe/nfs Nfs)
glob_dir(NfsTests ${PROJECT_SOURCE_DIR}/src/maidsafe/nfs/tests Tests)


#==================================================================================================#
# Define MaidSafe libraries and executables                                                        #
#==================================================================================================#
ms_add_static_library(nfs ${NfsAllFiles} ${OutputFile} ${InputFile}
                      ${PROJECT_SOURCE_DIR}/cmake/configure.cmake
                      ${PROJECT_SOURCE_DIR}/cmake/message_types.cmake)
target_link_libraries(maidsafe_nfs maidsafe_routing)

if(MaidsafeTesting)
  ms_add_executable(TESTnfs "Tests/NFS" ${NfsTestsAllFiles})
  target_link_libraries(TESTnfs maidsafe_nfs maidsafe_private)
endif()

rename_outdated_built_exes()


#==================================================================================================#
# Set compiler and linker flags                                                                    #
#==================================================================================================#
include(standard_flags)
if(MSVC AND MaidsafeTesting)
  set_target_properties(TESTnfs PROPERTIES COMPILE_FLAGS "/bigobj")
endif()


#==================================================================================================#
# Tests                                                                                            #
#==================================================================================================#
if(MaidsafeTesting)
  add_style_test()
  add_gtests(TESTnfs)
# label_as_critical_tests(IdentityPacketsTest.BEH_Full
                          #IdentityPacketsTest.BEH_ChangeDetails
                          #nfsTest.BEH_SigningPackets
                          #nfsTest.BEH_IdentityPackets
                          #nfsTest.BEH_ChangingIdentityPackets
                          #nfsTest.BEH_MoveMaidsafeInbox
                          #  nfsTest.BEH_SerialiseParse
                          #                          )
  add_project_experimental()
  test_summary_output()
endif()
