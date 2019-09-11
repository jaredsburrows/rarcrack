#!/bin/bash

# Exit if fail
set -e

if [ "$TRAVIS_OS_NAME" = linux -o -z "$TRAVIS_OS_NAME" ]; then

    # install
    sudo apt-get install unrar p7zip

elif [ "$TRAVIS_OS_NAME" = osx ]; then

    # Make sure xcode is setup
    xcode-select --install

    # Update brew
    brew update || echo "suppress failures in order to ignore warnings"
    brew doctor || echo "suppress failures in order to ignore warnings"
    sudo brew uninstall --force p7zip unrar || echo "a"

    # Install cmake
    brew install p7zip unrar || echo "suppress failures in order to ignore warnings"
    brew upgrade p7zip unrar || echo "suppress failures in order to ignore warnings"
fi

# Debug information
7z | grep Version
unrar | grep UNRAR
