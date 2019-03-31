#!/bin/bash

set -e
set -x

source $(dirname $0)/travis-deploy-common.sh
source $(dirname $0)/travis-build-common.sh

notify "Sending coverage info"

collect_coverage

BINARY_ASSET="binaries_${BUILDVARIANT}.tar.gz"
LIBRARY_ASSET="libraries_${BUILDVARIANT}.tar.gz"
TARGET_SLUG="$(get_deploy_slug)"

notify "Creating artifacts"

[ -f ${SOURCE_DIR}/deploy-script.sh ] && source ${SOURCE_DIR}/deploy-script.sh && exit 0

pushd $TRAVIS_BUILD_DIR

package_binaries "${BUILD_DIR}" "$BINARY_ASSET"
package_libraries "${BUILD_DIR}" "$LIBRARY_ASSET"

deploy_asset "$BINARY_ASSET" ${TARGET_SLUG}
deploy_asset "$LIBRARY_ASSET" ${TARGET_SLUG}

[ -f ${SOURCE_DIR}/deploy-post-trigger.sh ] && source ${SOURCE_DIR}/deploy-post-trigger.sh

popd
