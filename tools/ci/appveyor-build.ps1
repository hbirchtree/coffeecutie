mkdir $env:BUILD_DIR
cd $env:BUILD_DIR
$env:MAKEFILE_DIR
#& $env:APPVEYOR_BUILD_FOLDER\$env:MAKEFILE_DIR\Makefile.windows.ps1 -SourceDir $env:APPVEYOR_BUILD_FOLDER -Target uwp.amd64 -Config Debug -CMakeBin $env:CMAKE_BIN

& $env:APPVEYOR_BUILD_FOLDER\$env:MAKEFILE_DIR\Makefile.windows.ps1 -SourceDir $env:APPVEYOR_BUILD_FOLDER -Target win32.amd64 -Config Debug -CMakeBin $env:CMAKE_BIN -ExtraArgs ("-DCOFFEE_BUILD_OPENAL=OFF","")

cd "$env:APPVEYOR_BUILD_FOLDER"
7z a Coffee_Win64_"$env:APPVEYOR_BUILD_NUMBER"_$env:APPVEYOR_REPO_COMMIT.zip $env:INSTALL_DIR