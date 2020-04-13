$env:APPVEYOR_BUILD_FOLDER = "$PWD"
$env:BUILDVARIANT = $args[0]
$env:BUILD_DIR = "$PWD/multi_build/$env:BUILDVARIANT"
$env:CONFIGURATION = "Debug"
$env:CMAKE_BIN = "cmake.exe"
$env:SOURCE_DIR = "$PWD"
$env:APPVEYOR_BUILD_WORKER_IMAGE = "$VSVER"

echo "-- Building $env:BUILDVARIANT in $env:CONFIGURATION mode"

. $PWD\toolchain\ci\appveyor-build.ps1

cmake --build $env:BUILD_DIR --target install
