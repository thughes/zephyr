#include <zephyr/ztest.h>

ZTEST(reproduce_issue, test_dummy)
{
    zassert_true(true, "Dummy test");
}

ZTEST_SUITE(reproduce_issue, NULL, NULL, NULL, NULL, NULL);
