# Support for Snappy packages

It's ghetto, but it kind of works for single-arch builds
Multi-arch builds might be done with one central snapcraft.yaml file

## What does not work

 - OpenAL:

        AL lib: (WW) alc_initconfig: Failed to initialize backend "pulse"
        AL lib: (WW) alc_initconfig: Failed to initialize backend "alsa"
        AL lib: (EE) ALCplaybackOSS_open: Could not open /dev/dsp: No such file or directory

 - Multiple GPU driver configurations (eg. Intel + NVIDIA)

        multiple nvidia drivers detected, this is not supported

