cmake_minimum_required(VERSION 3.17)

# User Defines

# ! IMPORTANT, Toggle only between these two options
# set(TARGET_BOARD "CC1352P_2_LAUNCHXL")
set(TARGET_BOARD "CC1352R1_LAUNCHXL")

set(UART_BAUD_RATE 9600) # 115200 (console) or 9600 (bluetooth)
set(BLUETOOTH 1) # 1 or 0

set(PROJECT_NAME "${TARGET_BOARD}_cli_ftd")
option(TESTING OFF)

# Constants
set(SYSCFG_FILE "${TARGET_BOARD}/cli_ftd.syscfg")

include(cmake/env_check.cmake)
include(cmake/sysconfig_gui.cmake)
include(cmake/test_target.cmake)

if (TESTING)

message("Setting Unit-Testing Project")

project(${PROJECT_NAME})

include(CTest)
enable_testing()

# Libraries
include(cmake/unit_testing.cmake)

# Add more Tests here
add_subdirectory(third_party/line_buffer/test)

else()

message("Setting ARM Project")

set(CMAKE_TOOLCHAIN_FILE "cmake/toolchain.cmake")

project(${PROJECT_NAME})

# Libraries
include(cmake/libmbedcrypto.cmake)
include(cmake/libopenthread_platform_utils_ftd.cmake)
include(cmake/libopenthread_cli_ftd.cmake)
include(cmake/libopenthread_ftd.cmake)

# Common sources
set(USER_CLI_FTD_SOURCES 
    "user/user.c"
    "user/tinyprintf/tinyprintf_override.c"
    "user/utility/float.c"
    "user/utility/type_convert.c"
    "user/utility/type_print.c"
)

# CC1352P_2 specific code
if (${TARGET_BOARD} STREQUAL "CC1352P_2_LAUNCHXL")
set(USER_CLI_FTD_SOURCES "${USER_CLI_FTD_SOURCES}"
)
endif()

# CC1352R1 specific code
if (${TARGET_BOARD} STREQUAL "CC1352R1_LAUNCHXL")
set(USER_CLI_FTD_SOURCES "${USER_CLI_FTD_SOURCES}"
    "user/udp_communication/udp_comm.c"
    "user/heartbeat/heartbeat_task.c"
    "user/heartbeat/heartbeat.c"

    "user/sensor/aggregate.c"
    "user/sensor/stm32/stm32_i2c.c"
    "user/sensor/stm32/stm32_i2c_task.c"
    "user/peer_to_peer_alert/peer_to_peer_alert.c"
)
endif()

message("Data: ${USER_CLI_FTD_SOURCES}")

include(cmake/cli_ftd.cmake)

endif()
