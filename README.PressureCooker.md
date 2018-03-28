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

## Existing extensions

 - Model compression
   - Extracts Assimp-compatible models and create `.vertices`, `.elements`, `.dcall`, `.draws`, `.attributes` and `.graph` for each file. Depending on size, models are compressed.
   - Automatically compresses element array losslessly (downcast to u16 and u8)
 - Texture compression (S3TC/DXT)
   - Compresses PNG, JPG and TGA into DXT5 using libsquish
   - Can be made configurable for spectrum of DXT1, DXT3 and DXT5

## Accessing pressure-cooked data

### Assimp data

Assimp data is split into:

 - vertices
   - Array buffer for direct upload
 - elements
   - Element array buffer for direct upload
 - dcall
   - A single `RHI::CGraphics_API::DrawCall` structure
 - draws
   - A list of `RHI::CGraphics_API::DrawInstanceData` structures
 - attributes
   - A list of `RHI::CGraphics_API::VertexAttribute` structures
 - graph
   - An `ASSIMP::MeshLoader::SerialNodeList` structure, can be used in-place

This acts like the data coming out of `BufferDescription` in `assimp_iterators.h`, without the Assimp dependency.
This also makes your application profilable with Valgrind (Assimp stops Valgrind in its tracks).

### Texture data (S3TC/DXTn)

Loading textures becomes slightly more tricky:

 - Depending on the input format, textures might have the following extensions:
   - `.dxt1`, DXT1 compression, for RGB textures (eg. JPG)
   - `.dxt5`, DXT5 compression, for RGBA textures (eg. PNG, TGA)
