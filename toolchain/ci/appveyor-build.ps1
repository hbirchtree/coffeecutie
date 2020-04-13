function BuildProject()
{
    $Toolchain = "win32"
    $Preload = "generic"
    $LibType = "Win32"
    $Arch = "Win64"
    $CMakeArch = "x64"

    switch -regex ($env:BUILDVARIANT)
    {
        "win32.amd64.*" {
            echo "W32/AMD64"
        }
        "win32.x86.*" {
            echo "W32/x86"
            $Arch = ""
            $CMakeArch = "x86"
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
			$CMakeArch = "x86"
            $LibType = "WinUWP"
        }
        "uwp.arm.*" {
            echo "UWP/ARM"
            $Toolchain = "uwp"
            $Preload = "uwp"
            $Arch = "ARM"
			$CMakeArch = "ARM"
            $LibType = "WinUWP"
        }
        default {
            echo "Undefined build configuration: $Target"
            return
        }
    }

	$variant, $arch_, $version = $env:BUILDVARIANT.split(".")

	$GeneratorVersion = "-GVisual Studio 16 2019"
	$ArchArgument = "-A$Arch"

	if($version -Eq "vs19")
	{
		$ArchArgument = "-A$CMakeArch"
	}
	if($version -Eq "vs17")
	{
		$GeneratorVersion = "Visual Studio 15 2017 $Arch"
	}

	md -Force $env:BUILD_DIR
	pushd $env:BUILD_DIR

	if($variant -Eq "uwp")
	{
		nuget install ANGLE.WindowsStore
		$ANGLE_DIR = (ls $PWD/ANGLE.* | % FullName)
		echo "ANGLE found in: $ANGLE_DIR"
	} else
	{
		echo "Not downloading ANGLE for $variant"
	}

	echo "Beginning CMake"
	echo "-------------------------------------------"

	$ToolchainFile = "$env:SOURCE_DIR/toolchain/cmake/Toolchains/windows-" + $Toolchain + "_windows.toolchain.cmake"

    $env:PATH = $env:PATH + ";C:/Program Files/NASM/;"

	& cmake $env:SOURCE_DIR `
		-DCMAKE_TOOLCHAIN_FILE="$ToolchainFile" `
		-C"$env:SOURCE_DIR/toolchain/cmake/Preload/windows-$Preload.cmake" `
		$GeneratorVersion `
                $ArchArgument `
		-DCMAKE_BUILD_TYPE="$env:CONFIGURATION" `
		-DGIT_DEP_BUILDVARIANT="$env:BUILDVARIANT" `
		-DANGLE_ROOT_DIR="$ANGLE_DIR" `
		-DCMAKE_INSTALL_PREFIX="$env:BUILD_DIR/install"

	echo "-------------------------------------------"
	popd
}

BuildProject

