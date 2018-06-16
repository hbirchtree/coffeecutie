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
   - Supports using libtiff to read TIFF images
   - Can be made configurable for spectrum of DXT1, DXT3 and DXT5
   - Configurable through `TEXCOOK_MAX_SIZE` and `TEXCOOK_MIN_SIZE`
 - Shader cross-compilation
   - Generates GLSL 3.30, 4.30, 4.60 and ESSL 1.00 and 3.00 source files
   - Generates OpenGL SPIR-V binary- shader.core.330.vert

## Details on extensions

### Texture data

The `PressurizeTextures` extension supports the following file formats:
 - PNG
 - JPG/JPEG
 - TGA
 - TIF/TIFF (if `libtiff` is available)

The following per-directory JSON configuration is supported:

```
{
    "max_size": <max mip size in pixels>,
    "min_size": <min mip size in pixels>,
    "channels": <1-4>,
    "formats": ["RAW", "DXT", "ETC", "ATC", "PVRTC"]
}
```

Configuration is put into a file named `<directory>/ALL.texture.json`.
Only the configuration for the current directory is used, no inheritance.

### Shaders

The `PressurizeShaders` extension recognizes the following file formats:
 - `.vert`
 - `.frag`
 - `.geom`
 - `.tesc`
 - `.tese`
 - `.comp`

The following per-file or per-directory JSON configuration is supported:

```
{
    "versions": [330, 430, 450, 460, 10200, 10300, 10320],
    "target": <redirected shader filename>
}
```

Configuration can be made directory-wide with `<directory>/ALL.shader.json`
 or per-file with `<file basename>.shader.json`.

## Accessing pressure-cooked data

Most extensions support

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
   - An `ASSIMP::MeshLoader::SerialNodeList` structure, can be used in-place. Contains matrices.

This acts like the data coming out of `BufferDescription` in `assimp_iterators.h`, without the Assimp dependency.
This also makes your application profilable with Valgrind (Assimp stops Valgrind in its tracks).

### Texture data

Loading textures becomes slightly more tricky:

 - Depending on the input format, textures might have the following extensions:
   - `.dxt1`, DXT1 compression
   - `.dxt5`, DXT5 compression
   - `.etc1`, ETC1 compression
   - `.etc2`, ETC2 compression, including all variants
   - `.png`/`.jpg`, as a fallback format
 - Mipmapping is done on the images

### Shaders

Using `RHI::GLEAM::ShaderResolver()`, you get a URL resolver which picks and appropriate source file.
The generated files for a given `shader.vert` become:

 - shader.core.330.vert
 - shader.core.430.vert
 - shader.es.100.vert
 - shader.es.300.vert
 - shader.vert.spv

The `UrlResolver` structure will handle this based on the API version provided.

The ESSL 1.00 shader is created using a custom SPIRV-Tools optimizer pass.
