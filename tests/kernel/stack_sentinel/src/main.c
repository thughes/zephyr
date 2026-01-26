#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

K_THREAD_STACK_DEFINE(my_stack, 1024);
struct k_thread my_thread;

ZTEST(stack_sentinel, test_stack_info_start)
{
    k_tid_t tid = k_thread_create(&my_thread, my_stack,
                                  K_THREAD_STACK_SIZEOF(my_stack),
                                  NULL, NULL, NULL, NULL,
                                  5, 0, K_NO_WAIT);

    uintptr_t stack_buf_start = (uintptr_t)K_KERNEL_STACK_BUFFER(my_stack);

    TC_PRINT("stack_info.start: 0x%lx\n", my_thread.stack_info.start);
    TC_PRINT("stack_buf_start:  0x%lx\n", stack_buf_start);

    if (IS_ENABLED(CONFIG_STACK_SENTINEL)) {
        zassert_equal(my_thread.stack_info.start, stack_buf_start + 4,
                      "stack_info.start (%lx) should be stack_buf_start + 4 (%lx)",
                      my_thread.stack_info.start, stack_buf_start + 4);
    } else {
        zassert_equal(my_thread.stack_info.start, stack_buf_start,
                      "stack_info.start should be stack_buf_start");
    }

    k_thread_abort(tid);
}

ZTEST_SUITE(stack_sentinel, NULL, NULL, NULL, NULL, NULL);
