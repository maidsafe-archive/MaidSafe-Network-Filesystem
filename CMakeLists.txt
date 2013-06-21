#==================================================================================================#
#                                                                                                  #
#  Copyright (c) 2012 MaidSafe.net limited                                                         #
#                                                                                                  #
#  The following source code is property of MaidSafe.net limited and is not meant for external     #
#  use.  The use of this code is governed by the license file licence.txt found in the root        #
#  directory of this project and also on www.maidsafe.net.                                         #
#                                                                                                  #
#  You are not free to copy, amend or otherwise use this source code without the explicit written  #
#  permission of the board of directors of MaidSafe.net.                                           #
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
glob_dir(Nfs ${PROJECT_SOURCE_DIR}/src/maidsafe/nfs Nfs)
glob_dir(NfsTests ${PROJECT_SOURCE_DIR}/src/maidsafe/nfs/tests Tests)


#==================================================================================================#
# Define MaidSafe libraries and executables                                                        #
#==================================================================================================#
ms_add_static_library(nfs ${NfsAllFiles})
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
