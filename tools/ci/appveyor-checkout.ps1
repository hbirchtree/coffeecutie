$URL = "https://github.com/$env:APPVEYOR_REPO_NAME.git"

if($env:APPVEYOR_PULL_REQUEST_NUMBER.Length -Eq 0) {
    echo " * Doing normal checkout"
    & git clone -q --recursive --depth=30 --branch=$env:APPVEYOR_REPO_BRANCH $URL $env:SOURCE_DIR
} else {
    echo " * Doing pull request checkout"
    & git clone -q --recursive --depth=30 $URL $env:SOURCE_DIR
}

cd $env:SOURCE_DIR
git checkout -qf $env:APPVEYOR_REPO_COMMIT
