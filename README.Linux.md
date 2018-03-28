# Building on Linux

For quick installation of dependencies, you can run the following command:

        # Core functionality
        apt install cmake ninja-build build-essential libopenal-dev libsdl2-dev libssl-dev libunwind-dev

        # Media extensions, language bindings, GUI tools
        apt install libavcodec-dev libavdevice-dev libavformat-dev libswresample-dev libswscale-dev swig3.0 qt5-default

        # Some Kinect 2-specific can be used with:
        apt install libpcl-dev

Equivalent packages will be needed for Fedora and similar.

The Linux port supports everything.
