#pragma once

#include "gl_shared_enum_convert.h"

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/cdef/memtypes.h>
#include <coffee/core/types/cdef/geometry.h>
#include <coffee/core/types/vector_types.h>

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-bool-conversion"
#endif

namespace Coffee{
namespace CGL{
#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
template<typename ReqVer>
struct CGL_10
{

#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void BlendFunc(GLenum sfactor, GLenum dfactor)
{
#ifndef NDEBUG
    if(!glBlendFunc)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendFunc(sfactor, dfactor);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Clear(u32 mask)
{
#ifndef NDEBUG
    if(!glClear)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClear(mask);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void ClearColor(_cbasic_vec4<scalar> const& color)
{
#ifndef NDEBUG
    if(!glClearColor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearColor(color.r(), color.g(), color.b(), color.a());
}
#endif


#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_10)
STATICINLINE void ClearDepth(bigscalar depth)
{
#ifndef NDEBUG
    if(!glClearDepth)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearDepth(depth);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void ClearStencil(i32 s)
{
#ifndef NDEBUG
    if(!glClearStencil)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearStencil(s);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void ColorMask(_cbasic_vec4<u8> const& color)
{
#ifndef NDEBUG
    if(!glColorMask)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glColorMask(color.r(), color.g(), color.b(), color.a());
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void CullFace(Face mode)
{
#ifndef NDEBUG
    if(!glCullFace)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCullFace(to_enum(mode));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void DepthFunc(GLenum func)
{
#ifndef NDEBUG
    if(!glDepthFunc)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthFunc(func);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void DepthMask(bool flag)
{
#ifndef NDEBUG
    if(!glDepthMask)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthMask(flag ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_10)
STATICINLINE void DepthRange(bigscalar n, bigscalar f)
{
#ifndef NDEBUG
    if(!glDepthRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthRange(n, f);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Disable(Feature cap)
{
#ifndef NDEBUG
    if(!glDisable)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDisable(to_enum(cap));
}
#endif


#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_10)
STATICINLINE void DrawBuffer(GLenum buf)
{
#ifndef NDEBUG
    if(!glDrawBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawBuffer(buf);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Enable(Feature cap)
{
#ifndef NDEBUG
    if(!glEnable)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEnable(to_enum(cap));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Finish()
{
#ifndef NDEBUG
    if(!glFinish)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFinish();
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Flush()
{
#ifndef NDEBUG
    if(!glFlush)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFlush();
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void FrontFace(GLenum mode)
{
#ifndef NDEBUG
    if(!glFrontFace)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFrontFace(mode);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void BoolGetv(GLenum pname, u8 * data)
{
#ifndef NDEBUG
    if(!glGetBooleanv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetBooleanv(pname, data);
}
#endif


#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_10)
STATICINLINE void ScalarGetv(GLenum pname, bigscalar * data)
{
#ifndef NDEBUG
    if(!glGetDoublev)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetDoublev(pname, data);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE GLenum ErrorGet()
{
#ifndef NDEBUG
    if(!glGetError)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetError();
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void ScalarfGetv(GLenum pname, scalar * data)
{
#ifndef NDEBUG
    if(!glGetFloatv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetFloatv(pname, data);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void IntegerGetv(GLenum pname, i32 * data)
{
#ifndef NDEBUG
    if(!glGetIntegerv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetIntegerv(pname, data);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE const u8 * GetString(GLenum name)
{
#ifndef NDEBUG
    if(!glGetString)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetString(name);
}
#endif


#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_10)
STATICINLINE void TexGetImage(TexComp::tex_flag const& target, i32 level, PixCmp format, BitFmt type, void * pixels)
{
#ifndef NDEBUG
    if(!glGetTexImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexImage(texture_to_enum(target), level, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x310)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_31)
STATICINLINE void TexGetLevelParameterfv(TexComp::tex_flag const& target, i32 level, GLenum pname, scalar * params)
{
#ifndef NDEBUG
    if(!glGetTexLevelParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexLevelParameterfv(texture_to_enum(target), level, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x310)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_31)
STATICINLINE void TexGetLevelParameteriv(TexComp::tex_flag const& target, i32 level, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetTexLevelParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexLevelParameteriv(texture_to_enum(target), level, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexGetParameterfv(TexComp::tex_flag const& target, GLenum pname, scalar * params)
{
#ifndef NDEBUG
    if(!glGetTexParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexParameterfv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexGetParameteriv(TexComp::tex_flag const& target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetTexParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexParameteriv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Hint(GLenum target, GLenum mode)
{
#ifndef NDEBUG
    if(!glHint)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glHint(target, mode);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE u8 IsEnabled(Feature cap)
{
#ifndef NDEBUG
    if(!glIsEnabled)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsEnabled(to_enum(cap));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void LineWidth(scalar width)
{
#ifndef NDEBUG
    if(!glLineWidth)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glLineWidth(width);
}
#endif


#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_10)
STATICINLINE void LogicOp(GLenum opcode)
{
#ifndef NDEBUG
    if(!glLogicOp)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glLogicOp(opcode);
}
#endif


#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_10)
STATICINLINE void PixelStoref(GLenum pname, scalar param)
{
#ifndef NDEBUG
    if(!glPixelStoref)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPixelStoref(pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void PixelStorei(GLenum pname, i32 param)
{
#ifndef NDEBUG
    if(!glPixelStorei)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPixelStorei(pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_10)
STATICINLINE void PointSize(scalar size)
{
#ifndef NDEBUG
    if(!glPointSize)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPointSize(size);
}
#endif


#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_10)
STATICINLINE void PolygonMode(Face face, DrawMode mode)
{
#ifndef NDEBUG
    if(!glPolygonMode)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPolygonMode(to_enum(face), to_enum(mode));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x300)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_30)
STATICINLINE void BufRead(GLenum src)
{
#ifndef NDEBUG
    if(!glReadBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReadBuffer(src);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void ReadPixels(i32 x, i32 y, Size const& size, PixCmp format, BitFmt type, void * pixels)
{
#ifndef NDEBUG
    if(!glReadPixels)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReadPixels(x, y, size.w, size.h, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Scissor(i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glScissor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glScissor(x, y, size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void StencilFunc(GLenum func, i32 ref, u32 mask)
{
#ifndef NDEBUG
    if(!glStencilFunc)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glStencilFunc(func, ref, mask);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void StencilMask(u32 mask)
{
#ifndef NDEBUG
    if(!glStencilMask)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glStencilMask(mask);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void StencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
#ifndef NDEBUG
    if(!glStencilOp)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glStencilOp(fail, zfail, zpass);
}
#endif


#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_10)
STATICINLINE void TexImage1D(TexComp::tex_flag const& target, i32 level, PixFmt internalformat, i32 width, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage1D(texture_to_enum(target), level, to_enum(internalformat), width, 0, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexImage2D(TexComp::tex_flag const& target, i32 level, PixFmt internalformat, Size const& size, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage2D(texture_to_enum(target), level, to_enum(internalformat), size.w, size.h, 0, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexParameterf(TexComp::tex_flag const& target, GLenum pname, scalar param)
{
#ifndef NDEBUG
    if(!glTexParameterf)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexParameterf(texture_to_enum(target), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexParameterfv(TexComp::tex_flag const& target, GLenum pname, const scalar * params)
{
#ifndef NDEBUG
    if(!glTexParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexParameterfv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexParameteri(TexComp::tex_flag const& target, GLenum pname, i32 param)
{
#ifndef NDEBUG
    if(!glTexParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexParameteri(texture_to_enum(target), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexParameteriv(TexComp::tex_flag const& target, GLenum pname, const i32 * params)
{
#ifndef NDEBUG
    if(!glTexParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexParameteriv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Viewport(i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glViewport)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glViewport(x, y, size.w, size.h);
}
#endif

};
template<typename ReqVer>
struct CGL_11
    : CGL_10<ReqVer>
{
using Parent = CGL_10<ReqVer>;

#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexBind(TexComp::tex_flag const& target, glhnd const& texture)
{
#ifndef NDEBUG
    if(!glBindTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindTexture(texture_to_enum(target), C_OCAST<u32>(texture));
}
#endif


#if GL_VERSION_VERIFY(0x110, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_11)
STATICINLINE void TexCopyImage1D(TexComp::tex_flag const& target, i32 level, PixFmt internalformat, i32 x, i32 y, i32 width)
{
#ifndef NDEBUG
    if(!glCopyTexImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexImage1D(texture_to_enum(target), level, to_enum(internalformat), x, y, width, 0);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexCopyImage2D(TexComp::tex_flag const& target, i32 level, PixFmt internalformat, i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glCopyTexImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexImage2D(texture_to_enum(target), level, to_enum(internalformat), x, y, size.w, size.h, 0);
}
#endif


#if GL_VERSION_VERIFY(0x110, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_11)
STATICINLINE void TexCopySubImage1D(TexComp::tex_flag const& target, i32 level, i32 xoffset, i32 x, i32 y, i32 width)
{
#ifndef NDEBUG
    if(!glCopyTexSubImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexSubImage1D(texture_to_enum(target), level, xoffset, x, y, width);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexCopySubImage2D(TexComp::tex_flag const& target, i32 level, Point const& offset, i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glCopyTexSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexSubImage2D(texture_to_enum(target), level, offset.x, offset.y, x, y, size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexFree(Span<const u32> const& textures)
{
#ifndef NDEBUG
    if(!glDeleteTextures)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteTextures(C_FCAST<i32>(textures.elements), textures.data);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void DrawArrays(DrwMd const& mode, i32 first, i32 count)
{
#ifndef NDEBUG
    if(!glDrawArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawArrays(to_enum(mode.t, mode.c), first, count);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void DrawElements(DrwMd const& mode, i32 count, TypeEnum type, uintptr indices)
{
#ifndef NDEBUG
    if(!glDrawElements)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElements(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void *>(indices));
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexAlloc(Span<u32> const& textures)
{
#ifndef NDEBUG
    if(!glGenTextures)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenTextures(C_FCAST<i32>(textures.elements), textures.data);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE u8 IsTexture(glhnd const& texture)
{
#ifndef NDEBUG
    if(!glIsTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsTexture(C_OCAST<u32>(texture));
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void PolygonOffset(scalar factor, scalar units)
{
#ifndef NDEBUG
    if(!glPolygonOffset)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPolygonOffset(factor, units);
}
#endif


#if GL_VERSION_VERIFY(0x110, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_11)
STATICINLINE void TexSubImage1D(TexComp::tex_flag const& target, i32 level, i32 xoffset, i32 width, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexSubImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexSubImage1D(texture_to_enum(target), level, xoffset, width, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexSubImage2D(TexComp::tex_flag const& target, i32 level, Point const& offset, Size const& size, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexSubImage2D(texture_to_enum(target), level, offset.x, offset.y, size.w, size.h, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif

};
template<typename ReqVer>
struct CGL_12
    : CGL_11<ReqVer>
{
using Parent = CGL_11<ReqVer>;

#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void TexCopySubImage3D(TexComp::tex_flag const& target, i32 level, Point3 const& offset, i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glCopyTexSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexSubImage3D(texture_to_enum(target), level, offset.x, offset.y, offset.z, x, y, size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void DrawRangeElements(DrwMd const& mode, u32 start, u32 end, i32 count, TypeEnum type, uintptr indices)
{
#ifndef NDEBUG
    if(!glDrawRangeElements)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawRangeElements(to_enum(mode.t, mode.c), start, end, count, to_enum(type), C_RCAST<const void *>(indices));
}
#endif


#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void TexImage3D(TexComp::tex_flag const& target, i32 level, PixFmt internalformat, Size3 const& size, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage3D(texture_to_enum(target), level, to_enum(internalformat), size.width, size.height, size.depth, 0, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void TexSubImage3D(TexComp::tex_flag const& target, i32 level, Point3 const& offset, Size3 const& size, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexSubImage3D(texture_to_enum(target), level, offset.x, offset.y, offset.z, size.width, size.height, size.depth, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif

};
template<typename ReqVer>
struct CGL_13
    : CGL_12<ReqVer>
{
using Parent = CGL_12<ReqVer>;

#if GL_VERSION_VERIFY(0x130, 0x200)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_20)
STATICINLINE void TexActive(GLenum texture)
{
#ifndef NDEBUG
    if(!glActiveTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glActiveTexture(texture);
}
#endif


#if GL_VERSION_VERIFY(0x130, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_13)
STATICINLINE void TexCompressedImage1D(TexComp::tex_flag const& target, i32 level, CompFmt internalformat, i32 width, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexImage1D)
    {
#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
        if(glCompressedTexImage1DARB) Throw(undefined_behavior("extension GL_ARB_texture_compression is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexImage1D(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), width, 0, imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x200)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_20)
STATICINLINE void TexCompressedImage2D(TexComp::tex_flag const& target, i32 level, CompFmt internalformat, Size const& size, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexImage2D)
    {
#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
        if(glCompressedTexImage2DARB) Throw(undefined_behavior("extension GL_ARB_texture_compression is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexImage2D(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), size.w, size.h, 0, imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x300)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_30)
STATICINLINE void TexCompressedImage3D(TexComp::tex_flag const& target, i32 level, CompFmt internalformat, Size3 const& size, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexImage3D)
    {
#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
        if(glCompressedTexImage3DARB) Throw(undefined_behavior("extension GL_ARB_texture_compression is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexImage3D(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), size.width, size.height, size.depth, 0, imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_13)
STATICINLINE void TexCompressedSubImage1D(TexComp::tex_flag const& target, i32 level, i32 xoffset, i32 width, CompFmt format, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexSubImage1D)
    {
#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
        if(glCompressedTexSubImage1DARB) Throw(undefined_behavior("extension GL_ARB_texture_compression is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexSubImage1D(texture_to_enum(target), level, xoffset, width, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x200)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_20)
STATICINLINE void TexCompressedSubImage2D(TexComp::tex_flag const& target, i32 level, Point const& offset, Size const& size, CompFmt format, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexSubImage2D)
    {
#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
        if(glCompressedTexSubImage2DARB) Throw(undefined_behavior("extension GL_ARB_texture_compression is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexSubImage2D(texture_to_enum(target), level, offset.x, offset.y, size.w, size.h, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x300)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_30)
STATICINLINE void TexCompressedSubImage3D(TexComp::tex_flag const& target, i32 level, Point3 const& offset, Size3 const& size, CompFmt format, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexSubImage3D)
    {
#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
        if(glCompressedTexSubImage3DARB) Throw(undefined_behavior("extension GL_ARB_texture_compression is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexSubImage3D(texture_to_enum(target), level, offset.x, offset.y, offset.z, size.width, size.height, size.depth, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_13)
STATICINLINE void TexGetCompressedImage(TexComp::tex_flag const& target, i32 level, void * img)
{
#ifndef NDEBUG
    if(!glGetCompressedTexImage)
    {
#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
        if(glGetCompressedTexImageARB) Throw(undefined_behavior("extension GL_ARB_texture_compression is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetCompressedTexImage(texture_to_enum(target), level, img);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x200)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_20)
STATICINLINE void SampleCoverage(scalar value, bool invert)
{
#ifndef NDEBUG
    if(!glSampleCoverage)
    {
#if defined(GL_ARB_multisample) && GL_ARB_multisample
        if(glSampleCoverageARB) Throw(undefined_behavior("extension GL_ARB_multisample is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSampleCoverage(value, invert ? GL_TRUE : GL_FALSE);
}
#endif

};
template<typename ReqVer>
struct CGL_14
    : CGL_13<ReqVer>
{
using Parent = CGL_13<ReqVer>;

#if GL_VERSION_VERIFY(0x140, 0x200)
GL_VERSION_REQ_COMBO(GLVER_14, GLESVER_20)
STATICINLINE void BlendColor(_cbasic_vec4<scalar> const& color)
{
#ifndef NDEBUG
    if(!glBlendColor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendColor(color.r(), color.g(), color.b(), color.a());
}
#endif


#if GL_VERSION_VERIFY(0x140, 0x200)
GL_VERSION_REQ_COMBO(GLVER_14, GLESVER_20)
STATICINLINE void BlendEquation(GLenum mode)
{
#ifndef NDEBUG
    if(!glBlendEquation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendEquation(mode);
}
#endif


#if GL_VERSION_VERIFY(0x140, 0x200)
GL_VERSION_REQ_COMBO(GLVER_14, GLESVER_20)
STATICINLINE void BlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
#ifndef NDEBUG
    if(!glBlendFuncSeparate)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}
#endif


#if GL_VERSION_VERIFY(0x140, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_14)
STATICINLINE void MultiDrawArrays(DrwMd const& mode, const i32 * first, const i32 * count, i32 drawcount)
{
#ifndef NDEBUG
    if(!glMultiDrawArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMultiDrawArrays(to_enum(mode.t, mode.c), first, count, drawcount);
}
#endif


#if GL_VERSION_VERIFY(0x140, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_14)
STATICINLINE void MultiDrawElements(DrwMd const& mode, const i32 * count, TypeEnum type, uintptr indices, i32 drawcount)
{
#ifndef NDEBUG
    if(!glMultiDrawElements)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMultiDrawElements(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void * *const>(indices), drawcount);
}
#endif


#if GL_VERSION_VERIFY(0x140, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_14)
STATICINLINE void PointParameterf(GLenum pname, scalar param)
{
#ifndef NDEBUG
    if(!glPointParameterf)
    {
#if defined(GL_ARB_point_parameters) && GL_ARB_point_parameters
        if(glPointParameterfARB) Throw(undefined_behavior("extension GL_ARB_point_parameters is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPointParameterf(pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x140, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_14)
STATICINLINE void PointParameterfv(GLenum pname, const scalar * params)
{
#ifndef NDEBUG
    if(!glPointParameterfv)
    {
#if defined(GL_ARB_point_parameters) && GL_ARB_point_parameters
        if(glPointParameterfvARB) Throw(undefined_behavior("extension GL_ARB_point_parameters is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPointParameterfv(pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x140, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_14)
STATICINLINE void PointParameteri(GLenum pname, i32 param)
{
#ifndef NDEBUG
    if(!glPointParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPointParameteri(pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x140, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_14)
STATICINLINE void PointParameteriv(GLenum pname, const i32 * params)
{
#ifndef NDEBUG
    if(!glPointParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPointParameteriv(pname, params);
}
#endif

};
template<typename ReqVer>
struct CGL_15
    : CGL_14<ReqVer>
{
using Parent = CGL_14<ReqVer>;

#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryBegin(QueryComp::query_flag const& target, u32 id)
{
#ifndef NDEBUG
    if(!glBeginQuery)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glBeginQueryARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBeginQuery(query_to_enum(target), id);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufBind(BufferComp::buf_flag const& target, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glBindBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindBuffer(buffer_to_enum(target), C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufData(BufferComp::buf_flag const& target, BytesConst const& data, RSCA usage)
{
#ifndef NDEBUG
    if(!glBufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBufferData(buffer_to_enum(target), C_FCAST<ptroff>(data.size), data.data, to_enum1(usage));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufSubData(BufferComp::buf_flag const& target, ptroff offset, BytesConst const& data)
{
#ifndef NDEBUG
    if(!glBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBufferSubData(buffer_to_enum(target), offset, C_FCAST<ptroff>(data.size), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufFree(Span<const u32> const& buffers)
{
#ifndef NDEBUG
    if(!glDeleteBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteBuffers(C_FCAST<i32>(buffers.elements), buffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryFree(Span<const u32> const& ids)
{
#ifndef NDEBUG
    if(!glDeleteQueries)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glDeleteQueriesARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteQueries(C_FCAST<i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryEnd(QueryComp::query_flag const& target)
{
#ifndef NDEBUG
    if(!glEndQuery)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glEndQueryARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEndQuery(query_to_enum(target));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufAlloc(Span<u32> const& buffers)
{
#ifndef NDEBUG
    if(!glGenBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenBuffers(C_FCAST<i32>(buffers.elements), buffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryAlloc(Span<u32> const& ids)
{
#ifndef NDEBUG
    if(!glGenQueries)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glGenQueriesARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenQueries(C_FCAST<i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufGetParameteriv(BufferComp::buf_flag const& target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetBufferParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetBufferParameteriv(buffer_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void BufGetPointerv(BufferComp::buf_flag const& target, GLenum pname, void * * params)
{
#ifndef NDEBUG
    if(!glGetBufferPointerv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetBufferPointerv(buffer_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x150, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_15)
STATICINLINE void BufGetSubData(BufferComp::buf_flag const& target, ptroff offset, Bytes const& data)
{
#ifndef NDEBUG
    if(!glGetBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetBufferSubData(buffer_to_enum(target), offset, C_FCAST<ptroff>(data.size), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_15)
STATICINLINE void QueryGetObjectiv(u32 id, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetQueryObjectiv)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glGetQueryObjectivARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryObjectiv(id, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryGetObjectuiv(u32 id, GLenum pname, u32 * params)
{
#ifndef NDEBUG
    if(!glGetQueryObjectuiv)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glGetQueryObjectuivARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryObjectuiv(id, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryGetiv(QueryComp::query_flag const& target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetQueryiv)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glGetQueryivARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryiv(query_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE u8 IsBuffer(glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glIsBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsBuffer(C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE u8 IsQuery(u32 id)
{
#ifndef NDEBUG
    if(!glIsQuery)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glIsQueryARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsQuery(id);
}
#endif


#if GL_VERSION_VERIFY(0x150, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_15)
STATICINLINE void * BufMap(BufferComp::buf_flag const& target, RSCA access)
{
#ifndef NDEBUG
    if(!glMapBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glMapBuffer(buffer_to_enum(target), to_enum2(access));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE u8 BufUnmap(BufferComp::buf_flag const& target)
{
#ifndef NDEBUG
    if(!glUnmapBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glUnmapBuffer(buffer_to_enum(target));
}
#endif

};
template<typename ReqVer>
struct CGL_20
    : CGL_15<ReqVer>
{
using Parent = CGL_15<ReqVer>;

#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderAttach(glhnd const& program, glhnd const& shader)
{
#ifndef NDEBUG
    if(!glAttachShader)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glAttachObjectARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glAttachShader(C_OCAST<u32>(program), C_OCAST<u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void AttribBindLocation(glhnd const& program, u32 index, const GLchar * name)
{
#ifndef NDEBUG
    if(!glBindAttribLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindAttribLocation(C_OCAST<u32>(program), index, name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
#ifndef NDEBUG
    if(!glBlendEquationSeparate)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendEquationSeparate(modeRGB, modeAlpha);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderCompile(glhnd const& shader)
{
#ifndef NDEBUG
    if(!glCompileShader)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glCompileShaderARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompileShader(C_OCAST<u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE u32 ProgramAllocEx()
{
#ifndef NDEBUG
    if(!glCreateProgram)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glCreateProgramObjectARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glCreateProgram();
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE u32 ShaderAllocEx(ShaderStage type)
{
#ifndef NDEBUG
    if(!glCreateShader)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glCreateShaderObjectARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glCreateShader(to_enum1(type));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramFree(glhnd const& program)
{
#ifndef NDEBUG
    if(!glDeleteProgram)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteProgram(C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderFree(glhnd const& shader)
{
#ifndef NDEBUG
    if(!glDeleteShader)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteShader(C_OCAST<u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderDetach(glhnd const& program, glhnd const& shader)
{
#ifndef NDEBUG
    if(!glDetachShader)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glDetachObjectARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDetachShader(C_OCAST<u32>(program), C_OCAST<u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAODisableArray(u32 index)
{
#ifndef NDEBUG
    if(!glDisableVertexAttribArray)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDisableVertexAttribArray(index);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x300)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_30)
STATICINLINE void DrawBuffers(i32 n, const GLenum * bufs)
{
#ifndef NDEBUG
    if(!glDrawBuffers)
    {
#if defined(GL_ARB_draw_buffers) && GL_ARB_draw_buffers
        if(glDrawBuffersARB) Throw(undefined_behavior("extension GL_ARB_draw_buffers is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawBuffers(n, bufs);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAOEnableArray(u32 index)
{
#ifndef NDEBUG
    if(!glEnableVertexAttribArray)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEnableVertexAttribArray(index);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ActiveAttribGet(glhnd const& program, u32 index, i32 bufSize, i32 * length, i32 * size, GLenum * type, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetActiveAttrib)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveAttrib(C_OCAST<u32>(program), index, bufSize, length, size, type, name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ActiveUnifGet(glhnd const& program, u32 index, i32 bufSize, i32 * length, i32 * size, GLenum * type, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetActiveUniform)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glGetActiveUniformARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniform(C_OCAST<u32>(program), index, bufSize, length, size, type, name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void AttachedShadersGet(glhnd const& program, i32 maxCount, i32 * count, u32 * shaders)
{
#ifndef NDEBUG
    if(!glGetAttachedShaders)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetAttachedShaders(C_OCAST<u32>(program), maxCount, count, shaders);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE i32 AttribGetLocation(glhnd const& program, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetAttribLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetAttribLocation(C_OCAST<u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramGetInfoLog(glhnd const& program, i32 bufSize, i32 * length, GLchar * infoLog)
{
#ifndef NDEBUG
    if(!glGetProgramInfoLog)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramInfoLog(C_OCAST<u32>(program), bufSize, length, infoLog);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramGetiv(glhnd const& program, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramiv(C_OCAST<u32>(program), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderGetInfoLog(glhnd const& shader, i32 bufSize, i32 * length, GLchar * infoLog)
{
#ifndef NDEBUG
    if(!glGetShaderInfoLog)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderInfoLog(C_OCAST<u32>(shader), bufSize, length, infoLog);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderGetSource(glhnd const& shader, i32 bufSize, i32 * length, GLchar * source)
{
#ifndef NDEBUG
    if(!glGetShaderSource)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glGetShaderSourceARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderSource(C_OCAST<u32>(shader), bufSize, length, source);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderGetiv(glhnd const& shader, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetShaderiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderiv(C_OCAST<u32>(shader), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE i32 UnifGetLocation(glhnd const& program, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetUniformLocation)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glGetUniformLocationARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetUniformLocation(C_OCAST<u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void UnifGetfv(glhnd const& program, i32 location, scalar * params)
{
#ifndef NDEBUG
    if(!glGetUniformfv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glGetUniformfvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformfv(C_OCAST<u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void UnifGetiv(glhnd const& program, i32 location, i32 * params)
{
#ifndef NDEBUG
    if(!glGetUniformiv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glGetUniformivARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformiv(C_OCAST<u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAOGetPointerv(u32 index, GLenum pname, void * * pointer)
{
#ifndef NDEBUG
    if(!glGetVertexAttribPointerv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribPointerv(index, pname, pointer);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAOGetdv(u32 index, GLenum pname, bigscalar * params)
{
#ifndef NDEBUG
    if(!glGetVertexAttribdv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribdv(index, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAOGetfv(u32 index, GLenum pname, scalar * params)
{
#ifndef NDEBUG
    if(!glGetVertexAttribfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribfv(index, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAOGetiv(u32 index, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetVertexAttribiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribiv(index, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE u8 IsProgram(glhnd const& program)
{
#ifndef NDEBUG
    if(!glIsProgram)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsProgram(C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE u8 IsShader(glhnd const& shader)
{
#ifndef NDEBUG
    if(!glIsShader)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsShader(C_OCAST<u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramLink(glhnd const& program)
{
#ifndef NDEBUG
    if(!glLinkProgram)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glLinkProgramARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glLinkProgram(C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderSource(glhnd const& shader, i32 count, const GLchar * *const string, const i32 * length)
{
#ifndef NDEBUG
    if(!glShaderSource)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glShaderSourceARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glShaderSource(C_OCAST<u32>(shader), count, string, length);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void StencilFuncSeparate(GLenum face, GLenum func, i32 ref, u32 mask)
{
#ifndef NDEBUG
    if(!glStencilFuncSeparate)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glStencilFuncSeparate(face, func, ref, mask);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void StencilMaskSeparate(GLenum face, u32 mask)
{
#ifndef NDEBUG
    if(!glStencilMaskSeparate)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glStencilMaskSeparate(face, mask);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void StencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
#ifndef NDEBUG
    if(!glStencilOpSeparate)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glStencilOpSeparate(face, sfail, dpfail, dppass);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif1f(i32 location, scalar v0)
{
#ifndef NDEBUG
    if(!glUniform1f)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform1fARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1f(location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glUniform1fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform1fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1fv(location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif1i(i32 location, i32 v0)
{
#ifndef NDEBUG
    if(!glUniform1i)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform1iARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1i(location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(i32 location, Span<const i32> const& value)
{
#ifndef NDEBUG
    if(!glUniform1iv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform1ivARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1iv(location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif2f(i32 location, scalar v0, scalar v1)
{
#ifndef NDEBUG
    if(!glUniform2f)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform2fARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2f(location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, Span<Vecf2> const& value)
{
#ifndef NDEBUG
    if(!glUniform2fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform2fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2fv(location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif2i(i32 location, i32 v0, i32 v1)
{
#ifndef NDEBUG
    if(!glUniform2i)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform2iARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2i(location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(i32 location, Span<Veci2> const& value)
{
#ifndef NDEBUG
    if(!glUniform2iv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform2ivARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2iv(location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif3f(i32 location, scalar v0, scalar v1, scalar v2)
{
#ifndef NDEBUG
    if(!glUniform3f)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform3fARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3f(location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, Span<Vecf3> const& value)
{
#ifndef NDEBUG
    if(!glUniform3fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform3fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3fv(location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif3i(i32 location, i32 v0, i32 v1, i32 v2)
{
#ifndef NDEBUG
    if(!glUniform3i)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform3iARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3i(location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(i32 location, Span<Veci3> const& value)
{
#ifndef NDEBUG
    if(!glUniform3iv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform3ivARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3iv(location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif4f(i32 location, scalar v0, scalar v1, scalar v2, scalar v3)
{
#ifndef NDEBUG
    if(!glUniform4f)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform4fARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4f(location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, Span<Vecf4> const& value)
{
#ifndef NDEBUG
    if(!glUniform4fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform4fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4fv(location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif4i(i32 location, i32 v0, i32 v1, i32 v2, i32 v3)
{
#ifndef NDEBUG
    if(!glUniform4i)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform4iARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4i(location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(i32 location, Span<Veci4> const& value)
{
#ifndef NDEBUG
    if(!glUniform4iv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform4ivARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4iv(location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniformMatrix2fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniformMatrix3fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniformMatrix4fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramUse(glhnd const& program)
{
#ifndef NDEBUG
    if(!glUseProgram)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUseProgramObjectARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUseProgram(C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramValidate(glhnd const& program)
{
#ifndef NDEBUG
    if(!glValidateProgram)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glValidateProgramARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glValidateProgram(C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO1d(u32 index, bigscalar x)
{
#ifndef NDEBUG
    if(!glVertexAttrib1d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib1d(index, x);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO1dv(u32 index, const bigscalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib1dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib1dv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO1f(u32 index, scalar x)
{
#ifndef NDEBUG
    if(!glVertexAttrib1f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib1f(index, x);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO1fv(u32 index, const scalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib1fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib1fv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO1s(u32 index, i16 x)
{
#ifndef NDEBUG
    if(!glVertexAttrib1s)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib1s(index, x);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO1sv(u32 index, const i16 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib1sv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib1sv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO2d(u32 index, bigscalar x, bigscalar y)
{
#ifndef NDEBUG
    if(!glVertexAttrib2d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib2d(index, x, y);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO2dv(u32 index, const bigscalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib2dv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO2f(u32 index, scalar x, scalar y)
{
#ifndef NDEBUG
    if(!glVertexAttrib2f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib2f(index, x, y);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO2fv(u32 index, const scalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib2fv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO2s(u32 index, i16 x, i16 y)
{
#ifndef NDEBUG
    if(!glVertexAttrib2s)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib2s(index, x, y);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO2sv(u32 index, const i16 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib2sv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib2sv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO3d(u32 index, bigscalar x, bigscalar y, bigscalar z)
{
#ifndef NDEBUG
    if(!glVertexAttrib3d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib3d(index, x, y, z);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO3dv(u32 index, const bigscalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib3dv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO3f(u32 index, scalar x, scalar y, scalar z)
{
#ifndef NDEBUG
    if(!glVertexAttrib3f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib3f(index, x, y, z);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO3fv(u32 index, const scalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib3fv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO3s(u32 index, i16 x, i16 y, i16 z)
{
#ifndef NDEBUG
    if(!glVertexAttrib3s)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib3s(index, x, y, z);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO3sv(u32 index, const i16 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib3sv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib3sv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4Nbv(u32 index, const i8 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4Nbv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4Nbv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4Niv(u32 index, const i32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4Niv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4Niv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4Nsv(u32 index, const i16 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4Nsv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4Nsv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4Nub(u32 index, bool x, bool y, bool z, bool w)
{
#ifndef NDEBUG
    if(!glVertexAttrib4Nub)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4Nub(index, x ? GL_TRUE : GL_FALSE, y ? GL_TRUE : GL_FALSE, z ? GL_TRUE : GL_FALSE, w ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4Nubv(u32 index, const u8 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4Nubv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4Nubv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4Nuiv(u32 index, const u32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4Nuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4Nuiv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4Nusv(u32 index, const u16 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4Nusv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4Nusv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4bv(u32 index, const i8 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4bv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4bv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4d(u32 index, bigscalar x, bigscalar y, bigscalar z, bigscalar w)
{
#ifndef NDEBUG
    if(!glVertexAttrib4d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4d(index, x, y, z, w);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4dv(u32 index, const bigscalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4dv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO4f(u32 index, scalar x, scalar y, scalar z, scalar w)
{
#ifndef NDEBUG
    if(!glVertexAttrib4f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4f(index, x, y, z, w);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO4fv(u32 index, const scalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4fv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4iv(u32 index, const i32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4iv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4s(u32 index, i16 x, i16 y, i16 z, i16 w)
{
#ifndef NDEBUG
    if(!glVertexAttrib4s)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4s(index, x, y, z, w);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4sv(u32 index, const i16 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4sv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4sv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4ubv(u32 index, const u8 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4ubv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4ubv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4uiv(u32 index, const u32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4uiv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4usv(u32 index, const u16 * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4usv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4usv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAOPointer(u32 index, i32 size, GLenum type, bool normalized, i32 stride, const void * pointer)
{
#ifndef NDEBUG
    if(!glVertexAttribPointer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, stride, pointer);
}
#endif

};
template<typename ReqVer>
struct CGL_21
    : CGL_20<ReqVer>
{
using Parent = CGL_20<ReqVer>;
using Parent::Uniffv;

#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf2_3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2x3fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf2_4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2x4fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf3_2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3x2fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf3_4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3x4fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf4_2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4x2fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf4_3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4x3fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif

};
template<typename ReqVer>
struct CGL_30
    : CGL_21<ReqVer>
{
using Parent = CGL_21<ReqVer>;

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void ConditionalRenderBegin(u32 id, Delay mode)
{
#ifndef NDEBUG
    if(!glBeginConditionalRender)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBeginConditionalRender(id, to_enum1(mode));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void XFBegin(GLenum primitiveMode)
{
#ifndef NDEBUG
    if(!glBeginTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBeginTransformFeedback(primitiveMode);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufBindBase(BufferComp::buf_flag const& target, u32 index, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glBindBufferBase)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindBufferBase(buffer_to_enum(target), index, C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufBindRange(BufferComp::buf_flag const& target, u32 index, glhnd const& buffer, ptroff offset, ptroff size)
{
#ifndef NDEBUG
    if(!glBindBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindBufferRange(buffer_to_enum(target), index, C_OCAST<u32>(buffer), offset, size);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void BindFragDataLocation(glhnd const& program, u32 color, const GLchar * name)
{
#ifndef NDEBUG
    if(!glBindFragDataLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindFragDataLocation(C_OCAST<u32>(program), color, name);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBBind(FramebufferT target, u32 framebuffer)
{
#ifndef NDEBUG
    if(!glBindFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindFramebuffer(to_enum(target), framebuffer);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufBind(GLenum target, u32 renderbuffer)
{
#ifndef NDEBUG
    if(!glBindRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindRenderbuffer(target, renderbuffer);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOBind(glhnd const& array)
{
#ifndef NDEBUG
    if(!glBindVertexArray)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindVertexArray(C_OCAST<u32>(array));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void FBBlit(i32 srcX0, i32 srcY0, i32 srcX1, i32 srcY1, i32 dstX0, i32 dstY0, i32 dstX1, i32 dstY1, u32 mask, GLenum filter)
{
#ifndef NDEBUG
    if(!glBlitFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE GLenum FBCheckStatus(FramebufferT target)
{
#ifndef NDEBUG
    if(!glCheckFramebufferStatus)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glCheckFramebufferStatus(to_enum(target));
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void ClampColor(GLenum target, GLenum clamp)
{
#ifndef NDEBUG
    if(!glClampColor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClampColor(target, clamp);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufClearfi(GLenum buffer, i32 drawbuffer, scalar depth, i32 stencil)
{
#ifndef NDEBUG
    if(!glClearBufferfi)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearBufferfi(buffer, drawbuffer, depth, stencil);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufClearfv(GLenum buffer, i32 drawbuffer, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glClearBufferfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearBufferfv(buffer, drawbuffer, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufCleariv(GLenum buffer, i32 drawbuffer, Span<const i32> const& value)
{
#ifndef NDEBUG
    if(!glClearBufferiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearBufferiv(buffer, drawbuffer, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufClearuiv(GLenum buffer, i32 drawbuffer, Span<const u32> const& value)
{
#ifndef NDEBUG
    if(!glClearBufferuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearBufferuiv(buffer, drawbuffer, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void ColorMaski(u32 index, bool r, bool g, bool b, bool a)
{
#ifndef NDEBUG
    if(!glColorMaski)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glColorMaski(index, r ? GL_TRUE : GL_FALSE, g ? GL_TRUE : GL_FALSE, b ? GL_TRUE : GL_FALSE, a ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBFree(Span<const u32> const& framebuffers)
{
#ifndef NDEBUG
    if(!glDeleteFramebuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteFramebuffers(C_FCAST<i32>(framebuffers.elements), framebuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufFree(Span<const u32> const& renderbuffers)
{
#ifndef NDEBUG
    if(!glDeleteRenderbuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteRenderbuffers(C_FCAST<i32>(renderbuffers.elements), renderbuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOFree(Span<const u32> const& arrays)
{
#ifndef NDEBUG
    if(!glDeleteVertexArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteVertexArrays(C_FCAST<i32>(arrays.elements), arrays.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void Disablei(Feature target, u32 index)
{
#ifndef NDEBUG
    if(!glDisablei)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDisablei(to_enum(target), index);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void Enablei(Feature target, u32 index)
{
#ifndef NDEBUG
    if(!glEnablei)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEnablei(to_enum(target), index);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void ConditionalRenderEnd()
{
#ifndef NDEBUG
    if(!glEndConditionalRender)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEndConditionalRender();
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void XFEnd()
{
#ifndef NDEBUG
    if(!glEndTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEndTransformFeedback();
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufFlushMappedRange(BufferComp::buf_flag const& target, ptroff offset, ptroff length)
{
#ifndef NDEBUG
    if(!glFlushMappedBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFlushMappedBufferRange(buffer_to_enum(target), offset, length);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBRenderbuffer(FramebufferT target, GLenum attachment, GLenum renderbuffertarget, u32 renderbuffer)
{
#ifndef NDEBUG
    if(!glFramebufferRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferRenderbuffer(to_enum(target), attachment, renderbuffertarget, renderbuffer);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void FBTexture1D(FramebufferT target, GLenum attachment, TexComp::tex_flag const& textarget, glhnd const& texture, i32 level)
{
#ifndef NDEBUG
    if(!glFramebufferTexture1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTexture1D(to_enum(target), attachment, texture_to_enum(textarget), C_OCAST<u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBTexture2D(FramebufferT target, GLenum attachment, TexComp::tex_flag const& textarget, glhnd const& texture, i32 level)
{
#ifndef NDEBUG
    if(!glFramebufferTexture2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTexture2D(to_enum(target), attachment, texture_to_enum(textarget), C_OCAST<u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void FBTexture3D(FramebufferT target, GLenum attachment, TexComp::tex_flag const& textarget, glhnd const& texture, i32 level, i32 zoffset)
{
#ifndef NDEBUG
    if(!glFramebufferTexture3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTexture3D(to_enum(target), attachment, texture_to_enum(textarget), C_OCAST<u32>(texture), level, zoffset);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void FBTextureLayer(FramebufferT target, GLenum attachment, glhnd const& texture, i32 level, i32 layer)
{
#ifndef NDEBUG
    if(!glFramebufferTextureLayer)
    {
#if defined(GL_ARB_geometry_shader4) && GL_ARB_geometry_shader4
        if(glFramebufferTextureLayerARB) Throw(undefined_behavior("extension GL_ARB_geometry_shader4 is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTextureLayer(to_enum(target), attachment, C_OCAST<u32>(texture), level, layer);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBAlloc(Span<u32> const& framebuffers)
{
#ifndef NDEBUG
    if(!glGenFramebuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenFramebuffers(C_FCAST<i32>(framebuffers.elements), framebuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufAlloc(Span<u32> const& renderbuffers)
{
#ifndef NDEBUG
    if(!glGenRenderbuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenRenderbuffers(C_FCAST<i32>(renderbuffers.elements), renderbuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOAlloc(Span<u32> const& arrays)
{
#ifndef NDEBUG
    if(!glGenVertexArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenVertexArrays(C_FCAST<i32>(arrays.elements), arrays.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void GenerateMipmap(TexComp::tex_flag const& target)
{
#ifndef NDEBUG
    if(!glGenerateMipmap)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenerateMipmap(texture_to_enum(target));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x310)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_31)
STATICINLINE void BoolGeti_v(BufferComp::buf_flag const& target, u32 index, u8 * data)
{
#ifndef NDEBUG
    if(!glGetBooleani_v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetBooleani_v(buffer_to_enum(target), index, data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE i32 GetFragDataLocation(glhnd const& program, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetFragDataLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetFragDataLocation(C_OCAST<u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBGetAttachmentParameteriv(FramebufferT target, GLenum attachment, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetFramebufferAttachmentParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetFramebufferAttachmentParameteriv(to_enum(target), attachment, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void IntegerGeti_v(GLenum target, u32 index, i32 * data)
{
#ifndef NDEBUG
    if(!glGetIntegeri_v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetIntegeri_v(target, index, data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufGetParameteriv(GLenum target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetRenderbufferParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetRenderbufferParameteriv(target, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE const u8 * GetStringi(GLenum name, u32 index)
{
#ifndef NDEBUG
    if(!glGetStringi)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetStringi(name, index);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void TexGetParameterIiv(TexComp::tex_flag const& target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetTexParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexParameterIiv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void TexGetParameterIuiv(TexComp::tex_flag const& target, GLenum pname, u32 * params)
{
#ifndef NDEBUG
    if(!glGetTexParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexParameterIuiv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void XFGetVarying(glhnd const& program, u32 index, i32 bufSize, i32 * length, i32 * size, GLenum * type, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetTransformFeedbackVarying)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTransformFeedbackVarying(C_OCAST<u32>(program), index, bufSize, length, size, type, name);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void UnifGetuiv(glhnd const& program, i32 location, u32 * params)
{
#ifndef NDEBUG
    if(!glGetUniformuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformuiv(C_OCAST<u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOGetIiv(u32 index, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetVertexAttribIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribIiv(index, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOGetIuiv(u32 index, GLenum pname, u32 * params)
{
#ifndef NDEBUG
    if(!glGetVertexAttribIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribIuiv(index, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE u8 IsEnabledi(Feature target, u32 index)
{
#ifndef NDEBUG
    if(!glIsEnabledi)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsEnabledi(to_enum(target), index);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE u8 IsFramebuffer(u32 framebuffer)
{
#ifndef NDEBUG
    if(!glIsFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsFramebuffer(framebuffer);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE u8 IsRenderbuffer(u32 renderbuffer)
{
#ifndef NDEBUG
    if(!glIsRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsRenderbuffer(renderbuffer);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE u8 IsVertexArray(glhnd const& array)
{
#ifndef NDEBUG
    if(!glIsVertexArray)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsVertexArray(C_OCAST<u32>(array));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void * BufMapRange(BufferComp::buf_flag const& target, ptroff offset, ptroff length, RSCA access)
{
#ifndef NDEBUG
    if(!glMapBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glMapBufferRange(buffer_to_enum(target), offset, length, to_enum2(access));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufStorage(GLenum target, PixFmt internalformat, Size const& size)
{
#ifndef NDEBUG
    if(!glRenderbufferStorage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glRenderbufferStorage(target, to_enum(internalformat), size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void RBufStorageMultisample(GLenum target, i32 samples, PixFmt internalformat, Size const& size)
{
#ifndef NDEBUG
    if(!glRenderbufferStorageMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glRenderbufferStorageMultisample(target, samples, to_enum(internalformat), size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void TexParameterIiv(TexComp::tex_flag const& target, GLenum pname, const i32 * params)
{
#ifndef NDEBUG
    if(!glTexParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexParameterIiv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void TexParameterIuiv(TexComp::tex_flag const& target, GLenum pname, const u32 * params)
{
#ifndef NDEBUG
    if(!glTexParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexParameterIuiv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void XFVaryings(glhnd const& program, i32 count, const GLchar * *const varyings, GLenum bufferMode)
{
#ifndef NDEBUG
    if(!glTransformFeedbackVaryings)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTransformFeedbackVaryings(C_OCAST<u32>(program), count, varyings, bufferMode);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif1ui(i32 location, u32 v0)
{
#ifndef NDEBUG
    if(!glUniform1ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1ui(location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(i32 location, Span<const u32> const& value)
{
#ifndef NDEBUG
    if(!glUniform1uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1uiv(location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif2ui(i32 location, u32 v0, u32 v1)
{
#ifndef NDEBUG
    if(!glUniform2ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2ui(location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(i32 location, Span<Vecui2> const& value)
{
#ifndef NDEBUG
    if(!glUniform2uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2uiv(location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif3ui(i32 location, u32 v0, u32 v1, u32 v2)
{
#ifndef NDEBUG
    if(!glUniform3ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3ui(location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(i32 location, Span<Vecui3> const& value)
{
#ifndef NDEBUG
    if(!glUniform3uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3uiv(location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif4ui(i32 location, u32 v0, u32 v1, u32 v2, u32 v3)
{
#ifndef NDEBUG
    if(!glUniform4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4ui(location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(i32 location, Span<Vecui4> const& value)
{
#ifndef NDEBUG
    if(!glUniform4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4uiv(location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI1i(u32 index, i32 x)
{
#ifndef NDEBUG
    if(!glVertexAttribI1i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI1i(index, x);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI1iv(u32 index, const i32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI1iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI1iv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI1ui(u32 index, u32 x)
{
#ifndef NDEBUG
    if(!glVertexAttribI1ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI1ui(index, x);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI1uiv(u32 index, const u32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI1uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI1uiv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI2i(u32 index, i32 x, i32 y)
{
#ifndef NDEBUG
    if(!glVertexAttribI2i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI2i(index, x, y);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI2iv(u32 index, const i32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI2iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI2iv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI2ui(u32 index, u32 x, u32 y)
{
#ifndef NDEBUG
    if(!glVertexAttribI2ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI2ui(index, x, y);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI2uiv(u32 index, const u32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI2uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI2uiv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI3i(u32 index, i32 x, i32 y, i32 z)
{
#ifndef NDEBUG
    if(!glVertexAttribI3i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI3i(index, x, y, z);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI3iv(u32 index, const i32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI3iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI3iv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI3ui(u32 index, u32 x, u32 y, u32 z)
{
#ifndef NDEBUG
    if(!glVertexAttribI3ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI3ui(index, x, y, z);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI3uiv(u32 index, const u32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI3uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI3uiv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI4bv(u32 index, const i8 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI4bv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4bv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOI4i(u32 index, i32 x, i32 y, i32 z, i32 w)
{
#ifndef NDEBUG
    if(!glVertexAttribI4i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4i(index, x, y, z, w);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOI4iv(u32 index, const i32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI4iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4iv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI4sv(u32 index, const i16 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI4sv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4sv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI4ubv(u32 index, const u8 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI4ubv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4ubv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOI4ui(u32 index, u32 x, u32 y, u32 z, u32 w)
{
#ifndef NDEBUG
    if(!glVertexAttribI4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4ui(index, x, y, z, w);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOI4uiv(u32 index, const u32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4uiv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI4usv(u32 index, const u16 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI4usv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4usv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOIPointer(u32 index, i32 size, GLenum type, i32 stride, const void * pointer)
{
#ifndef NDEBUG
    if(!glVertexAttribIPointer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribIPointer(index, size, type, stride, pointer);
}
#endif

};
template<typename ReqVer>
struct CGL_31
    : CGL_30<ReqVer>
{
using Parent = CGL_30<ReqVer>;

#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void BufCopySubData(GLenum readTarget, GLenum writeTarget, ptroff readOffset, ptroff writeOffset, ptroff size)
{
#ifndef NDEBUG
    if(!glCopyBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void DrawArraysInstanced(DrwMd const& mode, i32 first, i32 count, i32 instancecount)
{
#ifndef NDEBUG
    if(!glDrawArraysInstanced)
    {
#if defined(GL_ARB_draw_instanced) && GL_ARB_draw_instanced
        if(glDrawArraysInstancedARB) Throw(undefined_behavior("extension GL_ARB_draw_instanced is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawArraysInstanced(to_enum(mode.t, mode.c), first, count, instancecount);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void DrawElementsInstanced(DrwMd const& mode, i32 count, TypeEnum type, uintptr indices, i32 instancecount)
{
#ifndef NDEBUG
    if(!glDrawElementsInstanced)
    {
#if defined(GL_ARB_draw_instanced) && GL_ARB_draw_instanced
        if(glDrawElementsInstancedARB) Throw(undefined_behavior("extension GL_ARB_draw_instanced is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElementsInstanced(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void *>(indices), instancecount);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void ActiveUnifBlockGetName(glhnd const& program, u32 uniformBlockIndex, i32 bufSize, i32 * length, GLchar * uniformBlockName)
{
#ifndef NDEBUG
    if(!glGetActiveUniformBlockName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformBlockName(C_OCAST<u32>(program), uniformBlockIndex, bufSize, length, uniformBlockName);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void ActiveUnifBlockGetiv(glhnd const& program, u32 uniformBlockIndex, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetActiveUniformBlockiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformBlockiv(C_OCAST<u32>(program), uniformBlockIndex, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x310, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_31)
STATICINLINE void ActiveUnifGetName(glhnd const& program, u32 uniformIndex, i32 bufSize, i32 * length, GLchar * uniformName)
{
#ifndef NDEBUG
    if(!glGetActiveUniformName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformName(C_OCAST<u32>(program), uniformIndex, bufSize, length, uniformName);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void ActiveUnifGetsiv(glhnd const& program, i32 uniformCount, const u32 * uniformIndices, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetActiveUniformsiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformsiv(C_OCAST<u32>(program), uniformCount, uniformIndices, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE u32 UnifBlockGetIndex(glhnd const& program, const GLchar * uniformBlockName)
{
#ifndef NDEBUG
    if(!glGetUniformBlockIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetUniformBlockIndex(C_OCAST<u32>(program), uniformBlockName);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void UnifGetIndices(glhnd const& program, i32 uniformCount, const GLchar * *const uniformNames, u32 * uniformIndices)
{
#ifndef NDEBUG
    if(!glGetUniformIndices)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformIndices(C_OCAST<u32>(program), uniformCount, uniformNames, uniformIndices);
}
#endif


#if GL_VERSION_VERIFY(0x310, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_31)
STATICINLINE void PrimitiveRestartIndex(u32 index)
{
#ifndef NDEBUG
    if(!glPrimitiveRestartIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPrimitiveRestartIndex(index);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x320)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_32)
STATICINLINE void TexBuffer(TexComp::tex_flag const& target, PixFmt internalformat, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glTexBuffer)
    {
#if defined(GL_ARB_texture_buffer_object) && GL_ARB_texture_buffer_object
        if(glTexBufferARB) Throw(undefined_behavior("extension GL_ARB_texture_buffer_object is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexBuffer(texture_to_enum(target), to_enum(internalformat), C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void UnifBlockBinding(glhnd const& program, u32 uniformBlockIndex, u32 uniformBlockBinding)
{
#ifndef NDEBUG
    if(!glUniformBlockBinding)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformBlockBinding(C_OCAST<u32>(program), uniformBlockIndex, uniformBlockBinding);
}
#endif

};
template<typename ReqVer>
struct CGL_32
    : CGL_31<ReqVer>
{
using Parent = CGL_31<ReqVer>;

#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE GLenum SyncClientWait(GLsync sync, RSCA flags, u64 timeout)
{
#ifndef NDEBUG
    if(!glClientWaitSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glClientWaitSync(sync, to_enum2(flags), timeout);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void SyncFree(GLsync sync)
{
#ifndef NDEBUG
    if(!glDeleteSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteSync(sync);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x320)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_32)
STATICINLINE void DrawElementsBaseVertex(DrwMd const& mode, i32 count, TypeEnum type, uintptr indices, i32 basevertex)
{
#ifndef NDEBUG
    if(!glDrawElementsBaseVertex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElementsBaseVertex(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void *>(indices), basevertex);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x320)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_32)
STATICINLINE void DrawElementsInstancedBaseVertex(DrwMd const& mode, i32 count, TypeEnum type, uintptr indices, i32 instancecount, i32 basevertex)
{
#ifndef NDEBUG
    if(!glDrawElementsInstancedBaseVertex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElementsInstancedBaseVertex(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void *>(indices), instancecount, basevertex);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x320)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_32)
STATICINLINE void DrawRangeElementsBaseVertex(DrwMd const& mode, u32 start, u32 end, i32 count, TypeEnum type, uintptr indices, i32 basevertex)
{
#ifndef NDEBUG
    if(!glDrawRangeElementsBaseVertex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawRangeElementsBaseVertex(to_enum(mode.t, mode.c), start, end, count, to_enum(type), C_RCAST<const void *>(indices), basevertex);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE GLsync SyncFence(GLenum condition, RSCA flags)
{
#ifndef NDEBUG
    if(!glFenceSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glFenceSync(condition, to_enum2(flags));
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x320)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_32)
STATICINLINE void FBTexture(FramebufferT target, GLenum attachment, glhnd const& texture, i32 level)
{
#ifndef NDEBUG
    if(!glFramebufferTexture)
    {
#if defined(GL_ARB_geometry_shader4) && GL_ARB_geometry_shader4
        if(glFramebufferTextureARB) Throw(undefined_behavior("extension GL_ARB_geometry_shader4 is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTexture(to_enum(target), attachment, C_OCAST<u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void BufGetParameteri64v(BufferComp::buf_flag const& target, GLenum pname, i64 * params)
{
#ifndef NDEBUG
    if(!glGetBufferParameteri64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetBufferParameteri64v(buffer_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void IntegerGet64i_v(GLenum target, u32 index, i64 * data)
{
#ifndef NDEBUG
    if(!glGetInteger64i_v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetInteger64i_v(target, index, data);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void IntegerGet64v(GLenum pname, i64 * data)
{
#ifndef NDEBUG
    if(!glGetInteger64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetInteger64v(pname, data);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x310)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_31)
STATICINLINE void GetMultisamplefv(GLenum pname, u32 index, scalar * val)
{
#ifndef NDEBUG
    if(!glGetMultisamplefv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetMultisamplefv(pname, index, val);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void SyncGetiv(GLsync sync, GLenum pname, i32 bufSize, i32 * length, i32 * values)
{
#ifndef NDEBUG
    if(!glGetSynciv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSynciv(sync, pname, bufSize, length, values);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE u8 IsSync(GLsync sync)
{
#ifndef NDEBUG
    if(!glIsSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsSync(sync);
}
#endif


#if GL_VERSION_VERIFY(0x320, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_32)
STATICINLINE void MultiDrawElementsBaseVertex(DrwMd const& mode, const i32 * count, TypeEnum type, uintptr indices, i32 drawcount, const i32 * basevertex)
{
#ifndef NDEBUG
    if(!glMultiDrawElementsBaseVertex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMultiDrawElementsBaseVertex(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void * *const>(indices), drawcount, basevertex);
}
#endif


#if GL_VERSION_VERIFY(0x320, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_32)
STATICINLINE void VAOProvoking(GLenum mode)
{
#ifndef NDEBUG
    if(!glProvokingVertex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProvokingVertex(mode);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x310)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_31)
STATICINLINE void SampleMaski(u32 maskNumber, u32 mask)
{
#ifndef NDEBUG
    if(!glSampleMaski)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSampleMaski(maskNumber, mask);
}
#endif


#if GL_VERSION_VERIFY(0x320, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_32)
STATICINLINE void TexImage2DMultisample(TexComp::tex_flag const& target, i32 samples, PixFmt internalformat, Size const& size, bool fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTexImage2DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage2DMultisample(texture_to_enum(target), samples, to_enum(internalformat), size.w, size.h, fixedsamplelocations ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x320, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_32)
STATICINLINE void TexImage3DMultisample(TexComp::tex_flag const& target, i32 samples, PixFmt internalformat, Size3 const& size, bool fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTexImage3DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage3DMultisample(texture_to_enum(target), samples, to_enum(internalformat), size.width, size.height, size.depth, fixedsamplelocations ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void SyncWait(GLsync sync, RSCA flags, u64 timeout)
{
#ifndef NDEBUG
    if(!glWaitSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glWaitSync(sync, to_enum2(flags), timeout);
}
#endif

};
template<typename ReqVer>
struct CGL_33
    : CGL_32<ReqVer>
{
using Parent = CGL_32<ReqVer>;

#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void BindFragDataLocationIndexed(glhnd const& program, u32 colorNumber, u32 index, const GLchar * name)
{
#ifndef NDEBUG
    if(!glBindFragDataLocationIndexed)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindFragDataLocationIndexed(C_OCAST<u32>(program), colorNumber, index, name);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerBind(u32 unit, glhnd const& sampler)
{
#ifndef NDEBUG
    if(!glBindSampler)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindSampler(unit, C_OCAST<u32>(sampler));
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerFree(Span<const u32> const& samplers)
{
#ifndef NDEBUG
    if(!glDeleteSamplers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteSamplers(C_FCAST<i32>(samplers.elements), samplers.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerAlloc(Span<u32> const& samplers)
{
#ifndef NDEBUG
    if(!glGenSamplers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenSamplers(C_FCAST<i32>(samplers.elements), samplers.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE i32 GetFragDataIndex(glhnd const& program, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetFragDataIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetFragDataIndex(C_OCAST<u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void QueryGetObjecti64v(u32 id, GLenum pname, i64 * params)
{
#ifndef NDEBUG
    if(!glGetQueryObjecti64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryObjecti64v(id, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void QueryGetObjectui64v(u32 id, GLenum pname, u64 * params)
{
#ifndef NDEBUG
    if(!glGetQueryObjectui64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryObjectui64v(id, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerGetParameterIiv(glhnd const& sampler, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameterIiv(C_OCAST<u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerGetParameterIuiv(glhnd const& sampler, GLenum pname, u32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameterIuiv(C_OCAST<u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerGetParameterfv(glhnd const& sampler, GLenum pname, scalar * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameterfv(C_OCAST<u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerGetParameteriv(glhnd const& sampler, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameteriv(C_OCAST<u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE u8 IsSampler(glhnd const& sampler)
{
#ifndef NDEBUG
    if(!glIsSampler)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsSampler(C_OCAST<u32>(sampler));
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void QueryCounter(u32 id, QueryComp::query_flag const& target)
{
#ifndef NDEBUG
    if(!glQueryCounter)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glQueryCounter(id, query_to_enum(target));
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerParameterIiv(glhnd const& sampler, GLenum pname, const i32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterIiv(C_OCAST<u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerParameterIuiv(glhnd const& sampler, GLenum pname, const u32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterIuiv(C_OCAST<u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameterf(glhnd const& sampler, GLenum pname, scalar param)
{
#ifndef NDEBUG
    if(!glSamplerParameterf)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterf(C_OCAST<u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameterfv(glhnd const& sampler, GLenum pname, const scalar * param)
{
#ifndef NDEBUG
    if(!glSamplerParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterfv(C_OCAST<u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameteri(glhnd const& sampler, GLenum pname, i32 param)
{
#ifndef NDEBUG
    if(!glSamplerParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameteri(C_OCAST<u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameteriv(glhnd const& sampler, GLenum pname, const i32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameteriv(C_OCAST<u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void VAODivisor(u32 index, u32 divisor)
{
#ifndef NDEBUG
    if(!glVertexAttribDivisor)
    {
#if defined(GL_ARB_instanced_arrays) && GL_ARB_instanced_arrays
        if(glVertexAttribDivisorARB) Throw(undefined_behavior("extension GL_ARB_instanced_arrays is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribDivisor(index, divisor);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP1ui(u32 index, GLenum type, bool normalized, u32 value)
{
#ifndef NDEBUG
    if(!glVertexAttribP1ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP1ui(index, type, normalized ? GL_TRUE : GL_FALSE, value);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP1uiv(u32 index, GLenum type, bool normalized, Span<const u32> const& value)
{
#ifndef NDEBUG
    if(!glVertexAttribP1uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP1uiv(index, type, normalized ? GL_TRUE : GL_FALSE, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP2ui(u32 index, GLenum type, bool normalized, u32 value)
{
#ifndef NDEBUG
    if(!glVertexAttribP2ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP2ui(index, type, normalized ? GL_TRUE : GL_FALSE, value);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP2uiv(u32 index, GLenum type, bool normalized, Span<const u32> const& value)
{
#ifndef NDEBUG
    if(!glVertexAttribP2uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP2uiv(index, type, normalized ? GL_TRUE : GL_FALSE, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP3ui(u32 index, GLenum type, bool normalized, u32 value)
{
#ifndef NDEBUG
    if(!glVertexAttribP3ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP3ui(index, type, normalized ? GL_TRUE : GL_FALSE, value);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP3uiv(u32 index, GLenum type, bool normalized, Span<const u32> const& value)
{
#ifndef NDEBUG
    if(!glVertexAttribP3uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP3uiv(index, type, normalized ? GL_TRUE : GL_FALSE, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP4ui(u32 index, GLenum type, bool normalized, u32 value)
{
#ifndef NDEBUG
    if(!glVertexAttribP4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP4ui(index, type, normalized ? GL_TRUE : GL_FALSE, value);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP4uiv(u32 index, GLenum type, bool normalized, Span<const u32> const& value)
{
#ifndef NDEBUG
    if(!glVertexAttribP4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP4uiv(index, type, normalized ? GL_TRUE : GL_FALSE, value.data);
}
#endif

};
template<typename ReqVer>
struct CGL_40
    : CGL_33<ReqVer>
{
using Parent = CGL_33<ReqVer>;

#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void QueryBeginIndexed(QueryComp::query_flag const& target, u32 index, u32 id)
{
#ifndef NDEBUG
    if(!glBeginQueryIndexed)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBeginQueryIndexed(query_to_enum(target), index, id);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFBind(GLenum target, u32 id)
{
#ifndef NDEBUG
    if(!glBindTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindTransformFeedback(target, id);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void BlendEquationSeparatei(u32 buf, GLenum modeRGB, GLenum modeAlpha)
{
#ifndef NDEBUG
    if(!glBlendEquationSeparatei)
    {
#if defined(GL_ARB_draw_buffers_blend) && GL_ARB_draw_buffers_blend
        if(glBlendEquationSeparateiARB) Throw(undefined_behavior("extension GL_ARB_draw_buffers_blend is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendEquationSeparatei(buf, modeRGB, modeAlpha);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void BlendEquationi(u32 buf, GLenum mode)
{
#ifndef NDEBUG
    if(!glBlendEquationi)
    {
#if defined(GL_ARB_draw_buffers_blend) && GL_ARB_draw_buffers_blend
        if(glBlendEquationiARB) Throw(undefined_behavior("extension GL_ARB_draw_buffers_blend is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendEquationi(buf, mode);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void BlendFuncSeparatei(u32 buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
#ifndef NDEBUG
    if(!glBlendFuncSeparatei)
    {
#if defined(GL_ARB_draw_buffers_blend) && GL_ARB_draw_buffers_blend
        if(glBlendFuncSeparateiARB) Throw(undefined_behavior("extension GL_ARB_draw_buffers_blend is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendFuncSeparatei(buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void BlendFunci(u32 buf, GLenum src, GLenum dst)
{
#ifndef NDEBUG
    if(!glBlendFunci)
    {
#if defined(GL_ARB_draw_buffers_blend) && GL_ARB_draw_buffers_blend
        if(glBlendFunciARB) Throw(undefined_behavior("extension GL_ARB_draw_buffers_blend is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendFunci(buf, src, dst);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFFree(Span<const u32> const& ids)
{
#ifndef NDEBUG
    if(!glDeleteTransformFeedbacks)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteTransformFeedbacks(C_FCAST<i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x310)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_31)
STATICINLINE void DrawArraysIndirect(DrwMd const& mode, uintptr indirect)
{
#ifndef NDEBUG
    if(!glDrawArraysIndirect)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawArraysIndirect(to_enum(mode.t, mode.c), C_RCAST<const void *>(indirect));
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x310)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_31)
STATICINLINE void DrawElementsIndirect(DrwMd const& mode, TypeEnum type, uintptr indirect)
{
#ifndef NDEBUG
    if(!glDrawElementsIndirect)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElementsIndirect(to_enum(mode.t, mode.c), to_enum(type), C_RCAST<const void *>(indirect));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void DrawTransformFeedback(DrwMd const& mode, u32 id)
{
#ifndef NDEBUG
    if(!glDrawTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawTransformFeedback(to_enum(mode.t, mode.c), id);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void DrawTransformFeedbackStream(DrwMd const& mode, u32 id, u32 stream)
{
#ifndef NDEBUG
    if(!glDrawTransformFeedbackStream)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawTransformFeedbackStream(to_enum(mode.t, mode.c), id, stream);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void QueryEndIndexed(QueryComp::query_flag const& target, u32 index)
{
#ifndef NDEBUG
    if(!glEndQueryIndexed)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEndQueryIndexed(query_to_enum(target), index);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFAlloc(Span<u32> const& ids)
{
#ifndef NDEBUG
    if(!glGenTransformFeedbacks)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenTransformFeedbacks(C_FCAST<i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void GetActiveSubroutineName(glhnd const& program, ShaderStage shadertype, u32 index, i32 bufsize, i32 * length, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetActiveSubroutineName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveSubroutineName(C_OCAST<u32>(program), to_enum1(shadertype), index, bufsize, length, name);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void UnifGetActiveSubroutineName(glhnd const& program, ShaderStage shadertype, u32 index, i32 bufsize, i32 * length, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetActiveSubroutineUniformName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveSubroutineUniformName(C_OCAST<u32>(program), to_enum1(shadertype), index, bufsize, length, name);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void UnifGetActiveSubroutineiv(glhnd const& program, ShaderStage shadertype, u32 index, GLenum pname, i32 * values)
{
#ifndef NDEBUG
    if(!glGetActiveSubroutineUniformiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveSubroutineUniformiv(C_OCAST<u32>(program), to_enum1(shadertype), index, pname, values);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void ProgramGetStageiv(glhnd const& program, ShaderStage shadertype, GLenum pname, i32 * values)
{
#ifndef NDEBUG
    if(!glGetProgramStageiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramStageiv(C_OCAST<u32>(program), to_enum1(shadertype), pname, values);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void QueryGetIndexediv(GLenum target, u32 index, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetQueryIndexediv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryIndexediv(target, index, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE u32 GetSubroutineIndex(glhnd const& program, ShaderStage shadertype, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetSubroutineIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetSubroutineIndex(C_OCAST<u32>(program), to_enum1(shadertype), name);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE i32 UnifGetSubroutineLocation(glhnd const& program, ShaderStage shadertype, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetSubroutineUniformLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetSubroutineUniformLocation(C_OCAST<u32>(program), to_enum1(shadertype), name);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void UnifGetSubroutineuiv(ShaderStage shadertype, i32 location, u32 * params)
{
#ifndef NDEBUG
    if(!glGetUniformSubroutineuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformSubroutineuiv(to_enum1(shadertype), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void UnifGetdv(glhnd const& program, i32 location, bigscalar * params)
{
#ifndef NDEBUG
    if(!glGetUniformdv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformdv(C_OCAST<u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE u8 IsTransformFeedback(u32 id)
{
#ifndef NDEBUG
    if(!glIsTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsTransformFeedback(id);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void MinSampleShading(scalar value)
{
#ifndef NDEBUG
    if(!glMinSampleShading)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMinSampleShading(value);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void PatchParameterfv(PatchProperty pname, const scalar * values)
{
#ifndef NDEBUG
    if(!glPatchParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPatchParameterfv(to_enum(pname), values);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void PatchParameteri(PatchProperty pname, i32 value)
{
#ifndef NDEBUG
    if(!glPatchParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPatchParameteri(to_enum(pname), value);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFPause()
{
#ifndef NDEBUG
    if(!glPauseTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPauseTransformFeedback();
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFResume()
{
#ifndef NDEBUG
    if(!glResumeTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glResumeTransformFeedback();
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unif1d(i32 location, bigscalar x)
{
#ifndef NDEBUG
    if(!glUniform1d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1d(location, x);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, Span<const bigscalar> const& value)
{
#ifndef NDEBUG
    if(!glUniform1dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1dv(location, C_FCAST<i32>(value.elements), C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unif2d(i32 location, bigscalar x, bigscalar y)
{
#ifndef NDEBUG
    if(!glUniform2d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2d(location, x, y);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, Span<Vecd2> const& value)
{
#ifndef NDEBUG
    if(!glUniform2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2dv(location, C_FCAST<i32>(value.elements), C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unif3d(i32 location, bigscalar x, bigscalar y, bigscalar z)
{
#ifndef NDEBUG
    if(!glUniform3d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3d(location, x, y, z);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, Span<Vecd3> const& value)
{
#ifndef NDEBUG
    if(!glUniform3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3dv(location, C_FCAST<i32>(value.elements), C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unif4d(i32 location, bigscalar x, bigscalar y, bigscalar z, bigscalar w)
{
#ifndef NDEBUG
    if(!glUniform4d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4d(location, x, y, z, w);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, Span<Vecd4> const& value)
{
#ifndef NDEBUG
    if(!glUniform4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4dv(location, C_FCAST<i32>(value.elements), C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, bool transpose, Span<Matd2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2dv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, bool transpose, Span<Matd2_3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2x3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2x3dv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, bool transpose, Span<Matd2_4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2x4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2x4dv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, bool transpose, Span<Matd3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3dv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, bool transpose, Span<Matd3_2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3x2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3x2dv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, bool transpose, Span<Matd3_4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3x4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3x4dv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, bool transpose, Span<Matd4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4dv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, bool transpose, Span<Matd4_2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4x2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4x2dv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(i32 location, bool transpose, Span<Matd4_3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4x3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4x3dv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void UnifSubroutinesuiv(ShaderStage shadertype, i32 count, const u32 * indices)
{
#ifndef NDEBUG
    if(!glUniformSubroutinesuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformSubroutinesuiv(to_enum1(shadertype), count, indices);
}
#endif

};
template<typename ReqVer>
struct CGL_41
    : CGL_40<ReqVer>
{
using Parent = CGL_40<ReqVer>;
using Parent::Unif1f;
using Parent::Uniffv;
using Parent::Unif1i;
using Parent::Unifiv;
using Parent::Unif2f;
using Parent::Unif2i;
using Parent::Unif3f;
using Parent::Unif3i;
using Parent::Unif4f;
using Parent::Unif4i;
using Parent::Unif1ui;
using Parent::Unifuiv;
using Parent::Unif2ui;
using Parent::Unif3ui;
using Parent::Unif4ui;
using Parent::Unif1d;
using Parent::Unifdv;
using Parent::Unif2d;
using Parent::Unif3d;
using Parent::Unif4d;

#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void ProgramActiveShader(glhnd const& pipeline, glhnd const& program)
{
#ifndef NDEBUG
    if(!glActiveShaderProgram)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glActiveShaderProgram(C_OCAST<u32>(pipeline), C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineBind(glhnd const& pipeline)
{
#ifndef NDEBUG
    if(!glBindProgramPipeline)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindProgramPipeline(C_OCAST<u32>(pipeline));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void ClearDepthf(scalar d)
{
#ifndef NDEBUG
    if(!glClearDepthf)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearDepthf(d);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE u32 ShaderProgramvAllocEx(ShaderStage type, Span<const GLchar *> const& strings)
{
#ifndef NDEBUG
    if(!glCreateShaderProgramv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glCreateShaderProgramv(to_enum1(type), C_FCAST<i32>(strings.elements), strings.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineFree(Span<const u32> const& pipelines)
{
#ifndef NDEBUG
    if(!glDeleteProgramPipelines)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteProgramPipelines(C_FCAST<i32>(pipelines.elements), pipelines.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void DepthRangeArrayv(u32 first, i32 count, const bigscalar * v)
{
#ifndef NDEBUG
    if(!glDepthRangeArrayv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthRangeArrayv(first, count, v);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void DepthRangeIndexed(u32 index, bigscalar n, bigscalar f)
{
#ifndef NDEBUG
    if(!glDepthRangeIndexed)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthRangeIndexed(index, n, f);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void DepthRangef(scalar n, scalar f)
{
#ifndef NDEBUG
    if(!glDepthRangef)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthRangef(n, f);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineAlloc(Span<u32> const& pipelines)
{
#ifndef NDEBUG
    if(!glGenProgramPipelines)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenProgramPipelines(C_FCAST<i32>(pipelines.elements), pipelines.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ScalarGeti_v(GLenum target, u32 index, bigscalar * data)
{
#ifndef NDEBUG
    if(!glGetDoublei_v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetDoublei_v(target, index, data);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ScalarfGeti_v(GLenum target, u32 index, scalar * data)
{
#ifndef NDEBUG
    if(!glGetFloati_v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetFloati_v(target, index, data);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x300)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_30)
STATICINLINE void ProgramGetBinary(glhnd const& program, i32 * length, GLenum * binaryFormat, Bytes const& binary)
{
#ifndef NDEBUG
    if(!glGetProgramBinary)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramBinary(C_OCAST<u32>(program), C_FCAST<i32>(binary.size), length, binaryFormat, binary.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineGetInfoLog(glhnd const& pipeline, i32 bufSize, i32 * length, GLchar * infoLog)
{
#ifndef NDEBUG
    if(!glGetProgramPipelineInfoLog)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramPipelineInfoLog(C_OCAST<u32>(pipeline), bufSize, length, infoLog);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineGetiv(glhnd const& pipeline, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramPipelineiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramPipelineiv(C_OCAST<u32>(pipeline), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void ShaderGetPrecisionFormat(ShaderStage shadertype, GLenum precisiontype, i32 * range, i32 * precision)
{
#ifndef NDEBUG
    if(!glGetShaderPrecisionFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderPrecisionFormat(to_enum1(shadertype), precisiontype, range, precision);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOGetLdv(u32 index, GLenum pname, bigscalar * params)
{
#ifndef NDEBUG
    if(!glGetVertexAttribLdv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribLdv(index, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE u8 IsProgramPipeline(glhnd const& pipeline)
{
#ifndef NDEBUG
    if(!glIsProgramPipeline)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsProgramPipeline(C_OCAST<u32>(pipeline));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x300)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_30)
STATICINLINE void ProgramBinary(glhnd const& program, GLenum binaryFormat, const void * binary, i32 length)
{
#ifndef NDEBUG
    if(!glProgramBinary)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramBinary(C_OCAST<u32>(program), binaryFormat, binary, length);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x300)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_30)
STATICINLINE void ProgramParameteri(glhnd const& program, GLenum pname, i32 value)
{
#ifndef NDEBUG
    if(!glProgramParameteri)
    {
#if defined(GL_ARB_geometry_shader4) && GL_ARB_geometry_shader4
        if(glProgramParameteriARB) Throw(undefined_behavior("extension GL_ARB_geometry_shader4 is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramParameteri(C_OCAST<u32>(program), pname, value);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unif1d(glhnd const& program, i32 location, bigscalar v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1d(C_OCAST<u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, Span<const bigscalar> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif1f(glhnd const& program, i32 location, scalar v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1f(C_OCAST<u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif1i(glhnd const& program, i32 location, i32 v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1i(C_OCAST<u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, i32 location, Span<const i32> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1iv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif1ui(glhnd const& program, i32 location, u32 v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1ui(C_OCAST<u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, i32 location, Span<const u32> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1uiv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unif2d(glhnd const& program, i32 location, bigscalar v0, bigscalar v1)
{
#ifndef NDEBUG
    if(!glProgramUniform2d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2d(C_OCAST<u32>(program), location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, Span<Vecd2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif2f(glhnd const& program, i32 location, scalar v0, scalar v1)
{
#ifndef NDEBUG
    if(!glProgramUniform2f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2f(C_OCAST<u32>(program), location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, Span<Vecf2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif2i(glhnd const& program, i32 location, i32 v0, i32 v1)
{
#ifndef NDEBUG
    if(!glProgramUniform2i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2i(C_OCAST<u32>(program), location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, i32 location, Span<Veci2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2iv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif2ui(glhnd const& program, i32 location, u32 v0, u32 v1)
{
#ifndef NDEBUG
    if(!glProgramUniform2ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2ui(C_OCAST<u32>(program), location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, i32 location, Span<Vecui2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2uiv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unif3d(glhnd const& program, i32 location, bigscalar v0, bigscalar v1, bigscalar v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3d(C_OCAST<u32>(program), location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, Span<Vecd3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif3f(glhnd const& program, i32 location, scalar v0, scalar v1, scalar v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3f(C_OCAST<u32>(program), location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, Span<Vecf3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif3i(glhnd const& program, i32 location, i32 v0, i32 v1, i32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3i(C_OCAST<u32>(program), location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, i32 location, Span<Veci3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3iv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif3ui(glhnd const& program, i32 location, u32 v0, u32 v1, u32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3ui(C_OCAST<u32>(program), location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, i32 location, Span<Vecui3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3uiv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unif4d(glhnd const& program, i32 location, bigscalar v0, bigscalar v1, bigscalar v2, bigscalar v3)
{
#ifndef NDEBUG
    if(!glProgramUniform4d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4d(C_OCAST<u32>(program), location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, Span<Vecd4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif4f(glhnd const& program, i32 location, scalar v0, scalar v1, scalar v2, scalar v3)
{
#ifndef NDEBUG
    if(!glProgramUniform4f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4f(C_OCAST<u32>(program), location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, Span<Vecf4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif4i(glhnd const& program, i32 location, i32 v0, i32 v1, i32 v2, i32 v3)
{
#ifndef NDEBUG
    if(!glProgramUniform4i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4i(C_OCAST<u32>(program), location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, i32 location, Span<Veci4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4iv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif4ui(glhnd const& program, i32 location, u32 v0, u32 v1, u32 v2, u32 v3)
{
#ifndef NDEBUG
    if(!glProgramUniform4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4ui(C_OCAST<u32>(program), location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, i32 location, Span<Vecui4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4uiv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, bool transpose, Span<Matd2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, bool transpose, Span<Matd2_3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2x3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2x3dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf2_3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2x3fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, bool transpose, Span<Matd2_4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2x4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2x4dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf2_4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2x4fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, bool transpose, Span<Matd3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, bool transpose, Span<Matd3_2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3x2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3x2dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf3_2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3x2fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, bool transpose, Span<Matd3_4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3x4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3x4dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf3_4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3x4fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, bool transpose, Span<Matd4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, bool transpose, Span<Matd4_2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4x2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4x2dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf4_2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4x2fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, i32 location, bool transpose, Span<Matd4_3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4x3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4x3dv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const bigscalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf4_3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4x3fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void ShaderReleaseCompiler()
{
#ifndef NDEBUG
    if(!glReleaseShaderCompiler)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReleaseShaderCompiler();
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ScissorArrayv(u32 first, i32 count, const i32 * v)
{
#ifndef NDEBUG
    if(!glScissorArrayv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glScissorArrayv(first, count, v);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ScissorIndexed(u32 index, i32 left, i32 bottom, Size const& size)
{
#ifndef NDEBUG
    if(!glScissorIndexed)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glScissorIndexed(index, left, bottom, size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ScissorIndexedv(u32 index, const i32 * v)
{
#ifndef NDEBUG
    if(!glScissorIndexedv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glScissorIndexedv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void ShaderBinary(i32 count, const u32 * shaders, GLenum binaryformat, const void * binary, i32 length)
{
#ifndef NDEBUG
    if(!glShaderBinary)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glShaderBinary(count, shaders, binaryformat, binary, length);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void ProgramUseStages(glhnd const& pipeline, ShaderStage stages, glhnd const& program)
{
#ifndef NDEBUG
    if(!glUseProgramStages)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUseProgramStages(C_OCAST<u32>(pipeline), to_enum2(stages), C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineValidate(glhnd const& pipeline)
{
#ifndef NDEBUG
    if(!glValidateProgramPipeline)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glValidateProgramPipeline(C_OCAST<u32>(pipeline));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOL1d(u32 index, bigscalar x)
{
#ifndef NDEBUG
    if(!glVertexAttribL1d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL1d(index, x);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOL1dv(u32 index, const bigscalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttribL1dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL1dv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOL2d(u32 index, bigscalar x, bigscalar y)
{
#ifndef NDEBUG
    if(!glVertexAttribL2d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL2d(index, x, y);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOL2dv(u32 index, const bigscalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttribL2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL2dv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOL3d(u32 index, bigscalar x, bigscalar y, bigscalar z)
{
#ifndef NDEBUG
    if(!glVertexAttribL3d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL3d(index, x, y, z);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOL3dv(u32 index, const bigscalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttribL3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL3dv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOL4d(u32 index, bigscalar x, bigscalar y, bigscalar z, bigscalar w)
{
#ifndef NDEBUG
    if(!glVertexAttribL4d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL4d(index, x, y, z, w);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOL4dv(u32 index, const bigscalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttribL4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL4dv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOLPointer(u32 index, i32 size, GLenum type, i32 stride, const void * pointer)
{
#ifndef NDEBUG
    if(!glVertexAttribLPointer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribLPointer(index, size, type, stride, pointer);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ViewportArrayv(u32 first, i32 count, const scalar * v)
{
#ifndef NDEBUG
    if(!glViewportArrayv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glViewportArrayv(first, count, v);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ViewportIndexedf(u32 index, scalar x, scalar y, scalar w, scalar h)
{
#ifndef NDEBUG
    if(!glViewportIndexedf)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glViewportIndexedf(index, x, y, w, h);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ViewportIndexedfv(u32 index, const scalar * v)
{
#ifndef NDEBUG
    if(!glViewportIndexedfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glViewportIndexedfv(index, v);
}
#endif

};
template<typename ReqVer>
struct CGL_42
    : CGL_41<ReqVer>
{
using Parent = CGL_41<ReqVer>;

#if GL_VERSION_VERIFY(0x420, 0x310)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_31)
STATICINLINE void TexBindImage(u32 unit, glhnd const& texture, i32 level, bool layered, i32 layer, RSCA access, PixFmt format)
{
#ifndef NDEBUG
    if(!glBindImageTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindImageTexture(unit, C_OCAST<u32>(texture), level, layered ? GL_TRUE : GL_FALSE, layer, to_enum2(access), to_enum(format));
}
#endif


#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_42)
STATICINLINE void DrawArraysInstancedBaseInstance(DrwMd const& mode, i32 first, i32 count, i32 instancecount, u32 baseinstance)
{
#ifndef NDEBUG
    if(!glDrawArraysInstancedBaseInstance)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawArraysInstancedBaseInstance(to_enum(mode.t, mode.c), first, count, instancecount, baseinstance);
}
#endif


#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_42)
STATICINLINE void DrawElementsInstancedBaseInstance(DrwMd const& mode, i32 count, TypeEnum type, uintptr indices, i32 instancecount, u32 baseinstance)
{
#ifndef NDEBUG
    if(!glDrawElementsInstancedBaseInstance)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElementsInstancedBaseInstance(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void *>(indices), instancecount, baseinstance);
}
#endif


#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_42)
STATICINLINE void DrawElementsInstancedBaseVertexBaseInstance(DrwMd const& mode, i32 count, TypeEnum type, uintptr indices, i32 instancecount, i32 basevertex, u32 baseinstance)
{
#ifndef NDEBUG
    if(!glDrawElementsInstancedBaseVertexBaseInstance)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElementsInstancedBaseVertexBaseInstance(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void *>(indices), instancecount, basevertex, baseinstance);
}
#endif


#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_42)
STATICINLINE void DrawTransformFeedbackInstanced(DrwMd const& mode, u32 id, i32 instancecount)
{
#ifndef NDEBUG
    if(!glDrawTransformFeedbackInstanced)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawTransformFeedbackInstanced(to_enum(mode.t, mode.c), id, instancecount);
}
#endif


#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_42)
STATICINLINE void DrawTransformFeedbackStreamInstanced(DrwMd const& mode, u32 id, u32 stream, i32 instancecount)
{
#ifndef NDEBUG
    if(!glDrawTransformFeedbackStreamInstanced)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawTransformFeedbackStreamInstanced(to_enum(mode.t, mode.c), id, stream, instancecount);
}
#endif


#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_42)
STATICINLINE void BufGetActiveAtomicCounteriv(glhnd const& program, u32 bufferIndex, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetActiveAtomicCounterBufferiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveAtomicCounterBufferiv(C_OCAST<u32>(program), bufferIndex, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x300)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_30)
STATICINLINE void GetInternalformativ(TexComp::tex_flag const& target, PixFmt internalformat, GLenum pname, i32 bufSize, i32 * params)
{
#ifndef NDEBUG
    if(!glGetInternalformativ)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetInternalformativ(texture_to_enum(target), to_enum(internalformat), pname, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x310)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_31)
STATICINLINE void MemoryBarrier(u32 barriers)
{
#ifndef NDEBUG
    if(!glMemoryBarrier)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMemoryBarrier(barriers);
}
#endif


#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_42)
STATICINLINE void TexStorage1D(TexComp::tex_flag const& target, i32 levels, PixFmt internalformat, i32 width)
{
#ifndef NDEBUG
    if(!glTexStorage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage1D(texture_to_enum(target), levels, to_enum(internalformat), width);
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x300)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_30)
STATICINLINE void TexStorage2D(TexComp::tex_flag const& target, i32 levels, PixFmt internalformat, Size const& size)
{
#ifndef NDEBUG
    if(!glTexStorage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage2D(texture_to_enum(target), levels, to_enum(internalformat), size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x300)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_30)
STATICINLINE void TexStorage3D(TexComp::tex_flag const& target, i32 levels, PixFmt internalformat, Size3 const& size)
{
#ifndef NDEBUG
    if(!glTexStorage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage3D(texture_to_enum(target), levels, to_enum(internalformat), size.width, size.height, size.depth);
}
#endif

};
template<typename ReqVer>
struct CGL_43
    : CGL_42<ReqVer>
{
using Parent = CGL_42<ReqVer>;

#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VertBufBind(u32 bindingindex, glhnd const& buffer, ptroff offset, i32 stride)
{
#ifndef NDEBUG
    if(!glBindVertexBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindVertexBuffer(bindingindex, C_OCAST<u32>(buffer), offset, stride);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void BufClearData(BufferComp::buf_flag const& target, PixFmt internalformat, PixCmp format, BitFmt type, const void * data)
{
#ifndef NDEBUG
    if(!glClearBufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearBufferData(buffer_to_enum(target), to_enum(internalformat), to_enum(format, PixFmt::None), to_enum(type), data);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void BufClearSubData(GLenum target, PixFmt internalformat, ptroff offset, PixCmp format, BitFmt type, BytesConst const& data)
{
#ifndef NDEBUG
    if(!glClearBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearBufferSubData(target, to_enum(internalformat), offset, C_FCAST<ptroff>(data.size), to_enum(format, PixFmt::None), to_enum(type), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void CopyImageSubData(u32 srcName, GLenum srcTarget, i32 srcLevel, i32 srcX, i32 srcY, i32 srcZ, u32 dstName, GLenum dstTarget, i32 dstLevel, i32 dstX, i32 dstY, i32 dstZ, i32 srcWidth, i32 srcHeight, i32 srcDepth)
{
#ifndef NDEBUG
    if(!glCopyImageSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyImageSubData(srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void DebugMessageCallback(GLDEBUGPROC callback, const void * userParam)
{
#ifndef NDEBUG
    if(!glDebugMessageCallback)
    {
#if defined(GL_ARB_debug_output) && GL_ARB_debug_output
        if(glDebugMessageCallbackARB) Throw(undefined_behavior("extension GL_ARB_debug_output is available"));
#endif
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glDebugMessageCallbackKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageCallback(callback, userParam);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void DebugMessageControl(GLenum source, GLenum type, GLenum severity, i32 count, const u32 * ids, bool enabled)
{
#ifndef NDEBUG
    if(!glDebugMessageControl)
    {
#if defined(GL_ARB_debug_output) && GL_ARB_debug_output
        if(glDebugMessageControlARB) Throw(undefined_behavior("extension GL_ARB_debug_output is available"));
#endif
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glDebugMessageControlKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageControl(source, type, severity, count, ids, enabled ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void DebugMessageInsert(GLenum source, GLenum type, u32 id, GLenum severity, i32 length, const GLchar * buf)
{
#ifndef NDEBUG
    if(!glDebugMessageInsert)
    {
#if defined(GL_ARB_debug_output) && GL_ARB_debug_output
        if(glDebugMessageInsertARB) Throw(undefined_behavior("extension GL_ARB_debug_output is available"));
#endif
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glDebugMessageInsertKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageInsert(source, type, id, severity, length, buf);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void DispatchCompute(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z)
{
#ifndef NDEBUG
    if(!glDispatchCompute)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void DispatchComputeIndirect(ptroff indirect)
{
#ifndef NDEBUG
    if(!glDispatchComputeIndirect)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDispatchComputeIndirect(indirect);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void FBParameteri(FramebufferT target, GLenum pname, i32 param)
{
#ifndef NDEBUG
    if(!glFramebufferParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferParameteri(to_enum(target), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE u32 GetDebugMessageLog(u32 count, i32 bufSize, GLenum * sources, GLenum * types, u32 * ids, GLenum * severities, i32 * lengths, GLchar * messageLog)
{
#ifndef NDEBUG
    if(!glGetDebugMessageLog)
    {
#if defined(GL_ARB_debug_output) && GL_ARB_debug_output
        if(glGetDebugMessageLogARB) Throw(undefined_behavior("extension GL_ARB_debug_output is available"));
#endif
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glGetDebugMessageLogKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetDebugMessageLog(count, bufSize, sources, types, ids, severities, lengths, messageLog);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void FBGetParameteriv(FramebufferT target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetFramebufferParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetFramebufferParameteriv(to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void GetInternalformati64v(TexComp::tex_flag const& target, PixFmt internalformat, GLenum pname, i32 bufSize, i64 * params)
{
#ifndef NDEBUG
    if(!glGetInternalformati64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetInternalformati64v(texture_to_enum(target), to_enum(internalformat), pname, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void GetObjectLabel(GLenum identifier, u32 name, i32 bufSize, i32 * length, GLchar * label)
{
#ifndef NDEBUG
    if(!glGetObjectLabel)
    {
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glGetObjectLabelKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetObjectLabel(identifier, name, bufSize, length, label);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void GetObjectPtrLabel(const void * ptr, i32 bufSize, i32 * length, GLchar * label)
{
#ifndef NDEBUG
    if(!glGetObjectPtrLabel)
    {
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glGetObjectPtrLabelKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetObjectPtrLabel(ptr, bufSize, length, label);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void ProgramGetInterfaceiv(glhnd const& program, GLenum programInterface, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramInterfaceiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramInterfaceiv(C_OCAST<u32>(program), programInterface, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE u32 ProgramGetResourceIndex(glhnd const& program, GLenum programInterface, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetProgramResourceIndex(C_OCAST<u32>(program), programInterface, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE i32 ProgramGetResourceLocation(glhnd const& program, GLenum programInterface, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetProgramResourceLocation(C_OCAST<u32>(program), programInterface, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE i32 ProgramGetResourceLocationIndex(glhnd const& program, GLenum programInterface, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceLocationIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetProgramResourceLocationIndex(C_OCAST<u32>(program), programInterface, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void ProgramGetResourceName(glhnd const& program, GLenum programInterface, u32 index, i32 bufSize, i32 * length, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramResourceName(C_OCAST<u32>(program), programInterface, index, bufSize, length, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void ProgramGetResourceiv(glhnd const& program, GLenum programInterface, u32 index, i32 propCount, const GLenum * props, i32 bufSize, i32 * length, i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramResourceiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramResourceiv(C_OCAST<u32>(program), programInterface, index, propCount, props, bufSize, length, params);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void BufInvalidateData(glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glInvalidateBufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateBufferData(C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void BufInvalidateSubData(glhnd const& buffer, ptroff offset, ptroff length)
{
#ifndef NDEBUG
    if(!glInvalidateBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateBufferSubData(C_OCAST<u32>(buffer), offset, length);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x300)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_30)
STATICINLINE void FBInvalidate(FramebufferT target, i32 numAttachments, const GLenum * attachments)
{
#ifndef NDEBUG
    if(!glInvalidateFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateFramebuffer(to_enum(target), numAttachments, attachments);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x300)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_30)
STATICINLINE void FBInvalidateSub(GLenum target, i32 numAttachments, const GLenum * attachments, i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glInvalidateSubFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateSubFramebuffer(target, numAttachments, attachments, x, y, size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void TexInvalidateImage(glhnd const& texture, i32 level)
{
#ifndef NDEBUG
    if(!glInvalidateTexImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateTexImage(C_OCAST<u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void TexInvalidateSubImage(glhnd const& texture, i32 level, Point3 const& offset, Size3 const& size)
{
#ifndef NDEBUG
    if(!glInvalidateTexSubImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateTexSubImage(C_OCAST<u32>(texture), level, offset.x, offset.y, offset.z, size.width, size.height, size.depth);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void MultiDrawArraysIndirect(DrwMd const& mode, uintptr indirect, i32 drawcount, i32 stride)
{
#ifndef NDEBUG
    if(!glMultiDrawArraysIndirect)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMultiDrawArraysIndirect(to_enum(mode.t, mode.c), C_RCAST<const void *>(indirect), drawcount, stride);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void MultiDrawElementsIndirect(DrwMd const& mode, TypeEnum type, uintptr indirect, i32 drawcount, i32 stride)
{
#ifndef NDEBUG
    if(!glMultiDrawElementsIndirect)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMultiDrawElementsIndirect(to_enum(mode.t, mode.c), to_enum(type), C_RCAST<const void *>(indirect), drawcount, stride);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void ObjectLabel(GLenum identifier, u32 name, i32 length, const GLchar * label)
{
#ifndef NDEBUG
    if(!glObjectLabel)
    {
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glObjectLabelKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glObjectLabel(identifier, name, length, label);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void ObjectPtrLabel(const void * ptr, i32 length, const GLchar * label)
{
#ifndef NDEBUG
    if(!glObjectPtrLabel)
    {
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glObjectPtrLabelKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glObjectPtrLabel(ptr, length, label);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void PopDebugGroup()
{
#ifndef NDEBUG
    if(!glPopDebugGroup)
    {
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glPopDebugGroupKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPopDebugGroup();
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void PushDebugGroup(GLenum source, u32 id, i32 length, const GLchar * message)
{
#ifndef NDEBUG
    if(!glPushDebugGroup)
    {
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glPushDebugGroupKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPushDebugGroup(source, id, length, message);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void SSBOBinding(glhnd const& program, u32 storageBlockIndex, u32 storageBlockBinding)
{
#ifndef NDEBUG
    if(!glShaderStorageBlockBinding)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glShaderStorageBlockBinding(C_OCAST<u32>(program), storageBlockIndex, storageBlockBinding);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void TexBufferRange(TexComp::tex_flag const& target, PixFmt internalformat, glhnd const& buffer, ptroff offset, ptroff size)
{
#ifndef NDEBUG
    if(!glTexBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexBufferRange(texture_to_enum(target), to_enum(internalformat), C_OCAST<u32>(buffer), offset, size);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void TexStorage2DMultisample(TexComp::tex_flag const& target, i32 samples, PixFmt internalformat, Size const& size, bool fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTexStorage2DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage2DMultisample(texture_to_enum(target), samples, to_enum(internalformat), size.w, size.h, fixedsamplelocations ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void TexStorage3DMultisample(TexComp::tex_flag const& target, i32 samples, PixFmt internalformat, Size3 const& size, bool fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTexStorage3DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage3DMultisample(texture_to_enum(target), samples, to_enum(internalformat), size.width, size.height, size.depth, fixedsamplelocations ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void TexView(glhnd const& texture, TexComp::tex_flag const& target, u32 origtexture, PixFmt internalformat, u32 minlevel, u32 numlevels, u32 minlayer, u32 numlayers)
{
#ifndef NDEBUG
    if(!glTextureView)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureView(C_OCAST<u32>(texture), texture_to_enum(target), origtexture, to_enum(internalformat), minlevel, numlevels, minlayer, numlayers);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VAOBinding(u32 attribindex, u32 bindingindex)
{
#ifndef NDEBUG
    if(!glVertexAttribBinding)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribBinding(attribindex, bindingindex);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VAOFormat(u32 attribindex, i32 size, GLenum type, bool normalized, u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexAttribFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribFormat(attribindex, size, type, normalized ? GL_TRUE : GL_FALSE, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VAOIFormat(u32 attribindex, i32 size, GLenum type, u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexAttribIFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribIFormat(attribindex, size, type, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void VAOLFormat(u32 attribindex, i32 size, GLenum type, u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexAttribLFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribLFormat(attribindex, size, type, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VAOBindingDivisor(u32 bindingindex, u32 divisor)
{
#ifndef NDEBUG
    if(!glVertexBindingDivisor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexBindingDivisor(bindingindex, divisor);
}
#endif

};
template<typename ReqVer>
struct CGL_44
    : CGL_43<ReqVer>
{
using Parent = CGL_43<ReqVer>;
using Parent::TexBind;
using Parent::BufBindBase;
using Parent::BufBindRange;
using Parent::SamplerBind;
using Parent::TexBindImage;
using Parent::VertBufBind;

#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_44)
STATICINLINE void BufBindBase(BufferComp::buf_flag const& target, u32 first, i32 count, const u32 * buffers)
{
#ifndef NDEBUG
    if(!glBindBuffersBase)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindBuffersBase(buffer_to_enum(target), first, count, buffers);
}
#endif


#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_44)
STATICINLINE void BufBindRange(BufferComp::buf_flag const& target, u32 first, i32 count, const u32 * buffers, const ptroff * offsets, const ptroff * sizes)
{
#ifndef NDEBUG
    if(!glBindBuffersRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindBuffersRange(buffer_to_enum(target), first, count, buffers, offsets, sizes);
}
#endif


#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_44)
STATICINLINE void TexBindImage(u32 first, i32 count, const u32 * textures)
{
#ifndef NDEBUG
    if(!glBindImageTextures)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindImageTextures(first, count, textures);
}
#endif


#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_44)
STATICINLINE void SamplerBind(u32 first, i32 count, const u32 * samplers)
{
#ifndef NDEBUG
    if(!glBindSamplers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindSamplers(first, count, samplers);
}
#endif


#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_44)
STATICINLINE void TexBind(u32 first, i32 count, const u32 * textures)
{
#ifndef NDEBUG
    if(!glBindTextures)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindTextures(first, count, textures);
}
#endif


#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_44)
STATICINLINE void VertBufBind(u32 first, i32 count, const u32 * buffers, const ptroff * offsets, const i32 * strides)
{
#ifndef NDEBUG
    if(!glBindVertexBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindVertexBuffers(first, count, buffers, offsets, strides);
}
#endif


#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_44)
STATICINLINE void BufStorage(BufferComp::buf_flag const& target, BytesConst const& data, RSCA flags)
{
#ifndef NDEBUG
    if(!glBufferStorage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBufferStorage(buffer_to_enum(target), C_FCAST<ptroff>(data.size), data.data, to_enum2(flags));
}
#endif


#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_44)
STATICINLINE void TexClearImage(glhnd const& texture, i32 level, PixCmp format, BitFmt type, const void * data)
{
#ifndef NDEBUG
    if(!glClearTexImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearTexImage(C_OCAST<u32>(texture), level, to_enum(format, PixFmt::None), to_enum(type), data);
}
#endif


#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_44)
STATICINLINE void TexClearSubImage(glhnd const& texture, i32 level, Point3 const& offset, Size3 const& size, PixCmp format, BitFmt type, const void * data)
{
#ifndef NDEBUG
    if(!glClearTexSubImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearTexSubImage(C_OCAST<u32>(texture), level, offset.x, offset.y, offset.z, size.width, size.height, size.depth, to_enum(format, PixFmt::None), to_enum(type), data);
}
#endif

};
template<typename ReqVer>
struct CGL_45
    : CGL_44<ReqVer>
{
using Parent = CGL_44<ReqVer>;
using Parent::TexGetImage;
using Parent::TexGetLevelParameterfv;
using Parent::TexGetLevelParameteriv;
using Parent::TexGetParameterfv;
using Parent::TexGetParameteriv;
using Parent::TexParameterf;
using Parent::TexParameterfv;
using Parent::TexParameteri;
using Parent::TexParameteriv;
using Parent::TexCopySubImage1D;
using Parent::TexCopySubImage2D;
using Parent::TexSubImage1D;
using Parent::TexSubImage2D;
using Parent::TexCopySubImage3D;
using Parent::TexSubImage3D;
using Parent::TexCompressedSubImage1D;
using Parent::TexCompressedSubImage2D;
using Parent::TexCompressedSubImage3D;
using Parent::TexGetCompressedImage;
using Parent::BufData;
using Parent::BufSubData;
using Parent::BufGetParameteriv;
using Parent::BufGetPointerv;
using Parent::BufGetSubData;
using Parent::BufMap;
using Parent::BufUnmap;
using Parent::VAOGetiv;
using Parent::BufFlushMappedRange;
using Parent::RBufGetParameteriv;
using Parent::TexGetParameterIiv;
using Parent::TexGetParameterIuiv;
using Parent::BufMapRange;
using Parent::RBufStorage;
using Parent::RBufStorageMultisample;
using Parent::TexParameterIiv;
using Parent::TexParameterIuiv;
using Parent::BufCopySubData;
using Parent::TexBuffer;
using Parent::BufGetParameteri64v;
using Parent::TexStorage1D;
using Parent::TexStorage2D;
using Parent::TexStorage3D;
using Parent::BufClearData;
using Parent::BufClearSubData;
using Parent::TexBufferRange;
using Parent::TexStorage2DMultisample;
using Parent::TexStorage3DMultisample;
using Parent::VAOBinding;
using Parent::VAOFormat;
using Parent::VAOIFormat;
using Parent::VAOLFormat;
using Parent::VAOBindingDivisor;
using Parent::BufStorage;

#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexBindUnit(u32 unit, glhnd const& texture)
{
#ifndef NDEBUG
    if(!glBindTextureUnit)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindTextureUnit(unit, C_OCAST<u32>(texture));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBBlitNamed(u32 readFramebuffer, u32 drawFramebuffer, i32 srcX0, i32 srcY0, i32 srcX1, i32 srcY1, i32 dstX0, i32 dstY0, i32 dstX1, i32 dstY1, u32 mask, GLenum filter)
{
#ifndef NDEBUG
    if(!glBlitNamedFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlitNamedFramebuffer(readFramebuffer, drawFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE GLenum FBCheckNamedStatus(u32 framebuffer, FramebufferT target)
{
#ifndef NDEBUG
    if(!glCheckNamedFramebufferStatus)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glCheckNamedFramebufferStatus(framebuffer, to_enum(target));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufClearData(glhnd const& buffer, PixFmt internalformat, PixCmp format, BitFmt type, const void * data)
{
#ifndef NDEBUG
    if(!glClearNamedBufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearNamedBufferData(C_OCAST<u32>(buffer), to_enum(internalformat), to_enum(format, PixFmt::None), to_enum(type), data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufClearSubData(glhnd const& buffer, PixFmt internalformat, ptroff offset, PixCmp format, BitFmt type, BytesConst const& data)
{
#ifndef NDEBUG
    if(!glClearNamedBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearNamedBufferSubData(C_OCAST<u32>(buffer), to_enum(internalformat), offset, C_FCAST<ptroff>(data.size), to_enum(format, PixFmt::None), to_enum(type), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBClearNamedfi(u32 framebuffer, GLenum buffer, i32 drawbuffer, scalar depth, i32 stencil)
{
#ifndef NDEBUG
    if(!glClearNamedFramebufferfi)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearNamedFramebufferfi(framebuffer, buffer, drawbuffer, depth, stencil);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBClearNamedfv(u32 framebuffer, GLenum buffer, i32 drawbuffer, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glClearNamedFramebufferfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearNamedFramebufferfv(framebuffer, buffer, drawbuffer, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBClearNamediv(u32 framebuffer, GLenum buffer, i32 drawbuffer, Span<const i32> const& value)
{
#ifndef NDEBUG
    if(!glClearNamedFramebufferiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearNamedFramebufferiv(framebuffer, buffer, drawbuffer, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBClearNameduiv(u32 framebuffer, GLenum buffer, i32 drawbuffer, Span<const u32> const& value)
{
#ifndef NDEBUG
    if(!glClearNamedFramebufferuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearNamedFramebufferuiv(framebuffer, buffer, drawbuffer, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void ClipControl(GLenum origin, GLenum depth)
{
#ifndef NDEBUG
    if(!glClipControl)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClipControl(origin, depth);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexCompressedSubImage1D(glhnd const& texture, i32 level, i32 xoffset, i32 width, CompFmt format, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTextureSubImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTextureSubImage1D(C_OCAST<u32>(texture), level, xoffset, width, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexCompressedSubImage2D(glhnd const& texture, i32 level, Point const& offset, Size const& size, CompFmt format, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTextureSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTextureSubImage2D(C_OCAST<u32>(texture), level, offset.x, offset.y, size.w, size.h, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexCompressedSubImage3D(glhnd const& texture, i32 level, Point3 const& offset, Size3 const& size, CompFmt format, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTextureSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTextureSubImage3D(C_OCAST<u32>(texture), level, offset.x, offset.y, offset.z, size.width, size.height, size.depth, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufCopySubData(u32 readBuffer, u32 writeBuffer, ptroff readOffset, ptroff writeOffset, ptroff size)
{
#ifndef NDEBUG
    if(!glCopyNamedBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyNamedBufferSubData(readBuffer, writeBuffer, readOffset, writeOffset, size);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexCopySubImage1D(glhnd const& texture, i32 level, i32 xoffset, i32 x, i32 y, i32 width)
{
#ifndef NDEBUG
    if(!glCopyTextureSubImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTextureSubImage1D(C_OCAST<u32>(texture), level, xoffset, x, y, width);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexCopySubImage2D(glhnd const& texture, i32 level, Point const& offset, i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glCopyTextureSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTextureSubImage2D(C_OCAST<u32>(texture), level, offset.x, offset.y, x, y, size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexCopySubImage3D(glhnd const& texture, i32 level, Point3 const& offset, i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glCopyTextureSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTextureSubImage3D(C_OCAST<u32>(texture), level, offset.x, offset.y, offset.z, x, y, size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufAllocEx(Span<u32> const& buffers)
{
#ifndef NDEBUG
    if(!glCreateBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateBuffers(C_FCAST<i32>(buffers.elements), buffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBAllocEx(Span<u32> const& framebuffers)
{
#ifndef NDEBUG
    if(!glCreateFramebuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateFramebuffers(C_FCAST<i32>(framebuffers.elements), framebuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void PipelineAllocEx(Span<u32> const& pipelines)
{
#ifndef NDEBUG
    if(!glCreateProgramPipelines)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateProgramPipelines(C_FCAST<i32>(pipelines.elements), pipelines.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void QueryAllocEx(QueryComp::query_flag const& target, Span<u32> const& ids)
{
#ifndef NDEBUG
    if(!glCreateQueries)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateQueries(query_to_enum(target), C_FCAST<i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void RBufAllocEx(Span<u32> const& renderbuffers)
{
#ifndef NDEBUG
    if(!glCreateRenderbuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateRenderbuffers(C_FCAST<i32>(renderbuffers.elements), renderbuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void SamplerAllocEx(Span<u32> const& samplers)
{
#ifndef NDEBUG
    if(!glCreateSamplers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateSamplers(C_FCAST<i32>(samplers.elements), samplers.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexAllocEx(TexComp::tex_flag const& target, Span<u32> const& textures)
{
#ifndef NDEBUG
    if(!glCreateTextures)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateTextures(texture_to_enum(target), C_FCAST<i32>(textures.elements), textures.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void XFAllocEx(Span<u32> const& ids)
{
#ifndef NDEBUG
    if(!glCreateTransformFeedbacks)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateTransformFeedbacks(C_FCAST<i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOAllocEx(Span<u32> const& arrays)
{
#ifndef NDEBUG
    if(!glCreateVertexArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateVertexArrays(C_FCAST<i32>(arrays.elements), arrays.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAODisable(glhnd const& vaobj, u32 index)
{
#ifndef NDEBUG
    if(!glDisableVertexArrayAttrib)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDisableVertexArrayAttrib(C_OCAST<u32>(vaobj), index);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOEnable(glhnd const& vaobj, u32 index)
{
#ifndef NDEBUG
    if(!glEnableVertexArrayAttrib)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEnableVertexArrayAttrib(C_OCAST<u32>(vaobj), index);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufFlushMappedRange(glhnd const& buffer, ptroff offset, ptroff length)
{
#ifndef NDEBUG
    if(!glFlushMappedNamedBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFlushMappedNamedBufferRange(C_OCAST<u32>(buffer), offset, length);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGenerateMipmap(glhnd const& texture)
{
#ifndef NDEBUG
    if(!glGenerateTextureMipmap)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenerateTextureMipmap(C_OCAST<u32>(texture));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetCompressedImage(glhnd const& texture, i32 level, i32 bufSize, void * pixels)
{
#ifndef NDEBUG
    if(!glGetCompressedTextureImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetCompressedTextureImage(C_OCAST<u32>(texture), level, bufSize, pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetCompressedSubImage(glhnd const& texture, i32 level, Point3 const& offset, Size3 const& size, i32 bufSize, void * pixels)
{
#ifndef NDEBUG
    if(!glGetCompressedTextureSubImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetCompressedTextureSubImage(C_OCAST<u32>(texture), level, offset.x, offset.y, offset.z, size.width, size.height, size.depth, bufSize, pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE GLenum GetGraphicsResetStatus()
{
#ifndef NDEBUG
    if(!glGetGraphicsResetStatus)
    {
#if defined(GL_KHR_robustness) && GL_KHR_robustness
        if(glGetGraphicsResetStatusKHR) Throw(undefined_behavior("extension GL_KHR_robustness is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetGraphicsResetStatus();
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufGetParameteri64v(glhnd const& buffer, GLenum pname, i64 * params)
{
#ifndef NDEBUG
    if(!glGetNamedBufferParameteri64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedBufferParameteri64v(C_OCAST<u32>(buffer), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufGetParameteriv(glhnd const& buffer, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetNamedBufferParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedBufferParameteriv(C_OCAST<u32>(buffer), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufGetPointerv(glhnd const& buffer, GLenum pname, void * * params)
{
#ifndef NDEBUG
    if(!glGetNamedBufferPointerv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedBufferPointerv(C_OCAST<u32>(buffer), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufGetSubData(glhnd const& buffer, ptroff offset, Bytes const& data)
{
#ifndef NDEBUG
    if(!glGetNamedBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedBufferSubData(C_OCAST<u32>(buffer), offset, C_FCAST<ptroff>(data.size), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBGetNamedAttachmentParameteriv(u32 framebuffer, GLenum attachment, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetNamedFramebufferAttachmentParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedFramebufferAttachmentParameteriv(framebuffer, attachment, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBGetNamedParameteriv(u32 framebuffer, GLenum pname, i32 * param)
{
#ifndef NDEBUG
    if(!glGetNamedFramebufferParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedFramebufferParameteriv(framebuffer, pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void RBufGetParameteriv(u32 renderbuffer, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetNamedRenderbufferParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedRenderbufferParameteriv(renderbuffer, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void QueryGetBufferObjecti64v(u32 id, glhnd const& buffer, GLenum pname, ptroff offset)
{
#ifndef NDEBUG
    if(!glGetQueryBufferObjecti64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryBufferObjecti64v(id, C_OCAST<u32>(buffer), pname, offset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void QueryGetBufferObjectiv(u32 id, glhnd const& buffer, GLenum pname, ptroff offset)
{
#ifndef NDEBUG
    if(!glGetQueryBufferObjectiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryBufferObjectiv(id, C_OCAST<u32>(buffer), pname, offset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void QueryGetBufferObjectui64v(u32 id, glhnd const& buffer, GLenum pname, ptroff offset)
{
#ifndef NDEBUG
    if(!glGetQueryBufferObjectui64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryBufferObjectui64v(id, C_OCAST<u32>(buffer), pname, offset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void QueryGetBufferObjectuiv(u32 id, glhnd const& buffer, GLenum pname, ptroff offset)
{
#ifndef NDEBUG
    if(!glGetQueryBufferObjectuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryBufferObjectuiv(id, C_OCAST<u32>(buffer), pname, offset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetImage(glhnd const& texture, i32 level, PixCmp format, BitFmt type, i32 bufSize, void * pixels)
{
#ifndef NDEBUG
    if(!glGetTextureImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureImage(C_OCAST<u32>(texture), level, to_enum(format, PixFmt::None), to_enum(type), bufSize, pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetLevelParameterfv(glhnd const& texture, i32 level, GLenum pname, scalar * params)
{
#ifndef NDEBUG
    if(!glGetTextureLevelParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureLevelParameterfv(C_OCAST<u32>(texture), level, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetLevelParameteriv(glhnd const& texture, i32 level, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetTextureLevelParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureLevelParameteriv(C_OCAST<u32>(texture), level, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetParameterIiv(glhnd const& texture, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetTextureParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureParameterIiv(C_OCAST<u32>(texture), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetParameterIuiv(glhnd const& texture, GLenum pname, u32 * params)
{
#ifndef NDEBUG
    if(!glGetTextureParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureParameterIuiv(C_OCAST<u32>(texture), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetParameterfv(glhnd const& texture, GLenum pname, scalar * params)
{
#ifndef NDEBUG
    if(!glGetTextureParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureParameterfv(C_OCAST<u32>(texture), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetParameteriv(glhnd const& texture, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetTextureParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureParameteriv(C_OCAST<u32>(texture), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetSubImage(glhnd const& texture, i32 level, Point3 const& offset, Size3 const& size, PixCmp format, BitFmt type, i32 bufSize, void * pixels)
{
#ifndef NDEBUG
    if(!glGetTextureSubImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureSubImage(C_OCAST<u32>(texture), level, offset.x, offset.y, offset.z, size.width, size.height, size.depth, to_enum(format, PixFmt::None), to_enum(type), bufSize, pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void XFGeti64_v(u32 xfb, GLenum pname, u32 index, i64 * param)
{
#ifndef NDEBUG
    if(!glGetTransformFeedbacki64_v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTransformFeedbacki64_v(xfb, pname, index, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void XFGeti_v(u32 xfb, GLenum pname, u32 index, i32 * param)
{
#ifndef NDEBUG
    if(!glGetTransformFeedbacki_v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTransformFeedbacki_v(xfb, pname, index, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void XFGetiv(u32 xfb, GLenum pname, i32 * param)
{
#ifndef NDEBUG
    if(!glGetTransformFeedbackiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTransformFeedbackiv(xfb, pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOGetIndexed64iv(glhnd const& vaobj, u32 index, GLenum pname, i64 * param)
{
#ifndef NDEBUG
    if(!glGetVertexArrayIndexed64iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexArrayIndexed64iv(C_OCAST<u32>(vaobj), index, pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOGetIndexediv(glhnd const& vaobj, u32 index, GLenum pname, i32 * param)
{
#ifndef NDEBUG
    if(!glGetVertexArrayIndexediv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexArrayIndexediv(C_OCAST<u32>(vaobj), index, pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOGetiv(glhnd const& vaobj, GLenum pname, i32 * param)
{
#ifndef NDEBUG
    if(!glGetVertexArrayiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexArrayiv(C_OCAST<u32>(vaobj), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetnCompressedImage(TexComp::tex_flag const& target, i32 lod, i32 bufSize, void * pixels)
{
#ifndef NDEBUG
    if(!glGetnCompressedTexImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnCompressedTexImage(texture_to_enum(target), lod, bufSize, pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetnImage(TexComp::tex_flag const& target, i32 level, PixCmp format, BitFmt type, i32 bufSize, void * pixels)
{
#ifndef NDEBUG
    if(!glGetnTexImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnTexImage(texture_to_enum(target), level, to_enum(format, PixFmt::None), to_enum(type), bufSize, pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void UnifGetndv(glhnd const& program, i32 location, i32 bufSize, bigscalar * params)
{
#ifndef NDEBUG
    if(!glGetnUniformdv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformdv(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void UnifGetnfv(glhnd const& program, i32 location, i32 bufSize, scalar * params)
{
#ifndef NDEBUG
    if(!glGetnUniformfv)
    {
#if defined(GL_KHR_robustness) && GL_KHR_robustness
        if(glGetnUniformfvKHR) Throw(undefined_behavior("extension GL_KHR_robustness is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformfv(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void UnifGetniv(glhnd const& program, i32 location, i32 bufSize, i32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformiv)
    {
#if defined(GL_KHR_robustness) && GL_KHR_robustness
        if(glGetnUniformivKHR) Throw(undefined_behavior("extension GL_KHR_robustness is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformiv(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void UnifGetnuiv(glhnd const& program, i32 location, i32 bufSize, u32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformuiv)
    {
#if defined(GL_KHR_robustness) && GL_KHR_robustness
        if(glGetnUniformuivKHR) Throw(undefined_behavior("extension GL_KHR_robustness is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformuiv(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBInvalidateNamedData(u32 framebuffer, i32 numAttachments, const GLenum * attachments)
{
#ifndef NDEBUG
    if(!glInvalidateNamedFramebufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateNamedFramebufferData(framebuffer, numAttachments, attachments);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBInvalidateNamedSubData(u32 framebuffer, i32 numAttachments, const GLenum * attachments, i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glInvalidateNamedFramebufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateNamedFramebufferSubData(framebuffer, numAttachments, attachments, x, y, size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void * BufMap(glhnd const& buffer, RSCA access)
{
#ifndef NDEBUG
    if(!glMapNamedBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glMapNamedBuffer(C_OCAST<u32>(buffer), to_enum2(access));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void * BufMapRange(glhnd const& buffer, ptroff offset, ptroff length, RSCA access)
{
#ifndef NDEBUG
    if(!glMapNamedBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glMapNamedBufferRange(C_OCAST<u32>(buffer), offset, length, to_enum2(access));
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x310)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_31)
STATICINLINE void MemoryBarrierByRegion(u32 barriers)
{
#ifndef NDEBUG
    if(!glMemoryBarrierByRegion)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMemoryBarrierByRegion(barriers);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufData(glhnd const& buffer, BytesConst const& data, RSCA usage)
{
#ifndef NDEBUG
    if(!glNamedBufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedBufferData(C_OCAST<u32>(buffer), C_FCAST<ptroff>(data.size), data.data, to_enum1(usage));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufStorage(glhnd const& buffer, BytesConst const& data, RSCA flags)
{
#ifndef NDEBUG
    if(!glNamedBufferStorage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedBufferStorage(C_OCAST<u32>(buffer), C_FCAST<ptroff>(data.size), data.data, to_enum2(flags));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufSubData(glhnd const& buffer, ptroff offset, BytesConst const& data)
{
#ifndef NDEBUG
    if(!glNamedBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedBufferSubData(C_OCAST<u32>(buffer), offset, C_FCAST<ptroff>(data.size), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void NamedFramebufferDrawBuffer(u32 framebuffer, GLenum buf)
{
#ifndef NDEBUG
    if(!glNamedFramebufferDrawBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferDrawBuffer(framebuffer, buf);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void NamedFramebufferDrawBuffers(u32 framebuffer, i32 n, const GLenum * bufs)
{
#ifndef NDEBUG
    if(!glNamedFramebufferDrawBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferDrawBuffers(framebuffer, n, bufs);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBNamedParameteri(u32 framebuffer, GLenum pname, i32 param)
{
#ifndef NDEBUG
    if(!glNamedFramebufferParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferParameteri(framebuffer, pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBNamedReadBuffer(u32 framebuffer, GLenum src)
{
#ifndef NDEBUG
    if(!glNamedFramebufferReadBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferReadBuffer(framebuffer, src);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBNamedRenderbuffer(u32 framebuffer, GLenum attachment, GLenum renderbuffertarget, u32 renderbuffer)
{
#ifndef NDEBUG
    if(!glNamedFramebufferRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferRenderbuffer(framebuffer, attachment, renderbuffertarget, renderbuffer);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBNamedTexture(u32 framebuffer, GLenum attachment, glhnd const& texture, i32 level)
{
#ifndef NDEBUG
    if(!glNamedFramebufferTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferTexture(framebuffer, attachment, C_OCAST<u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBNamedTextureLayer(u32 framebuffer, GLenum attachment, glhnd const& texture, i32 level, i32 layer)
{
#ifndef NDEBUG
    if(!glNamedFramebufferTextureLayer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferTextureLayer(framebuffer, attachment, C_OCAST<u32>(texture), level, layer);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void RBufStorage(u32 renderbuffer, PixFmt internalformat, Size const& size)
{
#ifndef NDEBUG
    if(!glNamedRenderbufferStorage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedRenderbufferStorage(renderbuffer, to_enum(internalformat), size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void RBufStorageMultisample(u32 renderbuffer, i32 samples, PixFmt internalformat, Size const& size)
{
#ifndef NDEBUG
    if(!glNamedRenderbufferStorageMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedRenderbufferStorageMultisample(renderbuffer, samples, to_enum(internalformat), size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void ReadnPixels(i32 x, i32 y, Size const& size, PixCmp format, BitFmt type, i32 bufSize, void * data)
{
#ifndef NDEBUG
    if(!glReadnPixels)
    {
#if defined(GL_ARB_robustness) && GL_ARB_robustness
        if(glReadnPixelsARB) Throw(undefined_behavior("extension GL_ARB_robustness is available"));
#endif
#if defined(GL_KHR_robustness) && GL_KHR_robustness
        if(glReadnPixelsKHR) Throw(undefined_behavior("extension GL_KHR_robustness is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReadnPixels(x, y, size.w, size.h, to_enum(format, PixFmt::None), to_enum(type), bufSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexBarrier()
{
#ifndef NDEBUG
    if(!glTextureBarrier)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureBarrier();
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexBuffer(glhnd const& texture, PixFmt internalformat, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glTextureBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureBuffer(C_OCAST<u32>(texture), to_enum(internalformat), C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexBufferRange(glhnd const& texture, PixFmt internalformat, glhnd const& buffer, ptroff offset, ptroff size)
{
#ifndef NDEBUG
    if(!glTextureBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureBufferRange(C_OCAST<u32>(texture), to_enum(internalformat), C_OCAST<u32>(buffer), offset, size);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexParameterIiv(glhnd const& texture, GLenum pname, const i32 * params)
{
#ifndef NDEBUG
    if(!glTextureParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureParameterIiv(C_OCAST<u32>(texture), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexParameterIuiv(glhnd const& texture, GLenum pname, const u32 * params)
{
#ifndef NDEBUG
    if(!glTextureParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureParameterIuiv(C_OCAST<u32>(texture), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexParameterf(glhnd const& texture, GLenum pname, scalar param)
{
#ifndef NDEBUG
    if(!glTextureParameterf)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureParameterf(C_OCAST<u32>(texture), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexParameterfv(glhnd const& texture, GLenum pname, const scalar * param)
{
#ifndef NDEBUG
    if(!glTextureParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureParameterfv(C_OCAST<u32>(texture), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexParameteri(glhnd const& texture, GLenum pname, i32 param)
{
#ifndef NDEBUG
    if(!glTextureParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureParameteri(C_OCAST<u32>(texture), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexParameteriv(glhnd const& texture, GLenum pname, const i32 * param)
{
#ifndef NDEBUG
    if(!glTextureParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureParameteriv(C_OCAST<u32>(texture), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexStorage1D(glhnd const& texture, i32 levels, PixFmt internalformat, i32 width)
{
#ifndef NDEBUG
    if(!glTextureStorage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureStorage1D(C_OCAST<u32>(texture), levels, to_enum(internalformat), width);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexStorage2D(glhnd const& texture, i32 levels, PixFmt internalformat, Size const& size)
{
#ifndef NDEBUG
    if(!glTextureStorage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureStorage2D(C_OCAST<u32>(texture), levels, to_enum(internalformat), size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexStorage2DMultisample(glhnd const& texture, i32 samples, PixFmt internalformat, Size const& size, bool fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTextureStorage2DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureStorage2DMultisample(C_OCAST<u32>(texture), samples, to_enum(internalformat), size.w, size.h, fixedsamplelocations ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexStorage3D(glhnd const& texture, i32 levels, PixFmt internalformat, Size3 const& size)
{
#ifndef NDEBUG
    if(!glTextureStorage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureStorage3D(C_OCAST<u32>(texture), levels, to_enum(internalformat), size.width, size.height, size.depth);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexStorage3DMultisample(glhnd const& texture, i32 samples, PixFmt internalformat, Size3 const& size, bool fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTextureStorage3DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureStorage3DMultisample(C_OCAST<u32>(texture), samples, to_enum(internalformat), size.width, size.height, size.depth, fixedsamplelocations ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexSubImage1D(glhnd const& texture, i32 level, i32 xoffset, i32 width, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTextureSubImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureSubImage1D(C_OCAST<u32>(texture), level, xoffset, width, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexSubImage2D(glhnd const& texture, i32 level, Point const& offset, Size const& size, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTextureSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureSubImage2D(C_OCAST<u32>(texture), level, offset.x, offset.y, size.w, size.h, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexSubImage3D(glhnd const& texture, i32 level, Point3 const& offset, Size3 const& size, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTextureSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureSubImage3D(C_OCAST<u32>(texture), level, offset.x, offset.y, offset.z, size.width, size.height, size.depth, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufTransformFeedbackBase(u32 xfb, u32 index, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glTransformFeedbackBufferBase)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTransformFeedbackBufferBase(xfb, index, C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufTransformFeedbackRange(u32 xfb, u32 index, glhnd const& buffer, ptroff offset, ptroff size)
{
#ifndef NDEBUG
    if(!glTransformFeedbackBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTransformFeedbackBufferRange(xfb, index, C_OCAST<u32>(buffer), offset, size);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE u8 BufUnmap(glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glUnmapNamedBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glUnmapNamedBuffer(C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOBinding(glhnd const& vaobj, u32 attribindex, u32 bindingindex)
{
#ifndef NDEBUG
    if(!glVertexArrayAttribBinding)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayAttribBinding(C_OCAST<u32>(vaobj), attribindex, bindingindex);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOFormat(glhnd const& vaobj, u32 attribindex, i32 size, GLenum type, bool normalized, u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexArrayAttribFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayAttribFormat(C_OCAST<u32>(vaobj), attribindex, size, type, normalized ? GL_TRUE : GL_FALSE, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOIFormat(glhnd const& vaobj, u32 attribindex, i32 size, GLenum type, u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexArrayAttribIFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayAttribIFormat(C_OCAST<u32>(vaobj), attribindex, size, type, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOLFormat(glhnd const& vaobj, u32 attribindex, i32 size, GLenum type, u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexArrayAttribLFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayAttribLFormat(C_OCAST<u32>(vaobj), attribindex, size, type, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOBindingDivisor(glhnd const& vaobj, u32 bindingindex, u32 divisor)
{
#ifndef NDEBUG
    if(!glVertexArrayBindingDivisor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayBindingDivisor(C_OCAST<u32>(vaobj), bindingindex, divisor);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOElementBuffer(glhnd const& vaobj, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glVertexArrayElementBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayElementBuffer(C_OCAST<u32>(vaobj), C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOVertexBuffer(glhnd const& vaobj, u32 bindingindex, glhnd const& buffer, ptroff offset, i32 stride)
{
#ifndef NDEBUG
    if(!glVertexArrayVertexBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayVertexBuffer(C_OCAST<u32>(vaobj), bindingindex, C_OCAST<u32>(buffer), offset, stride);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOVertexBuffers(glhnd const& vaobj, u32 first, i32 count, const u32 * buffers, const ptroff * offsets, const i32 * strides)
{
#ifndef NDEBUG
    if(!glVertexArrayVertexBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayVertexBuffers(C_OCAST<u32>(vaobj), first, count, buffers, offsets, strides);
}
#endif

};
template<typename ReqVer>
struct CGL_46
    : CGL_45<ReqVer>
{
using Parent = CGL_45<ReqVer>;

#if GL_VERSION_VERIFY(0x460, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_46)
STATICINLINE void MultiDrawArraysIndirectCount(DrwMd const& mode, uintptr indirect, ptroff drawcount, i32 maxdrawcount, i32 stride)
{
#ifndef NDEBUG
    if(!glMultiDrawArraysIndirectCount)
    {
#if defined(GL_ARB_indirect_parameters) && GL_ARB_indirect_parameters
        if(glMultiDrawArraysIndirectCountARB) Throw(undefined_behavior("extension GL_ARB_indirect_parameters is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMultiDrawArraysIndirectCount(to_enum(mode.t, mode.c), C_RCAST<const void *>(indirect), drawcount, maxdrawcount, stride);
}
#endif


#if GL_VERSION_VERIFY(0x460, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_46)
STATICINLINE void MultiDrawElementsIndirectCount(DrwMd const& mode, GLenum type, uintptr indirect, ptroff drawcount, i32 maxdrawcount, i32 stride)
{
#ifndef NDEBUG
    if(!glMultiDrawElementsIndirectCount)
    {
#if defined(GL_ARB_indirect_parameters) && GL_ARB_indirect_parameters
        if(glMultiDrawElementsIndirectCountARB) Throw(undefined_behavior("extension GL_ARB_indirect_parameters is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMultiDrawElementsIndirectCount(to_enum(mode.t, mode.c), type, C_RCAST<const void *>(indirect), drawcount, maxdrawcount, stride);
}
#endif


#if GL_VERSION_VERIFY(0x460, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_46)
STATICINLINE void PolygonOffsetClamp(scalar factor, scalar units, scalar clamp)
{
#ifndef NDEBUG
    if(!glPolygonOffsetClamp)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPolygonOffsetClamp(factor, units, clamp);
}
#endif


#if GL_VERSION_VERIFY(0x460, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_46)
STATICINLINE void ShaderSpecialize(glhnd const& shader, const GLchar * pEntryPoint, u32 numSpecializationConstants, const u32 * pConstantIndex, const u32 * pConstantValue)
{
#ifndef NDEBUG
    if(!glSpecializeShader)
    {
#if defined(GL_ARB_gl_spirv) && GL_ARB_gl_spirv
        if(glSpecializeShaderARB) Throw(undefined_behavior("extension GL_ARB_gl_spirv is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSpecializeShader(C_OCAST<u32>(shader), pEntryPoint, numSpecializationConstants, pConstantIndex, pConstantValue);
}
#endif

};
#elif GL_VERSION_VERIFY(GL_VERSION_NONE, 0x200)
template<typename ReqVer>
struct CGL_20
{

#if GL_VERSION_VERIFY(0x130, 0x200)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_20)
STATICINLINE void TexActive(GLenum texture)
{
#ifndef NDEBUG
    if(!glActiveTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glActiveTexture(texture);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderAttach(glhnd const& program, glhnd const& shader)
{
#ifndef NDEBUG
    if(!glAttachShader)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glAttachObjectARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glAttachShader(C_OCAST<u32>(program), C_OCAST<u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void AttribBindLocation(glhnd const& program, u32 index, const GLchar * name)
{
#ifndef NDEBUG
    if(!glBindAttribLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindAttribLocation(C_OCAST<u32>(program), index, name);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufBind(BufferComp::buf_flag const& target, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glBindBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindBuffer(buffer_to_enum(target), C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBBind(FramebufferT target, u32 framebuffer)
{
#ifndef NDEBUG
    if(!glBindFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindFramebuffer(to_enum(target), framebuffer);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufBind(GLenum target, u32 renderbuffer)
{
#ifndef NDEBUG
    if(!glBindRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindRenderbuffer(target, renderbuffer);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexBind(TexComp::tex_flag const& target, glhnd const& texture)
{
#ifndef NDEBUG
    if(!glBindTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindTexture(texture_to_enum(target), C_OCAST<u32>(texture));
}
#endif


#if GL_VERSION_VERIFY(0x140, 0x200)
GL_VERSION_REQ_COMBO(GLVER_14, GLESVER_20)
STATICINLINE void BlendColor(_cbasic_vec4<scalar> const& color)
{
#ifndef NDEBUG
    if(!glBlendColor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendColor(color.r(), color.g(), color.b(), color.a());
}
#endif


#if GL_VERSION_VERIFY(0x140, 0x200)
GL_VERSION_REQ_COMBO(GLVER_14, GLESVER_20)
STATICINLINE void BlendEquation(GLenum mode)
{
#ifndef NDEBUG
    if(!glBlendEquation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendEquation(mode);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void BlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
{
#ifndef NDEBUG
    if(!glBlendEquationSeparate)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendEquationSeparate(modeRGB, modeAlpha);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void BlendFunc(GLenum sfactor, GLenum dfactor)
{
#ifndef NDEBUG
    if(!glBlendFunc)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendFunc(sfactor, dfactor);
}
#endif


#if GL_VERSION_VERIFY(0x140, 0x200)
GL_VERSION_REQ_COMBO(GLVER_14, GLESVER_20)
STATICINLINE void BlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
#ifndef NDEBUG
    if(!glBlendFuncSeparate)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufData(BufferComp::buf_flag const& target, BytesConst const& data, RSCA usage)
{
#ifndef NDEBUG
    if(!glBufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBufferData(buffer_to_enum(target), C_FCAST<ptroff>(data.size), data.data, to_enum1(usage));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufSubData(BufferComp::buf_flag const& target, ptroff offset, BytesConst const& data)
{
#ifndef NDEBUG
    if(!glBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBufferSubData(buffer_to_enum(target), offset, C_FCAST<ptroff>(data.size), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE GLenum FBCheckStatus(FramebufferT target)
{
#ifndef NDEBUG
    if(!glCheckFramebufferStatus)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glCheckFramebufferStatus(to_enum(target));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Clear(u32 mask)
{
#ifndef NDEBUG
    if(!glClear)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClear(mask);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void ClearColor(_cbasic_vec4<scalar> const& color)
{
#ifndef NDEBUG
    if(!glClearColor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearColor(color.r(), color.g(), color.b(), color.a());
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void ClearDepthf(scalar d)
{
#ifndef NDEBUG
    if(!glClearDepthf)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearDepthf(d);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void ClearStencil(i32 s)
{
#ifndef NDEBUG
    if(!glClearStencil)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearStencil(s);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void ColorMask(_cbasic_vec4<u8> const& color)
{
#ifndef NDEBUG
    if(!glColorMask)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glColorMask(color.r(), color.g(), color.b(), color.a());
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderCompile(glhnd const& shader)
{
#ifndef NDEBUG
    if(!glCompileShader)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glCompileShaderARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompileShader(C_OCAST<u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x200)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_20)
STATICINLINE void TexCompressedImage2D(TexComp::tex_flag const& target, i32 level, CompFmt internalformat, Size const& size, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexImage2D)
    {
#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
        if(glCompressedTexImage2DARB) Throw(undefined_behavior("extension GL_ARB_texture_compression is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexImage2D(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), size.w, size.h, 0, imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x200)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_20)
STATICINLINE void TexCompressedSubImage2D(TexComp::tex_flag const& target, i32 level, Point const& offset, Size const& size, CompFmt format, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexSubImage2D)
    {
#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
        if(glCompressedTexSubImage2DARB) Throw(undefined_behavior("extension GL_ARB_texture_compression is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexSubImage2D(texture_to_enum(target), level, offset.x, offset.y, size.w, size.h, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexCopyImage2D(TexComp::tex_flag const& target, i32 level, PixFmt internalformat, i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glCopyTexImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexImage2D(texture_to_enum(target), level, to_enum(internalformat), x, y, size.w, size.h, 0);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexCopySubImage2D(TexComp::tex_flag const& target, i32 level, Point const& offset, i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glCopyTexSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexSubImage2D(texture_to_enum(target), level, offset.x, offset.y, x, y, size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE u32 ProgramAllocEx()
{
#ifndef NDEBUG
    if(!glCreateProgram)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glCreateProgramObjectARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glCreateProgram();
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE u32 ShaderAllocEx(ShaderStage type)
{
#ifndef NDEBUG
    if(!glCreateShader)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glCreateShaderObjectARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glCreateShader(to_enum1(type));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void CullFace(Face mode)
{
#ifndef NDEBUG
    if(!glCullFace)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCullFace(to_enum(mode));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufFree(Span<const u32> const& buffers)
{
#ifndef NDEBUG
    if(!glDeleteBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteBuffers(C_FCAST<i32>(buffers.elements), buffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBFree(Span<const u32> const& framebuffers)
{
#ifndef NDEBUG
    if(!glDeleteFramebuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteFramebuffers(C_FCAST<i32>(framebuffers.elements), framebuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramFree(glhnd const& program)
{
#ifndef NDEBUG
    if(!glDeleteProgram)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteProgram(C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufFree(Span<const u32> const& renderbuffers)
{
#ifndef NDEBUG
    if(!glDeleteRenderbuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteRenderbuffers(C_FCAST<i32>(renderbuffers.elements), renderbuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderFree(glhnd const& shader)
{
#ifndef NDEBUG
    if(!glDeleteShader)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteShader(C_OCAST<u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexFree(Span<const u32> const& textures)
{
#ifndef NDEBUG
    if(!glDeleteTextures)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteTextures(C_FCAST<i32>(textures.elements), textures.data);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void DepthFunc(GLenum func)
{
#ifndef NDEBUG
    if(!glDepthFunc)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthFunc(func);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void DepthMask(bool flag)
{
#ifndef NDEBUG
    if(!glDepthMask)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthMask(flag ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void DepthRangef(scalar n, scalar f)
{
#ifndef NDEBUG
    if(!glDepthRangef)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthRangef(n, f);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderDetach(glhnd const& program, glhnd const& shader)
{
#ifndef NDEBUG
    if(!glDetachShader)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glDetachObjectARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDetachShader(C_OCAST<u32>(program), C_OCAST<u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Disable(Feature cap)
{
#ifndef NDEBUG
    if(!glDisable)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDisable(to_enum(cap));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAODisableArray(u32 index)
{
#ifndef NDEBUG
    if(!glDisableVertexAttribArray)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDisableVertexAttribArray(index);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void DrawArrays(DrwMd const& mode, i32 first, i32 count)
{
#ifndef NDEBUG
    if(!glDrawArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawArrays(to_enum(mode.t, mode.c), first, count);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void DrawElements(DrwMd const& mode, i32 count, TypeEnum type, uintptr indices)
{
#ifndef NDEBUG
    if(!glDrawElements)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElements(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void *>(indices));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Enable(Feature cap)
{
#ifndef NDEBUG
    if(!glEnable)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEnable(to_enum(cap));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAOEnableArray(u32 index)
{
#ifndef NDEBUG
    if(!glEnableVertexAttribArray)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEnableVertexAttribArray(index);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Finish()
{
#ifndef NDEBUG
    if(!glFinish)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFinish();
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Flush()
{
#ifndef NDEBUG
    if(!glFlush)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFlush();
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBRenderbuffer(FramebufferT target, GLenum attachment, GLenum renderbuffertarget, u32 renderbuffer)
{
#ifndef NDEBUG
    if(!glFramebufferRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferRenderbuffer(to_enum(target), attachment, renderbuffertarget, renderbuffer);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBTexture2D(FramebufferT target, GLenum attachment, TexComp::tex_flag const& textarget, glhnd const& texture, i32 level)
{
#ifndef NDEBUG
    if(!glFramebufferTexture2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTexture2D(to_enum(target), attachment, texture_to_enum(textarget), C_OCAST<u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void FrontFace(GLenum mode)
{
#ifndef NDEBUG
    if(!glFrontFace)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFrontFace(mode);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufAlloc(Span<u32> const& buffers)
{
#ifndef NDEBUG
    if(!glGenBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenBuffers(C_FCAST<i32>(buffers.elements), buffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBAlloc(Span<u32> const& framebuffers)
{
#ifndef NDEBUG
    if(!glGenFramebuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenFramebuffers(C_FCAST<i32>(framebuffers.elements), framebuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufAlloc(Span<u32> const& renderbuffers)
{
#ifndef NDEBUG
    if(!glGenRenderbuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenRenderbuffers(C_FCAST<i32>(renderbuffers.elements), renderbuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexAlloc(Span<u32> const& textures)
{
#ifndef NDEBUG
    if(!glGenTextures)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenTextures(C_FCAST<i32>(textures.elements), textures.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void GenerateMipmap(TexComp::tex_flag const& target)
{
#ifndef NDEBUG
    if(!glGenerateMipmap)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenerateMipmap(texture_to_enum(target));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ActiveAttribGet(glhnd const& program, u32 index, i32 bufSize, i32 * length, i32 * size, GLenum * type, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetActiveAttrib)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveAttrib(C_OCAST<u32>(program), index, bufSize, length, size, type, name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ActiveUnifGet(glhnd const& program, u32 index, i32 bufSize, i32 * length, i32 * size, GLenum * type, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetActiveUniform)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glGetActiveUniformARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniform(C_OCAST<u32>(program), index, bufSize, length, size, type, name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void AttachedShadersGet(glhnd const& program, i32 maxCount, i32 * count, u32 * shaders)
{
#ifndef NDEBUG
    if(!glGetAttachedShaders)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetAttachedShaders(C_OCAST<u32>(program), maxCount, count, shaders);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE i32 AttribGetLocation(glhnd const& program, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetAttribLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetAttribLocation(C_OCAST<u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void BoolGetv(GLenum pname, u8 * data)
{
#ifndef NDEBUG
    if(!glGetBooleanv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetBooleanv(pname, data);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufGetParameteriv(BufferComp::buf_flag const& target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetBufferParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetBufferParameteriv(buffer_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE GLenum ErrorGet()
{
#ifndef NDEBUG
    if(!glGetError)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetError();
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void ScalarfGetv(GLenum pname, scalar * data)
{
#ifndef NDEBUG
    if(!glGetFloatv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetFloatv(pname, data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBGetAttachmentParameteriv(FramebufferT target, GLenum attachment, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetFramebufferAttachmentParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetFramebufferAttachmentParameteriv(to_enum(target), attachment, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void IntegerGetv(GLenum pname, i32 * data)
{
#ifndef NDEBUG
    if(!glGetIntegerv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetIntegerv(pname, data);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramGetInfoLog(glhnd const& program, i32 bufSize, i32 * length, GLchar * infoLog)
{
#ifndef NDEBUG
    if(!glGetProgramInfoLog)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramInfoLog(C_OCAST<u32>(program), bufSize, length, infoLog);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramGetiv(glhnd const& program, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramiv(C_OCAST<u32>(program), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufGetParameteriv(GLenum target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetRenderbufferParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetRenderbufferParameteriv(target, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderGetInfoLog(glhnd const& shader, i32 bufSize, i32 * length, GLchar * infoLog)
{
#ifndef NDEBUG
    if(!glGetShaderInfoLog)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderInfoLog(C_OCAST<u32>(shader), bufSize, length, infoLog);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void ShaderGetPrecisionFormat(ShaderStage shadertype, GLenum precisiontype, i32 * range, i32 * precision)
{
#ifndef NDEBUG
    if(!glGetShaderPrecisionFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderPrecisionFormat(to_enum1(shadertype), precisiontype, range, precision);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderGetSource(glhnd const& shader, i32 bufSize, i32 * length, GLchar * source)
{
#ifndef NDEBUG
    if(!glGetShaderSource)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glGetShaderSourceARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderSource(C_OCAST<u32>(shader), bufSize, length, source);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderGetiv(glhnd const& shader, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetShaderiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderiv(C_OCAST<u32>(shader), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE const u8 * GetString(GLenum name)
{
#ifndef NDEBUG
    if(!glGetString)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetString(name);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexGetParameterfv(TexComp::tex_flag const& target, GLenum pname, scalar * params)
{
#ifndef NDEBUG
    if(!glGetTexParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexParameterfv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexGetParameteriv(TexComp::tex_flag const& target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetTexParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexParameteriv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE i32 UnifGetLocation(glhnd const& program, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetUniformLocation)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glGetUniformLocationARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetUniformLocation(C_OCAST<u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void UnifGetfv(glhnd const& program, i32 location, scalar * params)
{
#ifndef NDEBUG
    if(!glGetUniformfv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glGetUniformfvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformfv(C_OCAST<u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void UnifGetiv(glhnd const& program, i32 location, i32 * params)
{
#ifndef NDEBUG
    if(!glGetUniformiv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glGetUniformivARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformiv(C_OCAST<u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAOGetPointerv(u32 index, GLenum pname, void * * pointer)
{
#ifndef NDEBUG
    if(!glGetVertexAttribPointerv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribPointerv(index, pname, pointer);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAOGetfv(u32 index, GLenum pname, scalar * params)
{
#ifndef NDEBUG
    if(!glGetVertexAttribfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribfv(index, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAOGetiv(u32 index, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetVertexAttribiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribiv(index, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Hint(GLenum target, GLenum mode)
{
#ifndef NDEBUG
    if(!glHint)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glHint(target, mode);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE u8 IsBuffer(glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glIsBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsBuffer(C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE u8 IsEnabled(Feature cap)
{
#ifndef NDEBUG
    if(!glIsEnabled)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsEnabled(to_enum(cap));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE u8 IsFramebuffer(u32 framebuffer)
{
#ifndef NDEBUG
    if(!glIsFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsFramebuffer(framebuffer);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE u8 IsProgram(glhnd const& program)
{
#ifndef NDEBUG
    if(!glIsProgram)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsProgram(C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE u8 IsRenderbuffer(u32 renderbuffer)
{
#ifndef NDEBUG
    if(!glIsRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsRenderbuffer(renderbuffer);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE u8 IsShader(glhnd const& shader)
{
#ifndef NDEBUG
    if(!glIsShader)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsShader(C_OCAST<u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE u8 IsTexture(glhnd const& texture)
{
#ifndef NDEBUG
    if(!glIsTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsTexture(C_OCAST<u32>(texture));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void LineWidth(scalar width)
{
#ifndef NDEBUG
    if(!glLineWidth)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glLineWidth(width);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramLink(glhnd const& program)
{
#ifndef NDEBUG
    if(!glLinkProgram)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glLinkProgramARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glLinkProgram(C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void PixelStorei(GLenum pname, i32 param)
{
#ifndef NDEBUG
    if(!glPixelStorei)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPixelStorei(pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void PolygonOffset(scalar factor, scalar units)
{
#ifndef NDEBUG
    if(!glPolygonOffset)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPolygonOffset(factor, units);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void ReadPixels(i32 x, i32 y, Size const& size, PixCmp format, BitFmt type, void * pixels)
{
#ifndef NDEBUG
    if(!glReadPixels)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReadPixels(x, y, size.w, size.h, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void ShaderReleaseCompiler()
{
#ifndef NDEBUG
    if(!glReleaseShaderCompiler)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReleaseShaderCompiler();
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufStorage(GLenum target, PixFmt internalformat, Size const& size)
{
#ifndef NDEBUG
    if(!glRenderbufferStorage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glRenderbufferStorage(target, to_enum(internalformat), size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x200)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_20)
STATICINLINE void SampleCoverage(scalar value, bool invert)
{
#ifndef NDEBUG
    if(!glSampleCoverage)
    {
#if defined(GL_ARB_multisample) && GL_ARB_multisample
        if(glSampleCoverageARB) Throw(undefined_behavior("extension GL_ARB_multisample is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSampleCoverage(value, invert ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Scissor(i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glScissor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glScissor(x, y, size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void ShaderBinary(i32 count, const u32 * shaders, GLenum binaryformat, const void * binary, i32 length)
{
#ifndef NDEBUG
    if(!glShaderBinary)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glShaderBinary(count, shaders, binaryformat, binary, length);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderSource(glhnd const& shader, i32 count, const GLchar * *const string, const i32 * length)
{
#ifndef NDEBUG
    if(!glShaderSource)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glShaderSourceARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glShaderSource(C_OCAST<u32>(shader), count, string, length);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void StencilFunc(GLenum func, i32 ref, u32 mask)
{
#ifndef NDEBUG
    if(!glStencilFunc)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glStencilFunc(func, ref, mask);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void StencilFuncSeparate(GLenum face, GLenum func, i32 ref, u32 mask)
{
#ifndef NDEBUG
    if(!glStencilFuncSeparate)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glStencilFuncSeparate(face, func, ref, mask);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void StencilMask(u32 mask)
{
#ifndef NDEBUG
    if(!glStencilMask)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glStencilMask(mask);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void StencilMaskSeparate(GLenum face, u32 mask)
{
#ifndef NDEBUG
    if(!glStencilMaskSeparate)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glStencilMaskSeparate(face, mask);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void StencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
#ifndef NDEBUG
    if(!glStencilOp)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glStencilOp(fail, zfail, zpass);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void StencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
#ifndef NDEBUG
    if(!glStencilOpSeparate)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glStencilOpSeparate(face, sfail, dpfail, dppass);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexImage2D(TexComp::tex_flag const& target, i32 level, PixFmt internalformat, Size const& size, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage2D(texture_to_enum(target), level, to_enum(internalformat), size.w, size.h, 0, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexParameterf(TexComp::tex_flag const& target, GLenum pname, scalar param)
{
#ifndef NDEBUG
    if(!glTexParameterf)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexParameterf(texture_to_enum(target), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexParameterfv(TexComp::tex_flag const& target, GLenum pname, const scalar * params)
{
#ifndef NDEBUG
    if(!glTexParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexParameterfv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexParameteri(TexComp::tex_flag const& target, GLenum pname, i32 param)
{
#ifndef NDEBUG
    if(!glTexParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexParameteri(texture_to_enum(target), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexParameteriv(TexComp::tex_flag const& target, GLenum pname, const i32 * params)
{
#ifndef NDEBUG
    if(!glTexParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexParameteriv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexSubImage2D(TexComp::tex_flag const& target, i32 level, Point const& offset, Size const& size, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexSubImage2D(texture_to_enum(target), level, offset.x, offset.y, size.w, size.h, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif1f(i32 location, scalar v0)
{
#ifndef NDEBUG
    if(!glUniform1f)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform1fARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1f(location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glUniform1fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform1fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1fv(location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif1i(i32 location, i32 v0)
{
#ifndef NDEBUG
    if(!glUniform1i)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform1iARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1i(location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(i32 location, Span<const i32> const& value)
{
#ifndef NDEBUG
    if(!glUniform1iv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform1ivARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1iv(location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif2f(i32 location, scalar v0, scalar v1)
{
#ifndef NDEBUG
    if(!glUniform2f)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform2fARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2f(location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, Span<Vecf2> const& value)
{
#ifndef NDEBUG
    if(!glUniform2fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform2fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2fv(location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif2i(i32 location, i32 v0, i32 v1)
{
#ifndef NDEBUG
    if(!glUniform2i)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform2iARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2i(location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(i32 location, Span<Veci2> const& value)
{
#ifndef NDEBUG
    if(!glUniform2iv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform2ivARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2iv(location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif3f(i32 location, scalar v0, scalar v1, scalar v2)
{
#ifndef NDEBUG
    if(!glUniform3f)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform3fARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3f(location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, Span<Vecf3> const& value)
{
#ifndef NDEBUG
    if(!glUniform3fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform3fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3fv(location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif3i(i32 location, i32 v0, i32 v1, i32 v2)
{
#ifndef NDEBUG
    if(!glUniform3i)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform3iARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3i(location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(i32 location, Span<Veci3> const& value)
{
#ifndef NDEBUG
    if(!glUniform3iv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform3ivARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3iv(location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif4f(i32 location, scalar v0, scalar v1, scalar v2, scalar v3)
{
#ifndef NDEBUG
    if(!glUniform4f)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform4fARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4f(location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, Span<Vecf4> const& value)
{
#ifndef NDEBUG
    if(!glUniform4fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform4fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4fv(location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif4i(i32 location, i32 v0, i32 v1, i32 v2, i32 v3)
{
#ifndef NDEBUG
    if(!glUniform4i)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform4iARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4i(location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(i32 location, Span<Veci4> const& value)
{
#ifndef NDEBUG
    if(!glUniform4iv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniform4ivARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4iv(location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniformMatrix2fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniformMatrix3fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4fv)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUniformMatrix4fvARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramUse(glhnd const& program)
{
#ifndef NDEBUG
    if(!glUseProgram)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glUseProgramObjectARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUseProgram(C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramValidate(glhnd const& program)
{
#ifndef NDEBUG
    if(!glValidateProgram)
    {
#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
        if(glValidateProgramARB) Throw(undefined_behavior("extension GL_ARB_shader_objects is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glValidateProgram(C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO1f(u32 index, scalar x)
{
#ifndef NDEBUG
    if(!glVertexAttrib1f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib1f(index, x);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO1fv(u32 index, const scalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib1fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib1fv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO2f(u32 index, scalar x, scalar y)
{
#ifndef NDEBUG
    if(!glVertexAttrib2f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib2f(index, x, y);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO2fv(u32 index, const scalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib2fv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO3f(u32 index, scalar x, scalar y, scalar z)
{
#ifndef NDEBUG
    if(!glVertexAttrib3f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib3f(index, x, y, z);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO3fv(u32 index, const scalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib3fv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO4f(u32 index, scalar x, scalar y, scalar z, scalar w)
{
#ifndef NDEBUG
    if(!glVertexAttrib4f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4f(index, x, y, z, w);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO4fv(u32 index, const scalar * v)
{
#ifndef NDEBUG
    if(!glVertexAttrib4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4fv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAOPointer(u32 index, i32 size, GLenum type, bool normalized, i32 stride, const void * pointer)
{
#ifndef NDEBUG
    if(!glVertexAttribPointer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, stride, pointer);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Viewport(i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glViewport)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glViewport(x, y, size.w, size.h);
}
#endif

};
template<typename ReqVer>
struct CGL_30
    : CGL_20<ReqVer>
{
using Parent = CGL_20<ReqVer>;
using Parent::Uniffv;

#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryBegin(QueryComp::query_flag const& target, u32 id)
{
#ifndef NDEBUG
    if(!glBeginQuery)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glBeginQueryARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBeginQuery(query_to_enum(target), id);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void XFBegin(GLenum primitiveMode)
{
#ifndef NDEBUG
    if(!glBeginTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBeginTransformFeedback(primitiveMode);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufBindBase(BufferComp::buf_flag const& target, u32 index, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glBindBufferBase)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindBufferBase(buffer_to_enum(target), index, C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufBindRange(BufferComp::buf_flag const& target, u32 index, glhnd const& buffer, ptroff offset, ptroff size)
{
#ifndef NDEBUG
    if(!glBindBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindBufferRange(buffer_to_enum(target), index, C_OCAST<u32>(buffer), offset, size);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerBind(u32 unit, glhnd const& sampler)
{
#ifndef NDEBUG
    if(!glBindSampler)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindSampler(unit, C_OCAST<u32>(sampler));
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFBind(GLenum target, u32 id)
{
#ifndef NDEBUG
    if(!glBindTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindTransformFeedback(target, id);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOBind(glhnd const& array)
{
#ifndef NDEBUG
    if(!glBindVertexArray)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindVertexArray(C_OCAST<u32>(array));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void FBBlit(i32 srcX0, i32 srcY0, i32 srcX1, i32 srcY1, i32 dstX0, i32 dstY0, i32 dstX1, i32 dstY1, u32 mask, GLenum filter)
{
#ifndef NDEBUG
    if(!glBlitFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufClearfi(GLenum buffer, i32 drawbuffer, scalar depth, i32 stencil)
{
#ifndef NDEBUG
    if(!glClearBufferfi)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearBufferfi(buffer, drawbuffer, depth, stencil);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufClearfv(GLenum buffer, i32 drawbuffer, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glClearBufferfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearBufferfv(buffer, drawbuffer, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufCleariv(GLenum buffer, i32 drawbuffer, Span<const i32> const& value)
{
#ifndef NDEBUG
    if(!glClearBufferiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearBufferiv(buffer, drawbuffer, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufClearuiv(GLenum buffer, i32 drawbuffer, Span<const u32> const& value)
{
#ifndef NDEBUG
    if(!glClearBufferuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearBufferuiv(buffer, drawbuffer, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE GLenum SyncClientWait(GLsync sync, RSCA flags, u64 timeout)
{
#ifndef NDEBUG
    if(!glClientWaitSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glClientWaitSync(sync, to_enum2(flags), timeout);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x300)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_30)
STATICINLINE void TexCompressedImage3D(TexComp::tex_flag const& target, i32 level, CompFmt internalformat, Size3 const& size, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexImage3D)
    {
#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
        if(glCompressedTexImage3DARB) Throw(undefined_behavior("extension GL_ARB_texture_compression is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexImage3D(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), size.width, size.height, size.depth, 0, imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x300)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_30)
STATICINLINE void TexCompressedSubImage3D(TexComp::tex_flag const& target, i32 level, Point3 const& offset, Size3 const& size, CompFmt format, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexSubImage3D)
    {
#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
        if(glCompressedTexSubImage3DARB) Throw(undefined_behavior("extension GL_ARB_texture_compression is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexSubImage3D(texture_to_enum(target), level, offset.x, offset.y, offset.z, size.width, size.height, size.depth, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void BufCopySubData(GLenum readTarget, GLenum writeTarget, ptroff readOffset, ptroff writeOffset, ptroff size)
{
#ifndef NDEBUG
    if(!glCopyBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size);
}
#endif


#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void TexCopySubImage3D(TexComp::tex_flag const& target, i32 level, Point3 const& offset, i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glCopyTexSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexSubImage3D(texture_to_enum(target), level, offset.x, offset.y, offset.z, x, y, size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryFree(Span<const u32> const& ids)
{
#ifndef NDEBUG
    if(!glDeleteQueries)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glDeleteQueriesARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteQueries(C_FCAST<i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerFree(Span<const u32> const& samplers)
{
#ifndef NDEBUG
    if(!glDeleteSamplers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteSamplers(C_FCAST<i32>(samplers.elements), samplers.data);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void SyncFree(GLsync sync)
{
#ifndef NDEBUG
    if(!glDeleteSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteSync(sync);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFFree(Span<const u32> const& ids)
{
#ifndef NDEBUG
    if(!glDeleteTransformFeedbacks)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteTransformFeedbacks(C_FCAST<i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOFree(Span<const u32> const& arrays)
{
#ifndef NDEBUG
    if(!glDeleteVertexArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteVertexArrays(C_FCAST<i32>(arrays.elements), arrays.data);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void DrawArraysInstanced(DrwMd const& mode, i32 first, i32 count, i32 instancecount)
{
#ifndef NDEBUG
    if(!glDrawArraysInstanced)
    {
#if defined(GL_ARB_draw_instanced) && GL_ARB_draw_instanced
        if(glDrawArraysInstancedARB) Throw(undefined_behavior("extension GL_ARB_draw_instanced is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawArraysInstanced(to_enum(mode.t, mode.c), first, count, instancecount);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x300)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_30)
STATICINLINE void DrawBuffers(i32 n, const GLenum * bufs)
{
#ifndef NDEBUG
    if(!glDrawBuffers)
    {
#if defined(GL_ARB_draw_buffers) && GL_ARB_draw_buffers
        if(glDrawBuffersARB) Throw(undefined_behavior("extension GL_ARB_draw_buffers is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawBuffers(n, bufs);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void DrawElementsInstanced(DrwMd const& mode, i32 count, TypeEnum type, uintptr indices, i32 instancecount)
{
#ifndef NDEBUG
    if(!glDrawElementsInstanced)
    {
#if defined(GL_ARB_draw_instanced) && GL_ARB_draw_instanced
        if(glDrawElementsInstancedARB) Throw(undefined_behavior("extension GL_ARB_draw_instanced is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElementsInstanced(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void *>(indices), instancecount);
}
#endif


#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void DrawRangeElements(DrwMd const& mode, u32 start, u32 end, i32 count, TypeEnum type, uintptr indices)
{
#ifndef NDEBUG
    if(!glDrawRangeElements)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawRangeElements(to_enum(mode.t, mode.c), start, end, count, to_enum(type), C_RCAST<const void *>(indices));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryEnd(QueryComp::query_flag const& target)
{
#ifndef NDEBUG
    if(!glEndQuery)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glEndQueryARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEndQuery(query_to_enum(target));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void XFEnd()
{
#ifndef NDEBUG
    if(!glEndTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEndTransformFeedback();
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE GLsync SyncFence(GLenum condition, RSCA flags)
{
#ifndef NDEBUG
    if(!glFenceSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glFenceSync(condition, to_enum2(flags));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufFlushMappedRange(BufferComp::buf_flag const& target, ptroff offset, ptroff length)
{
#ifndef NDEBUG
    if(!glFlushMappedBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFlushMappedBufferRange(buffer_to_enum(target), offset, length);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void FBTextureLayer(FramebufferT target, GLenum attachment, glhnd const& texture, i32 level, i32 layer)
{
#ifndef NDEBUG
    if(!glFramebufferTextureLayer)
    {
#if defined(GL_ARB_geometry_shader4) && GL_ARB_geometry_shader4
        if(glFramebufferTextureLayerARB) Throw(undefined_behavior("extension GL_ARB_geometry_shader4 is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTextureLayer(to_enum(target), attachment, C_OCAST<u32>(texture), level, layer);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryAlloc(Span<u32> const& ids)
{
#ifndef NDEBUG
    if(!glGenQueries)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glGenQueriesARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenQueries(C_FCAST<i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerAlloc(Span<u32> const& samplers)
{
#ifndef NDEBUG
    if(!glGenSamplers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenSamplers(C_FCAST<i32>(samplers.elements), samplers.data);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFAlloc(Span<u32> const& ids)
{
#ifndef NDEBUG
    if(!glGenTransformFeedbacks)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenTransformFeedbacks(C_FCAST<i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOAlloc(Span<u32> const& arrays)
{
#ifndef NDEBUG
    if(!glGenVertexArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenVertexArrays(C_FCAST<i32>(arrays.elements), arrays.data);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void ActiveUnifBlockGetName(glhnd const& program, u32 uniformBlockIndex, i32 bufSize, i32 * length, GLchar * uniformBlockName)
{
#ifndef NDEBUG
    if(!glGetActiveUniformBlockName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformBlockName(C_OCAST<u32>(program), uniformBlockIndex, bufSize, length, uniformBlockName);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void ActiveUnifBlockGetiv(glhnd const& program, u32 uniformBlockIndex, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetActiveUniformBlockiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformBlockiv(C_OCAST<u32>(program), uniformBlockIndex, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void ActiveUnifGetsiv(glhnd const& program, i32 uniformCount, const u32 * uniformIndices, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetActiveUniformsiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformsiv(C_OCAST<u32>(program), uniformCount, uniformIndices, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void BufGetParameteri64v(BufferComp::buf_flag const& target, GLenum pname, i64 * params)
{
#ifndef NDEBUG
    if(!glGetBufferParameteri64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetBufferParameteri64v(buffer_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void BufGetPointerv(BufferComp::buf_flag const& target, GLenum pname, void * * params)
{
#ifndef NDEBUG
    if(!glGetBufferPointerv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetBufferPointerv(buffer_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE i32 GetFragDataLocation(glhnd const& program, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetFragDataLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetFragDataLocation(C_OCAST<u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void IntegerGet64i_v(GLenum target, u32 index, i64 * data)
{
#ifndef NDEBUG
    if(!glGetInteger64i_v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetInteger64i_v(target, index, data);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void IntegerGet64v(GLenum pname, i64 * data)
{
#ifndef NDEBUG
    if(!glGetInteger64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetInteger64v(pname, data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void IntegerGeti_v(GLenum target, u32 index, i32 * data)
{
#ifndef NDEBUG
    if(!glGetIntegeri_v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetIntegeri_v(target, index, data);
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x300)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_30)
STATICINLINE void GetInternalformativ(TexComp::tex_flag const& target, PixFmt internalformat, GLenum pname, i32 bufSize, i32 * params)
{
#ifndef NDEBUG
    if(!glGetInternalformativ)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetInternalformativ(texture_to_enum(target), to_enum(internalformat), pname, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x300)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_30)
STATICINLINE void ProgramGetBinary(glhnd const& program, i32 * length, GLenum * binaryFormat, Bytes const& binary)
{
#ifndef NDEBUG
    if(!glGetProgramBinary)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramBinary(C_OCAST<u32>(program), C_FCAST<i32>(binary.size), length, binaryFormat, binary.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryGetObjectuiv(u32 id, GLenum pname, u32 * params)
{
#ifndef NDEBUG
    if(!glGetQueryObjectuiv)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glGetQueryObjectuivARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryObjectuiv(id, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryGetiv(QueryComp::query_flag const& target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetQueryiv)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glGetQueryivARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryiv(query_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerGetParameterfv(glhnd const& sampler, GLenum pname, scalar * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameterfv(C_OCAST<u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerGetParameteriv(glhnd const& sampler, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameteriv(C_OCAST<u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE const u8 * GetStringi(GLenum name, u32 index)
{
#ifndef NDEBUG
    if(!glGetStringi)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetStringi(name, index);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void SyncGetiv(GLsync sync, GLenum pname, i32 bufSize, i32 * length, i32 * values)
{
#ifndef NDEBUG
    if(!glGetSynciv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSynciv(sync, pname, bufSize, length, values);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void XFGetVarying(glhnd const& program, u32 index, i32 bufSize, i32 * length, i32 * size, GLenum * type, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetTransformFeedbackVarying)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTransformFeedbackVarying(C_OCAST<u32>(program), index, bufSize, length, size, type, name);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE u32 UnifBlockGetIndex(glhnd const& program, const GLchar * uniformBlockName)
{
#ifndef NDEBUG
    if(!glGetUniformBlockIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetUniformBlockIndex(C_OCAST<u32>(program), uniformBlockName);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void UnifGetIndices(glhnd const& program, i32 uniformCount, const GLchar * *const uniformNames, u32 * uniformIndices)
{
#ifndef NDEBUG
    if(!glGetUniformIndices)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformIndices(C_OCAST<u32>(program), uniformCount, uniformNames, uniformIndices);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void UnifGetuiv(glhnd const& program, i32 location, u32 * params)
{
#ifndef NDEBUG
    if(!glGetUniformuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformuiv(C_OCAST<u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOGetIiv(u32 index, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetVertexAttribIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribIiv(index, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOGetIuiv(u32 index, GLenum pname, u32 * params)
{
#ifndef NDEBUG
    if(!glGetVertexAttribIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribIuiv(index, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x300)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_30)
STATICINLINE void FBInvalidate(FramebufferT target, i32 numAttachments, const GLenum * attachments)
{
#ifndef NDEBUG
    if(!glInvalidateFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateFramebuffer(to_enum(target), numAttachments, attachments);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x300)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_30)
STATICINLINE void FBInvalidateSub(GLenum target, i32 numAttachments, const GLenum * attachments, i32 x, i32 y, Size const& size)
{
#ifndef NDEBUG
    if(!glInvalidateSubFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateSubFramebuffer(target, numAttachments, attachments, x, y, size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE u8 IsQuery(u32 id)
{
#ifndef NDEBUG
    if(!glIsQuery)
    {
#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
        if(glIsQueryARB) Throw(undefined_behavior("extension GL_ARB_occlusion_query is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsQuery(id);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE u8 IsSampler(glhnd const& sampler)
{
#ifndef NDEBUG
    if(!glIsSampler)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsSampler(C_OCAST<u32>(sampler));
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE u8 IsSync(GLsync sync)
{
#ifndef NDEBUG
    if(!glIsSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsSync(sync);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE u8 IsTransformFeedback(u32 id)
{
#ifndef NDEBUG
    if(!glIsTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsTransformFeedback(id);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE u8 IsVertexArray(glhnd const& array)
{
#ifndef NDEBUG
    if(!glIsVertexArray)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsVertexArray(C_OCAST<u32>(array));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void * BufMapRange(BufferComp::buf_flag const& target, ptroff offset, ptroff length, RSCA access)
{
#ifndef NDEBUG
    if(!glMapBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glMapBufferRange(buffer_to_enum(target), offset, length, to_enum2(access));
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFPause()
{
#ifndef NDEBUG
    if(!glPauseTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPauseTransformFeedback();
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x300)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_30)
STATICINLINE void ProgramBinary(glhnd const& program, GLenum binaryFormat, const void * binary, i32 length)
{
#ifndef NDEBUG
    if(!glProgramBinary)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramBinary(C_OCAST<u32>(program), binaryFormat, binary, length);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x300)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_30)
STATICINLINE void ProgramParameteri(glhnd const& program, GLenum pname, i32 value)
{
#ifndef NDEBUG
    if(!glProgramParameteri)
    {
#if defined(GL_ARB_geometry_shader4) && GL_ARB_geometry_shader4
        if(glProgramParameteriARB) Throw(undefined_behavior("extension GL_ARB_geometry_shader4 is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramParameteri(C_OCAST<u32>(program), pname, value);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x300)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_30)
STATICINLINE void BufRead(GLenum src)
{
#ifndef NDEBUG
    if(!glReadBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReadBuffer(src);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void RBufStorageMultisample(GLenum target, i32 samples, PixFmt internalformat, Size const& size)
{
#ifndef NDEBUG
    if(!glRenderbufferStorageMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glRenderbufferStorageMultisample(target, samples, to_enum(internalformat), size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFResume()
{
#ifndef NDEBUG
    if(!glResumeTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glResumeTransformFeedback();
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameterf(glhnd const& sampler, GLenum pname, scalar param)
{
#ifndef NDEBUG
    if(!glSamplerParameterf)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterf(C_OCAST<u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameterfv(glhnd const& sampler, GLenum pname, const scalar * param)
{
#ifndef NDEBUG
    if(!glSamplerParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterfv(C_OCAST<u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameteri(glhnd const& sampler, GLenum pname, i32 param)
{
#ifndef NDEBUG
    if(!glSamplerParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameteri(C_OCAST<u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameteriv(glhnd const& sampler, GLenum pname, const i32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameteriv(C_OCAST<u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void TexImage3D(TexComp::tex_flag const& target, i32 level, PixFmt internalformat, Size3 const& size, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage3D(texture_to_enum(target), level, to_enum(internalformat), size.width, size.height, size.depth, 0, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x300)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_30)
STATICINLINE void TexStorage2D(TexComp::tex_flag const& target, i32 levels, PixFmt internalformat, Size const& size)
{
#ifndef NDEBUG
    if(!glTexStorage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage2D(texture_to_enum(target), levels, to_enum(internalformat), size.w, size.h);
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x300)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_30)
STATICINLINE void TexStorage3D(TexComp::tex_flag const& target, i32 levels, PixFmt internalformat, Size3 const& size)
{
#ifndef NDEBUG
    if(!glTexStorage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage3D(texture_to_enum(target), levels, to_enum(internalformat), size.width, size.height, size.depth);
}
#endif


#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void TexSubImage3D(TexComp::tex_flag const& target, i32 level, Point3 const& offset, Size3 const& size, PixCmp format, BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexSubImage3D(texture_to_enum(target), level, offset.x, offset.y, offset.z, size.width, size.height, size.depth, to_enum(format, PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void XFVaryings(glhnd const& program, i32 count, const GLchar * *const varyings, GLenum bufferMode)
{
#ifndef NDEBUG
    if(!glTransformFeedbackVaryings)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTransformFeedbackVaryings(C_OCAST<u32>(program), count, varyings, bufferMode);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif1ui(i32 location, u32 v0)
{
#ifndef NDEBUG
    if(!glUniform1ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1ui(location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(i32 location, Span<const u32> const& value)
{
#ifndef NDEBUG
    if(!glUniform1uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1uiv(location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif2ui(i32 location, u32 v0, u32 v1)
{
#ifndef NDEBUG
    if(!glUniform2ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2ui(location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(i32 location, Span<Vecui2> const& value)
{
#ifndef NDEBUG
    if(!glUniform2uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2uiv(location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif3ui(i32 location, u32 v0, u32 v1, u32 v2)
{
#ifndef NDEBUG
    if(!glUniform3ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3ui(location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(i32 location, Span<Vecui3> const& value)
{
#ifndef NDEBUG
    if(!glUniform3uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3uiv(location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif4ui(i32 location, u32 v0, u32 v1, u32 v2, u32 v3)
{
#ifndef NDEBUG
    if(!glUniform4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4ui(location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(i32 location, Span<Vecui4> const& value)
{
#ifndef NDEBUG
    if(!glUniform4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4uiv(location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void UnifBlockBinding(glhnd const& program, u32 uniformBlockIndex, u32 uniformBlockBinding)
{
#ifndef NDEBUG
    if(!glUniformBlockBinding)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformBlockBinding(C_OCAST<u32>(program), uniformBlockIndex, uniformBlockBinding);
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf2_3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2x3fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf2_4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2x4fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf3_2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3x2fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf3_4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3x4fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf4_2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4x2fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(i32 location, bool transpose, Span<Matf4_3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4x3fv(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE u8 BufUnmap(BufferComp::buf_flag const& target)
{
#ifndef NDEBUG
    if(!glUnmapBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glUnmapBuffer(buffer_to_enum(target));
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void VAODivisor(u32 index, u32 divisor)
{
#ifndef NDEBUG
    if(!glVertexAttribDivisor)
    {
#if defined(GL_ARB_instanced_arrays) && GL_ARB_instanced_arrays
        if(glVertexAttribDivisorARB) Throw(undefined_behavior("extension GL_ARB_instanced_arrays is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribDivisor(index, divisor);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOI4i(u32 index, i32 x, i32 y, i32 z, i32 w)
{
#ifndef NDEBUG
    if(!glVertexAttribI4i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4i(index, x, y, z, w);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOI4iv(u32 index, const i32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI4iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4iv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOI4ui(u32 index, u32 x, u32 y, u32 z, u32 w)
{
#ifndef NDEBUG
    if(!glVertexAttribI4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4ui(index, x, y, z, w);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOI4uiv(u32 index, const u32 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribI4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4uiv(index, v);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOIPointer(u32 index, i32 size, GLenum type, i32 stride, const void * pointer)
{
#ifndef NDEBUG
    if(!glVertexAttribIPointer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribIPointer(index, size, type, stride, pointer);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void SyncWait(GLsync sync, RSCA flags, u64 timeout)
{
#ifndef NDEBUG
    if(!glWaitSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glWaitSync(sync, to_enum2(flags), timeout);
}
#endif

};
template<typename ReqVer>
struct CGL_31
    : CGL_30<ReqVer>
{
using Parent = CGL_30<ReqVer>;
using Parent::Unif1f;
using Parent::Uniffv;
using Parent::Unif1i;
using Parent::Unifiv;
using Parent::Unif2f;
using Parent::Unif2i;
using Parent::Unif3f;
using Parent::Unif3i;
using Parent::Unif4f;
using Parent::Unif4i;
using Parent::Unif1ui;
using Parent::Unifuiv;
using Parent::Unif2ui;
using Parent::Unif3ui;
using Parent::Unif4ui;

#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void ProgramActiveShader(glhnd const& pipeline, glhnd const& program)
{
#ifndef NDEBUG
    if(!glActiveShaderProgram)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glActiveShaderProgram(C_OCAST<u32>(pipeline), C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x310)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_31)
STATICINLINE void TexBindImage(u32 unit, glhnd const& texture, i32 level, bool layered, i32 layer, RSCA access, PixFmt format)
{
#ifndef NDEBUG
    if(!glBindImageTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindImageTexture(unit, C_OCAST<u32>(texture), level, layered ? GL_TRUE : GL_FALSE, layer, to_enum2(access), to_enum(format));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineBind(glhnd const& pipeline)
{
#ifndef NDEBUG
    if(!glBindProgramPipeline)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindProgramPipeline(C_OCAST<u32>(pipeline));
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VertBufBind(u32 bindingindex, glhnd const& buffer, ptroff offset, i32 stride)
{
#ifndef NDEBUG
    if(!glBindVertexBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindVertexBuffer(bindingindex, C_OCAST<u32>(buffer), offset, stride);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE u32 ShaderProgramvAllocEx(ShaderStage type, Span<const GLchar *> const& strings)
{
#ifndef NDEBUG
    if(!glCreateShaderProgramv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glCreateShaderProgramv(to_enum1(type), C_FCAST<i32>(strings.elements), strings.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineFree(Span<const u32> const& pipelines)
{
#ifndef NDEBUG
    if(!glDeleteProgramPipelines)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteProgramPipelines(C_FCAST<i32>(pipelines.elements), pipelines.data);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void DispatchCompute(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z)
{
#ifndef NDEBUG
    if(!glDispatchCompute)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void DispatchComputeIndirect(ptroff indirect)
{
#ifndef NDEBUG
    if(!glDispatchComputeIndirect)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDispatchComputeIndirect(indirect);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x310)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_31)
STATICINLINE void DrawArraysIndirect(DrwMd const& mode, uintptr indirect)
{
#ifndef NDEBUG
    if(!glDrawArraysIndirect)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawArraysIndirect(to_enum(mode.t, mode.c), C_RCAST<const void *>(indirect));
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x310)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_31)
STATICINLINE void DrawElementsIndirect(DrwMd const& mode, TypeEnum type, uintptr indirect)
{
#ifndef NDEBUG
    if(!glDrawElementsIndirect)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElementsIndirect(to_enum(mode.t, mode.c), to_enum(type), C_RCAST<const void *>(indirect));
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void FBParameteri(FramebufferT target, GLenum pname, i32 param)
{
#ifndef NDEBUG
    if(!glFramebufferParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferParameteri(to_enum(target), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineAlloc(Span<u32> const& pipelines)
{
#ifndef NDEBUG
    if(!glGenProgramPipelines)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenProgramPipelines(C_FCAST<i32>(pipelines.elements), pipelines.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x310)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_31)
STATICINLINE void BoolGeti_v(BufferComp::buf_flag const& target, u32 index, u8 * data)
{
#ifndef NDEBUG
    if(!glGetBooleani_v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetBooleani_v(buffer_to_enum(target), index, data);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void FBGetParameteriv(FramebufferT target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetFramebufferParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetFramebufferParameteriv(to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x310)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_31)
STATICINLINE void GetMultisamplefv(GLenum pname, u32 index, scalar * val)
{
#ifndef NDEBUG
    if(!glGetMultisamplefv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetMultisamplefv(pname, index, val);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void ProgramGetInterfaceiv(glhnd const& program, GLenum programInterface, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramInterfaceiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramInterfaceiv(C_OCAST<u32>(program), programInterface, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineGetInfoLog(glhnd const& pipeline, i32 bufSize, i32 * length, GLchar * infoLog)
{
#ifndef NDEBUG
    if(!glGetProgramPipelineInfoLog)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramPipelineInfoLog(C_OCAST<u32>(pipeline), bufSize, length, infoLog);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineGetiv(glhnd const& pipeline, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramPipelineiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramPipelineiv(C_OCAST<u32>(pipeline), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE u32 ProgramGetResourceIndex(glhnd const& program, GLenum programInterface, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetProgramResourceIndex(C_OCAST<u32>(program), programInterface, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE i32 ProgramGetResourceLocation(glhnd const& program, GLenum programInterface, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetProgramResourceLocation(C_OCAST<u32>(program), programInterface, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void ProgramGetResourceName(glhnd const& program, GLenum programInterface, u32 index, i32 bufSize, i32 * length, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramResourceName(C_OCAST<u32>(program), programInterface, index, bufSize, length, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void ProgramGetResourceiv(glhnd const& program, GLenum programInterface, u32 index, i32 propCount, const GLenum * props, i32 bufSize, i32 * length, i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramResourceiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramResourceiv(C_OCAST<u32>(program), programInterface, index, propCount, props, bufSize, length, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x310)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_31)
STATICINLINE void TexGetLevelParameterfv(TexComp::tex_flag const& target, i32 level, GLenum pname, scalar * params)
{
#ifndef NDEBUG
    if(!glGetTexLevelParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexLevelParameterfv(texture_to_enum(target), level, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x310)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_31)
STATICINLINE void TexGetLevelParameteriv(TexComp::tex_flag const& target, i32 level, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetTexLevelParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexLevelParameteriv(texture_to_enum(target), level, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE u8 IsProgramPipeline(glhnd const& pipeline)
{
#ifndef NDEBUG
    if(!glIsProgramPipeline)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsProgramPipeline(C_OCAST<u32>(pipeline));
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x310)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_31)
STATICINLINE void MemoryBarrier(u32 barriers)
{
#ifndef NDEBUG
    if(!glMemoryBarrier)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMemoryBarrier(barriers);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x310)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_31)
STATICINLINE void MemoryBarrierByRegion(u32 barriers)
{
#ifndef NDEBUG
    if(!glMemoryBarrierByRegion)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMemoryBarrierByRegion(barriers);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif1f(glhnd const& program, i32 location, scalar v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1f(C_OCAST<u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif1i(glhnd const& program, i32 location, i32 v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1i(C_OCAST<u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, i32 location, Span<const i32> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1iv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif1ui(glhnd const& program, i32 location, u32 v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1ui(C_OCAST<u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, i32 location, Span<const u32> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1uiv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif2f(glhnd const& program, i32 location, scalar v0, scalar v1)
{
#ifndef NDEBUG
    if(!glProgramUniform2f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2f(C_OCAST<u32>(program), location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, Span<Vecf2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif2i(glhnd const& program, i32 location, i32 v0, i32 v1)
{
#ifndef NDEBUG
    if(!glProgramUniform2i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2i(C_OCAST<u32>(program), location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, i32 location, Span<Veci2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2iv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif2ui(glhnd const& program, i32 location, u32 v0, u32 v1)
{
#ifndef NDEBUG
    if(!glProgramUniform2ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2ui(C_OCAST<u32>(program), location, v0, v1);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, i32 location, Span<Vecui2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2uiv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif3f(glhnd const& program, i32 location, scalar v0, scalar v1, scalar v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3f(C_OCAST<u32>(program), location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, Span<Vecf3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif3i(glhnd const& program, i32 location, i32 v0, i32 v1, i32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3i(C_OCAST<u32>(program), location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, i32 location, Span<Veci3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3iv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif3ui(glhnd const& program, i32 location, u32 v0, u32 v1, u32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3ui(C_OCAST<u32>(program), location, v0, v1, v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, i32 location, Span<Vecui3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3uiv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif4f(glhnd const& program, i32 location, scalar v0, scalar v1, scalar v2, scalar v3)
{
#ifndef NDEBUG
    if(!glProgramUniform4f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4f(C_OCAST<u32>(program), location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, Span<Vecf4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif4i(glhnd const& program, i32 location, i32 v0, i32 v1, i32 v2, i32 v3)
{
#ifndef NDEBUG
    if(!glProgramUniform4i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4i(C_OCAST<u32>(program), location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, i32 location, Span<Veci4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4iv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif4ui(glhnd const& program, i32 location, u32 v0, u32 v1, u32 v2, u32 v3)
{
#ifndef NDEBUG
    if(!glProgramUniform4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4ui(C_OCAST<u32>(program), location, v0, v1, v2, v3);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, i32 location, Span<Vecui4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4uiv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf2_3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2x3fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf2_4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2x4fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf3_2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3x2fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf3_4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3x4fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf4_2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4x2fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, i32 location, bool transpose, Span<Matf4_3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4x3fv(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x310)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_31)
STATICINLINE void SampleMaski(u32 maskNumber, u32 mask)
{
#ifndef NDEBUG
    if(!glSampleMaski)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSampleMaski(maskNumber, mask);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void TexStorage2DMultisample(TexComp::tex_flag const& target, i32 samples, PixFmt internalformat, Size const& size, bool fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTexStorage2DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage2DMultisample(texture_to_enum(target), samples, to_enum(internalformat), size.w, size.h, fixedsamplelocations ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void ProgramUseStages(glhnd const& pipeline, ShaderStage stages, glhnd const& program)
{
#ifndef NDEBUG
    if(!glUseProgramStages)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUseProgramStages(C_OCAST<u32>(pipeline), to_enum2(stages), C_OCAST<u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineValidate(glhnd const& pipeline)
{
#ifndef NDEBUG
    if(!glValidateProgramPipeline)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glValidateProgramPipeline(C_OCAST<u32>(pipeline));
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VAOBinding(u32 attribindex, u32 bindingindex)
{
#ifndef NDEBUG
    if(!glVertexAttribBinding)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribBinding(attribindex, bindingindex);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VAOFormat(u32 attribindex, i32 size, GLenum type, bool normalized, u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexAttribFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribFormat(attribindex, size, type, normalized ? GL_TRUE : GL_FALSE, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VAOIFormat(u32 attribindex, i32 size, GLenum type, u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexAttribIFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribIFormat(attribindex, size, type, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VAOBindingDivisor(u32 bindingindex, u32 divisor)
{
#ifndef NDEBUG
    if(!glVertexBindingDivisor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexBindingDivisor(bindingindex, divisor);
}
#endif

};
template<typename ReqVer>
struct CGL_32
    : CGL_31<ReqVer>
{
using Parent = CGL_31<ReqVer>;

#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x320)
GL_VERSION_REQ_ES(GLESVER_32)
STATICINLINE void BlendBarrier()
{
#ifndef NDEBUG
    if(!glBlendBarrier)
    {
#if defined(GL_KHR_blend_equation_advanced) && GL_KHR_blend_equation_advanced
        if(glBlendBarrierKHR) Throw(undefined_behavior("extension GL_KHR_blend_equation_advanced is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendBarrier();
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void BlendEquationSeparatei(u32 buf, GLenum modeRGB, GLenum modeAlpha)
{
#ifndef NDEBUG
    if(!glBlendEquationSeparatei)
    {
#if defined(GL_ARB_draw_buffers_blend) && GL_ARB_draw_buffers_blend
        if(glBlendEquationSeparateiARB) Throw(undefined_behavior("extension GL_ARB_draw_buffers_blend is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendEquationSeparatei(buf, modeRGB, modeAlpha);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void BlendEquationi(u32 buf, GLenum mode)
{
#ifndef NDEBUG
    if(!glBlendEquationi)
    {
#if defined(GL_ARB_draw_buffers_blend) && GL_ARB_draw_buffers_blend
        if(glBlendEquationiARB) Throw(undefined_behavior("extension GL_ARB_draw_buffers_blend is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendEquationi(buf, mode);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void BlendFuncSeparatei(u32 buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
#ifndef NDEBUG
    if(!glBlendFuncSeparatei)
    {
#if defined(GL_ARB_draw_buffers_blend) && GL_ARB_draw_buffers_blend
        if(glBlendFuncSeparateiARB) Throw(undefined_behavior("extension GL_ARB_draw_buffers_blend is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendFuncSeparatei(buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void BlendFunci(u32 buf, GLenum src, GLenum dst)
{
#ifndef NDEBUG
    if(!glBlendFunci)
    {
#if defined(GL_ARB_draw_buffers_blend) && GL_ARB_draw_buffers_blend
        if(glBlendFunciARB) Throw(undefined_behavior("extension GL_ARB_draw_buffers_blend is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendFunci(buf, src, dst);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void ColorMaski(u32 index, bool r, bool g, bool b, bool a)
{
#ifndef NDEBUG
    if(!glColorMaski)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glColorMaski(index, r ? GL_TRUE : GL_FALSE, g ? GL_TRUE : GL_FALSE, b ? GL_TRUE : GL_FALSE, a ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void CopyImageSubData(u32 srcName, GLenum srcTarget, i32 srcLevel, i32 srcX, i32 srcY, i32 srcZ, u32 dstName, GLenum dstTarget, i32 dstLevel, i32 dstX, i32 dstY, i32 dstZ, i32 srcWidth, i32 srcHeight, i32 srcDepth)
{
#ifndef NDEBUG
    if(!glCopyImageSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyImageSubData(srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void DebugMessageCallback(GLDEBUGPROC callback, const void * userParam)
{
#ifndef NDEBUG
    if(!glDebugMessageCallback)
    {
#if defined(GL_ARB_debug_output) && GL_ARB_debug_output
        if(glDebugMessageCallbackARB) Throw(undefined_behavior("extension GL_ARB_debug_output is available"));
#endif
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glDebugMessageCallbackKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageCallback(callback, userParam);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void DebugMessageControl(GLenum source, GLenum type, GLenum severity, i32 count, const u32 * ids, bool enabled)
{
#ifndef NDEBUG
    if(!glDebugMessageControl)
    {
#if defined(GL_ARB_debug_output) && GL_ARB_debug_output
        if(glDebugMessageControlARB) Throw(undefined_behavior("extension GL_ARB_debug_output is available"));
#endif
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glDebugMessageControlKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageControl(source, type, severity, count, ids, enabled ? GL_TRUE : GL_FALSE);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void DebugMessageInsert(GLenum source, GLenum type, u32 id, GLenum severity, i32 length, const GLchar * buf)
{
#ifndef NDEBUG
    if(!glDebugMessageInsert)
    {
#if defined(GL_ARB_debug_output) && GL_ARB_debug_output
        if(glDebugMessageInsertARB) Throw(undefined_behavior("extension GL_ARB_debug_output is available"));
#endif
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glDebugMessageInsertKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageInsert(source, type, id, severity, length, buf);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void Disablei(Feature target, u32 index)
{
#ifndef NDEBUG
    if(!glDisablei)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDisablei(to_enum(target), index);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x320)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_32)
STATICINLINE void DrawElementsBaseVertex(DrwMd const& mode, i32 count, TypeEnum type, uintptr indices, i32 basevertex)
{
#ifndef NDEBUG
    if(!glDrawElementsBaseVertex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElementsBaseVertex(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void *>(indices), basevertex);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x320)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_32)
STATICINLINE void DrawElementsInstancedBaseVertex(DrwMd const& mode, i32 count, TypeEnum type, uintptr indices, i32 instancecount, i32 basevertex)
{
#ifndef NDEBUG
    if(!glDrawElementsInstancedBaseVertex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElementsInstancedBaseVertex(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void *>(indices), instancecount, basevertex);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x320)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_32)
STATICINLINE void DrawRangeElementsBaseVertex(DrwMd const& mode, u32 start, u32 end, i32 count, TypeEnum type, uintptr indices, i32 basevertex)
{
#ifndef NDEBUG
    if(!glDrawRangeElementsBaseVertex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawRangeElementsBaseVertex(to_enum(mode.t, mode.c), start, end, count, to_enum(type), C_RCAST<const void *>(indices), basevertex);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void Enablei(Feature target, u32 index)
{
#ifndef NDEBUG
    if(!glEnablei)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEnablei(to_enum(target), index);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x320)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_32)
STATICINLINE void FBTexture(FramebufferT target, GLenum attachment, glhnd const& texture, i32 level)
{
#ifndef NDEBUG
    if(!glFramebufferTexture)
    {
#if defined(GL_ARB_geometry_shader4) && GL_ARB_geometry_shader4
        if(glFramebufferTextureARB) Throw(undefined_behavior("extension GL_ARB_geometry_shader4 is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTexture(to_enum(target), attachment, C_OCAST<u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE u32 GetDebugMessageLog(u32 count, i32 bufSize, GLenum * sources, GLenum * types, u32 * ids, GLenum * severities, i32 * lengths, GLchar * messageLog)
{
#ifndef NDEBUG
    if(!glGetDebugMessageLog)
    {
#if defined(GL_ARB_debug_output) && GL_ARB_debug_output
        if(glGetDebugMessageLogARB) Throw(undefined_behavior("extension GL_ARB_debug_output is available"));
#endif
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glGetDebugMessageLogKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetDebugMessageLog(count, bufSize, sources, types, ids, severities, lengths, messageLog);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE GLenum GetGraphicsResetStatus()
{
#ifndef NDEBUG
    if(!glGetGraphicsResetStatus)
    {
#if defined(GL_KHR_robustness) && GL_KHR_robustness
        if(glGetGraphicsResetStatusKHR) Throw(undefined_behavior("extension GL_KHR_robustness is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetGraphicsResetStatus();
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void GetObjectLabel(GLenum identifier, u32 name, i32 bufSize, i32 * length, GLchar * label)
{
#ifndef NDEBUG
    if(!glGetObjectLabel)
    {
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glGetObjectLabelKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetObjectLabel(identifier, name, bufSize, length, label);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void GetObjectPtrLabel(const void * ptr, i32 bufSize, i32 * length, GLchar * label)
{
#ifndef NDEBUG
    if(!glGetObjectPtrLabel)
    {
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glGetObjectPtrLabelKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetObjectPtrLabel(ptr, bufSize, length, label);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerGetParameterIiv(glhnd const& sampler, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameterIiv(C_OCAST<u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerGetParameterIuiv(glhnd const& sampler, GLenum pname, u32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameterIuiv(C_OCAST<u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void TexGetParameterIiv(TexComp::tex_flag const& target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetTexParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexParameterIiv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void TexGetParameterIuiv(TexComp::tex_flag const& target, GLenum pname, u32 * params)
{
#ifndef NDEBUG
    if(!glGetTexParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexParameterIuiv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void UnifGetnfv(glhnd const& program, i32 location, i32 bufSize, scalar * params)
{
#ifndef NDEBUG
    if(!glGetnUniformfv)
    {
#if defined(GL_KHR_robustness) && GL_KHR_robustness
        if(glGetnUniformfvKHR) Throw(undefined_behavior("extension GL_KHR_robustness is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformfv(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void UnifGetniv(glhnd const& program, i32 location, i32 bufSize, i32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformiv)
    {
#if defined(GL_KHR_robustness) && GL_KHR_robustness
        if(glGetnUniformivKHR) Throw(undefined_behavior("extension GL_KHR_robustness is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformiv(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void UnifGetnuiv(glhnd const& program, i32 location, i32 bufSize, u32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformuiv)
    {
#if defined(GL_KHR_robustness) && GL_KHR_robustness
        if(glGetnUniformuivKHR) Throw(undefined_behavior("extension GL_KHR_robustness is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformuiv(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE u8 IsEnabledi(Feature target, u32 index)
{
#ifndef NDEBUG
    if(!glIsEnabledi)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsEnabledi(to_enum(target), index);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void MinSampleShading(scalar value)
{
#ifndef NDEBUG
    if(!glMinSampleShading)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMinSampleShading(value);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void ObjectLabel(GLenum identifier, u32 name, i32 length, const GLchar * label)
{
#ifndef NDEBUG
    if(!glObjectLabel)
    {
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glObjectLabelKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glObjectLabel(identifier, name, length, label);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void ObjectPtrLabel(const void * ptr, i32 length, const GLchar * label)
{
#ifndef NDEBUG
    if(!glObjectPtrLabel)
    {
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glObjectPtrLabelKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glObjectPtrLabel(ptr, length, label);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void PatchParameteri(PatchProperty pname, i32 value)
{
#ifndef NDEBUG
    if(!glPatchParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPatchParameteri(to_enum(pname), value);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void PopDebugGroup()
{
#ifndef NDEBUG
    if(!glPopDebugGroup)
    {
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glPopDebugGroupKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPopDebugGroup();
}
#endif


#if GL_VERSION_VERIFY(GL_VERSION_NONE, 0x320)
GL_VERSION_REQ_ES(GLESVER_32)
STATICINLINE void PrimitiveBoundingBox(scalar minX, scalar minY, scalar minZ, scalar minW, scalar maxX, scalar maxY, scalar maxZ, scalar maxW)
{
#ifndef NDEBUG
    if(!glPrimitiveBoundingBox)
    {
#if defined(GL_ARB_ES3_2_compatibility) && GL_ARB_ES3_2_compatibility
        if(glPrimitiveBoundingBoxARB) Throw(undefined_behavior("extension GL_ARB_ES3_2_compatibility is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPrimitiveBoundingBox(minX, minY, minZ, minW, maxX, maxY, maxZ, maxW);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void PushDebugGroup(GLenum source, u32 id, i32 length, const GLchar * message)
{
#ifndef NDEBUG
    if(!glPushDebugGroup)
    {
#if defined(GL_KHR_debug) && GL_KHR_debug
        if(glPushDebugGroupKHR) Throw(undefined_behavior("extension GL_KHR_debug is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPushDebugGroup(source, id, length, message);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void ReadnPixels(i32 x, i32 y, Size const& size, PixCmp format, BitFmt type, i32 bufSize, void * data)
{
#ifndef NDEBUG
    if(!glReadnPixels)
    {
#if defined(GL_ARB_robustness) && GL_ARB_robustness
        if(glReadnPixelsARB) Throw(undefined_behavior("extension GL_ARB_robustness is available"));
#endif
#if defined(GL_KHR_robustness) && GL_KHR_robustness
        if(glReadnPixelsKHR) Throw(undefined_behavior("extension GL_KHR_robustness is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReadnPixels(x, y, size.w, size.h, to_enum(format, PixFmt::None), to_enum(type), bufSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerParameterIiv(glhnd const& sampler, GLenum pname, const i32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterIiv(C_OCAST<u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerParameterIuiv(glhnd const& sampler, GLenum pname, const u32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterIuiv(C_OCAST<u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x320)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_32)
STATICINLINE void TexBuffer(TexComp::tex_flag const& target, PixFmt internalformat, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glTexBuffer)
    {
#if defined(GL_ARB_texture_buffer_object) && GL_ARB_texture_buffer_object
        if(glTexBufferARB) Throw(undefined_behavior("extension GL_ARB_texture_buffer_object is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexBuffer(texture_to_enum(target), to_enum(internalformat), C_OCAST<u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void TexBufferRange(TexComp::tex_flag const& target, PixFmt internalformat, glhnd const& buffer, ptroff offset, ptroff size)
{
#ifndef NDEBUG
    if(!glTexBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexBufferRange(texture_to_enum(target), to_enum(internalformat), C_OCAST<u32>(buffer), offset, size);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void TexParameterIiv(TexComp::tex_flag const& target, GLenum pname, const i32 * params)
{
#ifndef NDEBUG
    if(!glTexParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexParameterIiv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void TexParameterIuiv(TexComp::tex_flag const& target, GLenum pname, const u32 * params)
{
#ifndef NDEBUG
    if(!glTexParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexParameterIuiv(texture_to_enum(target), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void TexStorage3DMultisample(TexComp::tex_flag const& target, i32 samples, PixFmt internalformat, Size3 const& size, bool fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTexStorage3DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage3DMultisample(texture_to_enum(target), samples, to_enum(internalformat), size.width, size.height, size.depth, fixedsamplelocations ? GL_TRUE : GL_FALSE);
}
#endif

};
#endif
#if defined(GL_ARB_ES3_2_compatibility) && GL_ARB_ES3_2_compatibility
template<typename ReqVer>
struct CGL_ARB_ES3_2_compatibility
{

#if 0
/* No template check available */
STATICINLINE void PrimitiveBoundingBoxARB(scalar minX, scalar minY, scalar minZ, scalar minW, scalar maxX, scalar maxY, scalar maxZ, scalar maxW)
{
#ifndef NDEBUG
    if(!glPrimitiveBoundingBoxARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPrimitiveBoundingBoxARB(minX, minY, minZ, minW, maxX, maxY, maxZ, maxW);
}
#endif

};
#endif // GL_ARB_ES3_2_compatibility

#if defined(GL_ARB_bindless_texture) && GL_ARB_bindless_texture
template<typename ReqVer>
struct CGL_ARB_bindless_texture
{

#if 0
/* No template check available */
STATICINLINE u64 GetImageHandleARB(glhnd const& texture, i32 level, bool layered, i32 layer, GLenum format)
{
#ifndef NDEBUG
    if(!glGetImageHandleARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetImageHandleARB(C_OCAST<u32>(texture), level, layered ? GL_TRUE : GL_FALSE, layer, format);
}
#endif


#if 0
/* No template check available */
STATICINLINE u64 TexGetHandleARB(glhnd const& texture)
{
#ifndef NDEBUG
    if(!glGetTextureHandleARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetTextureHandleARB(C_OCAST<u32>(texture));
}
#endif


#if 0
/* No template check available */
STATICINLINE u64 TexGetSamplerHandleARB(glhnd const& texture, glhnd const& sampler)
{
#ifndef NDEBUG
    if(!glGetTextureSamplerHandleARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetTextureSamplerHandleARB(C_OCAST<u32>(texture), C_OCAST<u32>(sampler));
}
#endif


#if 0
/* No template check available */
STATICINLINE void VAOGetLui64vARB(u32 index, GLenum pname, u64 * params)
{
#ifndef NDEBUG
    if(!glGetVertexAttribLui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexAttribLui64vARB(index, pname, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE u8 IsImageHandleResidentARB(u64 handle)
{
#ifndef NDEBUG
    if(!glIsImageHandleResidentARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsImageHandleResidentARB(handle);
}
#endif


#if 0
/* No template check available */
STATICINLINE u8 IsTextureHandleResidentARB(u64 handle)
{
#ifndef NDEBUG
    if(!glIsTextureHandleResidentARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsTextureHandleResidentARB(handle);
}
#endif


#if 0
/* No template check available */
STATICINLINE void MakeImageHandleNonResidentARB(u64 handle)
{
#ifndef NDEBUG
    if(!glMakeImageHandleNonResidentARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMakeImageHandleNonResidentARB(handle);
}
#endif


#if 0
/* No template check available */
STATICINLINE void MakeImageHandleResidentARB(u64 handle, RSCA access)
{
#ifndef NDEBUG
    if(!glMakeImageHandleResidentARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMakeImageHandleResidentARB(handle, to_enum2(access));
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexMakeHandleNonResidentARB(u64 handle)
{
#ifndef NDEBUG
    if(!glMakeTextureHandleNonResidentARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMakeTextureHandleNonResidentARB(handle);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexMakeHandleResidentARB(u64 handle)
{
#ifndef NDEBUG
    if(!glMakeTextureHandleResidentARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMakeTextureHandleResidentARB(handle);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifHandleui64vARB(glhnd const& program, i32 location, Span<const u64> const& values)
{
#ifndef NDEBUG
    if(!glProgramUniformHandleui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformHandleui64vARB(C_OCAST<u32>(program), location, C_FCAST<i32>(values.elements), C_RCAST<const u64*>(values.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifHandleui64vARB(i32 location, Span<const u64> const& value)
{
#ifndef NDEBUG
    if(!glUniformHandleui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformHandleui64vARB(location, C_FCAST<i32>(value.elements), C_RCAST<const u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void VAOL1ui64ARB(u32 index, u64 x)
{
#ifndef NDEBUG
    if(!glVertexAttribL1ui64ARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL1ui64ARB(index, x);
}
#endif


#if 0
/* No template check available */
STATICINLINE void VAOL1ui64vARB(u32 index, const u64 * v)
{
#ifndef NDEBUG
    if(!glVertexAttribL1ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL1ui64vARB(index, v);
}
#endif

};
#endif // GL_ARB_bindless_texture

#if defined(GL_ARB_compute_variable_group_size) && GL_ARB_compute_variable_group_size
template<typename ReqVer>
struct CGL_ARB_compute_variable_group_size
{

#if 0
/* No template check available */
STATICINLINE void DispatchComputeGroupSizeARB(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z, u32 group_size_x, u32 group_size_y, u32 group_size_z)
{
#ifndef NDEBUG
    if(!glDispatchComputeGroupSizeARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDispatchComputeGroupSizeARB(num_groups_x, num_groups_y, num_groups_z, group_size_x, group_size_y, group_size_z);
}
#endif

};
#endif // GL_ARB_compute_variable_group_size

#if defined(GL_ARB_debug_output) && GL_ARB_debug_output
template<typename ReqVer>
struct CGL_ARB_debug_output
{

#if 0
/* No template check available */
STATICINLINE void DebugMessageCallbackARB(GLDEBUGPROCARB callback, const void * userParam)
{
#ifndef NDEBUG
    if(!glDebugMessageCallbackARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageCallbackARB(callback, userParam);
}
#endif


#if 0
/* No template check available */
STATICINLINE void DebugMessageControlARB(GLenum source, GLenum type, GLenum severity, i32 count, const u32 * ids, bool enabled)
{
#ifndef NDEBUG
    if(!glDebugMessageControlARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageControlARB(source, type, severity, count, ids, enabled ? GL_TRUE : GL_FALSE);
}
#endif


#if 0
/* No template check available */
STATICINLINE void DebugMessageInsertARB(GLenum source, GLenum type, u32 id, GLenum severity, i32 length, const GLchar * buf)
{
#ifndef NDEBUG
    if(!glDebugMessageInsertARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageInsertARB(source, type, id, severity, length, buf);
}
#endif


#if 0
/* No template check available */
STATICINLINE u32 GetDebugMessageLogARB(u32 count, i32 bufSize, GLenum * sources, GLenum * types, u32 * ids, GLenum * severities, i32 * lengths, GLchar * messageLog)
{
#ifndef NDEBUG
    if(!glGetDebugMessageLogARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetDebugMessageLogARB(count, bufSize, sources, types, ids, severities, lengths, messageLog);
}
#endif

};
#endif // GL_ARB_debug_output

#if defined(GL_ARB_draw_buffers) && GL_ARB_draw_buffers
template<typename ReqVer>
struct CGL_ARB_draw_buffers
{

#if 0
/* No template check available */
STATICINLINE void DrawBuffersARB(i32 n, const GLenum * bufs)
{
#ifndef NDEBUG
    if(!glDrawBuffersARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawBuffersARB(n, bufs);
}
#endif

};
#endif // GL_ARB_draw_buffers

#if defined(GL_ARB_draw_buffers_blend) && GL_ARB_draw_buffers_blend
template<typename ReqVer>
struct CGL_ARB_draw_buffers_blend
{

#if 0
/* No template check available */
STATICINLINE void BlendEquationSeparateiARB(u32 buf, GLenum modeRGB, GLenum modeAlpha)
{
#ifndef NDEBUG
    if(!glBlendEquationSeparateiARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendEquationSeparateiARB(buf, modeRGB, modeAlpha);
}
#endif


#if 0
/* No template check available */
STATICINLINE void BlendEquationiARB(u32 buf, GLenum mode)
{
#ifndef NDEBUG
    if(!glBlendEquationiARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendEquationiARB(buf, mode);
}
#endif


#if 0
/* No template check available */
STATICINLINE void BlendFuncSeparateiARB(u32 buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
#ifndef NDEBUG
    if(!glBlendFuncSeparateiARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendFuncSeparateiARB(buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
}
#endif


#if 0
/* No template check available */
STATICINLINE void BlendFunciARB(u32 buf, GLenum src, GLenum dst)
{
#ifndef NDEBUG
    if(!glBlendFunciARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendFunciARB(buf, src, dst);
}
#endif

};
#endif // GL_ARB_draw_buffers_blend

#if defined(GL_ARB_draw_instanced) && GL_ARB_draw_instanced
template<typename ReqVer>
struct CGL_ARB_draw_instanced
{

#if 0
/* No template check available */
STATICINLINE void DrawArraysInstancedARB(DrwMd const& mode, i32 first, i32 count, i32 primcount)
{
#ifndef NDEBUG
    if(!glDrawArraysInstancedARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawArraysInstancedARB(to_enum(mode.t, mode.c), first, count, primcount);
}
#endif


#if 0
/* No template check available */
STATICINLINE void DrawElementsInstancedARB(DrwMd const& mode, i32 count, TypeEnum type, uintptr indices, i32 primcount)
{
#ifndef NDEBUG
    if(!glDrawElementsInstancedARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawElementsInstancedARB(to_enum(mode.t, mode.c), count, to_enum(type), C_RCAST<const void *>(indices), primcount);
}
#endif

};
#endif // GL_ARB_draw_instanced

#if defined(GL_ARB_geometry_shader4) && GL_ARB_geometry_shader4
template<typename ReqVer>
struct CGL_ARB_geometry_shader4
{

#if 0
/* No template check available */
STATICINLINE void FBTextureARB(FramebufferT target, GLenum attachment, glhnd const& texture, i32 level)
{
#ifndef NDEBUG
    if(!glFramebufferTextureARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTextureARB(to_enum(target), attachment, C_OCAST<u32>(texture), level);
}
#endif


#if 0
/* No template check available */
STATICINLINE void FBTextureFaceARB(FramebufferT target, GLenum attachment, glhnd const& texture, i32 level, TexComp::tex_flag const& face)
{
#ifndef NDEBUG
    if(!glFramebufferTextureFaceARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTextureFaceARB(to_enum(target), attachment, C_OCAST<u32>(texture), level, texture_to_enum(face));
}
#endif


#if 0
/* No template check available */
STATICINLINE void FBTextureLayerARB(FramebufferT target, GLenum attachment, glhnd const& texture, i32 level, i32 layer)
{
#ifndef NDEBUG
    if(!glFramebufferTextureLayerARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTextureLayerARB(to_enum(target), attachment, C_OCAST<u32>(texture), level, layer);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ProgramParameteriARB(glhnd const& program, GLenum pname, i32 value)
{
#ifndef NDEBUG
    if(!glProgramParameteriARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramParameteriARB(C_OCAST<u32>(program), pname, value);
}
#endif

};
#endif // GL_ARB_geometry_shader4

#if defined(GL_ARB_gl_spirv) && GL_ARB_gl_spirv
template<typename ReqVer>
struct CGL_ARB_gl_spirv
{

#if 0
/* No template check available */
STATICINLINE void ShaderSpecializeARB(glhnd const& shader, const GLchar * pEntryPoint, u32 numSpecializationConstants, const u32 * pConstantIndex, const u32 * pConstantValue)
{
#ifndef NDEBUG
    if(!glSpecializeShaderARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSpecializeShaderARB(C_OCAST<u32>(shader), pEntryPoint, numSpecializationConstants, pConstantIndex, pConstantValue);
}
#endif

};
#endif // GL_ARB_gl_spirv

#if defined(GL_ARB_gpu_shader_int64) && GL_ARB_gpu_shader_int64
template<typename ReqVer>
struct CGL_ARB_gpu_shader_int64
{

#if 0
/* No template check available */
STATICINLINE void UnifGeti64vARB(glhnd const& program, i32 location, i64 * params)
{
#ifndef NDEBUG
    if(!glGetUniformi64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformi64vARB(C_OCAST<u32>(program), location, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetui64vARB(glhnd const& program, i32 location, u64 * params)
{
#ifndef NDEBUG
    if(!glGetUniformui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformui64vARB(C_OCAST<u32>(program), location, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetni64vARB(glhnd const& program, i32 location, i32 bufSize, i64 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformi64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformi64vARB(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnui64vARB(glhnd const& program, i32 location, i32 bufSize, u64 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformui64vARB(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1i64vARB(glhnd const& program, i32 location, Span<const i64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1i64vARB(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1ui64vARB(glhnd const& program, i32 location, Span<const u64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1ui64vARB(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2i64vARB(glhnd const& program, i32 location, Span<const i64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2i64vARB(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2ui64vARB(glhnd const& program, i32 location, Span<const u64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2ui64vARB(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3i64vARB(glhnd const& program, i32 location, Span<const i64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3i64vARB(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3ui64vARB(glhnd const& program, i32 location, Span<const u64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3ui64vARB(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4i64vARB(glhnd const& program, i32 location, Span<const i64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4i64vARB(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4ui64vARB(glhnd const& program, i32 location, Span<const u64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4ui64vARB(C_OCAST<u32>(program), location, C_FCAST<i32>(value.elements), C_RCAST<const u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1i64vARB(i32 location, Span<const i64> const& value)
{
#ifndef NDEBUG
    if(!glUniform1i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1i64vARB(location, C_FCAST<i32>(value.elements), C_RCAST<const i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1ui64vARB(i32 location, Span<const u64> const& value)
{
#ifndef NDEBUG
    if(!glUniform1ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1ui64vARB(location, C_FCAST<i32>(value.elements), C_RCAST<const u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2i64vARB(i32 location, Span<const i64> const& value)
{
#ifndef NDEBUG
    if(!glUniform2i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2i64vARB(location, C_FCAST<i32>(value.elements), C_RCAST<const i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2ui64vARB(i32 location, Span<const u64> const& value)
{
#ifndef NDEBUG
    if(!glUniform2ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2ui64vARB(location, C_FCAST<i32>(value.elements), C_RCAST<const u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3i64vARB(i32 location, Span<const i64> const& value)
{
#ifndef NDEBUG
    if(!glUniform3i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3i64vARB(location, C_FCAST<i32>(value.elements), C_RCAST<const i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3ui64vARB(i32 location, Span<const u64> const& value)
{
#ifndef NDEBUG
    if(!glUniform3ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3ui64vARB(location, C_FCAST<i32>(value.elements), C_RCAST<const u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4i64vARB(i32 location, Span<const i64> const& value)
{
#ifndef NDEBUG
    if(!glUniform4i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4i64vARB(location, C_FCAST<i32>(value.elements), C_RCAST<const i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4ui64vARB(i32 location, Span<const u64> const& value)
{
#ifndef NDEBUG
    if(!glUniform4ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4ui64vARB(location, C_FCAST<i32>(value.elements), C_RCAST<const u64*>(value.data));
}
#endif

};
#endif // GL_ARB_gpu_shader_int64

#if defined(GL_ARB_indirect_parameters) && GL_ARB_indirect_parameters
template<typename ReqVer>
struct CGL_ARB_indirect_parameters
{

#if 0
/* No template check available */
STATICINLINE void MultiDrawArraysIndirectCountARB(DrwMd const& mode, uintptr indirect, ptroff drawcount, i32 maxdrawcount, i32 stride)
{
#ifndef NDEBUG
    if(!glMultiDrawArraysIndirectCountARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMultiDrawArraysIndirectCountARB(to_enum(mode.t, mode.c), C_RCAST<const void *>(indirect), drawcount, maxdrawcount, stride);
}
#endif


#if 0
/* No template check available */
STATICINLINE void MultiDrawElementsIndirectCountARB(DrwMd const& mode, TypeEnum type, uintptr indirect, ptroff drawcount, i32 maxdrawcount, i32 stride)
{
#ifndef NDEBUG
    if(!glMultiDrawElementsIndirectCountARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMultiDrawElementsIndirectCountARB(to_enum(mode.t, mode.c), to_enum(type), C_RCAST<const void *>(indirect), drawcount, maxdrawcount, stride);
}
#endif

};
#endif // GL_ARB_indirect_parameters

#if defined(GL_ARB_instanced_arrays) && GL_ARB_instanced_arrays
template<typename ReqVer>
struct CGL_ARB_instanced_arrays
{

#if 0
/* No template check available */
STATICINLINE void VAODivisorARB(u32 index, u32 divisor)
{
#ifndef NDEBUG
    if(!glVertexAttribDivisorARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribDivisorARB(index, divisor);
}
#endif

};
#endif // GL_ARB_instanced_arrays

#if defined(GL_ARB_multisample) && GL_ARB_multisample
template<typename ReqVer>
struct CGL_ARB_multisample
{

#if 0
/* No template check available */
STATICINLINE void SampleCoverageARB(scalar value, bool invert)
{
#ifndef NDEBUG
    if(!glSampleCoverageARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSampleCoverageARB(value, invert ? GL_TRUE : GL_FALSE);
}
#endif

};
#endif // GL_ARB_multisample

#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
template<typename ReqVer>
struct CGL_ARB_occlusion_query
{

#if 0
/* No template check available */
STATICINLINE void QueryBeginARB(GLenum target, u32 id)
{
#ifndef NDEBUG
    if(!glBeginQueryARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBeginQueryARB(target, id);
}
#endif


#if 0
/* No template check available */
STATICINLINE void QueryDeleteARB(i32 n, const u32 * ids)
{
#ifndef NDEBUG
    if(!glDeleteQueriesARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteQueriesARB(n, ids);
}
#endif


#if 0
/* No template check available */
STATICINLINE void QueryEndARB(QueryComp::query_flag const& target)
{
#ifndef NDEBUG
    if(!glEndQueryARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEndQueryARB(query_to_enum(target));
}
#endif


#if 0
/* No template check available */
STATICINLINE void QueryGenARB(i32 n, u32 * ids)
{
#ifndef NDEBUG
    if(!glGenQueriesARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenQueriesARB(n, ids);
}
#endif


#if 0
/* No template check available */
STATICINLINE void QueryGetObjectivARB(u32 id, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetQueryObjectivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryObjectivARB(id, pname, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void QueryGetObjectuivARB(u32 id, GLenum pname, u32 * params)
{
#ifndef NDEBUG
    if(!glGetQueryObjectuivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryObjectuivARB(id, pname, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void QueryGetivARB(QueryComp::query_flag const& target, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetQueryivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryivARB(query_to_enum(target), pname, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE u8 IsQueryARB(u32 id)
{
#ifndef NDEBUG
    if(!glIsQueryARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsQueryARB(id);
}
#endif

};
#endif // GL_ARB_occlusion_query

#if defined(GL_ARB_parallel_shader_compile) && GL_ARB_parallel_shader_compile
template<typename ReqVer>
struct CGL_ARB_parallel_shader_compile
{

#if 0
/* No template check available */
STATICINLINE void ShaderMaxCompilerThreadsARB(u32 count)
{
#ifndef NDEBUG
    if(!glMaxShaderCompilerThreadsARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMaxShaderCompilerThreadsARB(count);
}
#endif

};
#endif // GL_ARB_parallel_shader_compile

#if defined(GL_ARB_point_parameters) && GL_ARB_point_parameters
template<typename ReqVer>
struct CGL_ARB_point_parameters
{

#if 0
/* No template check available */
STATICINLINE void PointParameterfARB(GLenum pname, scalar param)
{
#ifndef NDEBUG
    if(!glPointParameterfARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPointParameterfARB(pname, param);
}
#endif


#if 0
/* No template check available */
STATICINLINE void PointParameterfvARB(GLenum pname, const scalar * params)
{
#ifndef NDEBUG
    if(!glPointParameterfvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPointParameterfvARB(pname, params);
}
#endif

};
#endif // GL_ARB_point_parameters

#if defined(GL_ARB_robustness) && GL_ARB_robustness
template<typename ReqVer>
struct CGL_ARB_robustness
{

#if 0
/* No template check available */
STATICINLINE GLenum GetGraphicsResetStatusARB()
{
#ifndef NDEBUG
    if(!glGetGraphicsResetStatusARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetGraphicsResetStatusARB();
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnColorTableARB(GLenum target, PixCmp format, BitFmt type, i32 bufSize, void * table)
{
#ifndef NDEBUG
    if(!glGetnColorTableARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnColorTableARB(target, to_enum(format, PixFmt::None), to_enum(type), bufSize, table);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexGetnCompressedImageARB(TexComp::tex_flag const& target, i32 lod, i32 bufSize, void * img)
{
#ifndef NDEBUG
    if(!glGetnCompressedTexImageARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnCompressedTexImageARB(texture_to_enum(target), lod, bufSize, img);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnConvolutionFilterARB(GLenum target, PixCmp format, BitFmt type, i32 bufSize, void * image)
{
#ifndef NDEBUG
    if(!glGetnConvolutionFilterARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnConvolutionFilterARB(target, to_enum(format, PixFmt::None), to_enum(type), bufSize, image);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnHistogramARB(GLenum target, bool reset, PixCmp format, BitFmt type, i32 bufSize, void * values)
{
#ifndef NDEBUG
    if(!glGetnHistogramARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnHistogramARB(target, reset ? GL_TRUE : GL_FALSE, to_enum(format, PixFmt::None), to_enum(type), bufSize, values);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnMapdvARB(GLenum target, GLenum query, i32 bufSize, bigscalar * v)
{
#ifndef NDEBUG
    if(!glGetnMapdvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnMapdvARB(target, query, bufSize, v);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnMapfvARB(GLenum target, GLenum query, i32 bufSize, scalar * v)
{
#ifndef NDEBUG
    if(!glGetnMapfvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnMapfvARB(target, query, bufSize, v);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnMapivARB(GLenum target, GLenum query, i32 bufSize, i32 * v)
{
#ifndef NDEBUG
    if(!glGetnMapivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnMapivARB(target, query, bufSize, v);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnMinmaxARB(GLenum target, bool reset, PixCmp format, BitFmt type, i32 bufSize, void * values)
{
#ifndef NDEBUG
    if(!glGetnMinmaxARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnMinmaxARB(target, reset ? GL_TRUE : GL_FALSE, to_enum(format, PixFmt::None), to_enum(type), bufSize, values);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnPixelMapfvARB(GLenum map, i32 bufSize, scalar * values)
{
#ifndef NDEBUG
    if(!glGetnPixelMapfvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnPixelMapfvARB(map, bufSize, values);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnPixelMapuivARB(GLenum map, i32 bufSize, u32 * values)
{
#ifndef NDEBUG
    if(!glGetnPixelMapuivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnPixelMapuivARB(map, bufSize, values);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnPixelMapusvARB(GLenum map, i32 bufSize, u16 * values)
{
#ifndef NDEBUG
    if(!glGetnPixelMapusvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnPixelMapusvARB(map, bufSize, values);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnPolygonStippleARB(i32 bufSize, u8 * pattern)
{
#ifndef NDEBUG
    if(!glGetnPolygonStippleARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnPolygonStippleARB(bufSize, pattern);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnSeparableFilterARB(GLenum target, PixCmp format, BitFmt type, i32 rowBufSize, void * row, i32 columnBufSize, void * column, void * span)
{
#ifndef NDEBUG
    if(!glGetnSeparableFilterARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnSeparableFilterARB(target, to_enum(format, PixFmt::None), to_enum(type), rowBufSize, row, columnBufSize, column, span);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexGetnImageARB(TexComp::tex_flag const& target, i32 level, PixCmp format, BitFmt type, i32 bufSize, void * img)
{
#ifndef NDEBUG
    if(!glGetnTexImageARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnTexImageARB(texture_to_enum(target), level, to_enum(format, PixFmt::None), to_enum(type), bufSize, img);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetndvARB(glhnd const& program, i32 location, i32 bufSize, bigscalar * params)
{
#ifndef NDEBUG
    if(!glGetnUniformdvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformdvARB(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnfvARB(glhnd const& program, i32 location, i32 bufSize, scalar * params)
{
#ifndef NDEBUG
    if(!glGetnUniformfvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformfvARB(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnivARB(glhnd const& program, i32 location, i32 bufSize, i32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformivARB(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnuivARB(glhnd const& program, i32 location, i32 bufSize, u32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformuivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformuivARB(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ReadnPixelsARB(i32 x, i32 y, Size const& size, PixCmp format, BitFmt type, i32 bufSize, void * data)
{
#ifndef NDEBUG
    if(!glReadnPixelsARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReadnPixelsARB(x, y, size.w, size.h, to_enum(format, PixFmt::None), to_enum(type), bufSize, data);
}
#endif

};
#endif // GL_ARB_robustness

#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
template<typename ReqVer>
struct CGL_ARB_shader_objects
{

#if 0
/* No template check available */
STATICINLINE void AttachObjectARB(GLhandleARB containerObj, GLhandleARB obj)
{
#ifndef NDEBUG
    if(!glAttachObjectARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glAttachObjectARB(containerObj, obj);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ShaderCompileARB(GLhandleARB shaderObj)
{
#ifndef NDEBUG
    if(!glCompileShaderARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompileShaderARB(shaderObj);
}
#endif


#if 0
/* No template check available */
STATICINLINE GLhandleARB ProgramCreateObjectARB()
{
#ifndef NDEBUG
    if(!glCreateProgramObjectARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glCreateProgramObjectARB();
}
#endif


#if 0
/* No template check available */
STATICINLINE GLhandleARB ShaderCreateObjectARB(ShaderStage shaderType)
{
#ifndef NDEBUG
    if(!glCreateShaderObjectARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glCreateShaderObjectARB(to_enum1(shaderType));
}
#endif


#if 0
/* No template check available */
STATICINLINE void DeleteObjectARB(GLhandleARB obj)
{
#ifndef NDEBUG
    if(!glDeleteObjectARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteObjectARB(obj);
}
#endif


#if 0
/* No template check available */
STATICINLINE void DetachObjectARB(GLhandleARB containerObj, GLhandleARB attachedObj)
{
#ifndef NDEBUG
    if(!glDetachObjectARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDetachObjectARB(containerObj, attachedObj);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ActiveUnifGetARB(GLhandleARB programObj, u32 index, i32 maxLength, i32 * length, i32 * size, GLenum * type, GLcharARB * name)
{
#ifndef NDEBUG
    if(!glGetActiveUniformARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformARB(programObj, index, maxLength, length, size, type, name);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetAttachedObjectsARB(GLhandleARB containerObj, i32 maxCount, i32 * count, GLhandleARB * obj)
{
#ifndef NDEBUG
    if(!glGetAttachedObjectsARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetAttachedObjectsARB(containerObj, maxCount, count, obj);
}
#endif


#if 0
/* No template check available */
STATICINLINE GLhandleARB GetHandleARB(GLenum pname)
{
#ifndef NDEBUG
    if(!glGetHandleARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetHandleARB(pname);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetInfoLogARB(GLhandleARB obj, i32 maxLength, i32 * length, GLcharARB * infoLog)
{
#ifndef NDEBUG
    if(!glGetInfoLogARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetInfoLogARB(obj, maxLength, length, infoLog);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetObjectParameterfvARB(GLhandleARB obj, GLenum pname, scalar * params)
{
#ifndef NDEBUG
    if(!glGetObjectParameterfvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetObjectParameterfvARB(obj, pname, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetObjectParameterivARB(GLhandleARB obj, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetObjectParameterivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetObjectParameterivARB(obj, pname, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ShaderGetSourceARB(GLhandleARB obj, i32 maxLength, i32 * length, GLcharARB * source)
{
#ifndef NDEBUG
    if(!glGetShaderSourceARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderSourceARB(obj, maxLength, length, source);
}
#endif


#if 0
/* No template check available */
STATICINLINE i32 UnifGetLocationARB(GLhandleARB programObj, const GLcharARB * name)
{
#ifndef NDEBUG
    if(!glGetUniformLocationARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetUniformLocationARB(programObj, name);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetfvARB(GLhandleARB programObj, i32 location, scalar * params)
{
#ifndef NDEBUG
    if(!glGetUniformfvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformfvARB(programObj, location, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetivARB(GLhandleARB programObj, i32 location, i32 * params)
{
#ifndef NDEBUG
    if(!glGetUniformivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformivARB(programObj, location, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ProgramLinkARB(GLhandleARB programObj)
{
#ifndef NDEBUG
    if(!glLinkProgramARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glLinkProgramARB(programObj);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ShaderSourceARB(GLhandleARB shaderObj, i32 count, const GLcharARB * * string, const i32 * length)
{
#ifndef NDEBUG
    if(!glShaderSourceARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glShaderSourceARB(shaderObj, count, string, length);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1fARB(i32 location, scalar v0)
{
#ifndef NDEBUG
    if(!glUniform1fARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1fARB(location, v0);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1fvARB(i32 location, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glUniform1fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1fvARB(location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1iARB(i32 location, i32 v0)
{
#ifndef NDEBUG
    if(!glUniform1iARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1iARB(location, v0);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1ivARB(i32 location, Span<const i32> const& value)
{
#ifndef NDEBUG
    if(!glUniform1ivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1ivARB(location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2fARB(i32 location, scalar v0, scalar v1)
{
#ifndef NDEBUG
    if(!glUniform2fARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2fARB(location, v0, v1);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2fvARB(i32 location, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glUniform2fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2fvARB(location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2iARB(i32 location, i32 v0, i32 v1)
{
#ifndef NDEBUG
    if(!glUniform2iARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2iARB(location, v0, v1);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2ivARB(i32 location, Span<const i32> const& value)
{
#ifndef NDEBUG
    if(!glUniform2ivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2ivARB(location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3fARB(i32 location, scalar v0, scalar v1, scalar v2)
{
#ifndef NDEBUG
    if(!glUniform3fARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3fARB(location, v0, v1, v2);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3fvARB(i32 location, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glUniform3fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3fvARB(location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3iARB(i32 location, i32 v0, i32 v1, i32 v2)
{
#ifndef NDEBUG
    if(!glUniform3iARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3iARB(location, v0, v1, v2);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3ivARB(i32 location, Span<const i32> const& value)
{
#ifndef NDEBUG
    if(!glUniform3ivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3ivARB(location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4fARB(i32 location, scalar v0, scalar v1, scalar v2, scalar v3)
{
#ifndef NDEBUG
    if(!glUniform4fARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4fARB(location, v0, v1, v2, v3);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4fvARB(i32 location, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glUniform4fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4fvARB(location, C_FCAST<i32>(value.elements), C_RCAST<const scalar*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4iARB(i32 location, i32 v0, i32 v1, i32 v2, i32 v3)
{
#ifndef NDEBUG
    if(!glUniform4iARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4iARB(location, v0, v1, v2, v3);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4ivARB(i32 location, Span<const i32> const& value)
{
#ifndef NDEBUG
    if(!glUniform4ivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4ivARB(location, C_FCAST<i32>(value.elements), C_RCAST<const i32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifMatrix2fvARB(i32 location, bool transpose, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2fvARB(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifMatrix3fvARB(i32 location, bool transpose, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3fvARB(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifMatrix4fvARB(i32 location, bool transpose, Span<const scalar> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4fvARB(location, C_FCAST<i32>(value.elements), transpose ? GL_TRUE : GL_FALSE, C_RCAST<const scalar*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void ProgramUseObjectARB(GLhandleARB programObj)
{
#ifndef NDEBUG
    if(!glUseProgramObjectARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUseProgramObjectARB(programObj);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ProgramValidateARB(GLhandleARB programObj)
{
#ifndef NDEBUG
    if(!glValidateProgramARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glValidateProgramARB(programObj);
}
#endif

};
#endif // GL_ARB_shader_objects

#if defined(GL_ARB_shading_language_include) && GL_ARB_shading_language_include
template<typename ReqVer>
struct CGL_ARB_shading_language_include
{

#if 0
/* No template check available */
STATICINLINE void ShaderCompileIncludeARB(glhnd const& shader, i32 count, const GLchar * *const path, const i32 * length)
{
#ifndef NDEBUG
    if(!glCompileShaderIncludeARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompileShaderIncludeARB(C_OCAST<u32>(shader), count, path, length);
}
#endif


#if 0
/* No template check available */
STATICINLINE void DeleteNamedStringARB(i32 namelen, const GLchar * name)
{
#ifndef NDEBUG
    if(!glDeleteNamedStringARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteNamedStringARB(namelen, name);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetNamedStringARB(i32 namelen, const GLchar * name, i32 bufSize, i32 * stringlen, GLchar * string)
{
#ifndef NDEBUG
    if(!glGetNamedStringARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedStringARB(namelen, name, bufSize, stringlen, string);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetNamedStringivARB(i32 namelen, const GLchar * name, GLenum pname, i32 * params)
{
#ifndef NDEBUG
    if(!glGetNamedStringivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedStringivARB(namelen, name, pname, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE u8 IsNamedStringARB(i32 namelen, const GLchar * name)
{
#ifndef NDEBUG
    if(!glIsNamedStringARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glIsNamedStringARB(namelen, name);
}
#endif


#if 0
/* No template check available */
STATICINLINE void NamedStringARB(GLenum type, i32 namelen, const GLchar * name, i32 stringlen, const GLchar * string)
{
#ifndef NDEBUG
    if(!glNamedStringARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedStringARB(type, namelen, name, stringlen, string);
}
#endif

};
#endif // GL_ARB_shading_language_include

#if defined(GL_ARB_sparse_texture) && GL_ARB_sparse_texture
template<typename ReqVer>
struct CGL_ARB_sparse_texture
{

#if 0
/* No template check available */
STATICINLINE void TexPageCommitmentARB(GLenum target, i32 level, Point3 const& offset, Size3 const& size, bool commit)
{
#ifndef NDEBUG
    if(!glTexPageCommitmentARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexPageCommitmentARB(target, level, offset.x, offset.y, offset.z, size.width, size.height, size.depth, commit ? GL_TRUE : GL_FALSE);
}
#endif

};
#endif // GL_ARB_sparse_texture

#if defined(GL_ARB_texture_buffer_object) && GL_ARB_texture_buffer_object
template<typename ReqVer>
struct CGL_ARB_texture_buffer_object
{

#if 0
/* No template check available */
STATICINLINE void TexBufferARB(TexComp::tex_flag const& target, PixFmt internalformat, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glTexBufferARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexBufferARB(texture_to_enum(target), to_enum(internalformat), C_OCAST<u32>(buffer));
}
#endif

};
#endif // GL_ARB_texture_buffer_object

#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
template<typename ReqVer>
struct CGL_ARB_texture_compression
{

#if 0
/* No template check available */
STATICINLINE void TexCompressedImage1DARB(TexComp::tex_flag const& target, i32 level, CompFmt internalformat, i32 width, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexImage1DARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexImage1DARB(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), width, 0, imageSize, data);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexCompressedImage2DARB(TexComp::tex_flag const& target, i32 level, CompFmt internalformat, Size const& size, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexImage2DARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexImage2DARB(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), size.w, size.h, 0, imageSize, data);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexCompressedImage3DARB(TexComp::tex_flag const& target, i32 level, CompFmt internalformat, Size3 const& size, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexImage3DARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexImage3DARB(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), size.width, size.height, size.depth, 0, imageSize, data);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexCompressedSubImage1DARB(TexComp::tex_flag const& target, i32 level, i32 xoffset, i32 width, CompFmt format, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexSubImage1DARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexSubImage1DARB(texture_to_enum(target), level, xoffset, width, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexCompressedSubImage2DARB(TexComp::tex_flag const& target, i32 level, Point const& offset, Size const& size, CompFmt format, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexSubImage2DARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexSubImage2DARB(texture_to_enum(target), level, offset.x, offset.y, size.w, size.h, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexCompressedSubImage3DARB(TexComp::tex_flag const& target, i32 level, Point3 const& offset, Size3 const& size, CompFmt format, i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexSubImage3DARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexSubImage3DARB(texture_to_enum(target), level, offset.x, offset.y, offset.z, size.width, size.height, size.depth, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexGetCompressedImageARB(TexComp::tex_flag const& target, i32 level, void * img)
{
#ifndef NDEBUG
    if(!glGetCompressedTexImageARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetCompressedTexImageARB(texture_to_enum(target), level, img);
}
#endif

};
#endif // GL_ARB_texture_compression

#if defined(GL_KHR_blend_equation_advanced) && GL_KHR_blend_equation_advanced
template<typename ReqVer>
struct CGL_KHR_blend_equation_advanced
{

#if 0
/* No template check available */
STATICINLINE void BlendBarrierKHR()
{
#ifndef NDEBUG
    if(!glBlendBarrierKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBlendBarrierKHR();
}
#endif

};
#endif // GL_KHR_blend_equation_advanced

#if defined(GL_KHR_debug) && GL_KHR_debug
template<typename ReqVer>
struct CGL_KHR_debug
{

#if 0
/* No template check available */
STATICINLINE void DebugMessageCallbackKHR(GLDEBUGPROCKHR callback, const void * userParam)
{
#ifndef NDEBUG
    if(!glDebugMessageCallbackKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageCallbackKHR(callback, userParam);
}
#endif


#if 0
/* No template check available */
STATICINLINE void DebugMessageControlKHR(GLenum source, GLenum type, GLenum severity, i32 count, const u32 * ids, bool enabled)
{
#ifndef NDEBUG
    if(!glDebugMessageControlKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageControlKHR(source, type, severity, count, ids, enabled ? GL_TRUE : GL_FALSE);
}
#endif


#if 0
/* No template check available */
STATICINLINE void DebugMessageInsertKHR(GLenum source, GLenum type, u32 id, GLenum severity, i32 length, const GLchar * buf)
{
#ifndef NDEBUG
    if(!glDebugMessageInsertKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageInsertKHR(source, type, id, severity, length, buf);
}
#endif


#if 0
/* No template check available */
STATICINLINE u32 GetDebugMessageLogKHR(u32 count, i32 bufSize, GLenum * sources, GLenum * types, u32 * ids, GLenum * severities, i32 * lengths, GLchar * messageLog)
{
#ifndef NDEBUG
    if(!glGetDebugMessageLogKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetDebugMessageLogKHR(count, bufSize, sources, types, ids, severities, lengths, messageLog);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetObjectLabelKHR(GLenum identifier, u32 name, i32 bufSize, i32 * length, GLchar * label)
{
#ifndef NDEBUG
    if(!glGetObjectLabelKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetObjectLabelKHR(identifier, name, bufSize, length, label);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetObjectPtrLabelKHR(const void * ptr, i32 bufSize, i32 * length, GLchar * label)
{
#ifndef NDEBUG
    if(!glGetObjectPtrLabelKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetObjectPtrLabelKHR(ptr, bufSize, length, label);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetPointervKHR(GLenum pname, void * * params)
{
#ifndef NDEBUG
    if(!glGetPointervKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetPointervKHR(pname, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ObjectLabelKHR(GLenum identifier, u32 name, i32 length, const GLchar * label)
{
#ifndef NDEBUG
    if(!glObjectLabelKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glObjectLabelKHR(identifier, name, length, label);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ObjectPtrLabelKHR(const void * ptr, i32 length, const GLchar * label)
{
#ifndef NDEBUG
    if(!glObjectPtrLabelKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glObjectPtrLabelKHR(ptr, length, label);
}
#endif


#if 0
/* No template check available */
STATICINLINE void PopDebugGroupKHR()
{
#ifndef NDEBUG
    if(!glPopDebugGroupKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPopDebugGroupKHR();
}
#endif


#if 0
/* No template check available */
STATICINLINE void PushDebugGroupKHR(GLenum source, u32 id, i32 length, const GLchar * message)
{
#ifndef NDEBUG
    if(!glPushDebugGroupKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPushDebugGroupKHR(source, id, length, message);
}
#endif

};
#endif // GL_KHR_debug

#if defined(GL_KHR_parallel_shader_compile) && GL_KHR_parallel_shader_compile
template<typename ReqVer>
struct CGL_KHR_parallel_shader_compile
{

#if 0
/* No template check available */
STATICINLINE void ShaderMaxCompilerThreadsKHR(u32 count)
{
#ifndef NDEBUG
    if(!glMaxShaderCompilerThreadsKHR)
    {
#if defined(GL_ARB_parallel_shader_compile) && GL_ARB_parallel_shader_compile
        if(glMaxShaderCompilerThreadsARB) Throw(undefined_behavior("extension GL_ARB_parallel_shader_compile is available"));
#endif
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glMaxShaderCompilerThreadsKHR(count);
}
#endif

};
#endif // GL_KHR_parallel_shader_compile

#if defined(GL_KHR_robustness) && GL_KHR_robustness
template<typename ReqVer>
struct CGL_KHR_robustness
{

#if 0
/* No template check available */
STATICINLINE GLenum GetGraphicsResetStatusKHR()
{
#ifndef NDEBUG
    if(!glGetGraphicsResetStatusKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return glGetGraphicsResetStatusKHR();
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnfvKHR(glhnd const& program, i32 location, i32 bufSize, scalar * params)
{
#ifndef NDEBUG
    if(!glGetnUniformfvKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformfvKHR(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnivKHR(glhnd const& program, i32 location, i32 bufSize, i32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformivKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformivKHR(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnuivKHR(glhnd const& program, i32 location, i32 bufSize, u32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformuivKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformuivKHR(C_OCAST<u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ReadnPixelsKHR(i32 x, i32 y, Size const& size, PixCmp format, BitFmt type, i32 bufSize, void * data)
{
#ifndef NDEBUG
    if(!glReadnPixelsKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReadnPixelsKHR(x, y, size.w, size.h, to_enum(format, PixFmt::None), to_enum(type), bufSize, data);
}
#endif

};
#endif // GL_KHR_robustness

} // CGL
} // Coffee

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#pragma GCC diagnostic pop
#endif
