vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO iOrange/bcdec
    REF 026acf98ea271045cb10713daa96ba98528badb7
    SHA512 b87719e69d86f4e80a2c248f1e59fb6caa14af2951813ecac8fc1b4b2f6037d0b232f9d1a2a28b696a5b45a51beffc46c793b3174f16834590209252d64e4792
    HEAD_REF main
)

file(COPY ${SOURCE_PATH}/bcdec.h DESTINATION "${CURRENT_PACKAGES_DIR}/include")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/FindBCDec.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/vcpkg-cmake-wrapper.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
