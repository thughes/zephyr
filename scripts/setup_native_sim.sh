#!/bin/bash
set -e

# --- Configuration ---
# Use the current directory as the Zephyr source if run from within the repo
ZEPHYR_SOURCE_DIR=$(pwd)
WORKSPACE_DIR=~/zephyrproject
VENV_DIR="$WORKSPACE_DIR/.venv"

echo "Using Zephyr source from: $ZEPHYR_SOURCE_DIR"
echo "Setting up workspace at: $WORKSPACE_DIR"

# 1. Update and Upgrade OS
echo "--- Updating system packages ---"
sudo apt update
# Upgrade is optional and can be slow/interactive, uncomment if needed
# sudo DEBIAN_FRONTEND=noninteractive apt upgrade -y

# 2. Install Host Dependencies
echo "--- Installing host dependencies ---"
sudo DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
    git cmake ninja-build gperf \
    ccache dfu-util device-tree-compiler wget \
    python3-dev python3-venv python3-tk \
    xz-utils file make gcc gcc-multilib g++-multilib \
    libsdl2-dev libmagic1

# Verify versions
cmake --version
python3 --version
dtc --version

# 3. Create Workspace and Virtual Environment
mkdir -p "$WORKSPACE_DIR"

echo "Creating Python virtual environment..."
if [ ! -d "$VENV_DIR" ]; then
    python3 -m venv "$VENV_DIR"
fi

# 4. Activate Venv and Install West
echo "--- Installing West ---"
source "$VENV_DIR/bin/activate"
pip install west

# 5. Initialize Zephyr Workspace
REAL_SOURCE=$(realpath "$ZEPHYR_SOURCE_DIR")
REAL_TARGET=$(realpath -m "$WORKSPACE_DIR/zephyr")

if [ "$REAL_SOURCE" == "$REAL_TARGET" ]; then
    echo "Source and target are the same. Skipping clone."
else
    if [ -d "$WORKSPACE_DIR/zephyr" ]; then
        echo "Target zephyr directory already exists."
    else
        echo "Cloning local repo to workspace..."
        # Clone the local repository to the workspace to create a proper West manifest structure
        git clone "$ZEPHYR_SOURCE_DIR" "$WORKSPACE_DIR/zephyr"
    fi
fi

echo "--- Initializing West workspace ---"
if [ -d "$WORKSPACE_DIR/.west" ]; then
    echo "West workspace already initialized."
else
    # Initialize west using the cloned zephyr repo in the workspace
    west init -l "$WORKSPACE_DIR/zephyr"
fi

# 6. Update Workspace and Install Dependencies
echo "--- Updating West workspace ---"
cd "$WORKSPACE_DIR"
west update

echo "--- Exporting Zephyr CMake package ---"
west zephyr-export

echo "--- Installing Zephyr Python dependencies ---"
west packages pip --install

# 7. Install Zephyr SDK
echo "--- Installing Zephyr SDK ---"
cd "$WORKSPACE_DIR/zephyr"
# Remove -y as it is not supported by west sdk install
west sdk install

# 8. Setup Udev Rules
SDK_DIR=$(find "$HOME/.local/share" -maxdepth 1 -type d -name "zephyr-sdk-*" | sort -V | tail -n 1)
if [ -n "$SDK_DIR" ]; then
    echo "Found SDK at $SDK_DIR."
    if [ -d "/etc/udev/rules.d" ]; then
        echo "Copying udev rules..."
        sudo cp "$SDK_DIR"/sysroots/x86_64-pokysdk-linux/usr/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d/ || true
        sudo udevadm control --reload || true
    fi
fi

# 9. Verify native_sim build
echo "--- Verifying native_sim build ---"
cd "$WORKSPACE_DIR/zephyr"
source "$VENV_DIR/bin/activate"

# Clean previous builds
rm -rf twister-out

echo "Running twister verification..."
west twister -p native_sim -T samples/hello_world --integration

echo "###########################################################"
echo "Verification Complete!"
echo "###########################################################"
