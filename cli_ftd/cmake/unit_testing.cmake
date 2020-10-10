add_library(unity STATIC third_party/unity/unity.c)
target_include_directories(unity PUBLIC third_party/unity)

add_library(fff INTERFACE)
target_include_directories(fff INTERFACE third_party/fff)

set(openthread_platform_utils_ftd_folder "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/examples/platforms/utils")
set(openthread_core_path "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src/core")

add_library(test_includes INTERFACE)
target_include_directories(test_includes INTERFACE 
    .
    syscfg
    platform
    platform/nv
    platform/crypto
    otsupport

    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/devices/cc13x2_cc26x2"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/posix/gcc"

    # USER INCLUDES
    user

    # Library includes

    # MBEDCRYPTO
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/examples/platforms"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls/repo/include"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/include"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libmbedcrypto/config"

    # OPENTHREAD_CLI_FTD
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/examples/platforms" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls/repo/include" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/include"      
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src/core" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src" 

    # CONFIGS
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libmbedcrypto/config"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libopenthread_ftd/config"

    # OPENTHREAD_FTD
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/examples/platforms" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls/repo/include" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/include" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src/core" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src" 

    # Configs
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libmbedcrypto/config"
    "${openthread_core_path}/config"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libopenthread_ftd/config"

    # OPENTHREAD_PLATFORM_UTILS_FTD
    "${openthread_platform_utils_ftd_folder}"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/examples/platforms" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls/repo/include" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/include" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src/core" 
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src" 

    # CONFIGS
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libmbedcrypto/config"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libopenthread_ftd/config" 
)
