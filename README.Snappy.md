# Support for Snappy packages

It's ghetto, but it kind of works for single-arch builds

Multi-arch builds might be done with one central snapcraft.yaml file.
This will probably pop up in the coffeecutie-meta repository.
This would most likely include a bog-standard armeabi-v7a build,
arm64-v8a build and amd64 build.

One problem with Snap for the time being is the permission system,
which may be tricky to work with, but it is otherwise a fine way of
securing your system.

I also like the automatic dependency inclusion,
which may be a pain for larger dependencies (eg. CEF), but is bearable.

Debugging Snappy issues can be quite annoying, especially when
coredumps don't appear when they should.

## What does not work

 - OpenAL:

        AL lib: (WW) alc_initconfig: Failed to initialize backend "pulse"
        AL lib: (WW) alc_initconfig: Failed to initialize backend "alsa"
        AL lib: (EE) ALCplaybackOSS_open: Could not open /dev/dsp: No such file or directory

 - Multiple GPU driver configurations (eg. Intel + NVIDIA)

        multiple nvidia drivers detected, this is not supported
