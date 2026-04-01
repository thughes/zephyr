# Copyright (c) 2025 Zephyr
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lwm2m_pull_context_test)

zephyr_include_directories(${ZEPHYR_BASE}/subsys/net/lib/lwm2m)
target_sources(app PRIVATE src/main.c)
