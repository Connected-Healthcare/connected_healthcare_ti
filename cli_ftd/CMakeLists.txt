cmake_minimum_required(VERSION 3.17)

set(CMAKE_TOOLCHAIN_FILE "cmake/toolchain.cmake") 
set(TARGET_BOARD "CC1352P_2_LAUNCHXL")
set(PROJECT_NAME "cli_ftd")

# Important Variable definitions
if(NOT DEFINED ENV{SIMPLELINK_13x2_26x2_SDK})
  message(FATAL_ERROR "Set the SIMPLELINK_CC13x2_26x2_SDK environment variable")
else()
  message("SIMPLELINK_13x2_26x2_SDK: $ENV{SIMPLELINK_13x2_26x2_SDK}")
endif()

if(NOT DEFINED ENV{CCS_INSTALL_PATH})
  message(FATAL_ERROR "Set the CCS_INSTALL_PATH environment variable")
else()
  message("CCS_INSTALL_PATH: $ENV{CCS_INSTALL_PATH}")
endif()

if (NOT DEFINED ENV{XDCTOOLS_PATH})
  message(FATAL_ERROR "Download the JRE version of XDCTOOLS from TI and add XDCTOOLS_PATH environment variable")
else()
  message("XDCTOOLS_PATH: $ENV{XDCTOOLS_PATH}")
endif()

project(${PROJECT_NAME})

# Libraries
include(cmake/libmbedcrypto.cmake)
include(cmake/libopenthread_platform_utils_ftd.cmake)
include(cmake/libopenthread_cli_ftd.cmake)
include(cmake/libopenthread_ftd.cmake)

# Executable
include(cmake/cli_ftd.cmake)
