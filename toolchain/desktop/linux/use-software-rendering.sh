#!/usr/bin/env bash

echo "Setting up environment for software-rendered OpenGL:"
export LIBGL_ALWAYS_SOFTWARE=1
export __GLX_VENDOR_LIBRARY_NAME=mesa
