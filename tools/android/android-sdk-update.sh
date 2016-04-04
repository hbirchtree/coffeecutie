#!/bin/bash

# Installs latest build-tools, properly
android update sdk -u -t 1
# Install API levels and etc.
android update sdk -u --filter "tools,platform-tools,android-19,android-21"

# Workaround: zipalign not found in PATH
ZIPALIGN_UTIL="$(find ~/android-sdk-linux/ -name zipalign)"
ln -s "${ZIPALIGN_UTIL}" ~/android-sdk-linux/tools/
