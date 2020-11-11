if(NOT DEFINED ENV{SYSCONFIG_GUI_PATH})
message("Set the SYSCONFIG_GUI_PATH environment variable to use target 'SYSCONFIG_GUI'")
else()
message("SYSCONFIG_GUI_PATH: $ENV{SYSCONFIG_GUI_PATH}")

add_custom_target(SYSCONFIG_GUI
    COMMAND $ENV{SYSCONFIG_GUI_PATH}/nw/nw.exe $ENV{SYSCONFIG_GUI_PATH} -s "$ENV{SIMPLELINK_13x2_26x2_SDK}/.metadata/product.json" ${CMAKE_CURRENT_SOURCE_DIR}/cli_ftd.syscfg
    VERBATIM USES_TERMINAL
)

endif()