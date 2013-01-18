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

set(PROTO_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
file(GLOB PROTO_FILES RELATIVE ${PROTO_SOURCE_DIR} "${PROTO_SOURCE_DIR}/maidsafe/nfs/*.proto")
include(../../cmake_modules/maidsafe_standard_setup.cmake)

include_directories(${common_SOURCE_DIR}/include)
include_directories(${passport_SOURCE_DIR}/include)
include_directories(${private_SOURCE_DIR}/include)
include_directories(${routing_SOURCE_DIR}/include)
include_directories(${rudp_SOURCE_DIR}/include)


###################################################################################################
# Set up all files as GLOBs                                                                       #
###################################################################################################
file(GLOB NfsApi ${PROJECT_SOURCE_DIR}/include/maidsafe/nfs/*.h)

set(NfsSourcesRootDir ${PROJECT_SOURCE_DIR}/src/maidsafe/nfs)
file(GLOB NfsSources ${NfsSourcesRootDir}/*.cc)
file(GLOB NfsHeaders ${NfsSourcesRootDir}/*.h)

file(GLOB NfsProtos ${NfsSourcesRootDir}/*.proto)
file(GLOB NfsProtoSources ${NfsSourcesRootDir}/*.pb.cc)
file(GLOB NfsProtoHeaders ${NfsSourcesRootDir}/*pb.h)
list(REMOVE_ITEM NfsSources ${NfsProtoSources})
list(REMOVE_ITEM NfsHeaders ${NfsProtoHeaders})

source_group("NFS API Files" FILES ${NfsApi})
source_group("NFS Source Files" FILES ${NfsSources})
source_group("NFS Header Files" FILES ${NfsHeaders})
source_group("NFS Proto Files" FILES ${NfsProtos} ${NfsProtoSources} ${NfsProtoHeaders})

set(NfsAllFiles ${NfsApi} ${NfsSources} ${NfsHeaders} ${NfsProtos} ${NfsProtoSources} ${NfsProtoHeaders})

####    TESTS    ####
file(GLOB NfsTestsSources ${NfsSourcesRootDir}/tests/*.cc)
file(GLOB NfsTestsHeaders ${NfsSourcesRootDir}/tests/*.h)
set(NfsTestsAllFiles ${NfsTestsSources} ${NfsTestsHeaders})
source_group("Tests Source Files" FILES ${NfsTestsSources})
source_group("Tests Header Files" FILES ${NfsTestsHeaders})

set(StyleAllFiles ${NfsAllFiles} ${NfsTestsAllFiles})
list(REMOVE_ITEM StyleAllFiles ${NfsProtos} ${NfsProtoSources} ${NfsProtoHeaders})


###################################################################################################
# Define MaidSafe libraries and executables                                                       #
###################################################################################################
ms_add_static_library(nfs ${NfsAllFiles})
target_link_libraries(maidsafe_nfs maidsafe_routing)

if(MAIDSAFE_TESTING)
  ms_add_executable(TESTnfs "Tests/NFS" ${NfsTestsAllFiles})
  target_link_libraries(TESTnfs maidsafe_nfs maidsafe_private)
endif()

rename_outdated_built_exes()


###################################################################################################
# Set compiler and linker flags                                                                   #
###################################################################################################
include(maidsafe_standard_flags)

ms_disable_warnings(${NfsProtoSources})

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-effc++")
endif()


###################################################################################################
# Tests                                                                                           #
###################################################################################################
if(MAIDSAFE_TESTING)
  add_style_test(${StyleAllFiles})
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
