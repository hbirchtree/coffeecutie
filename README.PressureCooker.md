# On the Pressure Cooker utility

The Pressure Cooker utility found in `tools/data-management/pressure-cooker` is meant only for development purposes. Its task is to allow creation of CfVirtFS-files without the need for custom-built utilities per-project. It has a very simple task:

To pack your files.

That's all.

It may be extended in several ways:

 - You could add the ability to dynamically load extensions through .so libraries, which could allow:
   + Automatic creation of Spir-V shader binaries for every shader
   + Automatic texture compression support, eg. transferring PNG, JPG and TGA (all of which supported by CImage) into S3TC, ASTC, PVRTC and etc.
   + Automatically processing models into file-mappable format
 - Adding other compression schemes, we are concerned with simple linkage and etc., and only use zlib and Windows' internal botched zlib. Both of these may be tuned for speedy decompression. You could also compress the CfVirtFS, but that would be extreme.
