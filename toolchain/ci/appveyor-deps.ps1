iwr https://chocolatey.org/install.ps1 -UseBasicParsing | iex
choco install --no-progress -y python3 nuget.commandline activeperl nasm

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
