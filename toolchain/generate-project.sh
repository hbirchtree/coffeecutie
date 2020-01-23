#!/bin/bash

readonly COFFEE_DIR=$(dirname "$(readlink -f "${0}")")

echo "Calling into wrapper script, your project will be updated to use the Python version!"
/usr/bin/env python3 "${COFFEE_DIR}"/generate_project.py "$2" "$1" --execute --convert-sh
