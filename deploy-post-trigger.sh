PRESSURE_ASSET="pressurecooker+${BUILDVARIANT}.tar.gz"

# Package and release PressureCooker as its own bundle

pushd "${BUILD_DIR}"

# Path to PressureCooker and the extensions may vary,
#  therefore using $(ls ...) to add them conditionally
# Also, -C does not work with the $(ls ...) workaround, so pushd
tar -zcf "${PRESSURE_ASSET}" -C "${BUILD_DIR}" \
    $(ls \
        build/*/bin/PressureCooker* \
        build/*/bin/*/PressureCooker* \
        build/*/lib/*/*Pressurize* \
        build/*/lib/*Pressurize* \
    )

popd

deploy_asset "${PRESSURE_ASSET}" ${TARGET_SLUG}
