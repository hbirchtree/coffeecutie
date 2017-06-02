$ErrorActionPreference = "Stop"

. $Pwd\Makefile.windows-base.ps1

$SrcDir = $args[0]
$Toolchain = "win32"
$Preload = "generic"
$LibType = "Win32"
$Arch = "Win64"

switch ($args[1])
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

ConfigProject "$SrcDir" `
    "$Arch" windows-"$Toolchain"_windows "windows-$Preload" `
    "$LibType" (DownloadAngle)

BuildProject "windows-$Preload" "$Arch" "Release"

TestProject "windows-$Preload" "$Arch" "Release"

#ConfigProject "C:\tmp\jenkins\Windows-UWP_x86-64_Dbg\src" `
#    "" "windows-win32_windows" "windows-generic" `
#    "Win32" (DownloadANGLE) ""