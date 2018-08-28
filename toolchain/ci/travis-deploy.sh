#!/bin/bash

set -e
set -x

source $(dirname $0)/travis-deploy-common.sh
source $(dirname $0)/travis-build-common.sh

notify "Sending coverage info"

collect_coverage

BINARY_ASSET="$TRAVIS_BUILD_DIR/binaries_${DEPLOYED_BUILDVARIANT}.tar.gz"
LIBRARY_ASSET="$TRAVIS_BUILD_DIR/libraries_${DEPLOYED_BUILDVARIANT}.tar.gz"
TARGET_SLUG="$(get_deploy_slug)"

notify "Creating artifacts"


package_binaries "${BUILD_DIR}" "$BINARY_ASSET"
package_libraries "${BUILD_DIR}" "$LIBRARY_ASSET"

deploy_asset "$BINARY_ASSET" ${TARGET_SLUG}
deploy_asset "$LIBRARY_ASSET" ${TARGET_SLUG}
