#!/bin/bash

set -x

source $(dirname $0)/travis-build-common.sh

notify "Building $BUILDVARIANT"

download_dependencies "${DEPENDENCIES}" || die "Failed to download dependencies"

build_target "${BUILDVARIANT}"
