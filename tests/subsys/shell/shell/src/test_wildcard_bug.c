#include <zephyr/ztest.h>
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_dummy.h>
#include <string.h>

static int cmd_bug_check(const struct shell *sh, size_t argc, char **argv)
{
    // "bug_check param1 param2" -> length 23
    // We expect cmd_buff_len to be 23.
    size_t expected_len = 23;
    zassert_equal(sh->ctx->cmd_buff_len, expected_len, "cmd_buff_len (%d) != expected (%zu)", sh->ctx->cmd_buff_len, expected_len);
    return 0;
}

// Register command and subcommands for wildcard matching
SHELL_STATIC_SUBCMD_SET_CREATE(bug_check_subcmds,
	SHELL_CMD(param1, NULL, NULL, NULL),
	SHELL_CMD(param2, NULL, NULL, NULL),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(bug_check, &bug_check_subcmds, "Check for wildcard bug", cmd_bug_check);

static void execute_cmd(const char *cmd, int result)
{
	const struct shell *sh = shell_backend_dummy_get_ptr();
	int ret;

	ret = shell_execute_cmd(sh, cmd);

	zassert_true(ret == result, "cmd: %s, got:%d, expected:%d",
							cmd, ret, result);
}

ZTEST(sh, test_wildcard_bug_len_mismatch)
{
    // Execute command with wildcard
    // "bug_check p*" should expand to "bug_check param1 param2"
    execute_cmd("bug_check p*", 0);
}
