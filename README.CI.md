# CI configuration

This project uses a set of Python scripts to generate
 `.travis.yml` for **Travis CI**, 
 `appveyor.yml` for **Appveyor CI**
 and a Groovy Job DSL for **Jenkins CI**.
These are not really configurable, but the YML-based formats will
 retain your customizations.

To update the CI configuration, you may run `./cb ci --overwrite-ci-files`,
 which will introduce configuration changes from the scripts
 (running without `--overwrite-ci-files` will not do anything for the YML files).

## Customizing the CI process

In order to allow customization, the following files may be created:

 - `./build-script.sh` - Full build script, replaces the default
 - `./deploy-script.sh` - Full deploy script, replaces the default
 - `./deploy-post-trigger.sh` - Runs after the default process

These are sourced into `toolchain/ci/travis-build.sh`.

**NB**: The Appveyor side of things does not have these whistles.

