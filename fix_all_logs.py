import sys
import os

with open('unique_files.txt', 'r') as f:
    files = [line.strip().lstrip('./') for line in f]

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
    if filepath == 'lib/os/clock.c':
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
