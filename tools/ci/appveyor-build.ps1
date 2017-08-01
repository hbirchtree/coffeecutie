$PrevPwd = $Pwd

mkdir $env:BUILD_DIR
cd $env:BUILD_DIR

# "Authorization" = "token $env:GITHUB_TOKEN";

$INTERNAL_BUILD_DIR = "build_$env:BUILDVARIANT"

$LIBRARY_DIR = "$env:BUILD_DIR\libraries"

ForEach($dep in $DEPENDENCIES -split ";") {
    try{
        $release = (Invoke-RestMethod -Uri https://api.github.com/repos/$dep/releases/latest -Headers @{"Accept" = "application/vnd.github.v3+json"})

        echo "Found release for $dep"

        $dep = $dep.Replace("/", "_")

        $assets = $release.assets

        ForEach($asset in $assets) {
            if ($asset.name.Contains("$BUILDVARIANT")) {
                $zipfile = $asset.name
                $dest_dir = "$INTERNAL_BUILD_DIR\libs"
                echo "Downloading $dep library..."
                Invoke-WebRequest $asset.browser_download_url -UseBasicParsing -OutFile $zipfile
                echo "Successfully downloaded library for $dep!"

                echo "Extracting $dep from archive..."
                Expand-Archive -Path "$zipfile" -DestinationPath "$dep"
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
    -CMakeBin $env:CMAKE_BIN

BuildProject $env:BUILDVARIANT $env:APPVEYOR_BUILD_FOLDER $LIBRARY_DIR "Debug" $Args

cd "$env:APPVEYOR_BUILD_FOLDER"
7z a libraries_$env:BUILDVARIANT.zip $env:BUILD_DIR\out

cd $PrevPwd