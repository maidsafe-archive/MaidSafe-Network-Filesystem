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
set(IntermediateDir "${CMAKE_CURRENT_BINARY_DIR}/copied_message_types")

# An apparent bug in CMake means that file(READ...) can only be done from within
# CMAKE_CURRENT_BINARY_DIR.  Hence message_types.meta files are copied here to allow configure.cmake to work.
file(GLOB_RECURSE MetaFiles "${CMAKE_CURRENT_SOURCE_DIR}/cmake/*.message_types.meta")
foreach(MetaFile ${MetaFiles})
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" "" RelativeMetaFile "${MetaFile}")
  set(IntermediateOutputFile "${IntermediateDir}/${RelativeMetaFile}")
  get_filename_component(FilePath "${IntermediateOutputFile}" PATH)
  list(APPEND IntermediateOutputFiles "${IntermediateOutputFile}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${FilePath}")
  add_custom_command(OUTPUT "${IntermediateOutputFile}"
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different "${MetaFile}" "${IntermediateOutputFile}"
                     DEPENDS "${MetaFile}"
                     COMMENT "Copying ${RelativeMetaFile} to ${IntermediateDir}/")
endforeach()

# Grab the list of files currently in the binary dir and remove any which are no longer required
file(GLOB_RECURSE ExtraMetaFiles "${IntermediateDir}/*.message_types.meta")
list(REMOVE_ITEM ExtraMetaFiles ${IntermediateOutputFiles})
foreach(ExtraMetaFile ${ExtraMetaFiles})
  execute_process(COMMAND ${CMAKE_COMMAND} -E remove "${ExtraMetaFile}")
  message(STATUS "Removed \"${ExtraMetaFile}\"")
endforeach()

set(CMAKE_DISABLE_SOURCE_CHANGES OFF)
add_custom_command(OUTPUT ${OutputFile}
                   COMMAND ${CMAKE_COMMAND} -DOutputFile="${OutputFile}"
                                            -DInputFile="${InputFile}"
                                            -P ${PROJECT_SOURCE_DIR}/cmake/configure.cmake
                   DEPENDS ${IntermediateOutputFiles}
                           ${InputFile}
                           ${PROJECT_SOURCE_DIR}/cmake/configure.cmake
                   COMMENT "Configuring message_types.h")
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set_source_files_properties(${OutputFile} PROPERTIES GENERATED TRUE)
source_group("Nfs API Files" FILES ${OutputFile})
source_group("Nfs CMake Files" FILES ${CMAKE_CURRENT_LIST_FILE}
                                     ${InputFile}
                                     ${PROJECT_SOURCE_DIR}/cmake/configure.cmake
                                     ${MetaFiles})

glob_dir(Nfs ${PROJECT_SOURCE_DIR}/src/maidsafe/nfs Nfs)
glob_dir(NfsClient ${PROJECT_SOURCE_DIR}/src/maidsafe/nfs/client "Nfs Client")
glob_dir(NfsVault ${PROJECT_SOURCE_DIR}/src/maidsafe/nfs/vault "Nfs Vault")
glob_dir(NfsTests ${PROJECT_SOURCE_DIR}/src/maidsafe/nfs/tests Tests)


#==================================================================================================#
# Define MaidSafe libraries and executables                                                        #
#==================================================================================================#
ms_add_static_library(nfs_core ${NfsAllFiles} ${OutputFile} ${InputFile}
                      ${PROJECT_SOURCE_DIR}/cmake/configure.cmake
                      ${MetaFiles})
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
