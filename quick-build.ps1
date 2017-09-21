$env:BUILD_DIR = "$PWD\multi_build"
$env:APPVEYOR_BUILD_FOLDER = "$PWD"
$env:BUILDVARIANT = $args[0]
$env:CONFIGURATION = "Debug"
$env:CMAKE_BIN = "cmake.exe"
$env:SOURCE_DIR = "$PWD"

$PYTHON="C:\Python36\python.exe"

function build_info() {
    . $PYTHON $PWD\buildinfo.py $args
}

$env:SCRIPT_DIR = (build_info script_location)
$env:MAKEFILE_DIR = (build_info makefile_location)
$env:DEPENDENCIES = (build_info dependencies)

echo "-- Building $env:BUILDVARIANT in $env:CONFIGURATION mode"

. $PWD\$env:SCRIPT_DIR\appveyor-build.ps1
