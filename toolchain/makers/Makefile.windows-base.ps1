param(
    [string] $CMakeBin = "cmake",
    [string] $BuildDir = "build_Windows-unknown"
    )

$NativeLibDir = "$Pwd\NativeLibs"

function DownloadANGLE {
    $VAR = (nuget install ANGLE.WindowsStore)
    ls $Pwd/ANGLE.* | % FullName
}

function CheckArch($arch) {
    if ($arch.Length -lt 1) {
        echo "x86"
    }else{
        echo $arch
    }
}

function yearToVsVersion($year)
{
    switch($year)
    {
        "2019" { echo "16" }
        "2017" { echo "15" }

        default { echo "14" }
    }
}

function ConfigProject([String] $SrcDir,[String] $arch,[String] $toolchain, `
					[String] $preload,[String] $LibVariant,[String] $ANGLEDir, `
					[String[]] $ExtraArgs, [String] $BuildDir) {

    $archfix = (CheckArch $arch)

    mkdir -Force $BuildDir
    cd $BuildDir

	#gci env:* | sort-object name

    if(-Not $env:AZURE_IMAGE -eq "")
    {
		echo "Azure Pipelines detected: $env:AZURE_IMAGE"
    } elseif(-Not $env:APPVEYOR_BUILD_WORKER_IMAGE -eq "")
    {
        echo "Appveyor detected: $env:APPVEYOR_BUILD_WORKER_IMAGE"
    }

    if($env:APPVEYOR_BUILD_WORKER_IMAGE -eq "Visual Studio 2017")
    {
        $Generator = "Visual Studio 15 2017 $arch"
    }elseif($env:APPVEYOR_BUILD_WORKER_IMAGE -eq "Visual Studio 2015")
    {
        $Generator = "Visual Studio 14 2015 $arch"
    }elseif($env:AZURE_IMAGE.substring(0, 2) -eq "vs")
    {
        $VsYear = $env:AZURE_IMAGE.substring(2, 4)
        $Generator = "Visual Studio " + (yearToVsVersion $VsYear) + " $VsYear"
    }
	elseif($env:AZURE_IMAGE.substring(0, 8) -eq "windows-")
	{
        $VsYear = $env:AZURE_IMAGE.substring(9, 4)
        $Generator = "Visual Studio" + (yearToVsVersion $VsYear) + " $VsYear"
	}

	$Generator = $Generator.Trim()

    if ( "$Generator" -eq "" )
    {
        echo "Using default generator"
        $GeneratorArg = ""
    }else {
        echo "Found generator $Generator from environment"
        $GeneratorArg = "-G$Generator"
    }

    & $CMakeBin $SrcDir `
        $GeneratorArg `
        -DCMAKE_TOOLCHAIN_FILE="$SrcDir/toolchain/cmake/Toolchains/$toolchain.toolchain.cmake" `
        -DANGLE_ROOT_DIR="$ANGLEDir" `
        -C"$SrcDir/toolchain/cmake/Preload/$preload.cmake" `
        -DCMAKE_INSTALL_PREFIX="$Pwd/Out" `
        $ExtraArgs
}

function CompileProject($preload, $arch, $config, $BuildDir) {
    $arch = (CheckArch $arch)

    & $CMakeBin --build $BuildDir `
        --target install --config $config
}

function TestProject($preload, $arch, $config, $BuildDir) {
    $arch = (CheckArch $arch)

    & $CMakeBin --build $BuildDir `
        --target RUN_TESTS --config $config
}
