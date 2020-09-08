message("Running openthread_cli_ftd.cmake")

set(openthread_cli_ftd_source "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src/cli")
add_library(openthread_cli_ftd STATIC 
    "${openthread_cli_ftd_source}/cli.cpp"
    "${openthread_cli_ftd_source}/cli_coap.cpp"
    "${openthread_cli_ftd_source}/cli_coap_secure.cpp"
    "${openthread_cli_ftd_source}/cli_commissioner.cpp"
    "${openthread_cli_ftd_source}/cli_console.cpp"
    "${openthread_cli_ftd_source}/cli_dataset.cpp"
    "${openthread_cli_ftd_source}/cli_joiner.cpp"
    "${openthread_cli_ftd_source}/cli_server.cpp"
    "${openthread_cli_ftd_source}/cli_uart.cpp"
    "${openthread_cli_ftd_source}/cli_udp.cpp"
)

target_compile_options(openthread_cli_ftd PRIVATE 
    -mcpu=cortex-m4 
    -mthumb 
    -mfloat-abi=hard 
    -mfpu=fpv4-sp-d16 
    -fno-exceptions 
    -Os 
    -ffunction-sections 
    -fdata-sections 
    -g 
    -gstrict-dwarf 
    -Wall 
    -Wextra 
    -Wshadow 
    -MMD 
    -MP 
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-c++14-compat>
)

target_compile_definitions(openthread_cli_ftd PRIVATE      
    SIMPLELINK_OPENTHREAD_SDK_BUILD=1 
    SIMPLELINK_OPENTHREAD_CONFIG_CC1352=1
    MBEDTLS_CONFIG_FILE="mbedtls-config-cc1352-gcc.h"
    HAVE_CONFIG_H
    _BSD_SOURCE=1
    _DEFAULT_SOURCE=1
    OPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-cc1352-config-ide.h"
    OPENTHREAD_FTD=1 
    OPENTHREAD_CONFIG_FILE="openthread-config-cc1352-gcc-ftd.h"
)

target_include_directories(openthread_cli_ftd PUBLIC 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/examples/platforms" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls/repo/include" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/include"      
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src/core" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include/newlib-nano" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include"

    # CONFIGS
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libmbedcrypto/config"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libopenthread_ftd/config" 
)
