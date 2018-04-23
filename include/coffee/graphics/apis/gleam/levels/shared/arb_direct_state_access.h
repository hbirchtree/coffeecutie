#pragma once

#include "gl_shared_enum_convert.h"
#include "gl_shared_include.h"
#include <coffee/core/type_safety.h>
#include <coffee/core/types/cdef/memtypes.h>
#include <coffee/core/types/edef/pixenum.h>

namespace Coffee {
namespace CGL {

template<typename ReqVer>
struct CGL_DirectStateAccess
{
    /* Allocations, initializing variants */

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool TexAlloc(Texture tex_type, Span<CGhnd> const& handles)
    {
        glCreateTextures(
            to_enum(tex_type), C_FCAST<i32>(handles.elements), handles.data);
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool BufAlloc(Span<CGhnd> const& handles)
    {
        glCreateBuffers(C_FCAST<i32>(handles.elements), handles.data);
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool FBAlloc(Span<CGhnd> const& handles)
    {
        glCreateFramebuffers(C_FCAST<i32>(handles.elements), handles.data);
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool XFAlloc(Span<CGhnd> const& handles)
    {
        glCreateTransformFeedbacks(
            C_FCAST<i32>(handles.elements), handles.data);
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool RenderBufferAlloc(Span<CGhnd> const& handles)
    {
        glCreateRenderbuffers(C_FCAST<i32>(handles.elements), handles.data);
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool SamplerAlloc(Span<CGhnd> const& handles)
    {
        glCreateSamplers(C_FCAST<i32>(handles.elements), handles.data);
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool QueryAlloc(QueryT t, Span<CGhnd> const& handles)
    {
        glCreateQueries(
            to_enum(t), C_FCAST<i32>(handles.elements), handles.data);
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool VAOAlloc(Span<CGhnd> const& handles)
    {
        glCreateVertexArrays(C_FCAST<i32>(handles.elements), handles.data);
        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool PipelineAlloc(Span<CGhnd> const& handles)
    {
        glCreateProgramPipelines(C_FCAST<i32>(handles.elements), handles.data);
        return true;
    }

    /* Textures */
    GL_VERSION_REQ(GLVER_44)
    STATICINLINE bool TexStorage2D(
        CGhnd hnd, i32 levels, PixFmt ifmt, i32 w, i32 h)
    {
        glTextureStorage2D(hnd, levels, to_enum(ifmt), w, h);

        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool TexStorage3D(
        CGhnd hnd, i32 levels, PixFmt ifmt, i32 w, i32 h, i32 d)
    {
        glTextureStorage3D(hnd, levels, to_enum(ifmt), w, h, d);

        return true;
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void TexSubImage2D(
        CGhnd  hnd,
        i32    level,
        i32    x,
        i32    y,
        i32    w,
        i32    h,
        PixFmt fmt,
        BitFmt bfmt,
        c_cptr data)
    {
        glTextureSubImage2D(
            hnd, level, x, y, w, h, to_enum(fmt), to_enum(bfmt), data);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void TexSubImage3D(
        CGhnd  hnd,
        i32    level,
        i32    x,
        i32    y,
        i32    z,
        i32    w,
        i32    h,
        i32    d,
        PixFmt fmt,
        BitFmt bfmt,
        c_cptr data)
    {
        glTextureSubImage3D(
            hnd,
            level,
            x,
            y,
            z,
            w,
            h,
            d,
            to_enum(GetPixComponent(fmt), fmt),
            to_enum(bfmt),
            data);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void TexSubImageCompressed2D(
        CGhnd   hnd,
        i32     level,
        i32     x,
        i32     y,
        i32     w,
        i32     h,
        CompFmt cfmt,
        i32     data_size,
        c_cptr  data)
    {
        glCompressedTextureSubImage2D(
            hnd,
            level,
            x,
            y,
            w,
            h,
            to_enum(cfmt.base_fmt, cfmt.p_flags, cfmt.c_flags),
            data_size,
            data);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void TexSubImageCompressed3D(
        CGhnd   hnd,
        i32     level,
        i32     x,
        i32     y,
        i32     z,
        i32     w,
        i32     h,
        i32     d,
        CompFmt cfmt,
        i32     data_size,
        c_cptr  data)
    {
        glCompressedTextureSubImage3D(
            hnd,
            level,
            x,
            y,
            z,
            w,
            h,
            d,
            to_enum(cfmt.base_fmt, cfmt.p_flags, cfmt.c_flags),
            data_size,
            data);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void TexCopySubImage2D(
        CGhnd hnd,
        i32   level,
        i32   trgX,
        i32   trgY,
        i32   srcX,
        i32   srcY,
        i32   w,
        i32   h)
    {
        glCopyTextureSubImage2D(hnd, level, trgX, trgY, srcX, srcY, w, h);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void TexGetImage(
        CGhnd hnd, i32 level, PixFmt fmt, BitFmt bfmt, Bytes& data)
    {
        glGetTextureImage(
            hnd,
            level,
            to_enum(fmt),
            to_enum(bfmt),
            C_FCAST<i32>(data.size),
            data.data);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void TexGenMipmap(CGhnd t)
    {
        glGenerateTextureMipmap(t);
    }

    GL_VERSION_REQ(GLVER_44)
    STATICINLINE void ImageBindTextures(u32 f, Span<CGhnd> const& handles)
    {
        glBindImageTextures(f, C_FCAST<i32>(handles.size), handles.data);
    }

    /* Buffers */
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void BufStorage(CGhnd h, Bytes const& data, RSCA storage)
    {
        glNamedBufferStorage(
            h, C_FCAST<i64>(data.size), data.data, to_enum2(storage));
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void BufData(CGhnd hnd, Bytes const& data, RSCA acc)
    {
        glNamedBufferData(
            hnd, C_FCAST<i32>(data.size), data.data, to_enum1(acc));
    }
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void BufSubData(CGhnd hnd, szptr offset, Bytes const& data)
    {
        glNamedBufferSubData(
            hnd, C_FCAST<i32>(offset), C_FCAST<i32>(data.size), data.data);
    }
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void BufCopySubData(
        CGhnd  readHnd,
        CGhnd  writeHnd,
        ptroff readOff,
        ptroff writeOff,
        ptroff size)
    {
        glCopyNamedBufferSubData(readHnd, writeHnd, readOff, writeOff, size);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void BufBindRanges(
        BufType             type,
        u32                 first,
        Span<CGhnd> const&  handles,
        Span<ptroff> const& offsets,
        Span<ptroff> const& sizes)
    {
#if !defined(NDEBUG)
        if(handles.size != offsets.size || handles.size != sizes.size)
            throw undefined_behavior("invalid buffer bind");
#endif

        glBindBuffersRange(
            to_enum(type),
            first,
            C_FCAST<i32>(handles.size),
            handles.data,
            offsets.data,
            sizes.data);
    }
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void* BufMapRange(
        CGhnd hnd, ptroff off, ptroff size, RSCA access)
    {
        return glMapNamedBufferRange(hnd, off, size, to_enum2(access));
    }
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void BufUnmap(CGhnd h)
    {
        glUnmapNamedBuffer(h);
    }

    /* FB */
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE bool FBValidate(CGhnd)
    {
        return false;
    }

    /* VAO */
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void VAOAttribFormat(
        CGhnd hnd, u32 idx, i32 size, TypeEnum type, bool norm, u32 offset)
    {
        glVertexArrayAttribFormat(hnd, idx, size, to_enum(type), norm, offset);
    }
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void VAOAttribFormatI(
        CGhnd hnd, u32 idx, i32 size, TypeEnum type, u32 offset)
    {
        glVertexArrayAttribIFormat(hnd, idx, size, to_enum(type), offset);
    }
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void VAOAttribFormatL(
        CGhnd hnd, u32 idx, i32 size, TypeEnum type, u32 offset)
    {
        glVertexArrayAttribLFormat(hnd, idx, size, to_enum(type), offset);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void VAOBindingDivisor(CGhnd hnd, u32 binding, u32 divisor)
    {
        glVertexArrayBindingDivisor(hnd, binding, divisor);
    }
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void VAOAttribBinding(CGhnd hnd, u32 idx, u32 binding)
    {
        glVertexArrayAttribBinding(hnd, idx, binding);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void VAOElementBuffer(CGhnd hnd, CGhnd bufHnd)
    {
        glVertexArrayElementBuffer(hnd, bufHnd);
    }

    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void VAOBindVertexBuffer(
        CGhnd hnd, u32 bind, CGhnd bufHnd, ptroff offset, i32 size)
    {
        glVertexArrayVertexBuffer(hnd, bind, bufHnd, offset, size);
    }
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void VAOBindVertexBuffers(
        CGhnd               hnd,
        u32                 first,
        Span<CGhnd> const&  buffers,
        Span<ptroff> const& offsets,
        Span<i32> const&    strides)
    {
        if(buffers.size != offsets.size || buffers.size != strides.size)
            throw undefined_behavior("invalid buffer bind");

        glVertexArrayVertexBuffers(
            hnd,
            first,
            C_FCAST<i32>(buffers.size),
            buffers.data,
            offsets.data,
            strides.data);
    }

    /* Queries */
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void QueryBufferGetiv(CGhnd, CGhnd, CGenum, i64)
    {
    }
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void QueryBufferGeti64v(CGhnd, CGhnd, CGenum, i64)
    {
    }
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void QueryBufferGetui64v(CGhnd, CGhnd, CGenum, i64)
    {
    }
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void QueryBufferGetuiv(CGhnd, CGhnd, CGenum, i64)
    {
    }
};

} // namespace CGL
} // namespace Coffee
