/**
 * @file debug/stack.h
 * Stack usage analysis helpers
 */

/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DEBUG_STACK_H_
#define ZEPHYR_INCLUDE_DEBUG_STACK_H_

#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/toolchain.h>
#include <stdbool.h>

#if defined(CONFIG_INIT_STACKS) && defined(CONFIG_THREAD_STACK_INFO)
void z_log_stack_usage(const struct k_thread *thread);
#endif

static inline void log_stack_usage(const struct k_thread *thread)
{
#if defined(CONFIG_INIT_STACKS) && defined(CONFIG_THREAD_STACK_INFO)
	z_log_stack_usage(thread);
#else
	ARG_UNUSED(thread);
#endif
}
#endif /* ZEPHYR_INCLUDE_DEBUG_STACK_H_ */
