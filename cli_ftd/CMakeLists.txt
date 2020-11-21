cmake_minimum_required(VERSION 3.17)

# User Defines

# ! IMPORTANT, Toggle only between these two options
# set(TARGET_BOARD "CC1352P_2_LAUNCHXL")
set(TARGET_BOARD "CC1352R1_LAUNCHXL")

set(PROJECT_NAME "${TARGET_BOARD}_cli_ftd")
set(TESTING OFF)

include(cmake/env_check.cmake)
include(cmake/sysconfig_gui.cmake)

if (TESTING)

message("Setting Unit-Testing Project")

project(${PROJECT_NAME})

include(CTest)
enable_testing()

# Libraries
include(cmake/unit_testing.cmake)

# Add more Tests here

else()

message("Setting ARM Project")

set(CMAKE_TOOLCHAIN_FILE "cmake/toolchain.cmake")

project(${PROJECT_NAME})

# Libraries
include(cmake/libmbedcrypto.cmake)
include(cmake/libopenthread_platform_utils_ftd.cmake)
include(cmake/libopenthread_cli_ftd.cmake)
include(cmake/libopenthread_ftd.cmake)

set(USER_CLI_FTD_SOURCES 
    "user/user.c" 
    "user/tinyprintf/tinyprintf_override.c"
)
include(cmake/cli_ftd.cmake)

endif()
