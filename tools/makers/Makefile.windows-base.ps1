param(
    [string] $CMakeBin = "cmake",
    [string] $BuildDir = "build_Windows-unknown"
    )

$NativeLibDir = "$Pwd\NativeLibs"

function DownloadANGLE {
    $VAR = (nuget install ANGLE.WindowsStore)
    ls $Pwd/ANGLE.* | % FullName
}

function DownloadNativeLibs {
    if ( Test-Path $NativeLibDir) {
        try {
            git -C $NativeLibDir pull
        }
        catch {
        }
    }else{
        mkdir -Force $NativeLibDir
        try {
            git clone "https://github.com/hbirchtree/native-library-bundle.git" $NativeLibDir
        }
        catch {
        }
    }
}

function CheckArch($arch) {
    if ($arch.Length -lt 1) {
        echo "x86"
    }else{
        echo $arch
    }
}

function ConfigProject([String] $SrcDir,[String] $arch,[String] $toolchain, `
					[String] $preload,[String] $LibVariant,[String] $ANGLEDir, `
					[String[]] $ExtraArgs, [String] $BuildDir) {

    $archfix = (CheckArch $arch)

    mkdir -Force $BuildDir
    cd $BuildDir

    
    DownloadNativeLibs

    $Generator = "Visual Studio 14 2015 $arch".Trim()

	echo $ExtraArgs
    & $CMakeBin $SrcDir `
        -G"$Generator" `
        -DCMAKE_TOOLCHAIN_FILE="$SrcDir/cmake/Toolchains/$toolchain.toolchain.cmake" `
        -DANGLE_ROOT_DIR="$ANGLEDir" `
        -DNATIVE_LIBRARY_DIR="$NativeLibDir/$LibVariant" `
        -C"$SrcDir/cmake/Preload/$preload.cmake" `
        -DCMAKE_INSTALL_PREFIX="$Pwd/Out" `
        $ExtraArgs

    cd ..
}

function BuildProject($preload, $arch, $config, $BuildDir) {
    $arch = (CheckArch $arch)

    & $CMakeBin --build $BuildDir `
        --target install --config $config
}

function TestProject($preload, $arch, $config) {
    $arch = (CheckArch $arch)

    & $CMakeBin --build $Pwd/build_"$preload"_$arch `
        --target RUN_TESTS --config $config
}

#ConfigProject "C:\tmp\jenkins\Windows-UWP_x86-64_Dbg\src" `
#    "Win64" "windows-uwp_windows" "windows-uwp" `
#    "WinUWP" (DownloadAngle)
    
#ConfigProject "C:\tmp\jenkins\Windows-UWP_x86-64_Dbg\src" `
#    "Win64" "windows-win32_windows" "windows-generic" `
#    "Win32" (DownloadANGLE) ""
