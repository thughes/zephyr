/*
 * Copyright (c) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/random/random.h>
#include <zephyr/internal/syscall_handler.h>

static inline void z_vrfy_sys_rand_get(void *dst, size_t len)
{
	K_OOPS(K_SYSCALL_MEMORY_WRITE(dst, len));

	z_impl_sys_rand_get(dst, len);
}
#include <zephyr/syscalls/sys_rand_get_mrsh.c>

#ifdef CONFIG_CSPRNG_ENABLED
static inline int z_vrfy_sys_csrand_get(void *dst, size_t len)
{
	K_OOPS(K_SYSCALL_MEMORY_WRITE(dst, len));

	return z_impl_sys_csrand_get(dst, len);
}
#include <zephyr/syscalls/sys_csrand_get_mrsh.c>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(os, CONFIG_KERNEL_LOG_LEVEL);
#endif
