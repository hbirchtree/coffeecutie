$URL = "https://github.com/$env:APPVEYOR_REPO_NAME.git"

if((echo $env:APPVEYOR_PULL_REQUEST_NUMER) != "") {
    & git clone -q --recursive $URL
} else {
    & git clone -q --recursive --branch=$env:APPVEYOR_REPO_BRANCH $URL
}
