cmake_minimum_required(VERSION 3.17)

set(CMAKE_TOOLCHAIN_FILE "cmake/toolchain.cmake") 
set(TARGET_BOARD "CC1352P_2_LAUNCHXL")
set(PROJECT_NAME "cli_ftd")

# Important Variable definitions
if(NOT DEFINED ENV{SIMPLELINK_13x2_26x2_SDK})
  message(FATAL_ERROR "Set the SIMPLELINK_CC13x2_26x2_SDK PATH variable")
else()
  message("SIMPLELINK_13x2_26x2_SDK: $ENV{SIMPLELINK_13x2_26x2_SDK}")
endif()


project(${PROJECT_NAME})

include(cmake/libmbedcrypto.cmake)
include(cmake/libopenthread_platform_utils_ftd.cmake)
include(cmake/libopenthread_cli_ftd.cmake)
include(cmake/libopenthread_ftd.cmake)
