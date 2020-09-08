message("Running openthread_cli_ftd.cmake")

function(get_globbed_cpp_files message path_var out_var)
file(GLOB globbed_sources CONFIGURE_DEPENDS ${path_var}/*.cpp)
list(LENGTH globbed_sources sources_length)
message("${message} _LENGTH: ${sources_length}")

set(${out_var} ${globbed_sources} PARENT_SCOPE)
endfunction()


set(openthread_core_path "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src/core")

get_globbed_cpp_files("CORE API" "${openthread_core_path}/api" api_sources)
get_globbed_cpp_files("BACKBONE_ROUTER" "${openthread_core_path}/backbone_router" backbone_router_sources)
get_globbed_cpp_files("COAP" "${openthread_core_path}/coap" coap_sources)
get_globbed_cpp_files("COMMON" "${openthread_core_path}/common" common_sources)
get_globbed_cpp_files("CRYPTO" "${openthread_core_path}/crypto" crypto_sources)
get_globbed_cpp_files("DIAGS" "${openthread_core_path}/diags" diags_sources)
get_globbed_cpp_files("MAC" "${openthread_core_path}/mac" mac_sources)
get_globbed_cpp_files("MESHCOP" "${openthread_core_path}/meshcop" meshcop_sources)
get_globbed_cpp_files("NET" "${openthread_core_path}/net" net_sources)
get_globbed_cpp_files("RADIO" "${openthread_core_path}/radio" radio_sources)
get_globbed_cpp_files("THREAD" "${openthread_core_path}/thread" thread_sources)
get_globbed_cpp_files("UTILS" "${openthread_core_path}/utils" utils_sources)

set(openthread_lib_path "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src/lib")

# Remove these files
list(REMOVE_ITEM common_sources ${openthread_core_path}/common/extension_example.cpp)

add_library(openthread_ftd STATIC 
        ${api_sources}
        ${backbone_router_sources}
        ${coap_sources}
        ${common_sources}
        ${crypto_sources}
        ${diags_sources}
        ${mac_sources}
        ${meshcop_sources}
        ${net_sources}
        ${radio_sources}
        ${thread_sources}
        ${utils_sources}
        "${openthread_lib_path}/platform/exit_code.c"
)

target_compile_options(openthread_ftd PRIVATE 
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

target_compile_definitions(openthread_ftd PRIVATE      
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

target_include_directories(openthread_ftd PUBLIC 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/examples/platforms" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls/repo/include" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/include" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src/core" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include/newlib-nano" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include"

    # Configs
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libmbedcrypto/config"
    "${openthread_core_path}/config"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libopenthread_ftd/config"
)
