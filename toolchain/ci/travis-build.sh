#!/bin/bash

set -x

source $(dirname $0)/travis-build-common.sh

notify "Building $BUILDVARIANT"

download_dependencies "${DEPENDENCIES}" || die "Failed to download dependencies"

[ -f ${SOURCE_DIR}/pre-build-script.sh ] && source ${SOURCE_DIR}/pre-build-script.sh

[ -f ${SOURCE_DIR}/build-script.sh ] && source ${SOURCE_DIR}/build-script.sh && exit 0

build_target "${BUILDVARIANT}"
