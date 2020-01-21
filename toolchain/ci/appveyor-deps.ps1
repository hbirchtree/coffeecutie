#git clone -q --recursive --branch=$env:APPVEYOR_REPO_BRANCH https://github.com/$env:APPVEYOR_REPO_NAME.git $env:APPVEYOR_BUILD_FOLDER
#git checkout -qf %APPVEYOR_REPO_COMMIT%

iwr https://chocolatey.org/install.ps1 -UseBasicParsing | iex
choco install --no-progress -y python3 nuget.commandline

refreshenv

$ErrorActionPreference = "Continue"
pip -q install pyyaml requests
$ErrorActionPreference = "Stop"

# We create our own OpenSSL library directory
# This can be used by FindOpenSSL.cmake
md $env:OPENSSL_ROOT_DIR\openssl-libs
md $env:OPENSSL_ROOT_DIR\openssl-libs\lib

nuget install openssl-vc141
pushd openssl-vc141*\build\native

echo "SSL Dir: $env:OPENSSL_ROOT_DIR"

Copy-Item -Recurse include $env:OPENSSL_ROOT_DIR
Copy-Item bin\*.lib $env:OPENSSL_ROOT_DIR\lib

popd
