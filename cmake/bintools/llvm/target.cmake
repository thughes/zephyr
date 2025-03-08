# SPDX-License-Identifier: Apache-2.0

# Configures binary tools as llvm binary tool set

#if(DEFINED TOOLCHAIN_HOME)
#  set(find_program_clang_args PATHS ${TOOLCHAIN_HOME} NO_DEFAULT_PATH)
#  set(find_program_binutils_args PATHS ${TOOLCHAIN_HOME})
#endif()
#
## Extract the clang version.  Debian (maybe other distros?) will
## append a version to llvm-objdump/objcopy
#execute_process(COMMAND ${CMAKE_C_COMPILER} --version
#                OUTPUT_VARIABLE CLANGVER)
#string(REGEX REPLACE "[^0-9]*([0-9]+).*" "\\1" CLANGVER ${CLANGVER})
#
#find_program(CMAKE_AR      llvm-ar      ${find_program_clang_args})
#find_program(CMAKE_NM      llvm-nm      ${find_program_clang_args})
#find_program(CMAKE_OBJDUMP NAMES
#                           llvm-objdump
#                           llvm-objdump-${CLANGVER}
#                           ${find_program_clang_args})
#find_program(CMAKE_RANLIB  llvm-ranlib  ${find_program_clang_args})
#find_program(CMAKE_STRIP   llvm-strip   ${find_program_clang_args})
#find_program(CMAKE_OBJCOPY NAMES
#                           llvm-objcopy
#                           llvm-objcopy-${CLANGVER}
#                           objcopy
#                           ${find_program_binutils_args})
#find_program(CMAKE_READELF NAMES
#                           llvm-readelf
#                           llvm-readelf-${CLANGVER}
#                           readelf
#                           ${find_program_binutils_args})
#
## Use the gnu binutil abstraction
#include(${ZEPHYR_BASE}/cmake/bintools/llvm/target_bintools.cmake)

# Use generic bintools.
include("${TOOLCHAIN_ROOT}/cmake/bintools/llvm/generic.cmake")

if ("${ARCH}" STREQUAL "arm" OR "${ARCH}" STREQUAL "riscv")
  set(CMAKE_AR         "${CROSS_COMPILE}ar")
  set(CMAKE_NM         "${CROSS_COMPILE}nm")
  set(CMAKE_OBJCOPY    "${CROSS_COMPILE}objcopy")
  set(CMAKE_OBJDUMP    "${CROSS_COMPILE}objdump")
  set(CMAKE_RANLIB     "${CROSS_COMPILE}ranlib")
  set(CMAKE_READELF    "${CROSS_COMPILE}readelf")

  # CMake is looking for bintools by adding a suffix to compiler binary
  # e.g for AR it would be armv7m-cros-eabi-clang-ar, which doesn't exist.
  # Set bintools locations manually
  set(CMAKE_C_COMPILER_AR         "${CROSS_COMPILE}ar")
  set(CMAKE_C_COMPILER_NM         "${CROSS_COMPILE}nm")
  set(CMAKE_C_COMPILER_OBJCOPY    "${CROSS_COMPILE}objcopy")
  set(CMAKE_C_COMPILER_OBJDUMP    "${CROSS_COMPILE}objdump")
  set(CMAKE_C_COMPILER_RANLIB     "${CROSS_COMPILE}ranlib")
  set(CMAKE_C_COMPILER_READELF    "${CROSS_COMPILE}readelf")

  # And for C++
  set(CMAKE_CXX_COMPILER_AR         "${CROSS_COMPILE}ar")
  set(CMAKE_CXX_COMPILER_NM         "${CROSS_COMPILE}nm")
  set(CMAKE_CXX_COMPILER_OBJCOPY    "${CROSS_COMPILE}objcopy")
  set(CMAKE_CXX_COMPILER_OBJDUMP    "${CROSS_COMPILE}objdump")
  set(CMAKE_CXX_COMPILER_RANLIB     "${CROSS_COMPILE}ranlib")
  set(CMAKE_CXX_COMPILER_READELF    "${CROSS_COMPILE}readelf")

endif()

# Include the GNU bintools properties as a base.
include("${ZEPHYR_BASE}/cmake/bintools/gnu/target_bintools.cmake")

