PRESSURE_ASSET="pressurecooker+${BUILDVARIANT}.tar.gz"

# Package and release PressureCooker as its own bundle

tar -zcf "${PRESSURE_ASSET}" -C "${BUILD_DIR}" \
    $(ls \
        build/*/bin/PressureCooker* \
        build/*/bin/*/PressureCooker* \
        build/*/lib/*/*Pressurize* \
        build/*/lib/*Pressurize* \
    )

package_binaries "${BUILD_DIR}" "${PRESSURE_ASSET}"
deploy_asset "${PRESSURE_ASSET}" ${TARGET_SLUG}
