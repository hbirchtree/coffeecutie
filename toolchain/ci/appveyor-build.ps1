$PrevPwd = $Pwd

mkdir $env:BUILD_DIR
cd $env:BUILD_DIR

function build_info(){
    python $env:SOURCE_DIR\toolchain\buildinfo.py $args
}


$LIBRARY_DIR = "$env:BUILD_DIR\libraries"
md -Force $LIBRARY_DIR

rm -r $LIBRARY_DIR

if ( "$env:APPVEYOR_BUILD_WORKER_IMAGE" -eq "" ) {
    $VCPP_VERSION = $env:AZURE_IMAGE.split("-")[0].substring(2)
} else {
    $VCPP_VERSION = $env:APPVEYOR_BUILD_WORKER_IMAGE.split(" ")[2]
}
$BUILDVARIANT_VERSION = "$VCPP_VERSION+$env:BUILDVARIANT"

ForEach($dep in $env:DEPENDENCIES -split ";") {
    if ($dep.Length -eq 0) {
        continue
    }
    try{
        $dep_version = (build_info dependencies $dep)

        $all_releases = (Invoke-RestMethod -Uri https://api.github.com/repos/$dep/releases -Headers @{"Accept" = "application/vnd.github.v3+json"; "Authorization" = "token $env:GITHUB_TOKEN"})

        echo "Searching for ${dep}:${dep_version}"

        ForEach($rel in $all_releases) {
             if($rel.tag_name -Eq $dep_version){
                 $release = $rel
            }
        }

        if($release) {
        } else {
             exit 1
        }

        echo "Found release for $dep"

        $dep = $dep.Replace("/", "_")

        $assets = $release.assets

        ForEach($asset in $assets) {
            if ($asset.name.Contains("$env:BUILDVARIANT_VERSION")) {
                $zipfile = $asset.name
                $dest_dir = $LIBRARY_DIR
                echo "Downloading $dep $env:BUILDVARIANT_VERSION library..."
                Invoke-WebRequest $asset.browser_download_url -UseBasicParsing -OutFile $zipfile
                echo "Successfully downloaded library for $dep!"

                echo "Extracting $dep from archive..."
                Expand-Archive -Path $zipfile -DestinationPath $dest_dir -Force
                #mv -Force "$LIBRARY_DIR\out\*" "$LIBRARY_DIR\"
                echo "Successfully extracted library $dep!"
            }
        }
    } catch {
        echo "Failed to download $dep"
        echo $_.Exception.Message
    }
}

switch -regex ($env:BUILDVARIANT)
{
    "^win32.x86$" {
        $Args = ("-DCOFFEE_BUILD_OPENSSL=OFF","-DCOFFEE_BUILD_OPENAL=OFF",`
                 "-DSKIP_HIGHMEM_TESTS=ON","-DSKIP_LINKAGE_TEST=ON", "-DCOFFEE_ROOT_DIR=$env:BUILD_DIR\libraries",`
	         "-DCOFFEE_BUILD_ASSIMP=OFF")
    }
    "^win32.amd64$" {
        $Args = ("-DCOFFEE_BUILD_OPENSSL=ON","-DCOFFEE_BUILD_OPENAL=OFF",`
                 "-DSKIP_HIGHMEM_TESTS=ON","-DSKIP_LINKAGE_TEST=ON", "-DCOFFEE_ROOT_DIR=$env:BUILD_DIR\libraries",`
	         "-DCOFFEE_BUILD_ASSIMP=ON")
    }
    "uwp.+" {
        $Args = ("-DCOFFEE_BUILD_OPENSSL=OFF","-DCOFFEE_BUILD_OPENAL=OFF",`
                 "-DSKIP_HIGHMEM_TESTS=ON","-DSKIP_LINKAGE_TEST=ON", "-DCOFFEE_ROOT_DIR=$env:BUILD_DIR\libraries",`
	         "-DCOFFEE_BUILD_ASSIMP=OFF", "-DCOFFEE_BUILD_ASIO=OFF")
    }
}

. $env:SOURCE_DIR\$env:MAKEFILE_DIR\Makefile.windows.ps1 `
    -CMakeBin $env:CMAKE_BIN -Standalone

if($env:SAME_BUILD_DIR) {
    $BuildDir = $env:BUILD_DIR
} else {
    $BuildDir = "$env:BUILD_DIR\build_$env:BUILDVARIANT"
}

echo "Starting CMake process"

echo $Args

BuildProject $env:BUILDVARIANT $env:SOURCE_DIR $BuildDir $LIBRARY_DIR "$env:CONFIGURATION" $Args

cd $PrevPwd
