#!/bin/bash

# Exit on any command failure
set -e

# Executes a command with a banner
execute_cmd() {
    local cmd="$@"
    local middle_line="Command: $cmd"
    local middle_line_len=$(echo -n "$middle_line" | wc -m)
    local extra_chars=20
    local total_length=$((middle_line_len + extra_chars))
    local hash_line=$(printf '%*s' "$total_length" | tr ' ' '=')

    printf "\n"
    printf "%s\n" "$hash_line"
    printf "          %s\n" "$middle_line"
    printf "%s\n" "$hash_line"
    printf "\n"

    # Execute the command and check for errors
    if ! eval "$cmd"; then
        echo "Command failed: $cmd"
        exit 1
    fi
}

# Check if the script is running with root privileges
if [ "$(id -u)" -ne 0 ]; then
    echo "This script requires root privileges. Please run with sudo/root."
    exit 1
fi

# Detect the operating system using uname
OS="$(uname -s)"

INSTALL=""

# Perform necessary setup and get the
# installation command depending on
# the OS
case "$OS" in
    Linux*)
        # Detect the package manager
        PM=""
        if command -v apt-get &> /dev/null; then
            PM="apt-get"
            execute_cmd apt-get -y update
            execute_cmd apt-get -y dist-upgrade
        elif command -v yum &> /dev/null; then
            PM="yum"
            execute_cmd yum -y check-update
            execute_cmd yum -y update
        elif command -v dnf &> /dev/null; then
            PM="dnf"
            execute_cmd dnf -y check-update
            execute_cmd dnf -y upgrade
        else
            echo "No known package manager found"
            exit 1
        fi
        INSTALL="$PM install -y"
        ;;
    Darwin*)
        # Check if Homebrew is installed; install it if not
        if ! command -v brew &> /dev/null; then
            echo "Homebrew not found. Installing Homebrew..."
            execute_cmd curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh
        fi
        INSTALL="brew install"
        ;;
    *)
        echo "Unknown Operating System $OS"
        exit 1
        ;;
esac

# List of packages to install
PACKAGES="git g++ make cmake wget tar valgrind python3 python3-pip gcovr doxygen graphviz"

# Install each package and echo the command
for PACKAGE in $PACKAGES; do
    CMD="$INSTALL $PACKAGE"
    execute_cmd $CMD
done

# Python packages to install
PYTHON_PACKAGES="cpplint"

# Install each package and echo the command
for PACKAGE in $PYTHON_PACKAGES; do
    CMD="python3 -m pip install -q $PACKAGE"
    execute_cmd $CMD
done