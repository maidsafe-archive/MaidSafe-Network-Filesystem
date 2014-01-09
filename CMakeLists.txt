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
#  at: http://www.maidsafe.net/licenses                                                            #
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

project(nfs)

include(../../cmake_modules/standard_setup.cmake)


#==================================================================================================#
# Set up all files as GLOBs                                                                        #
#==================================================================================================#
set(OutputFile ${PROJECT_SOURCE_DIR}/include/maidsafe/nfs/message_types.h)
set(InputFile ${PROJECT_SOURCE_DIR}/cmake/message_types.h.in)
file(GLOB_RECURSE MetaFiles "${CMAKE_CURRENT_SOURCE_DIR}/cmake/*.message_types.meta")
ms_set_meta_files_custom_commands("${OutputFile}" "${InputFile}" "${MetaFiles}" "Nfs API Files" "Nfs CMake Files")

set(NfsSourcesDir ${PROJECT_SOURCE_DIR}/src/maidsafe/nfs)
ms_glob_dir(Nfs ${NfsSourcesDir} Nfs)
ms_glob_dir(NfsClient ${NfsSourcesDir}/client "Nfs Client")
ms_glob_dir(NfsVault ${NfsSourcesDir}/vault "Nfs Vault")
ms_glob_dir(NfsTests ${NfsSourcesDir}/tests Tests)


#==================================================================================================#
# Define MaidSafe libraries and executables                                                        #
#==================================================================================================#
ms_add_static_library(maidsafe_nfs_core ${NfsAllFiles} ${OutputFile} ${InputFile} ${MetaFiles})
ms_add_static_library(maidsafe_nfs_client ${NfsClientAllFiles})
ms_add_static_library(maidsafe_nfs_vault ${NfsVaultAllFiles})
target_include_directories(maidsafe_nfs_core PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(maidsafe_nfs_client PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(maidsafe_nfs_vault PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(maidsafe_nfs_core maidsafe_routing)
target_link_libraries(maidsafe_nfs_client maidsafe_nfs_vault maidsafe_nfs_core)
target_link_libraries(maidsafe_nfs_vault maidsafe_nfs_client maidsafe_nfs_core)

if(MaidsafeTesting)
  ms_add_executable(TESTnfs "Tests/NFS" ${NfsTestsAllFiles})
  target_link_libraries(TESTnfs maidsafe_nfs_core maidsafe_nfs_client maidsafe_nfs_vault maidsafe_private)
endif()

ms_rename_outdated_built_exes()


#==================================================================================================#
# Set compiler and linker flags                                                                    #
#==================================================================================================#
include(standard_flags)


#==================================================================================================#
# Tests                                                                                            #
#==================================================================================================#
if(MaidsafeTesting)
  ms_add_style_test()
  ms_add_gtests(TESTnfs)
  ms_add_project_experimental()
  ms_test_summary_output()
endif()
