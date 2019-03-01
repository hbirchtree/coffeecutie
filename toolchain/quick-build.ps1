$env:BUILD_DIR = "$PWD\multi_build"
$env:APPVEYOR_BUILD_FOLDER = "$PWD"
$env:BUILDVARIANT = $args[0]
$env:CONFIGURATION = "Debug"
$env:CMAKE_BIN = "cmake.exe"
$env:SOURCE_DIR = "$PWD"
$env:APPVEYOR_BUILD_WORKER_IMAGE = "$VSVER"

function build_info() {
    python $PWD\toolchain\buildinfo.py $args
}

$env:SCRIPT_DIR = (build_info script_location)
$env:MAKEFILE_DIR = (build_info makefile_location)
$DEPENDENCIES_ARRAY = (build_info dependencies)

$env:DEPENDENCIES = ""
foreach($dep in $DEPENDENCIES_ARRAY) {
    $env:DEPENDENCIES = "${env:DEPENDENCIES};$dep"
}

echo "-- Dependencies: $env:DEPENDENCIES"

echo "-- Building $env:BUILDVARIANT in $env:CONFIGURATION mode"

. $PWD\$env:SCRIPT_DIR\appveyor-build.ps1
