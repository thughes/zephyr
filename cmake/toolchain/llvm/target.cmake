# SPDX-License-Identifier: Apache-2.0

#if(CONFIG_LLVM_USE_LD)
#  set(LINKER ld)
#elseif(CONFIG_LLVM_USE_LLD)
#  set(LINKER lld)
#endif()
#
#if("${ARCH}" STREQUAL "arm")
#  if(DEFINED CONFIG_ARMV8_M_MAINLINE)
#    # ARMv8-M mainline is ARMv7-M with additional features from ARMv8-M.
#    set(triple armv8m.main-none-eabi)
#  elseif(DEFINED CONFIG_ARMV8_M_BASELINE)
#    # ARMv8-M baseline is ARMv6-M with additional features from ARMv8-M.
#    set(triple armv8m.base-none-eabi)
#  elseif(DEFINED CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
#    # ARMV7_M_ARMV8_M_MAINLINE means that ARMv7-M or backward compatible ARMv8-M
#    # processor is used.
#    set(triple armv7m-none-eabi)
#  elseif(DEFINED CONFIG_ARMV6_M_ARMV8_M_BASELINE)
#    # ARMV6_M_ARMV8_M_BASELINE means that ARMv6-M or ARMv8-M supporting the
#    # Baseline implementation processor is used.
#    set(triple armv6m-none-eabi)
#  else()
#    # Default ARM target supported by all processors.
#    set(triple arm-none-eabi)
#  endif()
#elseif("${ARCH}" STREQUAL "arm64")
#  set(triple aarch64-none-elf)
#elseif("${ARCH}" STREQUAL "x86")
#  if(CONFIG_64BIT)
#    set(triple x86_64-pc-none-elf)
#  else()
#    set(triple i686-pc-none-elf)
#  endif()
#elseif("${ARCH}" STREQUAL "riscv")
#  if(CONFIG_64BIT)
#    set(triple riscv64-unknown-elf)
#  else()
#    set(triple riscv32-unknown-elf)
#  endif()
#endif()
#
#if(DEFINED triple)
#  set(CMAKE_C_COMPILER_TARGET   ${triple})
#  set(CMAKE_ASM_COMPILER_TARGET ${triple})
#  set(CMAKE_CXX_COMPILER_TARGET ${triple})
#
#  unset(triple)
#endif()
#
#if(CONFIG_LIBGCC_RTLIB)
#  set(runtime_lib "libgcc")
#elseif(CONFIG_COMPILER_RT_RTLIB)
#  set(runtime_lib "compiler_rt")
#endif()
#
#list(APPEND TOOLCHAIN_C_FLAGS --config
#	${ZEPHYR_BASE}/cmake/toolchain/llvm/clang_${runtime_lib}.cfg)
#list(APPEND TOOLCHAIN_LD_FLAGS --config
#	${ZEPHYR_BASE}/cmake/toolchain/llvm/clang_${runtime_lib}.cfg)

set(COMPILER clang)
set(LINKER lld)
set(BINTOOLS llvm)

# Look for toolchain binaries in /usr/bin
set(TOOLCHAIN_HOME "$ENV{HOME}/chromiumos/chroot/usr/bin")

if("${ARCH}" STREQUAL "posix")
set(LINKER ld)
endif()

# Mapping of Zephyr architecture -> toolchain triple
set(CROSS_COMPILE_TARGET_posix        x86_64-pc-linux-gnu)
set(CROSS_COMPILE_TARGET_unit_testing x86_64-pc-linux-gnu)
set(CROSS_COMPILE_TARGET_x86          x86_64-pc-linux-gnu)

set(CROSS_COMPILE_TARGET          ${CROSS_COMPILE_TARGET_${ARCH}})

if("${ARCH}" STREQUAL "arm")
  if(DEFINED CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
    # ARMV7_M_ARMV8_M_MAINLINE means that ARMv7-M or backward compatible ARMv8-M
    # processor is used.
    #
    # The armv7m-cros-eabi compiler-rt was compiled with assumption that it will
    # run on Cortex-M4 MCU with enabled FPU. Calling builtin functions that use
    # floating-point instructions (e.g. __aeabi_ul2d) with FPU disabled leads to
    # Usage Fault. Let's check if FPU is enabled.
    if(NOT DEFINED CONFIG_FPU)
      message(FATAL_ERROR "The armv7m-cros-eabi toolchain requires enabled FPU")
    endif()
    set(CROSS_COMPILE_TARGET armv7m-cros-eabi)
  elseif(DEFINED CONFIG_ARMV6_M_ARMV8_M_BASELINE)
    # ARMV6_M_ARMV8_M_BASELINE means that ARMv6-M or ARMv8-M supporting the
    # Baseline implementation processor is used.
    set(CROSS_COMPILE_TARGET arm-none-eabi)
  endif()

  # LLVM based toolchains for ARM use newlib as a libc.
  # This variable is set AFTER all Kconfig files were processed, so it doesn't
  # affect them, but it's still useful for filtering tests.
  set(TOOLCHAIN_HAS_NEWLIB ON CACHE BOOL "True if toolchain supports newlib")
else ()
  #message(FATAL_ERROR "Unrecognized architecture: ${ARCH}")
  set(CROSS_COMPILE_TARGET riscv32-cros-elf)
  set(TOOLCHAIN_HAS_NEWLIB ON CACHE BOOL "True if toolchain supports newlib")
  message("XXX: env: $ENV{CROSTC_USER_ACKNOWLEDGES_THAT_RISCV_IS_EXPERIMENTAL}")
endif()

# LLVM_TOOLCHAIN_PATH is used as a base path to look for 'newlib.cfg' or
# 'picolibc.cfg' provided by toolchain. Our compiler doesn't provide these files
# but without this variable, CMake looks for these files starting from '/' which
# takes long time and can lead to errors if somebody creates the file somewhere.
set(LLVM_TOOLCHAIN_PATH "$ENV{HOME}/chromiumos/chroot/usr/${CROSS_COMPILE_TARGET}")

# CMAKE_{C, ASM, CXX}_COMPILER_TARGET is used by CMake to provide correct
# "--target" option to Clang and by Zephyr to determine which runtime library
# should be linked.
set(CMAKE_C_COMPILER_TARGET   ${CROSS_COMPILE_TARGET})
set(CMAKE_ASM_COMPILER_TARGET ${CROSS_COMPILE_TARGET})
set(CMAKE_CXX_COMPILER_TARGET ${CROSS_COMPILE_TARGET})

set(CC clang)

# TODO(b/286589977): Remove if() when hermetic host toolchain is added to fwsdk.
# Use non-hermetic host toolchain when running outside chroot.
#if(EXISTS /etc/cros_chroot_version)
  set(CROSS_COMPILE "$ENV{HOME}/chromiumos/chroot/usr/bin/${CROSS_COMPILE_TARGET}-")
#endif()
