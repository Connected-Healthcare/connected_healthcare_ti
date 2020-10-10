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
- [Unit Testing and Mocking](#unit-testing-and-mocking)
  - [Setting up your Native Toolchain](#setting-up-your-native-toolchain)
    - [**RECOMMENDED** Chocolatey](#recommended-chocolatey)
    - [**Long Process** MSYS2](#long-process-msys2)
  - [Adding your Unit Test](#adding-your-unit-test)
    - [Root CMakeLists configuration](#root-cmakelists-configuration)
    - [Custom Test CMakeLists.txt](#custom-test-cmakeliststxt)
    - [Unit Test Configuration](#unit-test-configuration)
    - [FFF Configuration](#fff-configuration)
    - [Testing your Unit Tests](#testing-your-unit-tests)

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
- SYSCONFIG_PATH
  - ` D:/Software/ti/ccs1000/ccs/utils/sysconfig_1.4.0`
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
- Load the elf file first
  - Verify checks the binary file loaded on the chip

# Unit Testing and Mocking

**IMPORTANT** When shifting from TESTING=ON to TESTING=OFF, OR VICE VERSA, **always delete the build folder and reconfigure the project**

## Setting up your Native Toolchain

### **RECOMMENDED** Chocolatey

This is a hassle free process

**https://chocolatey.org/search?q=gcc**

- Install Chocolatey
- Install GCC MingW Compiler for Windows
- If you are on MAC or Linux the process might be different (or you might already have the compilers)

### **Long Process** MSYS2

> Not writing this, since I dont encourage it

- [Link here](https://www.msys2.org/)

## Adding your Unit Test

Steps to configure your Unit Testing

**I REPEAT, Always delete your BUILD folder when changing / toggling your TESTING flag**

### Root CMakeLists configuration

- `add_subdirectory(<path_to_your_custom_CMakeLists.txt>)`

### Custom Test CMakeLists.txt

- Example
```cmake

add_executable(test_exe test_file_one, test_file_two, ...)
target_link_libraries(test_exe PRIVATE unity fff test_includes)

add_test(NAME test_exe COMMAND test_exe)
```

### Unit Test Configuration

```cpp
#include "unity.h"
#include "fff.h"

// Required
void setUp() {}
void tearDown() {}

void test_number_one() {}

void test_number_two() {}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_number_one);
  RUN_TEST(test_number_two);

  return UNITY_END();
}
```

### FFF Configuration

- [Read this link perfectly](https://github.com/meekrosoft/fff)



```cpp
// ....
// ....

DEFINE_FFF_GLOBALS;

// Say we have 2 functions to mock
// void hello_world(const char *data);
// int hello_to_you(double number_of_people);

FAKE_VOID_FUNC(hello_world, const char *);
FAKE_VALUE_FUNC(int, hello_to_you, double);

// Required
void setUp() {
  RESET_FAKE(hello_world);
  RESET_FAKE(hello_to_you);

  FFF_RESET_HISTORY();
}
void tearDown() {}

void test_number_one() {
  call_your_main_function();

  TEST_ASSERT_EQUAL_XYZ(hello_world_fake.call_count, 3);
}

void test_number_two() {
  call_your_main_function();

  TEST_ASSERT_EQUAL_XYZ(hello_to_you.call_count, 1);
}

// ....
// ....
```

### Testing your Unit Tests

- Click the CTest button
- Should give you a verbose output on failure
- Should also mention the number of tests that have passed or failed
