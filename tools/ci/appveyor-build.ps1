$PrevPwd = $Pwd

mkdir $env:BUILD_DIR
cd $env:BUILD_DIR

$LIBRARY_DIR = "$env:BUILD_DIR\libraries"

rm -r $LIBRARY_DIR

ForEach($dep in $env:DEPENDENCIES -split ";") {
    if ($dep.Length -eq 0) {
        continue
    }
    try{
        $release = (Invoke-RestMethod -Uri https://api.github.com/repos/$dep/releases/latest -Headers @{"Accept" = "application/vnd.github.v3+json"})

        echo "Found release for $dep"

        $dep = $dep.Replace("/", "_")

        $assets = $release.assets

        ForEach($asset in $assets) {
            if ($asset.name.Contains("$env:BUILDVARIANT")) {
                $zipfile = $asset.name
                $dest_dir = $LIBRARY_DIR
                echo "Downloading $dep $env:BUILDVARIANT library..."
                Invoke-WebRequest $asset.browser_download_url -UseBasicParsing -OutFile $zipfile
                echo "Successfully downloaded library for $dep!"

                echo "Extracting $dep from archive..."
                Expand-Archive -Path $zipfile -DestinationPath $dest_dir -Force
                mv -Force "$LIBRARY_DIR\out\*" "$LIBRARY_DIR\"
                echo "Successfully extracted library $dep!"
            }
        }
    } catch {
        echo "Failed to download $dep"
    }
}

$Args = ("-DCOFFEE_BUILD_OPENSSL=OFF","-DCOFFEE_BUILD_OPENAL=OFF",`
         "-DSKIP_HIGHMEM_TESTS=ON","-DSKIP_LINKAGE_TEST=ON")

. $env:APPVEYOR_BUILD_FOLDER\$env:MAKEFILE_DIR\Makefile.windows.ps1 `
    -CMakeBin $env:CMAKE_BIN -Standalone

$BuildDir = "$env:BUILD_DIR\build_$env:BUILDVARIANT"

echo "Starting CMake process"
BuildProject $env:BUILDVARIANT $env:APPVEYOR_BUILD_FOLDER $BuildDir $LIBRARY_DIR "Debug" $Args

cd "$BuildDir\out"
7z a "$env:APPVEYOR_BUILD_FOLDER\libraries_$env:BUILDVARIANT.zip" "*"

cd $PrevPwd