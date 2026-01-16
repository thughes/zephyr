# ST Microelectronics stmemsc hal i/f
#
# Copyright (c) 2021 STMicroelectronics
#
# SPDX-License-Identifier: Apache-2.0
#
zephyr_library()

zephyr_library_sources_ifdef(CONFIG_I2C        stmemsc_i2c.c)
zephyr_library_sources_ifdef(CONFIG_I3C        stmemsc_i3c.c)
zephyr_library_sources_ifdef(CONFIG_SPI        stmemsc_spi.c)
zephyr_library_sources(stmemsc_mdelay.c)

# FIXME: fix the code in the hal_st module and remove this
zephyr_compile_options($<$<STREQUAL:$<TARGET_PROPERTY:NAME>,..__modules__hal__st>:-Wno-self-assign>)
