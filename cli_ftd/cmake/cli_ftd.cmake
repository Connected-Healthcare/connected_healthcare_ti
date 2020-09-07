message("Inside Project folder")

set(USER_PROJECT_NAME "${PROJECT_NAME}.elf")

# Get sources
file(GLOB platform_sources CONFIGURE_DEPENDS platform/*.c)
file(GLOB platform_nv_sources CONFIGURE_DEPENDS platform/nv/*.c)
file(GLOB platform_crypto_sources CONFIGURE_DEPENDS platform/crypto/*.c)

# SYSCFG
set(SYSCFG_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/syscfg/ti_devices_config.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/syscfg/ti_drivers_config.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/syscfg/ti_radio_config.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/syscfg/tiop_config.c"
)

add_custom_command(OUTPUT ${SYSCFG_SOURCES} 
    COMMAND "$ENV{CCS_INSTALL_PATH}/ccs/utils/sysconfig_1.4.0/sysconfig_cli.bat" -s "$ENV{SIMPLELINK_13x2_26x2_SDK}/.metadata/product.json" -o "${CMAKE_CURRENT_SOURCE_DIR}/syscfg" --compiler gcc "${CMAKE_CURRENT_SOURCE_DIR}/cli_ftd.syscfg"
    COMMENT "Building SYSCFG_SOURCES"
    VERBATIM USES_TERMINAL
)

add_custom_target(SYSCFG_TARGET DEPENDS ${SYSCFG_SOURCES})

# XDCTOOLS
set(XS_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/configPkg/compiler.opt"
    "${CMAKE_CURRENT_SOURCE_DIR}/configPkg/linker.cmd"
)

string(FIND "${CMAKE_C_COMPILER}" "/bin" CMAKE_COMPILER_PATH_INDEX)
string(SUBSTRING "${CMAKE_C_COMPILER}" 0 ${CMAKE_COMPILER_PATH_INDEX} CMAKE_COMPILER_PATH)
message("CMAKE_COMPILER_PATH: ${CMAKE_COMPILER_PATH}")

add_custom_command(OUTPUT ${XS_SOURCES}
    COMMAND "D:/Software/ti/xdctools_3_61_02_27_core/xs" --xdcpath "$ENV{SIMPLELINK_13x2_26x2_SDK}/source;$ENV{SIMPLELINK_13x2_26x2_SDK}/kernel/tirtos/packages;" xdc.tools.configuro -o "${CMAKE_CURRENT_SOURCE_DIR}/configPkg" -t gnu.targets.arm.M4F -p ti.platforms.simplelink:CC1352R1F3 -r release -c "${CMAKE_COMPILER_PATH}" --compileOptions "-mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-exceptions -DHAVE_CONFIG_H -DSIMPLELINK_OPENTHREAD_SDK_BUILD=1 -DSIMPLELINK_OPENTHREAD_CONFIG_CC1352=1 -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH -DNVOCMP_POSIX_MUTEX -DNVOCMP_NVPAGES=2 -DNDEBUG -DBOARD_DISPLAY_USE_UART=1 -DMBEDTLS_CONFIG_FILE='\"mbedtls-config-cc1352-gcc.h\"' -DOPENTHREAD_CONFIG_FILE='\"openthread-config-cc1352-gcc-ftd.h\"' -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='\"openthread-core-cc1352-config-ide.h\"' -DTIOP_ENABLE_UART=1 -DDeviceFamily_CC13X2 -I\"${CMAKE_CURRENT_SOURCE_DIR}\" -I\"${CMAKE_CURRENT_SOURCE_DIR}/platform/crypto\" -I\"$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libmbedcrypto/config\" -I\"$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/thread/rtos/${TARGET_BOARD}/thread/libopenthread_ftd/config\" -I\"$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/examples/platforms\" -I\"$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/include\" -I\"$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src/core\" -I\"$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/third_party/mbedtls/repo/include\" -I\"$ENV{SIMPLELINK_13x2_26x2_SDK}/source/third_party/openthread/src/cli\" -I\"$ENV{SIMPLELINK_13x2_26x2_SDK}/source\" -I\"$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/devices/cc13x2_cc26x2\" -I\"$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/posix/gcc\" -I\"$ENV{SIMPLELINK_13x2_26x2_SDK}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include/newlib-nano\" -I\"$ENV{SIMPLELINK_13x2_26x2_SDK}/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include\" -I\"${CMAKE_COMPILER_PATH}/arm-none-eabi/include\" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -fno-common  -std=c99" "${CMAKE_CURRENT_SOURCE_DIR}/release.cfg"
    COMMENT "Building XS_SOURCES"
    VERBATIM USES_TERMINAL
)

add_custom_target(XS_TARGET DEPENDS ${XS_SOURCES})

add_executable(${USER_PROJECT_NAME} 
  CC1352P_2_LAUNCHXL_fxns.c
  cli.c
  main.c
  otstack.c
  ${SYSCFG_SOURCES}
  ${platform_sources}
  ${platform_nv_sources}
  ${platform_crypto_sources}
  otsupport/otrtosapi.c
  missing/cxxhelpers.c
)

add_dependencies(${USER_PROJECT_NAME} SYSCFG_TARGET XS_TARGET)

set(COMMON_C_FLAGS
    -Os
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mthumb
    -march=armv7e-m     
    -mfloat-abi=hard

    -fno-exceptions
    -ffunction-sections 
    -fdata-sections 

    -g 
    -gdwarf-3 
    -gstrict-dwarf 
    -Wall 
    -fno-common 
)

target_compile_options(${USER_PROJECT_NAME} PRIVATE 
    ${COMMON_C_FLAGS}
    -std=c99
    @${CMAKE_CURRENT_SOURCE_DIR}/configPkg/compiler.opt
)

target_link_options(${USER_PROJECT_NAME} PRIVATE
  ${COMMON_C_FLAGS}

  -nostartfiles 
  -static 
  -Wl,--gc-sections
  -Wl,--defsym,NVOCMP_NVPAGES=2 

  --specs=nano.specs
  -lgcc
  -lm
  -lc
  -lnosys
  -Wl,-Map,${CMAKE_CURRENT_BINARY_DIR}/cli_ftd_CC1352P_2_LAUNCHXL_tirtos_gcc.map
  -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/tirtos/gcc/CC1352P_2_LAUNCHXL_TIRTOS.lds
  -Wl,-T${CMAKE_CURRENT_SOURCE_DIR}/configPkg/linker.cmd
)

target_compile_definitions(${USER_PROJECT_NAME} PRIVATE
    HAVE_CONFIG_H 
    SIMPLELINK_OPENTHREAD_SDK_BUILD=1 
    SIMPLELINK_OPENTHREAD_CONFIG_CC1352=1 
    Board_EXCLUDE_NVS_EXTERNAL_FLASH 
    NVOCMP_POSIX_MUTEX 
    NVOCMP_NVPAGES=2 
    NDEBUG 
    BOARD_DISPLAY_USE_UART=1 
    MBEDTLS_CONFIG_FILE="mbedtls-config-cc1352-gcc.h" 
    OPENTHREAD_CONFIG_FILE="openthread-config-cc1352-gcc-ftd.h" 
    OPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-cc1352-config-ide.h" 
    TIOP_ENABLE_UART=1 
    DeviceFamily_CC13X2
)

target_include_directories(${USER_PROJECT_NAME} PRIVATE 
    .
    syscfg
    platform
    platform/nv
    platform/crypto
    otsupport

    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/devices/cc13x2_cc26x2"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source/ti/posix/gcc"
)

target_link_directories(${USER_PROJECT_NAME} PRIVATE
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/source"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/kernel/tirtos/packages"
    "$ENV{SIMPLELINK_13x2_26x2_SDK}/kernel/tirtos/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/hard"
)

target_link_libraries(${USER_PROJECT_NAME} PRIVATE 
    # IMP, DO NOT CHANGE THE "ORDER" OF THESE LIBRARIES
    openthread_cli_ftd
    openthread_ftd
    openthread_platform_utils_ftd
    mbedcrypto

    -l:ti/display/lib/display.am4fg
    -l:ti/grlib/lib/gcc/m4f/grlib.a
    -l:third_party/spiffs/lib/gcc/m4f/spiffs_cc26xx.a
    -l:ti/drivers/rf/lib/rf_multiMode_cc13x2.am4fg
    -l:ti/drivers/lib/drivers_cc13x2.am4fg
    -l:ti/dpl/lib/dpl_cc13x2.am4fg
    -l:ti/devices/cc13x2_cc26x2/driverlib/bin/gcc/driverlib.lib    
)

# Get the size
add_custom_target(ELF_SIZE ALL
    COMMAND "arm-none-eabi-size" --format=berkeley ${CMAKE_CURRENT_BINARY_DIR}/${USER_PROJECT_NAME}
    DEPENDS ${USER_PROJECT_NAME}
)

# TODO, Add xds110 debug port detector

# TODO, Add flasher

