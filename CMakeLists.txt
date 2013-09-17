#==================================================================================================#
#                                                                                                  #
#  Copyright 2012 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,        #
#  version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which    #
#  licence you accepted on initial access to the Software (the "Licences").                        #
#                                                                                                  #
#  By contributing code to the MaidSafe Software, or to this project generally, you agree to be    #
#  bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root        #
#  directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available   #
#  at: http://www.novinet.com/license                                                              #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed    #
#  under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF   #
#  ANY KIND, either express or implied.                                                            #
#                                                                                                  #
#  See the Licences for the specific language governing permissions and limitations relating to    #
#  use of the MaidSafe Software.                                                                   #
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
file(GLOB_RECURSE MetaFiles "${CMAKE_CURRENT_SOURCE_DIR}/cmake/*.message_types.meta")
set_meta_files_custom_commands("${OutputFile}" "${InputFile}" "${MetaFiles}" "Nfs API Files" "Nfs CMake Files")

set(NfsSourcesDir ${PROJECT_SOURCE_DIR}/src/maidsafe/nfs)
glob_dir(Nfs ${NfsSourcesDir} Nfs)
glob_dir(NfsClient ${NfsSourcesDir}/client "Nfs Client")
glob_dir(NfsVault ${NfsSourcesDir}/vault "Nfs Vault")
glob_dir(NfsTests ${NfsSourcesDir}/tests Tests)


#==================================================================================================#
# Define MaidSafe libraries and executables                                                        #
#==================================================================================================#
ms_add_static_library(nfs_core ${NfsAllFiles} ${OutputFile} ${InputFile} ${MetaFiles})
ms_add_static_library(nfs_client ${NfsClientAllFiles})
ms_add_static_library(nfs_vault ${NfsVaultAllFiles})
target_link_libraries(maidsafe_nfs_core maidsafe_routing)
target_link_libraries(maidsafe_nfs_client maidsafe_nfs_vault maidsafe_nfs_core)
target_link_libraries(maidsafe_nfs_vault maidsafe_nfs_client maidsafe_nfs_core)

if(MaidsafeTesting)
  ms_add_executable(TESTnfs "Tests/NFS" ${NfsTestsAllFiles})
  target_link_libraries(TESTnfs maidsafe_nfs_core maidsafe_nfs_client maidsafe_nfs_vault maidsafe_private)
endif()

rename_outdated_built_exes()


#==================================================================================================#
# Set compiler and linker flags                                                                    #
#==================================================================================================#
include(standard_flags)


#==================================================================================================#
# Tests                                                                                            #
#==================================================================================================#
if(MaidsafeTesting)
  add_style_test()
  add_gtests(TESTnfs)
  add_project_experimental()
  test_summary_output()
endif()
