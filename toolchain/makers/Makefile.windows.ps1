param(
    [switch] $Standalone,
    [string] $CMakeBin = "cmake",
    [switch] $ConfigureOnly
	)

try {
    $ScriptPath = (split-path -parent $MyInvocation.MyCommand.Definition)

    . $ScriptPath\Makefile.windows-base.ps1 -CMakeBin "$CMakeBin"
}
catch {
    . $env:APPVEYOR_BUILD_FOLDER\$env:MAKEFILE_DIR\Makefile.windows-base.ps1 -CMakeBin "$CMakeBin"
}

function BuildProject($Target, $SourceDir,
                      $BuildDir,
                      $CoffeeRoot, $Config,
                      $ExtraArgs)
{
    $SrcDir = $sourcedir
    $Toolchain = "win32"
    $Preload = "generic"
    $LibType = "Win32"
    $Arch = "Win64"

    switch -regex ($Target)
    {
        "win32.amd64.*" {
            echo "W32/AMD64"
        }
        "win32.x86.*" {
            echo "W32/x86"
            $Arch = ""
        }
        "uwp.amd64.*" {
            echo "UWP/AMD64"
            $Toolchain = "uwp"
            $Preload = "uwp"
            $LibType = "WinUWP"
        }
        "uwp.x86.*" {
            echo "UWP/x86"
            $Toolchain = "uwp"
            $Preload = "uwp"
            $Arch = ""
            $LibType = "WinUWP"
        }
        "uwp.arm.*" {
            echo "UWP/ARM"
            $Toolchain = "uwp"
            $Preload = "uwp"
            $Arch = "ARM"
            $LibType = "WinUWP"
        }
        default {
            echo "Undefined build configuration: $Target"
            return
        }
    }

    if ($Standalone) {
	    echo "-- Building as standalone project"
	    $ExtraArgs += ,"-DCOFFEE_ROOT_DIR=$CoffeeRoot"
    }

    $ExtraArgs += ,"-DCMAKE_INSTALL_PREFIX=out"

    $CurrentDir = $Pwd

    ConfigProject "$SrcDir" `
        "$Arch" windows-"$Toolchain"_windows "windows-$Preload" `
        "$LibType" (DownloadAngle) $ExtraArgs $BuildDir

    cd $CurrentDir

    if ($ConfigureOnly -Or $env:NOBUILD) {
        echo " * Only configuring project"
    }else{
        CompileProject "windows-$Preload" $Arch $Config $BuildDir

        # TestProject "windows-$Preload" $Arch $Config $BuildDir
    }
}
