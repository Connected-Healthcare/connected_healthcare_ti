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

if (NOT DEFINED ENV{SYSCONFIG_PATH})
  message(FATAL_ERROR "Set the SYSCONFIG_PATH environment variable. Check the {CCS_INSTALL_PATH}/ccs/utils/ folder for your sysconfig version")
else()
  message("SYSCONFIG_PATH: $ENV{SYSCONFIG_PATH}")
endif()
