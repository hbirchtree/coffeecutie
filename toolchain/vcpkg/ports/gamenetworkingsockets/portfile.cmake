vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO ValveSoftware/GameNetworkingSockets
    REF 505c697d0abef5da2ff3be35aa4ea3687597c3e9 # v1.4.1
    SHA512 3e4b4da138f2b356169e6504aa899c9eca4fba5b5fcaed2a0ae8a2f5828976dd00af9f3262c75bd6d820300da87ebe32da152fecddc278a651f3b33eb59142df
    HEAD_REF master
    PATCHES
        fix-depend-protobuf.patch
        fix-android-support.patch
        skip-openssl-version-check.patch
        fix-mingw-compat.patch
        fix-openssl-dependency.patch
        fix-emscripten-support.patch
        add-webrtc-datachannel-transport.patch
)

set(CRYPTO_BACKEND OpenSSL)

# New source for the "webrtc-datachannel" feature (see
# add-webrtc-datachannel-transport.patch and
# examples/blam/cblam-testing/WEBRTC_TRANSPORT.md) -- genuinely new files,
# not a patch on existing ones, so injected the same way libwma's overlay
# port injects its own replacement sources: file(COPY ...) into the
# extracted tree at the same layout the patch's CMakeLists.txt changes
# expect (src/steamnetworkingsockets/clientlib/).
file(COPY
    "${CMAKE_CURRENT_LIST_DIR}/steamnetworkingsockets_p2p_webrtc_datachannel.h"
    "${CMAKE_CURRENT_LIST_DIR}/steamnetworkingsockets_p2p_webrtc_datachannel.cpp"
    DESTINATION "${SOURCE_PATH}/src/steamnetworkingsockets/clientlib"
)

vcpkg_check_features(
    OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        webrtc-datachannel ENABLE_WEBRTC_DATACHANNEL
)

# GNS's own native ICE client (ENABLE_ICE, default ON upstream) does its
# own separate NAT-traversal/candidate-gathering over raw UDP sockets --
# redundant work on Emscripten specifically, where there's no raw socket
# API to begin with and the browser's own WebRTC stack already did NAT
# traversal before CConnectionTransportP2PWebRTC ever gets constructed
# (see examples/blam/cblam-testing/WEBRTC_TRANSPORT.md). Confirmed via an
# actual run, not just reasoned about: CConnectionTransportP2PICE_Valve::Init
# was firing on every connection attempt regardless, gathering reflexive/
# relay candidates against an empty STUN/TURN server list (guaranteed to
# find nothing), and GNS's own instrumentation flagged the resulting lock
# hold time as a "Performance warning" in the log. Off only for Emscripten:
# an ICE-enabled native server coexists fine with an ICE-disabled client --
# ICEFailed() (steamnetworkingsockets_p2p_ice.cpp) just logs and drops the
# ICE transport for that one connection, non-fatally, so the mismatch
# alone isn't a reason to disable ICE server-side too. (Tried that --
# only swapped "ice_enabled" errors for "no available transports" once
# ICE could no longer mask the real gap: the custom transport not being
# attached to the accept-side connection in time.)
set(EXTRA_OPTIONS)
if(VCPKG_TARGET_IS_EMSCRIPTEN)
    list(APPEND EXTRA_OPTIONS -DENABLE_ICE=OFF)
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${FEATURE_OPTIONS}
        ${EXTRA_OPTIONS}
        -DBUILD_SHARED_LIB=OFF
        -DBUILD_TESTS=OFF
        -DBUILD_EXAMPLES=OFF
        -DBUILD_TOOLS=OFF
        -DUSE_CRYPTO=${CRYPTO_BACKEND}
        -DUSE_CRYPTO25519=${CRYPTO_BACKEND}
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/GameNetworkingSockets")
vcpkg_fixup_pkgconfig()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_copy_pdbs()
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
