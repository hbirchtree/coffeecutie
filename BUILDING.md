# How to build CoffeeCutie
 1. Create a build directory
    1. Create a sub-directory "libs"
    2. Configure CMakeLists.txt from "libs/" in this directory, build it!
    3. Configure and deploy ffmpeg to "libs/"
 2. Build the application, providing paths to Assimp, GLBinding and others if applicable.
 3. If all goes well, binaries are output to {build directory}/bin, readily linked and ready to go

Some applications have sample data, sourced from the sample_data directory in the root of the project directory. Some applications will crash without these files.

**NB**: Note that Windows is not fully supported or tested at all times.

**NB**: Note that Mac OS X is not supported (yet) and might not work at all.
