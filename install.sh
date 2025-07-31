#!/bin/bash
set -e

PACKAGE_NAME="hid-g20s"
PACKAGE_VERSION="1.0"
SOURCE_DIR="/usr/src/${PACKAGE_NAME}-${PACKAGE_VERSION}"

echo "Installing G20S Bluetooth Remote HID Driver..."

# Check if running as root
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root (use sudo)" 
   exit 1
fi

# Check if DKMS is installed
if ! command -v dkms &> /dev/null; then
    echo "DKMS is required but not installed. Please install it first:"
    echo "  Arch Linux: sudo pacman -S dkms"
    echo "  Ubuntu/Debian: sudo apt install dkms"
    echo "  Fedora: sudo dnf install dkms"
    exit 1
fi

# Create source directory
mkdir -p "$SOURCE_DIR"

# Copy files
echo "Copying source files..."
cp hid-g20s.c "$SOURCE_DIR/"
cp Makefile "$SOURCE_DIR/"
cp dkms.conf "$SOURCE_DIR/"

# Install udev rules
echo "Installing udev rules..."
cp 99-hid-g20s.rules /etc/udev/rules.d/
udevadm control --reload-rules

# Add to DKMS
echo "Adding module to DKMS..."
dkms add -m "$PACKAGE_NAME" -v "$PACKAGE_VERSION"

# Build and install
echo "Building and installing module..."
dkms build -m "$PACKAGE_NAME" -v "$PACKAGE_VERSION"
dkms install -m "$PACKAGE_NAME" -v "$PACKAGE_VERSION"

# Load the module
echo "Loading module..."
modprobe hid-g20s

echo ""
echo "Installation complete!"
echo "The hid-g20s module will now automatically load when a G20S remote is connected."
echo "DKMS will automatically rebuild the module for new kernel versions."
echo ""
echo "To uninstall:"
echo "  sudo dkms remove $PACKAGE_NAME/$PACKAGE_VERSION --all"
echo "  sudo rm /etc/udev/rules.d/99-hid-g20s.rules"
echo "  sudo rm -rf $SOURCE_DIR"
