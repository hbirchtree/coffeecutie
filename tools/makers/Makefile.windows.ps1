param(
	[string] $Target = "win32.amd64",
	[string] $SourceDir = "$Pwd\..",
	[switch] $Standalone = $false,
	[string] $CoffeeRoot = $Pwd,
	[string[]] $ExtraArgs = @(),
        [string] $Config = "Release",
        [string] $CMakeBin = "cmake"
	)
$ErrorActionPreference = "Stop"

$ScriptPath = (split-path -parent $MyInvocation.MyCommand.Definition)

. $ScriptPath\Makefile.windows-base.ps1 -CMakeBin "$CMakeBin"

$SrcDir = $sourcedir
$Toolchain = "win32"
$Preload = "generic"
$LibType = "Win32"
$Arch = "Win64"

switch ($target)
{
    "win32.amd64" {
        echo "W32/AMD64"
    }
    "win32.x86" {
        echo "W32/x86"
        $Arch = ""
    }

    "uwp.amd64" {
        echo "UWP/AMD64"
        $Toolchain = "uwp"
        $Preload = "uwp"
        $LibType = "WinUWP"
    }
    "uwp.x86" {
        echo "UWP/x86"
        $Toolchain = "uwp"
        $Preload = "uwp"
        $Arch = ""
        $LibType = "WinUWP"
    }
    "uwp.arm" {
        echo "UWP/ARM"
        $Toolchain = "uwp"
        $Preload = "uwp"
        $Arch = "ARM"
        $LibType = "WinUWP"
    }
    default {
        echo "Undefined build configuration"
        exit
    }
}

if ($Standalone) {
	echo "-- Building as standalone project"
	$ExtraArgs += ,"-DCOFFEE_ROOT_DIR=$CoffeeRoot"
}

$CurrentDir = $Pwd

ConfigProject "$SrcDir" `
    "$Arch" windows-"$Toolchain"_windows "windows-$Preload" `
    "$LibType" (DownloadAngle) $ExtraArgs

cd $CurrentDir

BuildProject "windows-$Preload" $Arch $Config

TestProject "windows-$Preload" $Arch $Config

#ConfigProject "C:\tmp\jenkins\Windows-UWP_x86-64_Dbg\src" `
#    "" "windows-win32_windows" "windows-generic" `
#    "Win32" (DownloadANGLE) ""
