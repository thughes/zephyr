import sys
import os

files = [
    "drivers/adc/adc_handlers.c",
    "drivers/can/can_handlers.c",
    "drivers/counter/counter_handlers.c",
    "drivers/i2c/i2c_handlers.c",
    "drivers/i3c/i3c_handlers.c",
    "drivers/ps2/ps2_handlers.c",
    "drivers/spi/spi_handlers.c",
    "drivers/usb/bc12/bc12_handlers.c",
    "drivers/w1/w1_handlers.c",
    "kernel/poll.c",
    "kernel/stack.c",
    "kernel/userspace_handler.c",
    "lib/os/clock.c",
    "lib/os/mutex.c",
    "lib/os/printk.c",
    "lib/os/sem.c",
    "lib/os/thread_entry.c",
    "lib/os/cbprintf.c",
    "lib/os/cbprintf_complete.c",
    "lib/os/cbprintf_packaged.c",
    "lib/os/assert.c"
]

for filepath in files:
    if not os.path.exists(filepath):
        print(f"Skipping {filepath}: not found")
        continue

    with open(filepath, 'r') as file:
        lines = file.readlines()

    # Check for existing REGISTER
    has_reg = any('LOG_MODULE_REGISTER' in l for l in lines)
    if has_reg:
        print(f"Skipping {filepath}: has REGISTER")
        continue

    # Clean existing DECLARE if inside ZTEST or duplicate?
    # For simplicity, if we detect DECLARE, we assume it is correct, UNLESS it is lib/os/clock.c
    # where we know it is inside ZTEST and we need one outside.

    if filepath == 'lib/os/clock.c':
        # Remove existing ones to be safe and re-add at top
        lines = [l for l in lines if 'LOG_MODULE_DECLARE(os' not in l]

    # Check again if DECLARE is present
    has_decl = any('LOG_MODULE_DECLARE' in l for l in lines)
    if has_decl:
        print(f"Skipping {filepath}: has DECLARE")
        continue

    # Add DECLARE
    print(f"Fixing {filepath}")

    has_log_h = any('zephyr/logging/log.h' in l for l in lines)

    last_include_idx = -1
    for i, line in enumerate(lines):
        if line.strip().startswith('#include'):
            last_include_idx = i

    if last_include_idx != -1:
        to_insert = []
        if not has_log_h:
            to_insert.append('#include <zephyr/logging/log.h>\n')
        to_insert.append('LOG_MODULE_DECLARE(os, CONFIG_KERNEL_LOG_LEVEL);\n')

        for item in reversed(to_insert):
            lines.insert(last_include_idx + 1, item)

        with open(filepath, 'w') as file:
            file.writelines(lines)
