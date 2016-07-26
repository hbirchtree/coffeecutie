# GLeamBaseTest_RHI
This is the sample code written in order to help the implementation of an RHI layer on top of the OpenGL wrapper headers.
A desired outcome is to have simple code that works on top of OpenGL 3.3, OpenGL 4.3, OpenGL 4.5, OpenGL ES 3.0 and OpenGL ES 3.2 backends.

As a result it will be quite simple to bring one codebase over to a different platform.


![GLeamBaseTest_RHI running on a Google Nexus 5X](https://github.com/hbirchtree/coffeecutie/blob/master/examples/gleam/rhi/base-test/android_23-nexus_5x.png?raw=true "Demo running on a Google Nexus 5X")
Running on Android Marshmallow, using the GLES 3.0 backend

![GLeamBaseTest_RHI running on Ubuntu](https://github.com/hbirchtree/coffeecutie/blob/master/examples/gleam/rhi/base-test/ubuntu-test.png?raw=true "Demo running on Ubuntu with NVIDIA graphics")
Running on Ubuntu 16.04, using the GL 3.3 backend

![GLeamBaseTest_RHI running on Ubuntu](https://github.com/hbirchtree/coffeecutie/blob/master/examples/gleam/rhi/base-test/windows_8.1-nvidia.png?raw=true "Demo running on Windows with NVIDIA graphics")
Running on Windows 8.1, using the GL 3.3 backend
