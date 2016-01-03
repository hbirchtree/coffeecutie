#ifndef COFFEE_GRAPHICS_APIS_RHI_GRAPHICS_API_H
#define COFFEE_GRAPHICS_APIS_RHI_GRAPHICS_API_H

namespace Coffee{
namespace CRHI{

struct CGraphicsAPI
{
    struct VertexDescription;

    struct VertexBuffer;
    struct IndexBuffer;
    struct UniformBuffer;
    struct ShaderBuffer;

    struct IndirectBuffer;

    struct Program;
    struct Shader;

    struct OccludeQuery;
    struct PerfQuery;

    struct Texture2D;
    struct VolumeTexture;

    struct Texture2DArray;
    struct VolumeTextureArray;

    struct RenderView; /* Contains framebuffer and viewport information for resizing */

    void cgAlloc(); /* Allocates n objects */
    void cgFree();

    void cgClearBuffer();

    void cgViewportResize();
    void cgScissorResize();
    void cgDepthResize();

    void cgBufferStorage(); /* For non-DSA, use hidden state tracking */
    void cgBufferUpload();
    void cgBufferMap();
    void cgBufferUnmap();
    void cgBufferBind(); /* Binds buffer to binding point/index */

    void cgTextureStorage();
    void cgTextureClear(); /* Clears texture data to value */
    void cgTextureUpload();
    void cgTextureDownload();
    void cgTextureMipmap();
    void cgTextureSetParam();

    void cgTextureImport(); /* Import from binary format */
    void cgTextureDump(); /* Dump texture to data location, should report data size when nullptr */

    void cgSamplerTexture(); /* Attaches texture to sampler */
    void cgSamplerMakeResident(); /* For non-bindless, bind and set active texture, alternative for sparse textures */
    void cgSamplerMakeNonResident();
    void cgSamplerSetParam();
    void cgSamplerGetParam();

    void cgFramebufferDefault();

    void cgFramebufferBlit();
    void cgFramebufferTexture();
    void cgFramebufferRenderbuffer();

    void cgSync();
    void cgSyncAwait();
    void cgSyncCheck();

    void cgMemoryBarrier();

    void cgProgramImport();
    void cgProgramExport();

    void cgProgramGetData();

    void cgProgramAttribBind();
    void cgProgramUBlockBind();
    void cgProgramUniform();

    void cgPipelineBind();

    void cgShaderBind();
    void cgShaderCompile();

    void cgOccludeQueryBegin();
    void cgOccludeQueryEnd();
};

}
}

#endif
