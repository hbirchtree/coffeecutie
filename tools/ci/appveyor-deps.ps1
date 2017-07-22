git clone -q --recursive --branch=$env:APPVEYOR_REPO_BRANCH https://github.com/$env:APPVEYOR_REPO_NAME.git $env:APPVEYOR_BUILD_FOLDER
git checkout -qf %APPVEYOR_REPO_COMMIT%

Invoke-WebRequest https://www.libsdl.org/release/SDL2-devel-2.0.4-VC.zip -OutFile "$env:TEMP/SDL2.zip"
Expand-Archive -Force -Path "$env:TEMP\SDL2.zip" -DestinationPath "$env:TEMP/SDL2"
mv -ErrorAction Ignore -Force "$env:TEMP/SDL2/*" "C:\"
mv -ErrorAction Ignore -Force "C:\SDL2-2.0.4" "C:\SDL2"
rm -r -ErrorAction Ignore -Force "C:\SDL2\SDL2-2.0.4"

iwr https://chocolatey.org/install.ps1 -UseBasicParsing | iex
choco install -y cmake

ls C:\
ls "C:\Program Files"
ls "C:\Program Files (x86)"