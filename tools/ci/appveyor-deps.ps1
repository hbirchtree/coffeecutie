#git clone -q --recursive --branch=$env:APPVEYOR_REPO_BRANCH https://github.com/$env:APPVEYOR_REPO_NAME.git $env:APPVEYOR_BUILD_FOLDER
#git checkout -qf %APPVEYOR_REPO_COMMIT%

iwr https://chocolatey.org/install.ps1 -UseBasicParsing | iex
choco install -y cmake

