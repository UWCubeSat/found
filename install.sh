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

# Detect if running as root (UID 0)
if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
else
    SUDO="sudo"
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
            PM="$SUDO apt-get"
            execute_cmd $SUDO apt-get -y update
            execute_cmd $SUDO apt-get -y dist-upgrade
        elif command -v yum &> /dev/null; then
            PM="$SUDO yum"
            execute_cmd $SUDO yum -y update
        # elif command -v dnf &> /dev/null; then
        #     PM="$SUDO dnf"
        #     execute_cmd $SUDO dnf -y upgrade
        else
            echo "No known package manager found"
            exit 1
        fi
        INSTALL="$PM install -y"
        # List of packages to install
        PACKAGES="git g++ make cmake wget tar python3 python3-pip pipx graphviz valgrind"
        ;;
    Darwin*)
        # Check if Homebrew is installed; install it if not
        if ! command -v brew &> /dev/null; then
            echo "Homebrew not found. Installing Homebrew..."
            execute_cmd "curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh"
        fi
        INSTALL="brew install"
        # List of packages to install
        PACKAGES="git gcc make cmake wget gnu-tar python pipx graphviz"
        ;;
    *)
        echo "Unknown Operating System $OS"
        exit 1
        ;;
esac

# Install each package and echo the command
for PACKAGE in $PACKAGES; do
    CMD="$INSTALL $PACKAGE"
    execute_cmd $CMD
done

# Install doxygen
CMD="$INSTALL doxygen || ( \
    $INSTALL bison && \
    $INSTALL flex && \
    wget https://sourceforge.net/projects/doxygen/files/rel-1.9.8/doxygen-1.9.8.src.tar.gz && \
    tar xf doxygen-1.9.8.src.tar.gz && \
    cd doxygen-1.9.8 && \
    mkdir build && cd build && cmake -G \"Unix Makefiles\" .. && make install && \
    cd ../.. && \
    rm -rf doxygen-1.9.8* \
)"
execute_cmd $CMD

# Python packages to install
PYTHON_PACKAGES="git+https://github.com/cpplint/cpplint.git@2.0.0#egg=cpplint git+https://github.com/gcovr/gcovr.git@8.3#egg=gcovr"

# Install each package and echo the command
for PACKAGE in $PYTHON_PACKAGES; do
    if [ "$(id -u)" -eq 0 ]; then
        CMD="pip install --break-system-packages $PACKAGE"
    else
        CMD="pipx install $PACKAGE"
    fi
    execute_cmd $CMD
    
done
execute_cmd pipx ensurepath

printf "\n============Please Restart your Terminal============\n"
