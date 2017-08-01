$PrevPwd = $Pwd

mkdir $env:BUILD_DIR
cd $env:BUILD_DIR

# "Authorization" = "token $env:GITHUB_TOKEN";

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
                $dest_dir = $LIBRARY_DIR
                echo "Downloading $dep library..."
                Invoke-WebRequest $asset.browser_download_url -UseBasicParsing -OutFile $zipfile
                echo "Successfully downloaded library for $dep!"

                echo "Extracting $dep from archive..."
                Expand-Archive -Path $zipfile -DestinationPath $dest_dir
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

$BuildDir = "$env:BUILD_DIR\build_$env:BUILDVARIANT"

BuildProject $env:BUILDVARIANT $env:APPVEYOR_BUILD_FOLDER $BuildDir $LIBRARY_DIR "Debug" $Args

cd "$env:APPVEYOR_BUILD_FOLDER"
7z a libraries_$env:BUILDVARIANT.zip "$BuildDir\out"

cd $PrevPwd