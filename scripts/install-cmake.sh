#!/bin/sh

# Exit if fail
set -e

if [ "$TRAVIS_OS_NAME" = linux -o -z "$TRAVIS_OS_NAME" ]; then

    # Download cmake
    wget http://www.cmake.org/files/v3.2/cmake-3.2.3-Linux-x86_64.sh
    chmod a+x cmake-3.2.3-Linux-x86_64.sh

    # Install
    sudo ./cmake-3.2.3-Linux-x86_64.sh --skip-license --prefix=/usr/local
    export PATH=/usr/local/bin:$PATH

elif [ "$TRAVIS_OS_NAME" = osx ]; then

    # Make sure xcode is setup
    xcode-select --install

    # Update brew
    brew update || echo "suppress failures in order to ignore warnings"
    brew doctor || echo "suppress failures in order to ignore warnings"
    brew list cmake || echo "suppress failures in order to ignore warnings"
    sudo brew uninstall --force cmake || "suppress failures in order to ignore warnings"
    brew search cmake || echo "suppress failures in order to ignore warnings"

    # Install cmake
    brew install cmake || echo "suppress failures in order to ignore warnings"
    brew upgrade cmake || echo "suppress failures in order to ignore warnings"
fi

# Debug information
cmake --version

