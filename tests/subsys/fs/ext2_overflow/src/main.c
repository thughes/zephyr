/*
 * Copyright (c) 2025 Zephyr
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/fs/ext2.h>
#include <zephyr/fs/fs.h>
#include <string.h>

#define TEST_FS_MNTP "/RAM:"

static struct fs_mount_t fs_mnt;

static void test_ext2_format_overflow(void)
{
	struct ext2_cfg cfg;
	int ret;

	/* Initialize config with default values */
	cfg.block_size = 1024;
	cfg.fs_size = 0x80000;
	cfg.bytes_per_inode = 4096;
	cfg.set_uuid = false;

	/* Set volume name to max length without null terminator */
	/* cfg.volume_name is 17 bytes */
	/* We fill it completely with 'A's. */
	/* When strcpy is called in ext2_format, it will read 17 bytes,
	   and look for null terminator. Since there is none in volume_name,
	   it will read past volume_name.
	   If it finds a null later, it will copy > 17 bytes to sb->s_volume_name (16 bytes).
	   This is buffer overflow.
	   Also read overflow on cfg.volume_name.
	 */
	memset(cfg.volume_name, 'A', 17);

	fs_mnt.type = FS_EXT2;
	fs_mnt.flags = FS_MOUNT_FLAG_USE_DISK_ACCESS;
	fs_mnt.storage_dev = "RAM";
	fs_mnt.mnt_point = TEST_FS_MNTP;
	fs_mnt.fs_data = (void *)0x12345678; /* Dummy */

	/* This calls ext2_format -> ext2_format */
	ret = fs_mkfs(FS_EXT2, (uintptr_t)&fs_mnt, &cfg, 0);

	zassert_equal(ret, 0, "mkfs failed");
}

ZTEST_SUITE(ext2_format, NULL, NULL, NULL, NULL, NULL);

ZTEST(ext2_format, test_overflow)
{
	test_ext2_format_overflow();
}
