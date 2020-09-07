# For syscfg data

"D:/Software/ti/ccs1000/ccs/utils/sysconfig_1.4.0/sysconfig_cli.bat" -s "D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/.metadata/product.json" -o "syscfg" --compiler gcc "cli_ftd.syscfg"
Running script...
Validating...
info: /ti/thread/thread network.panID: PAN ID 0xFFFF is reserved as the broadcast PAN ID. This is invalid for network formation, but is used for joiner devices when discovering network parameters via commissioning.
Generating Code...
    '/ti/drivers/RF' -> '/ti/devices/driverlib'
    '/ti/drivers/RF' -> '/ti/drivers'
    '/ti/drivers' -> '/ti/devices/driverlib'
    '/ti/display' -> '/ti/drivers'
Writing D:\Code\TI\TI_Dump\cli_ftd\syscfg\tiop_config.h...
Writing D:\Code\TI\TI_Dump\cli_ftd\syscfg\tiop_config.c...
Writing D:\Code\TI\TI_Dump\cli_ftd\syscfg\ti_devices_config.c...
Writing D:\Code\TI\TI_Dump\cli_ftd\syscfg\ti_radio_config.c...
Writing D:\Code\TI\TI_Dump\cli_ftd\syscfg\ti_radio_config.h...
Writing D:\Code\TI\TI_Dump\cli_ftd\syscfg\ti_drivers_config.c...
Writing D:\Code\TI\TI_Dump\cli_ftd\syscfg\ti_drivers_config.h...
Writing D:\Code\TI\TI_Dump\cli_ftd\syscfg\ti_utils_build_linker.cmd.exp...
Writing D:\Code\TI\TI_Dump\cli_ftd\syscfg\syscfg_c.rov.xs...


# Using XS

"D:/Software/ti/xdctools_3_61_02_27_core/xs" --xdcpath="D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source;D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/kernel/tirtos/packages;" xdc.tools.configuro -o configPkg -t gnu.targets.arm.M4F -p ti.platforms.simplelink:CC1352R1F3 -r release -c "D:\Software\GNU Arm Embedded Toolchain\9 2020-q2-update" --compileOptions "-mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-exceptions -DHAVE_CONFIG_H -DSIMPLELINK_OPENTHREAD_SDK_BUILD=1 -DSIMPLELINK_OPENTHREAD_CONFIG_CC1352=1 -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH -DNVOCMP_POSIX_MUTEX -DNVOCMP_NVPAGES=2 -DNDEBUG -DBOARD_DISPLAY_USE_UART=1 -DMBEDTLS_CONFIG_FILE='\"mbedtls-config-cc1352-gcc.h\"' -DOPENTHREAD_CONFIG_FILE='\"openthread-config-cc1352-gcc-ftd.h\"' -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='\"openthread-core-cc1352-config-ide.h\"' -DTIOP_ENABLE_UART=1 -DDeviceFamily_CC13X2 -I\"D:/Workspace/TI/cli_ftd_CC1352P_2_LAUNCHXL_tirtos_gcc\" -I\"D:/Workspace/TI/cli_ftd_CC1352P_2_LAUNCHXL_tirtos_gcc/Debug\" -I\"D:/Workspace/TI/libmbedcrypto_CC1352P_2_LAUNCHXL_tirtos_gcc/config\" -I\"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/third_party/openthread/examples/platforms\" -I\"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/third_party/openthread/include\" -I\"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/third_party/openthread/src/core\" -I\"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/third_party/openthread/third_party/mbedtls/repo/include\" -I\"D:/Workspace/TI/cli_ftd_CC1352P_2_LAUNCHXL_tirtos_gcc/platform/crypto\" -I\"D:/Workspace/TI/libopenthread_ftd_CC1352P_2_LAUNCHXL_tirtos_gcc/config\" -I\"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/third_party/openthread/src/cli\" -I\"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source\" -I\"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/ti/devices/cc13x2_cc26x2\" -I\"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/ti/posix/gcc\" -I\"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include/newlib-nano\" -I\"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include\" -I\"D:/Software/ti/ccs1000/gcc_arm_none_eabi_9_2_1/arm-none-eabi/include\" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -fno-common  -std=c99 " "release.cfg"

# Include Paths


"D:/Workspace/TI/cli_ftd_CC1352P_2_LAUNCHXL_tirtos_gcc" 
"D:/Workspace/TI/cli_ftd_CC1352P_2_LAUNCHXL_tirtos_gcc/Debug" 

"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/third_party/openthread/examples/platforms" 
"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/third_party/openthread/include" 
"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/third_party/openthread/src/core" 
"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/third_party/openthread/third_party/mbedtls/repo/include" 
"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/third_party/openthread/src/cli" 
"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source" 
"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/ti/devices/cc13x2_cc26x2" 
"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source/ti/posix/gcc" 
"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include/newlib-nano" 
"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include" 


"D:/Workspace/TI/cli_ftd_CC1352P_2_LAUNCHXL_tirtos_gcc/platform/crypto" 

"D:/Workspace/TI/libopenthread_ftd_CC1352P_2_LAUNCHXL_tirtos_gcc/config" 
"D:/Workspace/TI/libmbedcrypto_CC1352P_2_LAUNCHXL_tirtos_gcc/config" 

# Library Paths


"ti/display/lib/display.am4fg" 
"ti/grlib/lib/gcc/m4f/grlib.a" 

<!-- source -->
"third_party/spiffs/lib/gcc/m4f/spiffs_cc26xx.a" 
"ti/drivers/rf/lib/rf_multiMode_cc13x2.am4fg" 
"ti/drivers/lib/drivers_cc13x2.am4fg" 

<!-- tirtos/packages -->
"ti/dpl/lib/dpl_cc13x2.am4fg" 

"ti/devices/cc13x2_cc26x2/driverlib/bin/gcc/driverlib.lib"


"D:/Workspace/TI/libopenthread_cli_ftd_CC1352P_2_LAUNCHXL_tirtos_gcc/OptimizeSize" 
"D:/Workspace/TI/libopenthread_ftd_CC1352P_2_LAUNCHXL_tirtos_gcc/OptimizeSize" 
"D:/Workspace/TI/libopenthread_platform_utils_ftd_CC1352P_2_LAUNCHXL_tirtos_gcc/OptimizeSize" 
"D:/Workspace/TI/libmbedcrypto_CC1352P_2_LAUNCHXL_tirtos_gcc/OptimizeSize" 
"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/source" 
"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/kernel/tirtos/packages" 
"D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/hard"
