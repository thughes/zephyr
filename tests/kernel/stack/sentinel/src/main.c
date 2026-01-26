/*
 * Copyright (c) 2023 Meta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>

#define THREAD_STACK_SIZE (512 + CONFIG_TEST_EXTRA_STACK_SIZE)

K_THREAD_STACK_DEFINE(thread_stack, THREAD_STACK_SIZE);
static struct k_thread thread_data;

static void thread_entry(void *p1, void *p2, void *p3)
{
	/* Do nothing */
}

ZTEST(stack_sentinel, test_stack_sentinel_info)
{
	k_tid_t tid;
	size_t stack_size = K_THREAD_STACK_SIZEOF(thread_stack);
	uintptr_t stack_buf_start;

	tid = k_thread_create(&thread_data, thread_stack, stack_size,
			      thread_entry, NULL, NULL, NULL,
			      0, K_INHERIT_PERMS, K_NO_WAIT);

	stack_buf_start = (uintptr_t)K_THREAD_STACK_BUFFER(thread_stack);

	zassert_equal(thread_data.stack_info.start, stack_buf_start + 4,
		      "stack_info.start is not adjusted for sentinel");
	zassert_equal(thread_data.stack_info.size,
		      (stack_size - K_THREAD_STACK_RESERVED) - 4,
		      "stack_info.size is not adjusted for sentinel");

	k_thread_abort(tid);
}

ZTEST_SUITE(stack_sentinel, NULL, NULL, NULL, NULL, NULL);
