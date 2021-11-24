# CMAKE tool chain file for Linaro ARM cross-compiler
# this one is important
set(CMAKE_SYSTEM_NAME Linux)
#this one not so much
set(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
set(CROSS_COMPILE_TOOL_DIR $ENV{CROSS_COMPILE})
set(CROSS_COMPILE_PREFIX arm-linux-gnueabihf-)
set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(TARGET_ARCH arm-linux-gnueabihf)
set(TARGET_PLATFORM linux)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")

