- [Connected Healthcare](#connected-healthcare)
- [Pre Requisites](#pre-requisites)
  - [Tools](#tools)
  - [Texas Instruments Tools](#texas-instruments-tools)
  - [Windows Specific settings](#windows-specific-settings)
    - [Environment Variables](#environment-variables)
    - [Path Settings](#path-settings)
- [Generate and Compiling](#generate-and-compiling)
  - [Generate](#generate)
  - [Compile](#compile)
  - [Flash with Uniflash](#flash-with-uniflash)

# Connected Healthcare

Connected and Distributed Sensing system for Health Care Facilities

# Pre Requisites

## Tools

- ARM GCC toolchain (9.x STABLE VERSION)
  - Install it from [the ARM Page](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
- CMake
  - Install version 3.17 at minimum
  - `pip install cmake` (Python method)
- Ninja (Add to path)
  - Install via Chocolatey for windows
- VSCode
- VSCode Extensions
  - Better Comments (Aaron Bond)
  - C/C++ (Microsoft)
  - CMake (twxs)
  - CMake Tools (Microsoft)

## Texas Instruments Tools

- Install Code Composer Studio
- Install the `Simplelink_CC13x2_26x2_SDK` through Code Composer Studio

- Install [Uniflash latest version (6.x)](https://www.ti.com/tool/UNIFLASH)
- Install [XDCTools latest version **WITH JRE**](http://downloads.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/rtsc/3_55_02_22/index_FDS.html)

## Windows Specific settings

### Environment Variables

- Right click **My Computer** -> **Properties** -> **Advanced System Settings**
- **Advanced** -> **Environment Varibales**
- Under **User variables for `your_username`** -> **New**

- Add 3 environmental variables
- SIMPLELINK_13x2_26x2_SDK
  - ex: `D:/Software/ti/ccs1000/simplelink_cc13x2_26x2_sdk_4_20_01_04`
- CCS_INSTALL_PATH
  - `D:/Software/ti/ccs1000`
- XDCTOOLS_PATH **(with JRE)**
  - `D:/Software/ti/xdctools_3_61_02_27_core`

**IMPORTANT**: Make sure that the paths above have **Forward Slashes** and **Does Not** have **Trailing Slashes**

### Path Settings

- Check if `arm-none-eabi-gcc` is present on path
- Open Command Prompt
```
arm-none-eabi-gcc --version

# Should give output (or similar)
arm-none-eabi-gcc (GNU Arm Embedded Toolchain 9-2020-q2-update) 9.3.1 20200408 (release)
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```


- Check if `ninja` is present on path
- Open Command Prompt
```
ninja --version

# Should give output (or similar)
1.10.0 
```

# Generate and Compiling

## Generate

- Use the Ninja Generator it is overall faster than `make`
- You can also set Ninja to be the default generator within VSCode when using the VSCode CMake Generator
```
cmake -B build -G Ninja
```

## Compile

- Compiles the build folder
```
cmake --build build
```

- Clean compiles the project
```
cmake --build build --clean-first
```
- Run a specific target within the project
```
# See the available targets
cmake --build build --target help

# Run the ELF_SIZE target
cmake --build build --target ELF_SIZE

# Output (or similar)
text    data     bss     dec     hex filename
246713    1604   53988  302305   49ce1 D:/Repositories/connected_healthcare/cli_ftd/build/cli_ftd.elf
```
- See more build options (parallel builds to speed up compilation etc)
```
# See the compile options
cmake --build build --help

# Verbose output (note -- passes control to the `ninja` generator)
cmake --build build -- -v

# Parallel Builds
cmake --build build -j 12
```

## Flash with Uniflash

- Open your Uniflash GUI
- Connect to your board
- Under **Program** -> **Flash Images**
- Give it the path to the **.elf** binary generated
  - NOTE: The **.elf** binary is generated under the `-B` output folder (in this case the **build** folder as we specified above)
