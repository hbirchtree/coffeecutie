PRESSURE_ASSET="pressurecooker_${BUILDVARIANT}.tar.gz"

# Package and release PressureCooker as its own bundle

pushd "${BUILD_DIR}/${BUILDVARIANT}"

# Path to PressureCooker and the extensions may vary,
#  therefore using $(ls ...) to add them conditionally
# Also, -C does not work with the $(ls ...) workaround, so pushd
tar -zcf "${PRESSURE_ASSET}" \
    $(ls \
        bin/PressureCooker* \
        bin/*/PressureCooker* \
        lib/*/*Pressurize* \
        lib/*Pressurize* \
    )

deploy_asset "${PRESSURE_ASSET}" ${TARGET_SLUG}

popd

