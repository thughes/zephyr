/*
 * Copyright (c) 2020 Google LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/mgmt/ec_host_cmd/ec_host_cmd.h>
#include <zephyr/mgmt/ec_host_cmd/simulator.h>
#include <zephyr/ztest.h>

/* Variables used to record what is "sent" to host for verification. */
K_SEM_DEFINE(send_called, 0, 1);
struct ec_host_cmd_tx_buf *sent;

static int host_send(const struct ec_host_cmd_backend *backend)
{
	k_sem_give(&send_called);
	return 0;
}

struct ec_params_add {
	uint32_t in_data;
} __packed;

struct ec_response_add {
	uint32_t out_data;
} __packed;

/*
 * We need a buffer that is safe to use even if the configured RX size is small,
 * for the purpose of the test setup, but we must be careful when passing it to
 * the simulator backend.
 */
static uint8_t host_to_dut_buffer[256];
struct rx_structure {
	struct ec_host_cmd_request_header header;
	union {
		struct ec_params_add add;
		uint8_t raw[0];
	};
} __packed * const host_to_dut = (void *)&host_to_dut_buffer;

static void update_host_to_dut_checksum(void)
{
	host_to_dut->header.checksum = 0;
	uint8_t checksum = 0;
	for (size_t i = 0;
	     i < sizeof(host_to_dut->header) + host_to_dut->header.data_len;
	     ++i) {
		checksum += host_to_dut_buffer[i];
	}
	host_to_dut->header.checksum = (uint8_t)(-checksum);
}

#define EC_CMD_HELLO 0x0001
static enum ec_host_cmd_status
ec_host_cmd_add(struct ec_host_cmd_handler_args *args)
{
	/* Should not be called because init fails */
	return EC_HOST_CMD_SUCCESS;
}
EC_HOST_CMD_HANDLER(EC_CMD_HELLO, ec_host_cmd_add, BIT(0),
		    struct ec_params_add, struct ec_response_add);

ZTEST(ec_host_cmd, test_init_failed_invalid_buffer_size)
{
	/*
	 * This test verifies that if the TX or RX buffer is too small (e.g. 4 bytes),
	 * the EC Host Command subsystem fails to initialize.
	 */

	const struct ec_host_cmd *hc = ec_host_cmd_get_hc();

	printk("DEBUG: hc->state = %d\n", hc->state);
	zassert_equal(hc->state, EC_HOST_CMD_STATE_DISABLED,
		      "EC Host Command state should be DISABLED (%d) but is %d",
		      EC_HOST_CMD_STATE_DISABLED, hc->state);

	/*
	 * Attempt to send data to verify no crash/processing occurs.
	 *
	 * Note: We must be careful not to crash the simulator backend itself if
	 * CONFIG_EC_HOST_CMD_HANDLER_RX_BUFFER_SIZE is small (e.g. 4).
	 * The simulator backend blindly memcpys 'len' bytes to the RX buffer.
	 * If we are in the 'small_rx' test case, the RX buffer is 4 bytes.
	 */

	size_t data_len = sizeof(host_to_dut_buffer);

#if CONFIG_EC_HOST_CMD_HANDLER_RX_BUFFER_SIZE < 8
	/*
	 * In the small RX buffer case, we can only safely send what fits in the buffer
	 * to avoid crashing the simulator.
	 */
	data_len = CONFIG_EC_HOST_CMD_HANDLER_RX_BUFFER_SIZE;
#endif

	host_to_dut->header.prtcl_ver = 3;
	host_to_dut->header.cmd_id = EC_CMD_HELLO;
	host_to_dut->header.cmd_ver = 0;
	host_to_dut->header.reserved = 0;
	/* If data_len is small, this might be truncated, but that's fine,
	   we just want to ensure the handler doesn't run/crash */
	host_to_dut->header.data_len = sizeof(host_to_dut->add);
	host_to_dut->add.in_data = 0x10203040;

	update_host_to_dut_checksum();

	/* Simulate receiving data */
	int rv = ec_host_cmd_backend_sim_data_received(host_to_dut_buffer, data_len);
	zassert_equal(rv, 0, "Could not send data %d", rv);

	/* Ensure send was NOT called (timeout expected as init failed) */
	rv = k_sem_take(&send_called, K_MSEC(100));
	zassert_equal(rv, -EAGAIN, "Send should not be called as init should have failed");
}

static void *ec_host_cmd_tests_setup(void)
{
	ec_host_cmd_backend_sim_install_send_cb(host_send, &sent);
	return NULL;
}

ZTEST_SUITE(ec_host_cmd, NULL, ec_host_cmd_tests_setup, NULL, NULL, NULL);
