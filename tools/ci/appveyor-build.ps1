mkdir $env:BUILD_DIR
cd $env:BUILD_DIR

& $env:APPVEYOR_BUILD_FOLDER\$env:MAKEFILE_DIR\Makefile.windows.ps1 -SourceDir $env:APPVEYOR_BUILD_FOLDER -Target $env:BUILDVARIANT -Config Debug -CMakeBin $env:CMAKE_BIN -ExtraArgs ("-DCOFFEE_BUILD_OPENAL=OFF","-DSKIP_HIGHMEM_TESTS=ON","-DSKIP_LINKAGE_TEST=ON")

cd "$env:APPVEYOR_BUILD_FOLDER"
7z a libraries_$env:BUILDVARIANT.zip $env:BUILD_DIR\out
