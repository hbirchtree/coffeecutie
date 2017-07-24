mkdir $env:BUILD_DIR
cd $env:BUILD_DIR

$release = (Invoke-RestMethod -Uri https://github.com/repos/hbirchtree/coffeecutie/releases/latest/ -Headers @{"Authorization" = "token $env:GITHUB_TOKEN"; "Accept" = "application/vnd.github.v3+json"})

$assets = $release.assets

& $env:APPVEYOR_BUILD_FOLDER\$env:MAKEFILE_DIR\Makefile.windows.ps1 -SourceDir $env:APPVEYOR_BUILD_FOLDER -Target $env:BUILDVARIANT -Config Debug -CMakeBin $env:CMAKE_BIN -ExtraArgs ("-DCOFFEE_BUILD_OPENAL=OFF","-DSKIP_HIGHMEM_TESTS=ON","-DSKIP_LINKAGE_TEST=ON")

cd "$env:APPVEYOR_BUILD_FOLDER"
7z a libraries_$env:BUILDVARIANT.zip $env:BUILD_DIR\out
