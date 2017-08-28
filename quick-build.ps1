$env:BUILD_DIR = "$PWD\multi_build"
$env:APPVEYOR_BUILD_FOLDER = "$PWD"
$env:BUILDVARIANT = $args[0]
$env:CONFIGURATION = "Debug"
$env:CMAKE_BIN = "cmake.exe"

$env:SCRIPT_DIR = "tools\ci"
$env:MAKEFILE_DIR = "tools\makers"
$env:DEPENDENCIES = ""

echo "-- Building $env:BUILDVARIANT in $env:CONFIGURATION mode"

. $PWD\$env:SCRIPT_DIR\appveyor-build.ps1
