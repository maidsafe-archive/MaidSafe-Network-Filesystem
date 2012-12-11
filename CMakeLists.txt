#==============================================================================#
#                                                                              #
#  Copyright (c) 2012 MaidSafe.net limited                                     #
#                                                                              #
#  The following source code is property of MaidSafe.net limited and is not    #
#  meant for external use.  The use of this code is governed by the license    #
#  file licence.txt found in the root directory of this project and also on    #
#  www.maidsafe.net.                                                           #
#                                                                              #
#  You are not free to copy, amend or otherwise use this source code without   #
#  the explicit written permission of the board of directors of MaidSafe.net.  #
#                                                                              #
#==============================================================================#


set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
project(nfs)

include(../../cmake_modules/maidsafe_standard_setup.cmake)

include_directories(${common_SOURCE_DIR}/include)
include_directories(${private_SOURCE_DIR}/include)
include_directories(${routing_SOURCE_DIR}/include)


###################################################################################################
# Set up all files as GLOBs                                                                       #
###################################################################################################
file(GLOB SOURCE_FILES_NFS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/nfs/*.cc")
file(GLOB HEADER_FILES_NFS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/nfs/*.h")
file(GLOB API_FILES_NFS_DIR "${PROJECT_SOURCE_DIR}/include/maidsafe/nfs/*.h")
set(ALL_FILES_nfs_DIR ${SOURCE_FILES_nfs_DIR} ${HEADER_FILES_nfs_DIR} ${API_FILES_nfs_DIR})
source_group("nfs Source Files" FILES ${SOURCE_FILES_nfs_DIR})
source_group("nfs Header Files" FILES ${HEADER_FILES_nfs_DIR})
source_group("nfs API Files" FILES ${API_FILES_nfs_DIR})

file(GLOB SOURCE_FILES_NFS_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/nfs/tests/*.cc")
file(GLOB HEADER_FILES_NFS_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/nfs/tests/*.h")
set(ALL_FILES_NFS_TESTS_DIR ${SOURCE_FILES_nfs_TESTS_DIR} ${HEADER_FILES_nfs_TESTS_DIR})
source_group("Tests Source Files" FILES ${SOURCE_FILES_nfs_TESTS_DIR})
source_group("Tests Header Files" FILES ${HEADER_FILES_nfs_TESTS_DIR})

set(STYLE_nfs ${ALL_FILES_nfs_DIR} ${ALL_FILES_nfs_TESTS_DIR})

###################################################################################################
# Define MaidSafe libraries and executables                                                       #
###################################################################################################
ms_add_static_library(nfs ${ALL_FILES_nfs_DIR})
target_link_libraries(maidsafe_nfs maidsafe_common maidsafe_utilities)

if(MAIDSAFE_TESTING)
  ms_add_executable(TESTnfs "Tests/nfs" ${ALL_FILES_nfs_TESTS_DIR})
  target_link_libraries(TESTnfs maidsafe_common maidsafe_private maidsafe_routing)
endif()

rename_outdated_built_exes()


###################################################################################################
# Set compiler and linker flags                                                                   #
###################################################################################################
include(maidsafe_standard_flags)


###################################################################################################
# Tests                                                                                           #
###################################################################################################
if(MAIDSAFE_TESTING)
  add_style_test(${STYLE_nfs})
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
