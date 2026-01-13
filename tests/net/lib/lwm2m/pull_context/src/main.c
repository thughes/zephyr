/*
 * Copyright (c) 2025 Zephyr
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/net/lwm2m.h>
#include <stdlib.h>
#include "lwm2m_pull_context.h"
#include "lwm2m_engine.h"

static void result_cb(uint16_t obj_inst_id, int error_code)
{
    /* Expected failure due to invalid URI, but shouldn't crash */
    printk("Callback called with error %d\n", error_code);
}

static int write_cb(uint16_t obj_inst_id, uint16_t res_id, uint16_t res_inst_id,
		    uint8_t *data, uint16_t data_len, bool last_block,
		    size_t total_size, size_t offset)
{
    return 0;
}

static void test_lwm2m_pull_context_over_read(void)
{
    /* Allocate small buffer on heap */
    char *uri = malloc(10);
    zassert_not_null(uri, "Malloc failed");
    strcpy(uri, "short");

    struct requesting_object req = {
        .result_cb = result_cb,
        .write_cb = write_cb,
    };

    /* This should trigger ASAN heap-buffer-overflow (read) if bug is present. */
    /* With fix, it should run safely and call result_cb with error (parsing uri). */
    lwm2m_pull_context_start_transfer(uri, req, K_NO_WAIT);

    free(uri);
}

ZTEST_SUITE(lwm2m_pull_context, NULL, NULL, NULL, NULL, NULL);

ZTEST(lwm2m_pull_context, test_over_read)
{
    test_lwm2m_pull_context_over_read();
}
