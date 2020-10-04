#pragma once
#include "all_levels_include.h"

#if defined(COFFEE_GCC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-bool-conversion"
#elif defined(COFFEE_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpointer-bool-conversion"
#endif

namespace glwrap {
using namespace Coffee::CGL;

#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
template<typename ReqVer>
struct v10
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
STATICINLINE void Clear(libc_types::u32 mask)
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
STATICINLINE void ClearColor(typing::vectors::tvector<libc_types::f32,4> const& color)
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
STATICINLINE void ClearDepth(libc_types::f64 depth)
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
STATICINLINE void ClearStencil(libc_types::i32 s)
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
STATICINLINE void ColorMask(typing::vectors::tvector<libc_types::u8,4> const& color)
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
STATICINLINE void DepthMask(libc_types::u8 flag)
{
#ifndef NDEBUG
    if(!glDepthMask)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthMask(flag);
}
#endif


#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_10)
STATICINLINE void DepthRange(libc_types::f64 n, libc_types::f64 f)
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
STATICINLINE void BoolGetv(GLenum pname, libc_types::u8 * data)
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
STATICINLINE void ScalarGetv(GLenum pname, libc_types::f64 * data)
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
    return (GLenum)glGetError();
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void ScalarfGetv(GLenum pname, libc_types::f32 * data)
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
STATICINLINE void IntegerGetv(GLenum pname, libc_types::i32 * data)
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
STATICINLINE libc_types::cstring  GetString(GLenum name)
{
#ifndef NDEBUG
    if(!glGetString)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::cstring )glGetString(name);
}
#endif


#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_10)
STATICINLINE void TexGetImage(TexComp::tex_flag const& target, libc_types::i32 level, typing::PixCmp format, typing::pixels::BitFmt type, void * pixels)
{
#ifndef NDEBUG
    if(!glGetTexImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTexImage(texture_to_enum(target), level, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x310)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_31)
STATICINLINE void TexGetLevelParameterfv(TexComp::tex_flag const& target, libc_types::i32 level, GLenum pname, libc_types::f32 * params)
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
STATICINLINE void TexGetLevelParameteriv(TexComp::tex_flag const& target, libc_types::i32 level, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void TexGetParameterfv(TexComp::tex_flag const& target, GLenum pname, libc_types::f32 * params)
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
STATICINLINE void TexGetParameteriv(TexComp::tex_flag const& target, GLenum pname, libc_types::i32 * params)
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
STATICINLINE libc_types::u8 IsEnabled(Feature cap)
{
#ifndef NDEBUG
    if(!glIsEnabled)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsEnabled(to_enum(cap));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void LineWidth(libc_types::f32 width)
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
STATICINLINE void PixelStoref(GLenum pname, libc_types::f32 param)
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
STATICINLINE void PixelStorei(GLenum pname, libc_types::i32 param)
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
STATICINLINE void PointSize(libc_types::f32 size)
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
STATICINLINE void ReadPixels(typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, void * pixels)
{
#ifndef NDEBUG
    if(!glReadPixels)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReadPixels(x[0], x[1], width.w, width.h, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Scissor(typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glScissor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glScissor(x[0], x[1], width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void StencilFunc(GLenum func, libc_types::i32 ref, libc_types::u32 mask)
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
STATICINLINE void StencilMask(libc_types::u32 mask)
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
STATICINLINE void TexImage1D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::PixFmt internalformat, libc_types::i32 width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage1D(texture_to_enum(target), level, to_enum(internalformat), width, 0, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexImage2D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage2D(texture_to_enum(target), level, to_enum(internalformat), width.w, width.h, 0, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexParameterf(TexComp::tex_flag const& target, GLenum pname, libc_types::f32 param)
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
STATICINLINE void TexParameterfv(TexComp::tex_flag const& target, GLenum pname, const libc_types::f32 * params)
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
STATICINLINE void TexParameteri(TexComp::tex_flag const& target, GLenum pname, libc_types::i32 param)
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
STATICINLINE void TexParameteriv(TexComp::tex_flag const& target, GLenum pname, const libc_types::i32 * params)
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
STATICINLINE void Viewport(typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glViewport)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glViewport(x[0], x[1], width.w, width.h);
}
#endif

};
template<typename ReqVer>
struct v11
    : v10<ReqVer>
{
using Parent = v10<ReqVer>;

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
    glBindTexture(texture_to_enum(target), C_OCAST<libc_types::u32>(texture));
}
#endif


#if GL_VERSION_VERIFY(0x110, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_11)
STATICINLINE void TexCopyImage1D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::PixFmt internalformat, typing::vectors::tvector<libc_types::i32, 2> x, libc_types::i32 width)
{
#ifndef NDEBUG
    if(!glCopyTexImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexImage1D(texture_to_enum(target), level, to_enum(internalformat), x[0], x[1], width, 0);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexCopyImage2D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::PixFmt internalformat, typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glCopyTexImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexImage2D(texture_to_enum(target), level, to_enum(internalformat), x[0], x[1], width.w, width.h, 0);
}
#endif


#if GL_VERSION_VERIFY(0x110, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_11)
STATICINLINE void TexCopySubImage1D(TexComp::tex_flag const& target, libc_types::i32 level, libc_types::i32 xoffset, typing::vectors::tvector<libc_types::i32, 2> x, libc_types::i32 width)
{
#ifndef NDEBUG
    if(!glCopyTexSubImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexSubImage1D(texture_to_enum(target), level, xoffset, x[0], x[1], width);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexCopySubImage2D(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_2d<libc_types::i32> const& xoffset, typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glCopyTexSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexSubImage2D(texture_to_enum(target), level, xoffset.x, xoffset.y, x[0], x[1], width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexFree(semantic::Span<const libc_types::u32>&& textures)
{
#ifndef NDEBUG
    if(!glDeleteTextures)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteTextures(C_FCAST<libc_types::i32>(textures.elements), textures.data);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void DrawArrays(DrwMd const& mode, libc_types::i32 first, libc_types::i32 count)
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
STATICINLINE void DrawElements(DrwMd const& mode, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices)
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
STATICINLINE void TexAlloc(semantic::Span<libc_types::u32>&& textures)
{
#ifndef NDEBUG
    if(!glGenTextures)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenTextures(C_FCAST<libc_types::i32>(textures.elements), textures.data);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE libc_types::u8 IsTexture(glhnd const& texture)
{
#ifndef NDEBUG
    if(!glIsTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsTexture(C_OCAST<libc_types::u32>(texture));
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void PolygonOffset(libc_types::f32 factor, libc_types::f32 units)
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
STATICINLINE void TexSubImage1D(TexComp::tex_flag const& target, libc_types::i32 level, libc_types::i32 xoffset, libc_types::i32 width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexSubImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexSubImage1D(texture_to_enum(target), level, xoffset, width, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexSubImage2D(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_2d<libc_types::i32> const& xoffset, typing::geometry::size_2d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexSubImage2D(texture_to_enum(target), level, xoffset.x, xoffset.y, width.w, width.h, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif

};
template<typename ReqVer>
struct v12
    : v11<ReqVer>
{
using Parent = v11<ReqVer>;

#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void TexCopySubImage3D(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glCopyTexSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexSubImage3D(texture_to_enum(target), level, xoffset.x, xoffset.y, xoffset.z, x[0], x[1], width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void DrawRangeElements(DrwMd const& mode, libc_types::u32 start, libc_types::u32 end, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices)
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
STATICINLINE void TexImage3D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::PixFmt internalformat, typing::geometry::size_3d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage3D(texture_to_enum(target), level, to_enum(internalformat), width.width, width.height, width.depth, 0, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void TexSubImage3D(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::geometry::size_3d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexSubImage3D(texture_to_enum(target), level, xoffset.x, xoffset.y, xoffset.z, width.width, width.height, width.depth, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif

};
template<typename ReqVer>
struct v13
    : v12<ReqVer>
{
using Parent = v12<ReqVer>;

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
STATICINLINE void TexCompressedImage1D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::CompFmt internalformat, libc_types::i32 width, libc_types::i32 imageSize, const void * data)
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
STATICINLINE void TexCompressedImage2D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::CompFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width, libc_types::i32 imageSize, const void * data)
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
    glCompressedTexImage2D(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), width.w, width.h, 0, imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x300)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_30)
STATICINLINE void TexCompressedImage3D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::CompFmt internalformat, typing::geometry::size_3d<libc_types::i32> const& width, libc_types::i32 imageSize, const void * data)
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
    glCompressedTexImage3D(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), width.width, width.height, width.depth, 0, imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_13)
STATICINLINE void TexCompressedSubImage1D(TexComp::tex_flag const& target, libc_types::i32 level, libc_types::i32 xoffset, libc_types::i32 width, typing::pixels::CompFmt format, libc_types::i32 imageSize, const void * data)
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
STATICINLINE void TexCompressedSubImage2D(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_2d<libc_types::i32> const& xoffset, typing::geometry::size_2d<libc_types::i32> const& width, typing::pixels::CompFmt format, libc_types::i32 imageSize, const void * data)
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
    glCompressedTexSubImage2D(texture_to_enum(target), level, xoffset.x, xoffset.y, width.w, width.h, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x300)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_30)
STATICINLINE void TexCompressedSubImage3D(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::geometry::size_3d<libc_types::i32> const& width, typing::pixels::CompFmt format, libc_types::i32 imageSize, const void * data)
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
    glCompressedTexSubImage3D(texture_to_enum(target), level, xoffset.x, xoffset.y, xoffset.z, width.width, width.height, width.depth, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_13)
STATICINLINE void TexGetCompressedImage(TexComp::tex_flag const& target, libc_types::i32 level, void * img)
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
STATICINLINE void SampleCoverage(libc_types::f32 value, libc_types::u8 invert)
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
    glSampleCoverage(value, invert);
}
#endif

};
template<typename ReqVer>
struct v14
    : v13<ReqVer>
{
using Parent = v13<ReqVer>;

#if GL_VERSION_VERIFY(0x140, 0x200)
GL_VERSION_REQ_COMBO(GLVER_14, GLESVER_20)
STATICINLINE void BlendColor(typing::vectors::tvector<libc_types::f32,4> const& color)
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
STATICINLINE void MultiDrawArrays(DrwMd const& mode, const libc_types::i32 * first, const libc_types::i32 * count, libc_types::i32 drawcount)
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
STATICINLINE void MultiDrawElements(DrwMd const& mode, const libc_types::i32 * count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 drawcount)
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
STATICINLINE void PointParameterf(GLenum pname, libc_types::f32 param)
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
STATICINLINE void PointParameterfv(GLenum pname, const libc_types::f32 * params)
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
STATICINLINE void PointParameteri(GLenum pname, libc_types::i32 param)
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
STATICINLINE void PointParameteriv(GLenum pname, const libc_types::i32 * params)
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
struct v15
    : v14<ReqVer>
{
using Parent = v14<ReqVer>;

#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryBegin(QueryComp::query_flag const& target, glhnd const& id)
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
    glBeginQuery(query_to_enum(target), C_OCAST<libc_types::u32>(id));
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
    glBindBuffer(buffer_to_enum(target), C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufData(BufferComp::buf_flag const& target, semantic::BytesConst const& data, semantic::RSCA usage)
{
#ifndef NDEBUG
    if(!glBufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBufferData(buffer_to_enum(target), C_FCAST<libc_types::ptroff>(data.size), data.data, to_enum1(usage));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufSubData(BufferComp::buf_flag const& target, libc_types::ptroff offset, semantic::BytesConst const& data)
{
#ifndef NDEBUG
    if(!glBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBufferSubData(buffer_to_enum(target), offset, C_FCAST<libc_types::ptroff>(data.size), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufFree(semantic::Span<const libc_types::u32>&& buffers)
{
#ifndef NDEBUG
    if(!glDeleteBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteBuffers(C_FCAST<libc_types::i32>(buffers.elements), buffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryFree(semantic::Span<const libc_types::u32>&& ids)
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
    glDeleteQueries(C_FCAST<libc_types::i32>(ids.elements), ids.data);
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
STATICINLINE void BufAlloc(semantic::Span<libc_types::u32>&& buffers)
{
#ifndef NDEBUG
    if(!glGenBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenBuffers(C_FCAST<libc_types::i32>(buffers.elements), buffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryAlloc(semantic::Span<libc_types::u32>&& ids)
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
    glGenQueries(C_FCAST<libc_types::i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufGetParameteriv(BufferComp::buf_flag const& target, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void BufGetSubData(BufferComp::buf_flag const& target, libc_types::ptroff offset, semantic::Bytes const& data)
{
#ifndef NDEBUG
    if(!glGetBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetBufferSubData(buffer_to_enum(target), offset, C_FCAST<libc_types::ptroff>(data.size), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_15)
STATICINLINE void QueryGetObjectiv(glhnd const& id, GLenum pname, libc_types::i32 * params)
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
    glGetQueryObjectiv(C_OCAST<libc_types::u32>(id), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryGetObjectuiv(glhnd const& id, GLenum pname, libc_types::u32 * params)
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
    glGetQueryObjectuiv(C_OCAST<libc_types::u32>(id), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryGetiv(QueryComp::query_flag const& target, GLenum pname, libc_types::i32 * params)
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
STATICINLINE libc_types::u8 IsBuffer(glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glIsBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsBuffer(C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE libc_types::u8 IsQuery(glhnd const& id)
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
    return (libc_types::u8)glIsQuery(C_OCAST<libc_types::u32>(id));
}
#endif


#if GL_VERSION_VERIFY(0x150, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_15)
STATICINLINE void * BufMap(BufferComp::buf_flag const& target, semantic::RSCA access)
{
#ifndef NDEBUG
    if(!glMapBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (void *)glMapBuffer(buffer_to_enum(target), to_enum2(access));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE libc_types::u8 BufUnmap(BufferComp::buf_flag const& target)
{
#ifndef NDEBUG
    if(!glUnmapBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glUnmapBuffer(buffer_to_enum(target));
}
#endif

};
template<typename ReqVer>
struct v20
    : v15<ReqVer>
{
using Parent = v15<ReqVer>;

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
    glAttachShader(C_OCAST<libc_types::u32>(program), C_OCAST<libc_types::u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void AttribBindLocation(glhnd const& program, libc_types::u32 index, const GLchar * name)
{
#ifndef NDEBUG
    if(!glBindAttribLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindAttribLocation(C_OCAST<libc_types::u32>(program), index, name);
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
    glCompileShader(C_OCAST<libc_types::u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE libc_types::u32 ProgramAllocEx()
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
    return (libc_types::u32)glCreateProgram();
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE libc_types::u32 ShaderAllocEx(ShaderStage type)
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
    return (libc_types::u32)glCreateShader(to_enum1(type));
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
    glDeleteProgram(C_OCAST<libc_types::u32>(program));
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
    glDeleteShader(C_OCAST<libc_types::u32>(shader));
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
    glDetachShader(C_OCAST<libc_types::u32>(program), C_OCAST<libc_types::u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAODisableArray(libc_types::u32 index)
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
STATICINLINE void DrawBuffers(libc_types::i32 n, const GLenum * bufs)
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
STATICINLINE void VAOEnableArray(libc_types::u32 index)
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
STATICINLINE void ActiveAttribGet(glhnd const& program, libc_types::u32 index, libc_types::i32 bufSize, libc_types::i32 * length, libc_types::i32 * size, GLenum * type, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetActiveAttrib)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveAttrib(C_OCAST<libc_types::u32>(program), index, bufSize, length, size, type, name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ActiveUnifGet(glhnd const& program, libc_types::u32 index, libc_types::i32 bufSize, libc_types::i32 * length, libc_types::i32 * size, GLenum * type, GLchar * name)
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
    glGetActiveUniform(C_OCAST<libc_types::u32>(program), index, bufSize, length, size, type, name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void AttachedShadersGet(glhnd const& program, libc_types::i32 maxCount, libc_types::i32 * count, libc_types::u32 * shaders)
{
#ifndef NDEBUG
    if(!glGetAttachedShaders)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetAttachedShaders(C_OCAST<libc_types::u32>(program), maxCount, count, shaders);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE libc_types::i32 AttribGetLocation(glhnd const& program, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetAttribLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::i32)glGetAttribLocation(C_OCAST<libc_types::u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramGetInfoLog(glhnd const& program, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * infoLog)
{
#ifndef NDEBUG
    if(!glGetProgramInfoLog)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramInfoLog(C_OCAST<libc_types::u32>(program), bufSize, length, infoLog);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramGetiv(glhnd const& program, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramiv(C_OCAST<libc_types::u32>(program), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderGetInfoLog(glhnd const& shader, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * infoLog)
{
#ifndef NDEBUG
    if(!glGetShaderInfoLog)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderInfoLog(C_OCAST<libc_types::u32>(shader), bufSize, length, infoLog);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderGetSource(glhnd const& shader, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * source)
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
    glGetShaderSource(C_OCAST<libc_types::u32>(shader), bufSize, length, source);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderGetiv(glhnd const& shader, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetShaderiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderiv(C_OCAST<libc_types::u32>(shader), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE libc_types::i32 UnifGetLocation(glhnd const& program, const GLchar * name)
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
    return (libc_types::i32)glGetUniformLocation(C_OCAST<libc_types::u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void UnifGetfv(glhnd const& program, libc_types::i32 location, libc_types::f32 * params)
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
    glGetUniformfv(C_OCAST<libc_types::u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void UnifGetiv(glhnd const& program, libc_types::i32 location, libc_types::i32 * params)
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
    glGetUniformiv(C_OCAST<libc_types::u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAOGetPointerv(libc_types::u32 index, GLenum pname, void * * pointer)
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
STATICINLINE void VAOGetdv(libc_types::u32 index, GLenum pname, libc_types::f64 * params)
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
STATICINLINE void VAOGetfv(libc_types::u32 index, GLenum pname, libc_types::f32 * params)
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
STATICINLINE void VAOGetiv(libc_types::u32 index, GLenum pname, libc_types::i32 * params)
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
STATICINLINE libc_types::u8 IsProgram(glhnd const& program)
{
#ifndef NDEBUG
    if(!glIsProgram)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsProgram(C_OCAST<libc_types::u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE libc_types::u8 IsShader(glhnd const& shader)
{
#ifndef NDEBUG
    if(!glIsShader)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsShader(C_OCAST<libc_types::u32>(shader));
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
    glLinkProgram(C_OCAST<libc_types::u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderSource(glhnd const& shader, libc_types::i32 count, const GLchar * *const string, const libc_types::i32 * length)
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
    glShaderSource(C_OCAST<libc_types::u32>(shader), count, string, length);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void StencilFuncSeparate(GLenum face, GLenum func, libc_types::i32 ref, libc_types::u32 mask)
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
STATICINLINE void StencilMaskSeparate(GLenum face, libc_types::u32 mask)
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
STATICINLINE void Unif1f(libc_types::i32 location, libc_types::f32 v0)
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
STATICINLINE void Uniffv(libc_types::i32 location, semantic::Span<const libc_types::f32> const& value)
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
    glUniform1fv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif1i(libc_types::i32 location, libc_types::i32 v0)
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
STATICINLINE void Unifiv(libc_types::i32 location, semantic::Span<const libc_types::i32> const& value)
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
    glUniform1iv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif2f(libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 2> v0)
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
    glUniform2f(location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecf2> const& value)
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
    glUniform2fv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif2i(libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 2> v0)
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
    glUniform2i(location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(libc_types::i32 location, semantic::Span<typing::vector_types::Veci2> const& value)
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
    glUniform2iv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif3f(libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 3> v0, libc_types::f32 v2)
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
    glUniform3f(location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecf3> const& value)
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
    glUniform3fv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif3i(libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 3> v0, libc_types::i32 v2)
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
    glUniform3i(location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(libc_types::i32 location, semantic::Span<typing::vector_types::Veci3> const& value)
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
    glUniform3iv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif4f(libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 4> v0, libc_types::f32 v2)
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
    glUniform4f(location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecf4> const& value)
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
    glUniform4fv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif4i(libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 4> v0, libc_types::i32 v2)
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
    glUniform4i(location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(libc_types::i32 location, semantic::Span<typing::vector_types::Veci4> const& value)
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
    glUniform4iv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf2> const& value)
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
    glUniformMatrix2fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf3> const& value)
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
    glUniformMatrix3fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf4> const& value)
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
    glUniformMatrix4fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
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
    glUseProgram(C_OCAST<libc_types::u32>(program));
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
    glValidateProgram(C_OCAST<libc_types::u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO1d(libc_types::u32 index, libc_types::f64 x)
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
STATICINLINE void VAO1dv(libc_types::u32 index, const libc_types::f64 * v)
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
STATICINLINE void VAO1f(libc_types::u32 index, libc_types::f32 x)
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
STATICINLINE void VAO1fv(libc_types::u32 index, const libc_types::f32 * v)
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
STATICINLINE void VAO1s(libc_types::u32 index, libc_types::i16 x)
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
STATICINLINE void VAO1sv(libc_types::u32 index, const libc_types::i16 * v)
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
STATICINLINE void VAO2d(libc_types::u32 index, typing::vectors::tvector<libc_types::f64, 2> x)
{
#ifndef NDEBUG
    if(!glVertexAttrib2d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib2d(index, x[0], x[1]);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO2dv(libc_types::u32 index, const libc_types::f64 * v)
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
STATICINLINE void VAO2f(libc_types::u32 index, typing::vectors::tvector<libc_types::f32, 2> x)
{
#ifndef NDEBUG
    if(!glVertexAttrib2f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib2f(index, x[0], x[1]);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO2fv(libc_types::u32 index, const libc_types::f32 * v)
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
STATICINLINE void VAO2s(libc_types::u32 index, typing::vectors::tvector<libc_types::i16, 2> x)
{
#ifndef NDEBUG
    if(!glVertexAttrib2s)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib2s(index, x[0], x[1]);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO2sv(libc_types::u32 index, const libc_types::i16 * v)
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
STATICINLINE void VAO3d(libc_types::u32 index, typing::vectors::tvector<libc_types::f64, 3> x, libc_types::f64 z)
{
#ifndef NDEBUG
    if(!glVertexAttrib3d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib3d(index, x[0], x[1], z);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO3dv(libc_types::u32 index, const libc_types::f64 * v)
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
STATICINLINE void VAO3f(libc_types::u32 index, typing::vectors::tvector<libc_types::f32, 3> x, libc_types::f32 z)
{
#ifndef NDEBUG
    if(!glVertexAttrib3f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib3f(index, x[0], x[1], z);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO3fv(libc_types::u32 index, const libc_types::f32 * v)
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
STATICINLINE void VAO3s(libc_types::u32 index, typing::vectors::tvector<libc_types::i16, 3> x, libc_types::i16 z)
{
#ifndef NDEBUG
    if(!glVertexAttrib3s)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib3s(index, x[0], x[1], z);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO3sv(libc_types::u32 index, const libc_types::i16 * v)
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
STATICINLINE void VAO4Nbv(libc_types::u32 index, const libc_types::i8 * v)
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
STATICINLINE void VAO4Niv(libc_types::u32 index, const libc_types::i32 * v)
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
STATICINLINE void VAO4Nsv(libc_types::u32 index, const libc_types::i16 * v)
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
STATICINLINE void VAO4Nub(libc_types::u32 index, typing::vectors::tvector<libc_types::u8, 4> x, libc_types::u8 z)
{
#ifndef NDEBUG
    if(!glVertexAttrib4Nub)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4Nub(index, x[0], x[1], z, x[3]);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4Nubv(libc_types::u32 index, const libc_types::u8 * v)
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
STATICINLINE void VAO4Nuiv(libc_types::u32 index, const libc_types::u32 * v)
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
STATICINLINE void VAO4Nusv(libc_types::u32 index, const libc_types::u16 * v)
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
STATICINLINE void VAO4bv(libc_types::u32 index, const libc_types::i8 * v)
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
STATICINLINE void VAO4d(libc_types::u32 index, typing::vectors::tvector<libc_types::f64, 4> x, libc_types::f64 z)
{
#ifndef NDEBUG
    if(!glVertexAttrib4d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4d(index, x[0], x[1], z, x[3]);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4dv(libc_types::u32 index, const libc_types::f64 * v)
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
STATICINLINE void VAO4f(libc_types::u32 index, typing::vectors::tvector<libc_types::f32, 4> x, libc_types::f32 z)
{
#ifndef NDEBUG
    if(!glVertexAttrib4f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4f(index, x[0], x[1], z, x[3]);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO4fv(libc_types::u32 index, const libc_types::f32 * v)
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
STATICINLINE void VAO4iv(libc_types::u32 index, const libc_types::i32 * v)
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
STATICINLINE void VAO4s(libc_types::u32 index, typing::vectors::tvector<libc_types::i16, 4> x, libc_types::i16 z)
{
#ifndef NDEBUG
    if(!glVertexAttrib4s)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4s(index, x[0], x[1], z, x[3]);
}
#endif


#if GL_VERSION_VERIFY(0x200, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_20)
STATICINLINE void VAO4sv(libc_types::u32 index, const libc_types::i16 * v)
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
STATICINLINE void VAO4ubv(libc_types::u32 index, const libc_types::u8 * v)
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
STATICINLINE void VAO4uiv(libc_types::u32 index, const libc_types::u32 * v)
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
STATICINLINE void VAO4usv(libc_types::u32 index, const libc_types::u16 * v)
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
STATICINLINE void VAOPointer(libc_types::u32 index, libc_types::i32 size, GLenum type, libc_types::u8 normalized, libc_types::i32 stride, const void * pointer)
{
#ifndef NDEBUG
    if(!glVertexAttribPointer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}
#endif

};
template<typename ReqVer>
struct v21
    : v20<ReqVer>
{
using Parent = v20<ReqVer>;
using Parent::Uniffv;

#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf2_3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2x3fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf2_4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2x4fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf3_2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3x2fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf3_4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3x4fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf4_2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4x2fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf4_3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4x3fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif

};
template<typename ReqVer>
struct v30
    : v21<ReqVer>
{
using Parent = v21<ReqVer>;

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void ConditionalRenderBegin(glhnd const& id, Delay mode)
{
#ifndef NDEBUG
    if(!glBeginConditionalRender)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBeginConditionalRender(C_OCAST<libc_types::u32>(id), to_enum1(mode));
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
STATICINLINE void BufBindBase(BufferComp::buf_flag const& target, libc_types::u32 index, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glBindBufferBase)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindBufferBase(buffer_to_enum(target), index, C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufBindRange(BufferComp::buf_flag const& target, libc_types::u32 index, glhnd const& buffer, libc_types::ptroff offset, libc_types::ptroff size)
{
#ifndef NDEBUG
    if(!glBindBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindBufferRange(buffer_to_enum(target), index, C_OCAST<libc_types::u32>(buffer), offset, size);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void BindFragDataLocation(glhnd const& program, libc_types::u32 color, const GLchar * name)
{
#ifndef NDEBUG
    if(!glBindFragDataLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindFragDataLocation(C_OCAST<libc_types::u32>(program), color, name);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBBind(FramebufferT target, glhnd const& framebuffer)
{
#ifndef NDEBUG
    if(!glBindFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindFramebuffer(to_enum(target), C_OCAST<libc_types::u32>(framebuffer));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufBind(GLenum target, glhnd const& renderbuffer)
{
#ifndef NDEBUG
    if(!glBindRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindRenderbuffer(target, C_OCAST<libc_types::u32>(renderbuffer));
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
    glBindVertexArray(C_OCAST<libc_types::u32>(array));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void FBBlit(libc_types::i32 srcX0, libc_types::i32 srcY0, libc_types::i32 srcX1, libc_types::i32 srcY1, libc_types::i32 dstX0, libc_types::i32 dstY0, libc_types::i32 dstX1, libc_types::i32 dstY1, libc_types::u32 mask, GLenum filter)
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
    return (GLenum)glCheckFramebufferStatus(to_enum(target));
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
STATICINLINE void BufClearfi(GLenum buffer, libc_types::i32 drawbuffer, libc_types::f32 depth, libc_types::i32 stencil)
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
STATICINLINE void BufClearfv(GLenum buffer, libc_types::i32 drawbuffer, semantic::Span<const libc_types::f32> const& value)
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
STATICINLINE void BufCleariv(GLenum buffer, libc_types::i32 drawbuffer, semantic::Span<const libc_types::i32> const& value)
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
STATICINLINE void BufClearuiv(GLenum buffer, libc_types::i32 drawbuffer, semantic::Span<const libc_types::u32> const& value)
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
STATICINLINE void ColorMaski(libc_types::u32 index, libc_types::u8 r, libc_types::u8 g, libc_types::u8 b, libc_types::u8 a)
{
#ifndef NDEBUG
    if(!glColorMaski)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glColorMaski(index, r, g, b, a);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBFree(semantic::Span<const libc_types::u32>&& framebuffers)
{
#ifndef NDEBUG
    if(!glDeleteFramebuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteFramebuffers(C_FCAST<libc_types::i32>(framebuffers.elements), framebuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufFree(semantic::Span<const libc_types::u32>&& renderbuffers)
{
#ifndef NDEBUG
    if(!glDeleteRenderbuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteRenderbuffers(C_FCAST<libc_types::i32>(renderbuffers.elements), renderbuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOFree(semantic::Span<const libc_types::u32>&& arrays)
{
#ifndef NDEBUG
    if(!glDeleteVertexArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteVertexArrays(C_FCAST<libc_types::i32>(arrays.elements), arrays.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void Disablei(Feature target, libc_types::u32 index)
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
STATICINLINE void Enablei(Feature target, libc_types::u32 index)
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
STATICINLINE void BufFlushMappedRange(BufferComp::buf_flag const& target, libc_types::ptroff offset, libc_types::ptroff length)
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
STATICINLINE void FBRenderbuffer(FramebufferT target, GLenum attachment, GLenum renderbuffertarget, glhnd const& renderbuffer)
{
#ifndef NDEBUG
    if(!glFramebufferRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferRenderbuffer(to_enum(target), attachment, renderbuffertarget, C_OCAST<libc_types::u32>(renderbuffer));
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void FBTexture1D(FramebufferT target, GLenum attachment, TexComp::tex_flag const& textarget, glhnd const& texture, libc_types::i32 level)
{
#ifndef NDEBUG
    if(!glFramebufferTexture1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTexture1D(to_enum(target), attachment, texture_to_enum(textarget), C_OCAST<libc_types::u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBTexture2D(FramebufferT target, GLenum attachment, TexComp::tex_flag const& textarget, glhnd const& texture, libc_types::i32 level)
{
#ifndef NDEBUG
    if(!glFramebufferTexture2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTexture2D(to_enum(target), attachment, texture_to_enum(textarget), C_OCAST<libc_types::u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void FBTexture3D(FramebufferT target, GLenum attachment, TexComp::tex_flag const& textarget, glhnd const& texture, libc_types::i32 level, libc_types::i32 zoffset)
{
#ifndef NDEBUG
    if(!glFramebufferTexture3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTexture3D(to_enum(target), attachment, texture_to_enum(textarget), C_OCAST<libc_types::u32>(texture), level, zoffset);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void FBTextureLayer(FramebufferT target, GLenum attachment, glhnd const& texture, libc_types::i32 level, libc_types::i32 layer)
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
    glFramebufferTextureLayer(to_enum(target), attachment, C_OCAST<libc_types::u32>(texture), level, layer);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBAlloc(semantic::Span<libc_types::u32>&& framebuffers)
{
#ifndef NDEBUG
    if(!glGenFramebuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenFramebuffers(C_FCAST<libc_types::i32>(framebuffers.elements), framebuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufAlloc(semantic::Span<libc_types::u32>&& renderbuffers)
{
#ifndef NDEBUG
    if(!glGenRenderbuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenRenderbuffers(C_FCAST<libc_types::i32>(renderbuffers.elements), renderbuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOAlloc(semantic::Span<libc_types::u32>&& arrays)
{
#ifndef NDEBUG
    if(!glGenVertexArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenVertexArrays(C_FCAST<libc_types::i32>(arrays.elements), arrays.data);
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
STATICINLINE void BoolGeti_v(BufferComp::buf_flag const& target, libc_types::u32 index, libc_types::u8 * data)
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
STATICINLINE libc_types::i32 GetFragDataLocation(glhnd const& program, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetFragDataLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::i32)glGetFragDataLocation(C_OCAST<libc_types::u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBGetAttachmentParameteriv(FramebufferT target, GLenum attachment, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void IntegerGeti_v(GLenum target, libc_types::u32 index, libc_types::i32 * data)
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
STATICINLINE void RBufGetParameteriv(GLenum target, GLenum pname, libc_types::i32 * params)
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
STATICINLINE libc_types::cstring  GetStringi(GLenum name, libc_types::u32 index)
{
#ifndef NDEBUG
    if(!glGetStringi)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::cstring )glGetStringi(name, index);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void TexGetParameterIiv(TexComp::tex_flag const& target, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void TexGetParameterIuiv(TexComp::tex_flag const& target, GLenum pname, libc_types::u32 * params)
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
STATICINLINE void XFGetVarying(glhnd const& program, libc_types::u32 index, libc_types::i32 bufSize, libc_types::i32 * length, libc_types::i32 * size, GLenum * type, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetTransformFeedbackVarying)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTransformFeedbackVarying(C_OCAST<libc_types::u32>(program), index, bufSize, length, size, type, name);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void UnifGetuiv(glhnd const& program, libc_types::i32 location, libc_types::u32 * params)
{
#ifndef NDEBUG
    if(!glGetUniformuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformuiv(C_OCAST<libc_types::u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOGetIiv(libc_types::u32 index, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void VAOGetIuiv(libc_types::u32 index, GLenum pname, libc_types::u32 * params)
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
STATICINLINE libc_types::u8 IsEnabledi(Feature target, libc_types::u32 index)
{
#ifndef NDEBUG
    if(!glIsEnabledi)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsEnabledi(to_enum(target), index);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE libc_types::u8 IsFramebuffer(glhnd const& framebuffer)
{
#ifndef NDEBUG
    if(!glIsFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsFramebuffer(C_OCAST<libc_types::u32>(framebuffer));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE libc_types::u8 IsRenderbuffer(glhnd const& renderbuffer)
{
#ifndef NDEBUG
    if(!glIsRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsRenderbuffer(C_OCAST<libc_types::u32>(renderbuffer));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE libc_types::u8 IsVertexArray(glhnd const& array)
{
#ifndef NDEBUG
    if(!glIsVertexArray)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsVertexArray(C_OCAST<libc_types::u32>(array));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void * BufMapRange(BufferComp::buf_flag const& target, libc_types::ptroff offset, libc_types::ptroff length, semantic::RSCA access)
{
#ifndef NDEBUG
    if(!glMapBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (void *)glMapBufferRange(buffer_to_enum(target), offset, length, to_enum2(access));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufStorage(GLenum target, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glRenderbufferStorage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glRenderbufferStorage(target, to_enum(internalformat), width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void RBufStorageMultisample(GLenum target, libc_types::i32 samples, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glRenderbufferStorageMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glRenderbufferStorageMultisample(target, samples, to_enum(internalformat), width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void TexParameterIiv(TexComp::tex_flag const& target, GLenum pname, const libc_types::i32 * params)
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
STATICINLINE void TexParameterIuiv(TexComp::tex_flag const& target, GLenum pname, const libc_types::u32 * params)
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
STATICINLINE void XFVaryings(glhnd const& program, libc_types::i32 count, const GLchar * *const varyings, GLenum bufferMode)
{
#ifndef NDEBUG
    if(!glTransformFeedbackVaryings)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTransformFeedbackVaryings(C_OCAST<libc_types::u32>(program), count, varyings, bufferMode);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif1ui(libc_types::i32 location, libc_types::u32 v0)
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
STATICINLINE void Unifuiv(libc_types::i32 location, semantic::Span<const libc_types::u32> const& value)
{
#ifndef NDEBUG
    if(!glUniform1uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1uiv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif2ui(libc_types::i32 location, typing::vectors::tvector<libc_types::u32, 2> v0)
{
#ifndef NDEBUG
    if(!glUniform2ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2ui(location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecui2> const& value)
{
#ifndef NDEBUG
    if(!glUniform2uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2uiv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif3ui(libc_types::i32 location, typing::vectors::tvector<libc_types::u32, 3> v0, libc_types::u32 v2)
{
#ifndef NDEBUG
    if(!glUniform3ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3ui(location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecui3> const& value)
{
#ifndef NDEBUG
    if(!glUniform3uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3uiv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif4ui(libc_types::i32 location, typing::vectors::tvector<libc_types::u32, 4> v0, libc_types::u32 v2)
{
#ifndef NDEBUG
    if(!glUniform4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4ui(location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecui4> const& value)
{
#ifndef NDEBUG
    if(!glUniform4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4uiv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI1i(libc_types::u32 index, libc_types::i32 x)
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
STATICINLINE void VAOI1iv(libc_types::u32 index, const libc_types::i32 * v)
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
STATICINLINE void VAOI1ui(libc_types::u32 index, libc_types::u32 x)
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
STATICINLINE void VAOI1uiv(libc_types::u32 index, const libc_types::u32 * v)
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
STATICINLINE void VAOI2i(libc_types::u32 index, typing::vectors::tvector<libc_types::i32, 2> x)
{
#ifndef NDEBUG
    if(!glVertexAttribI2i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI2i(index, x[0], x[1]);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI2iv(libc_types::u32 index, const libc_types::i32 * v)
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
STATICINLINE void VAOI2ui(libc_types::u32 index, typing::vectors::tvector<libc_types::u32, 2> x)
{
#ifndef NDEBUG
    if(!glVertexAttribI2ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI2ui(index, x[0], x[1]);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI2uiv(libc_types::u32 index, const libc_types::u32 * v)
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
STATICINLINE void VAOI3i(libc_types::u32 index, typing::vectors::tvector<libc_types::i32, 3> x, libc_types::i32 z)
{
#ifndef NDEBUG
    if(!glVertexAttribI3i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI3i(index, x[0], x[1], z);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI3iv(libc_types::u32 index, const libc_types::i32 * v)
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
STATICINLINE void VAOI3ui(libc_types::u32 index, typing::vectors::tvector<libc_types::u32, 3> x, libc_types::u32 z)
{
#ifndef NDEBUG
    if(!glVertexAttribI3ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI3ui(index, x[0], x[1], z);
}
#endif


#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_30)
STATICINLINE void VAOI3uiv(libc_types::u32 index, const libc_types::u32 * v)
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
STATICINLINE void VAOI4bv(libc_types::u32 index, const libc_types::i8 * v)
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
STATICINLINE void VAOI4i(libc_types::u32 index, typing::vectors::tvector<libc_types::i32, 4> x, libc_types::i32 z)
{
#ifndef NDEBUG
    if(!glVertexAttribI4i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4i(index, x[0], x[1], z, x[3]);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOI4iv(libc_types::u32 index, const libc_types::i32 * v)
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
STATICINLINE void VAOI4sv(libc_types::u32 index, const libc_types::i16 * v)
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
STATICINLINE void VAOI4ubv(libc_types::u32 index, const libc_types::u8 * v)
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
STATICINLINE void VAOI4ui(libc_types::u32 index, typing::vectors::tvector<libc_types::u32, 4> x, libc_types::u32 z)
{
#ifndef NDEBUG
    if(!glVertexAttribI4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4ui(index, x[0], x[1], z, x[3]);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOI4uiv(libc_types::u32 index, const libc_types::u32 * v)
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
STATICINLINE void VAOI4usv(libc_types::u32 index, const libc_types::u16 * v)
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
STATICINLINE void VAOIPointer(libc_types::u32 index, libc_types::i32 size, GLenum type, libc_types::i32 stride, const void * pointer)
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
struct v31
    : v30<ReqVer>
{
using Parent = v30<ReqVer>;

#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void BufCopySubData(GLenum readTarget, GLenum writeTarget, libc_types::ptroff readOffset, libc_types::ptroff writeOffset, libc_types::ptroff size)
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
STATICINLINE void DrawArraysInstanced(DrwMd const& mode, libc_types::i32 first, libc_types::i32 count, libc_types::i32 instancecount)
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
STATICINLINE void DrawElementsInstanced(DrwMd const& mode, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 instancecount)
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
STATICINLINE void ActiveUnifBlockGetName(glhnd const& program, libc_types::u32 uniformBlockIndex, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * uniformBlockName)
{
#ifndef NDEBUG
    if(!glGetActiveUniformBlockName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformBlockName(C_OCAST<libc_types::u32>(program), uniformBlockIndex, bufSize, length, uniformBlockName);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void ActiveUnifBlockGetiv(glhnd const& program, libc_types::u32 uniformBlockIndex, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetActiveUniformBlockiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformBlockiv(C_OCAST<libc_types::u32>(program), uniformBlockIndex, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x310, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_31)
STATICINLINE void ActiveUnifGetName(glhnd const& program, libc_types::u32 uniformIndex, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * uniformName)
{
#ifndef NDEBUG
    if(!glGetActiveUniformName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformName(C_OCAST<libc_types::u32>(program), uniformIndex, bufSize, length, uniformName);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void ActiveUnifGetsiv(glhnd const& program, libc_types::i32 uniformCount, const libc_types::u32 * uniformIndices, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetActiveUniformsiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformsiv(C_OCAST<libc_types::u32>(program), uniformCount, uniformIndices, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE libc_types::u32 UnifBlockGetIndex(glhnd const& program, const GLchar * uniformBlockName)
{
#ifndef NDEBUG
    if(!glGetUniformBlockIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u32)glGetUniformBlockIndex(C_OCAST<libc_types::u32>(program), uniformBlockName);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void UnifGetIndices(glhnd const& program, libc_types::i32 uniformCount, const GLchar * *const uniformNames, libc_types::u32 * uniformIndices)
{
#ifndef NDEBUG
    if(!glGetUniformIndices)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformIndices(C_OCAST<libc_types::u32>(program), uniformCount, uniformNames, uniformIndices);
}
#endif


#if GL_VERSION_VERIFY(0x310, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_31)
STATICINLINE void PrimitiveRestartIndex(libc_types::u32 index)
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
STATICINLINE void TexBuffer(TexComp::tex_flag const& target, typing::pixels::PixFmt internalformat, glhnd const& buffer)
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
    glTexBuffer(texture_to_enum(target), to_enum(internalformat), C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void UnifBlockBinding(glhnd const& program, libc_types::u32 uniformBlockIndex, libc_types::u32 uniformBlockBinding)
{
#ifndef NDEBUG
    if(!glUniformBlockBinding)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformBlockBinding(C_OCAST<libc_types::u32>(program), uniformBlockIndex, uniformBlockBinding);
}
#endif

};
template<typename ReqVer>
struct v32
    : v31<ReqVer>
{
using Parent = v31<ReqVer>;

#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE GLenum SyncClientWait(GLsync sync, semantic::RSCA flags, libc_types::u64 timeout)
{
#ifndef NDEBUG
    if(!glClientWaitSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (GLenum)glClientWaitSync(sync, to_enum2(flags), timeout);
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
STATICINLINE void DrawElementsBaseVertex(DrwMd const& mode, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 basevertex)
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
STATICINLINE void DrawElementsInstancedBaseVertex(DrwMd const& mode, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 instancecount, libc_types::i32 basevertex)
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
STATICINLINE void DrawRangeElementsBaseVertex(DrwMd const& mode, libc_types::u32 start, libc_types::u32 end, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 basevertex)
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
STATICINLINE GLsync SyncFence(GLenum condition, semantic::RSCA flags)
{
#ifndef NDEBUG
    if(!glFenceSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (GLsync)glFenceSync(condition, to_enum2(flags));
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x320)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_32)
STATICINLINE void FBTexture(FramebufferT target, GLenum attachment, glhnd const& texture, libc_types::i32 level)
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
    glFramebufferTexture(to_enum(target), attachment, C_OCAST<libc_types::u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void BufGetParameteri64v(BufferComp::buf_flag const& target, GLenum pname, libc_types::i64 * params)
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
STATICINLINE void IntegerGet64i_v(GLenum target, libc_types::u32 index, libc_types::i64 * data)
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
STATICINLINE void IntegerGet64v(GLenum pname, libc_types::i64 * data)
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
STATICINLINE void GetMultisamplefv(GLenum pname, libc_types::u32 index, libc_types::f32 * val)
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
STATICINLINE void SyncGetiv(GLsync sync, GLenum pname, libc_types::i32 count, libc_types::i32 * length, libc_types::i32 * values)
{
#ifndef NDEBUG
    if(!glGetSynciv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSynciv(sync, pname, count, length, values);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE libc_types::u8 IsSync(GLsync sync)
{
#ifndef NDEBUG
    if(!glIsSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsSync(sync);
}
#endif


#if GL_VERSION_VERIFY(0x320, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_32)
STATICINLINE void MultiDrawElementsBaseVertex(DrwMd const& mode, const libc_types::i32 * count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 drawcount, const libc_types::i32 * basevertex)
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
STATICINLINE void SampleMaski(libc_types::u32 maskNumber, libc_types::u32 mask)
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
STATICINLINE void TexImage2DMultisample(TexComp::tex_flag const& target, libc_types::i32 samples, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width, libc_types::u8 fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTexImage2DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage2DMultisample(texture_to_enum(target), samples, to_enum(internalformat), width.w, width.h, fixedsamplelocations);
}
#endif


#if GL_VERSION_VERIFY(0x320, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_32)
STATICINLINE void TexImage3DMultisample(TexComp::tex_flag const& target, libc_types::i32 samples, typing::pixels::PixFmt internalformat, typing::geometry::size_3d<libc_types::i32> const& width, libc_types::u8 fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTexImage3DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage3DMultisample(texture_to_enum(target), samples, to_enum(internalformat), width.width, width.height, width.depth, fixedsamplelocations);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void SyncWait(GLsync sync, semantic::RSCA flags, libc_types::u64 timeout)
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
struct v33
    : v32<ReqVer>
{
using Parent = v32<ReqVer>;

#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void BindFragDataLocationIndexed(glhnd const& program, libc_types::u32 colorNumber, libc_types::u32 index, const GLchar * name)
{
#ifndef NDEBUG
    if(!glBindFragDataLocationIndexed)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindFragDataLocationIndexed(C_OCAST<libc_types::u32>(program), colorNumber, index, name);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerBind(libc_types::u32 unit, glhnd const& sampler)
{
#ifndef NDEBUG
    if(!glBindSampler)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindSampler(unit, C_OCAST<libc_types::u32>(sampler));
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerFree(semantic::Span<const libc_types::u32>&& samplers)
{
#ifndef NDEBUG
    if(!glDeleteSamplers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteSamplers(C_FCAST<libc_types::i32>(samplers.elements), samplers.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerAlloc(semantic::Span<libc_types::u32>&& samplers)
{
#ifndef NDEBUG
    if(!glGenSamplers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenSamplers(C_FCAST<libc_types::i32>(samplers.elements), samplers.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE libc_types::i32 GetFragDataIndex(glhnd const& program, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetFragDataIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::i32)glGetFragDataIndex(C_OCAST<libc_types::u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void QueryGetObjecti64v(glhnd const& id, GLenum pname, libc_types::i64 * params)
{
#ifndef NDEBUG
    if(!glGetQueryObjecti64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryObjecti64v(C_OCAST<libc_types::u32>(id), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void QueryGetObjectui64v(glhnd const& id, GLenum pname, libc_types::u64 * params)
{
#ifndef NDEBUG
    if(!glGetQueryObjectui64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryObjectui64v(C_OCAST<libc_types::u32>(id), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerGetParameterIiv(glhnd const& sampler, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameterIiv(C_OCAST<libc_types::u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerGetParameterIuiv(glhnd const& sampler, GLenum pname, libc_types::u32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameterIuiv(C_OCAST<libc_types::u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerGetParameterfv(glhnd const& sampler, GLenum pname, libc_types::f32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameterfv(C_OCAST<libc_types::u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerGetParameteriv(glhnd const& sampler, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameteriv(C_OCAST<libc_types::u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE libc_types::u8 IsSampler(glhnd const& sampler)
{
#ifndef NDEBUG
    if(!glIsSampler)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsSampler(C_OCAST<libc_types::u32>(sampler));
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void QueryCounter(glhnd const& id, GLenum target)
{
#ifndef NDEBUG
    if(!glQueryCounter)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glQueryCounter(C_OCAST<libc_types::u32>(id), target);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerParameterIiv(glhnd const& sampler, GLenum pname, const libc_types::i32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterIiv(C_OCAST<libc_types::u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerParameterIuiv(glhnd const& sampler, GLenum pname, const libc_types::u32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterIuiv(C_OCAST<libc_types::u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameterf(glhnd const& sampler, GLenum pname, libc_types::f32 param)
{
#ifndef NDEBUG
    if(!glSamplerParameterf)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterf(C_OCAST<libc_types::u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameterfv(glhnd const& sampler, GLenum pname, const libc_types::f32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterfv(C_OCAST<libc_types::u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameteri(glhnd const& sampler, GLenum pname, libc_types::i32 param)
{
#ifndef NDEBUG
    if(!glSamplerParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameteri(C_OCAST<libc_types::u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameteriv(glhnd const& sampler, GLenum pname, const libc_types::i32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameteriv(C_OCAST<libc_types::u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void VAODivisor(libc_types::u32 index, libc_types::u32 divisor)
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
STATICINLINE void VAOP1ui(libc_types::u32 index, GLenum type, libc_types::u8 normalized, libc_types::u32 value)
{
#ifndef NDEBUG
    if(!glVertexAttribP1ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP1ui(index, type, normalized, value);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP1uiv(libc_types::u32 index, GLenum type, libc_types::u8 normalized, semantic::Span<const libc_types::u32> const& value)
{
#ifndef NDEBUG
    if(!glVertexAttribP1uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP1uiv(index, type, normalized, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP2ui(libc_types::u32 index, GLenum type, libc_types::u8 normalized, libc_types::u32 value)
{
#ifndef NDEBUG
    if(!glVertexAttribP2ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP2ui(index, type, normalized, value);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP2uiv(libc_types::u32 index, GLenum type, libc_types::u8 normalized, semantic::Span<const libc_types::u32> const& value)
{
#ifndef NDEBUG
    if(!glVertexAttribP2uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP2uiv(index, type, normalized, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP3ui(libc_types::u32 index, GLenum type, libc_types::u8 normalized, libc_types::u32 value)
{
#ifndef NDEBUG
    if(!glVertexAttribP3ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP3ui(index, type, normalized, value);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP3uiv(libc_types::u32 index, GLenum type, libc_types::u8 normalized, semantic::Span<const libc_types::u32> const& value)
{
#ifndef NDEBUG
    if(!glVertexAttribP3uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP3uiv(index, type, normalized, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP4ui(libc_types::u32 index, GLenum type, libc_types::u8 normalized, libc_types::u32 value)
{
#ifndef NDEBUG
    if(!glVertexAttribP4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP4ui(index, type, normalized, value);
}
#endif


#if GL_VERSION_VERIFY(0x330, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_33)
STATICINLINE void VAOP4uiv(libc_types::u32 index, GLenum type, libc_types::u8 normalized, semantic::Span<const libc_types::u32> const& value)
{
#ifndef NDEBUG
    if(!glVertexAttribP4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribP4uiv(index, type, normalized, value.data);
}
#endif

};
template<typename ReqVer>
struct v40
    : v33<ReqVer>
{
using Parent = v33<ReqVer>;

#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void QueryBeginIndexed(QueryComp::query_flag const& target, libc_types::u32 index, glhnd const& id)
{
#ifndef NDEBUG
    if(!glBeginQueryIndexed)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBeginQueryIndexed(query_to_enum(target), index, C_OCAST<libc_types::u32>(id));
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFBind(GLenum target, glhnd const& id)
{
#ifndef NDEBUG
    if(!glBindTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindTransformFeedback(target, C_OCAST<libc_types::u32>(id));
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void BlendEquationSeparatei(libc_types::u32 buf, GLenum modeRGB, GLenum modeAlpha)
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
STATICINLINE void BlendEquationi(libc_types::u32 buf, GLenum mode)
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
STATICINLINE void BlendFuncSeparatei(libc_types::u32 buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
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
STATICINLINE void BlendFunci(libc_types::u32 buf, GLenum src, GLenum dst)
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
STATICINLINE void XFFree(semantic::Span<const libc_types::u32>&& ids)
{
#ifndef NDEBUG
    if(!glDeleteTransformFeedbacks)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteTransformFeedbacks(C_FCAST<libc_types::i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x310)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_31)
STATICINLINE void DrawArraysIndirect(DrwMd const& mode, libc_types::uintptr indirect)
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
STATICINLINE void DrawElementsIndirect(DrwMd const& mode, TypeEnum type, libc_types::uintptr indirect)
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
STATICINLINE void DrawTransformFeedback(DrwMd const& mode, glhnd const& id)
{
#ifndef NDEBUG
    if(!glDrawTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawTransformFeedback(to_enum(mode.t, mode.c), C_OCAST<libc_types::u32>(id));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void DrawTransformFeedbackStream(DrwMd const& mode, glhnd const& id, libc_types::u32 stream)
{
#ifndef NDEBUG
    if(!glDrawTransformFeedbackStream)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawTransformFeedbackStream(to_enum(mode.t, mode.c), C_OCAST<libc_types::u32>(id), stream);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void QueryEndIndexed(QueryComp::query_flag const& target, libc_types::u32 index)
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
STATICINLINE void XFAlloc(semantic::Span<libc_types::u32>&& ids)
{
#ifndef NDEBUG
    if(!glGenTransformFeedbacks)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenTransformFeedbacks(C_FCAST<libc_types::i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void GetActiveSubroutineName(glhnd const& program, ShaderStage shadertype, libc_types::u32 index, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetActiveSubroutineName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveSubroutineName(C_OCAST<libc_types::u32>(program), to_enum1(shadertype), index, bufSize, length, name);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void UnifGetActiveSubroutineName(glhnd const& program, ShaderStage shadertype, libc_types::u32 index, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetActiveSubroutineUniformName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveSubroutineUniformName(C_OCAST<libc_types::u32>(program), to_enum1(shadertype), index, bufSize, length, name);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void UnifGetActiveSubroutineiv(glhnd const& program, ShaderStage shadertype, libc_types::u32 index, GLenum pname, libc_types::i32 * values)
{
#ifndef NDEBUG
    if(!glGetActiveSubroutineUniformiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveSubroutineUniformiv(C_OCAST<libc_types::u32>(program), to_enum1(shadertype), index, pname, values);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void ProgramGetStageiv(glhnd const& program, ShaderStage shadertype, GLenum pname, libc_types::i32 * values)
{
#ifndef NDEBUG
    if(!glGetProgramStageiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramStageiv(C_OCAST<libc_types::u32>(program), to_enum1(shadertype), pname, values);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void QueryGetIndexediv(QueryComp::query_flag const& target, libc_types::u32 index, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetQueryIndexediv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryIndexediv(query_to_enum(target), index, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE libc_types::u32 GetSubroutineIndex(glhnd const& program, ShaderStage shadertype, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetSubroutineIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u32)glGetSubroutineIndex(C_OCAST<libc_types::u32>(program), to_enum1(shadertype), name);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE libc_types::i32 UnifGetSubroutineLocation(glhnd const& program, ShaderStage shadertype, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetSubroutineUniformLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::i32)glGetSubroutineUniformLocation(C_OCAST<libc_types::u32>(program), to_enum1(shadertype), name);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void UnifGetSubroutineuiv(ShaderStage shadertype, libc_types::i32 location, libc_types::u32 * params)
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
STATICINLINE void UnifGetdv(glhnd const& program, libc_types::i32 location, libc_types::f64 * params)
{
#ifndef NDEBUG
    if(!glGetUniformdv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformdv(C_OCAST<libc_types::u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE libc_types::u8 IsTransformFeedback(glhnd const& id)
{
#ifndef NDEBUG
    if(!glIsTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsTransformFeedback(C_OCAST<libc_types::u32>(id));
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void MinSampleShading(libc_types::f32 value)
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
STATICINLINE void PatchParameterfv(PatchProperty pname, const libc_types::f32 * values)
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
STATICINLINE void PatchParameteri(PatchProperty pname, libc_types::i32 value)
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
STATICINLINE void Unif1d(libc_types::i32 location, libc_types::f64 x)
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
STATICINLINE void Unifdv(libc_types::i32 location, semantic::Span<const libc_types::f64> const& value)
{
#ifndef NDEBUG
    if(!glUniform1dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1dv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unif2d(libc_types::i32 location, typing::vectors::tvector<libc_types::f64, 2> x)
{
#ifndef NDEBUG
    if(!glUniform2d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2d(location, x[0], x[1]);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecd2> const& value)
{
#ifndef NDEBUG
    if(!glUniform2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2dv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unif3d(libc_types::i32 location, typing::vectors::tvector<libc_types::f64, 3> x, libc_types::f64 z)
{
#ifndef NDEBUG
    if(!glUniform3d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3d(location, x[0], x[1], z);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecd3> const& value)
{
#ifndef NDEBUG
    if(!glUniform3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3dv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unif4d(libc_types::i32 location, typing::vectors::tvector<libc_types::f64, 4> x, libc_types::f64 z)
{
#ifndef NDEBUG
    if(!glUniform4d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4d(location, x[0], x[1], z, x[3]);
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecd4> const& value)
{
#ifndef NDEBUG
    if(!glUniform4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4dv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2dv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd2_3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2x3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2x3dv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd2_4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2x4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2x4dv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3dv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd3_2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3x2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3x2dv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd3_4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3x4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3x4dv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4dv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd4_2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4x2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4x2dv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void Unifdv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd4_3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4x3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4x3dv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_40)
STATICINLINE void UnifSubroutinesuiv(ShaderStage shadertype, libc_types::i32 count, const libc_types::u32 * indices)
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
struct v41
    : v40<ReqVer>
{
using Parent = v40<ReqVer>;
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
    glActiveShaderProgram(C_OCAST<libc_types::u32>(pipeline), C_OCAST<libc_types::u32>(program));
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
    glBindProgramPipeline(C_OCAST<libc_types::u32>(pipeline));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void ClearDepthf(libc_types::f32 d)
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
STATICINLINE libc_types::u32 ShaderProgramvAllocEx(ShaderStage type, semantic::Span<const GLchar *>&& strings)
{
#ifndef NDEBUG
    if(!glCreateShaderProgramv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u32)glCreateShaderProgramv(to_enum1(type), C_FCAST<libc_types::i32>(strings.elements), strings.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineFree(semantic::Span<const libc_types::u32>&& pipelines)
{
#ifndef NDEBUG
    if(!glDeleteProgramPipelines)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteProgramPipelines(C_FCAST<libc_types::i32>(pipelines.elements), pipelines.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void DepthRangeArrayv(libc_types::u32 first, semantic::Span<const libc_types::f64>&& v)
{
#ifndef NDEBUG
    if(!glDepthRangeArrayv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthRangeArrayv(first, C_FCAST<libc_types::i32>(v.elements), v.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void DepthRangeIndexed(libc_types::u32 index, libc_types::f64 n, libc_types::f64 f)
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
STATICINLINE void DepthRangef(libc_types::f32 n, libc_types::f32 f)
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
STATICINLINE void PipelineAlloc(semantic::Span<libc_types::u32>&& pipelines)
{
#ifndef NDEBUG
    if(!glGenProgramPipelines)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenProgramPipelines(C_FCAST<libc_types::i32>(pipelines.elements), pipelines.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ScalarGeti_v(GLenum target, libc_types::u32 index, libc_types::f64 * data)
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
STATICINLINE void ScalarfGeti_v(GLenum target, libc_types::u32 index, libc_types::f32 * data)
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
STATICINLINE void ProgramGetBinary(glhnd const& program, libc_types::i32 * length, GLenum * binaryFormat, semantic::Bytes const& binary)
{
#ifndef NDEBUG
    if(!glGetProgramBinary)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramBinary(C_OCAST<libc_types::u32>(program), C_FCAST<libc_types::i32>(binary.size), length, binaryFormat, binary.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineGetInfoLog(glhnd const& pipeline, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * infoLog)
{
#ifndef NDEBUG
    if(!glGetProgramPipelineInfoLog)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramPipelineInfoLog(C_OCAST<libc_types::u32>(pipeline), bufSize, length, infoLog);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineGetiv(glhnd const& pipeline, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramPipelineiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramPipelineiv(C_OCAST<libc_types::u32>(pipeline), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void ShaderGetPrecisionFormat(ShaderStage shadertype, GLenum precisiontype, libc_types::i32 * range, libc_types::i32 * precision)
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
STATICINLINE void VAOGetLdv(libc_types::u32 index, GLenum pname, libc_types::f64 * params)
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
STATICINLINE libc_types::u8 IsProgramPipeline(glhnd const& pipeline)
{
#ifndef NDEBUG
    if(!glIsProgramPipeline)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsProgramPipeline(C_OCAST<libc_types::u32>(pipeline));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x300)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_30)
STATICINLINE void ProgramBinary(glhnd const& program, GLenum binaryFormat, const void * binary, libc_types::i32 length)
{
#ifndef NDEBUG
    if(!glProgramBinary)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramBinary(C_OCAST<libc_types::u32>(program), binaryFormat, binary, length);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x300)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_30)
STATICINLINE void ProgramParameteri(glhnd const& program, GLenum pname, libc_types::i32 value)
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
    glProgramParameteri(C_OCAST<libc_types::u32>(program), pname, value);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unif1d(glhnd const& program, libc_types::i32 location, libc_types::f64 v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1d(C_OCAST<libc_types::u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::f64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif1f(glhnd const& program, libc_types::i32 location, libc_types::f32 v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1f(C_OCAST<libc_types::u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::f32> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif1i(glhnd const& program, libc_types::i32 location, libc_types::i32 v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1i(C_OCAST<libc_types::u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::i32> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1iv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif1ui(glhnd const& program, libc_types::i32 location, libc_types::u32 v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1ui(C_OCAST<libc_types::u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::u32> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1uiv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unif2d(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::f64, 2> v0)
{
#ifndef NDEBUG
    if(!glProgramUniform2d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2d(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecd2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif2f(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 2> v0)
{
#ifndef NDEBUG
    if(!glProgramUniform2f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2f(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecf2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif2i(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 2> v0)
{
#ifndef NDEBUG
    if(!glProgramUniform2i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2i(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Veci2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2iv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif2ui(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::u32, 2> v0)
{
#ifndef NDEBUG
    if(!glProgramUniform2ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2ui(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecui2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2uiv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unif3d(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::f64, 3> v0, libc_types::f64 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3d(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecd3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif3f(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 3> v0, libc_types::f32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3f(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecf3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif3i(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 3> v0, libc_types::i32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3i(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Veci3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3iv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif3ui(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::u32, 3> v0, libc_types::u32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3ui(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecui3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3uiv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unif4d(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::f64, 4> v0, libc_types::f64 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform4d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4d(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecd4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif4f(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 4> v0, libc_types::f32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform4f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4f(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecf4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif4i(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 4> v0, libc_types::i32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform4i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4i(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Veci4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4iv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif4ui(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::u32, 4> v0, libc_types::u32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4ui(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecui4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4uiv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd2_3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2x3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2x3dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf2_3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2x3fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd2_4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2x4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2x4dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf2_4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2x4fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd3_2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3x2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3x2dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf3_2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3x2fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd3_4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3x4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3x4dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf3_4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3x4fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd4_2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4x2dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4x2dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf4_2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4x2fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void Unifdv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matd4_3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4x3dv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4x3dv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f64*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf4_3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4x3fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
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
STATICINLINE void ScissorArrayv(libc_types::u32 first, semantic::Span<const libc_types::i32>&& v)
{
#ifndef NDEBUG
    if(!glScissorArrayv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glScissorArrayv(first, C_FCAST<libc_types::i32>(v.elements), v.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ScissorIndexed(libc_types::u32 index, libc_types::i32 left, libc_types::i32 bottom, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glScissorIndexed)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glScissorIndexed(index, left, bottom, width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ScissorIndexedv(libc_types::u32 index, const libc_types::i32 * v)
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
STATICINLINE void ShaderBinary(libc_types::i32 count, const libc_types::u32 * shaders, GLenum binaryformat, const void * binary, libc_types::i32 length)
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
    glUseProgramStages(C_OCAST<libc_types::u32>(pipeline), to_enum2(stages), C_OCAST<libc_types::u32>(program));
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
    glValidateProgramPipeline(C_OCAST<libc_types::u32>(pipeline));
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOL1d(libc_types::u32 index, libc_types::f64 x)
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
STATICINLINE void VAOL1dv(libc_types::u32 index, const libc_types::f64 * v)
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
STATICINLINE void VAOL2d(libc_types::u32 index, typing::vectors::tvector<libc_types::f64, 2> x)
{
#ifndef NDEBUG
    if(!glVertexAttribL2d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL2d(index, x[0], x[1]);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOL2dv(libc_types::u32 index, const libc_types::f64 * v)
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
STATICINLINE void VAOL3d(libc_types::u32 index, typing::vectors::tvector<libc_types::f64, 3> x, libc_types::f64 z)
{
#ifndef NDEBUG
    if(!glVertexAttribL3d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL3d(index, x[0], x[1], z);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOL3dv(libc_types::u32 index, const libc_types::f64 * v)
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
STATICINLINE void VAOL4d(libc_types::u32 index, typing::vectors::tvector<libc_types::f64, 4> x, libc_types::f64 z)
{
#ifndef NDEBUG
    if(!glVertexAttribL4d)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribL4d(index, x[0], x[1], z, x[3]);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void VAOL4dv(libc_types::u32 index, const libc_types::f64 * v)
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
STATICINLINE void VAOLPointer(libc_types::u32 index, libc_types::i32 size, GLenum type, libc_types::i32 stride, const void * pointer)
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
STATICINLINE void ViewportArrayv(libc_types::u32 first, semantic::Span<const libc_types::f32>&& v)
{
#ifndef NDEBUG
    if(!glViewportArrayv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glViewportArrayv(first, C_FCAST<libc_types::i32>(v.elements), v.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ViewportIndexedf(libc_types::u32 index, typing::vectors::tvector<libc_types::f32, 4> x, libc_types::f32 h)
{
#ifndef NDEBUG
    if(!glViewportIndexedf)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glViewportIndexedf(index, x[0], x[1], x[3], h);
}
#endif


#if GL_VERSION_VERIFY(0x410, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_41)
STATICINLINE void ViewportIndexedfv(libc_types::u32 index, const libc_types::f32 * v)
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
struct v42
    : v41<ReqVer>
{
using Parent = v41<ReqVer>;

#if GL_VERSION_VERIFY(0x420, 0x310)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_31)
STATICINLINE void TexBindImage(libc_types::u32 unit, glhnd const& texture, libc_types::i32 level, libc_types::u8 layered, libc_types::i32 layer, semantic::RSCA access, typing::pixels::PixFmt format)
{
#ifndef NDEBUG
    if(!glBindImageTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindImageTexture(unit, C_OCAST<libc_types::u32>(texture), level, layered, layer, to_enum2(access), to_enum(format));
}
#endif


#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_42)
STATICINLINE void DrawArraysInstancedBaseInstance(DrwMd const& mode, libc_types::i32 first, libc_types::i32 count, libc_types::i32 instancecount, libc_types::u32 baseinstance)
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
STATICINLINE void DrawElementsInstancedBaseInstance(DrwMd const& mode, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 instancecount, libc_types::u32 baseinstance)
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
STATICINLINE void DrawElementsInstancedBaseVertexBaseInstance(DrwMd const& mode, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 instancecount, libc_types::i32 basevertex, libc_types::u32 baseinstance)
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
STATICINLINE void DrawTransformFeedbackInstanced(DrwMd const& mode, glhnd const& id, libc_types::i32 instancecount)
{
#ifndef NDEBUG
    if(!glDrawTransformFeedbackInstanced)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawTransformFeedbackInstanced(to_enum(mode.t, mode.c), C_OCAST<libc_types::u32>(id), instancecount);
}
#endif


#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_42)
STATICINLINE void DrawTransformFeedbackStreamInstanced(DrwMd const& mode, glhnd const& id, libc_types::u32 stream, libc_types::i32 instancecount)
{
#ifndef NDEBUG
    if(!glDrawTransformFeedbackStreamInstanced)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDrawTransformFeedbackStreamInstanced(to_enum(mode.t, mode.c), C_OCAST<libc_types::u32>(id), stream, instancecount);
}
#endif


#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_42)
STATICINLINE void BufGetActiveAtomicCounteriv(glhnd const& program, libc_types::u32 bufferIndex, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetActiveAtomicCounterBufferiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveAtomicCounterBufferiv(C_OCAST<libc_types::u32>(program), bufferIndex, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x300)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_30)
STATICINLINE void GetInternalformativ(TexComp::tex_flag const& target, typing::pixels::PixFmt internalformat, GLenum pname, libc_types::i32 count, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetInternalformativ)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetInternalformativ(texture_to_enum(target), to_enum(internalformat), pname, count, params);
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x310)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_31)
STATICINLINE void MemoryBarrier(libc_types::u32 barriers)
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
STATICINLINE void TexStorage1D(TexComp::tex_flag const& target, libc_types::i32 levels, typing::pixels::PixFmt internalformat, libc_types::i32 width)
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
STATICINLINE void TexStorage2D(TexComp::tex_flag const& target, libc_types::i32 levels, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glTexStorage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage2D(texture_to_enum(target), levels, to_enum(internalformat), width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x300)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_30)
STATICINLINE void TexStorage3D(TexComp::tex_flag const& target, libc_types::i32 levels, typing::pixels::PixFmt internalformat, typing::geometry::size_3d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glTexStorage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage3D(texture_to_enum(target), levels, to_enum(internalformat), width.width, width.height, width.depth);
}
#endif

};
template<typename ReqVer>
struct v43
    : v42<ReqVer>
{
using Parent = v42<ReqVer>;

#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VertBufBind(libc_types::u32 bindingindex, glhnd const& buffer, libc_types::ptroff offset, libc_types::i32 stride)
{
#ifndef NDEBUG
    if(!glBindVertexBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindVertexBuffer(bindingindex, C_OCAST<libc_types::u32>(buffer), offset, stride);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void BufClearData(BufferComp::buf_flag const& target, typing::pixels::PixFmt internalformat, typing::PixCmp format, typing::pixels::BitFmt type, const void * data)
{
#ifndef NDEBUG
    if(!glClearBufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearBufferData(buffer_to_enum(target), to_enum(internalformat), to_enum(format, typing::pixels::PixFmt::None), to_enum(type), data);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void BufClearSubData(BufferComp::buf_flag const& target, typing::pixels::PixFmt internalformat, libc_types::ptroff offset, typing::PixCmp format, typing::pixels::BitFmt type, semantic::BytesConst const& data)
{
#ifndef NDEBUG
    if(!glClearBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearBufferSubData(buffer_to_enum(target), to_enum(internalformat), offset, C_FCAST<libc_types::ptroff>(data.size), to_enum(format, typing::pixels::PixFmt::None), to_enum(type), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void CopyImageSubData(libc_types::u32 srcName, GLenum srcTarget, libc_types::i32 srcLevel, libc_types::i32 srcX, libc_types::i32 srcY, libc_types::i32 srcZ, libc_types::u32 dstName, GLenum dstTarget, libc_types::i32 dstLevel, libc_types::i32 dstX, libc_types::i32 dstY, libc_types::i32 dstZ, libc_types::i32 srcWidth, libc_types::i32 srcHeight, libc_types::i32 srcDepth)
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
STATICINLINE void DebugMessageControl(GLenum source, GLenum type, GLenum severity, semantic::Span<const libc_types::u32>&& ids, libc_types::u8 enabled)
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
    glDebugMessageControl(source, type, severity, C_FCAST<libc_types::i32>(ids.elements), ids.data, enabled);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void DebugMessageInsert(GLenum source, GLenum type, glhnd const& id, GLenum severity, libc_types::i32 length, const GLchar * buf)
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
    glDebugMessageInsert(source, type, C_OCAST<libc_types::u32>(id), severity, length, buf);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void DispatchCompute(libc_types::u32 num_groups_x, libc_types::u32 num_groups_y, libc_types::u32 num_groups_z)
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
STATICINLINE void DispatchComputeIndirect(libc_types::ptroff indirect)
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
STATICINLINE void FBParameteri(FramebufferT target, GLenum pname, libc_types::i32 param)
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
STATICINLINE libc_types::u32 GetDebugMessageLog(libc_types::u32 count, libc_types::i32 bufSize, GLenum * sources, GLenum * types, libc_types::u32 * ids, GLenum * severities, libc_types::i32 * lengths, GLchar * messageLog)
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
    return (libc_types::u32)glGetDebugMessageLog(count, bufSize, sources, types, ids, severities, lengths, messageLog);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void FBGetParameteriv(FramebufferT target, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void GetInternalformati64v(TexComp::tex_flag const& target, typing::pixels::PixFmt internalformat, GLenum pname, libc_types::i32 count, libc_types::i64 * params)
{
#ifndef NDEBUG
    if(!glGetInternalformati64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetInternalformati64v(texture_to_enum(target), to_enum(internalformat), pname, count, params);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void GetObjectLabel(GLenum identifier, libc_types::u32 name, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * label)
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
STATICINLINE void GetObjectPtrLabel(const void * ptr, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * label)
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
STATICINLINE void ProgramGetInterfaceiv(glhnd const& program, GLenum programInterface, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramInterfaceiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramInterfaceiv(C_OCAST<libc_types::u32>(program), programInterface, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE libc_types::u32 ProgramGetResourceIndex(glhnd const& program, GLenum programInterface, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u32)glGetProgramResourceIndex(C_OCAST<libc_types::u32>(program), programInterface, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE libc_types::i32 ProgramGetResourceLocation(glhnd const& program, GLenum programInterface, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::i32)glGetProgramResourceLocation(C_OCAST<libc_types::u32>(program), programInterface, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE libc_types::i32 ProgramGetResourceLocationIndex(glhnd const& program, GLenum programInterface, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceLocationIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::i32)glGetProgramResourceLocationIndex(C_OCAST<libc_types::u32>(program), programInterface, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void ProgramGetResourceName(glhnd const& program, GLenum programInterface, libc_types::u32 index, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramResourceName(C_OCAST<libc_types::u32>(program), programInterface, index, bufSize, length, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void ProgramGetResourceiv(glhnd const& program, GLenum programInterface, libc_types::u32 index, libc_types::i32 propCount, const GLenum * props, libc_types::i32 count, libc_types::i32 * length, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramResourceiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramResourceiv(C_OCAST<libc_types::u32>(program), programInterface, index, propCount, props, count, length, params);
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
    glInvalidateBufferData(C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void BufInvalidateSubData(glhnd const& buffer, libc_types::ptroff offset, libc_types::ptroff length)
{
#ifndef NDEBUG
    if(!glInvalidateBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateBufferSubData(C_OCAST<libc_types::u32>(buffer), offset, length);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x300)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_30)
STATICINLINE void FBInvalidate(FramebufferT target, libc_types::i32 numAttachments, const GLenum * attachments)
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
STATICINLINE void FBInvalidateSub(FramebufferT target, libc_types::i32 numAttachments, const GLenum * attachments, typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glInvalidateSubFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateSubFramebuffer(to_enum(target), numAttachments, attachments, x[0], x[1], width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void TexInvalidateImage(glhnd const& texture, libc_types::i32 level)
{
#ifndef NDEBUG
    if(!glInvalidateTexImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateTexImage(C_OCAST<libc_types::u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void TexInvalidateSubImage(glhnd const& texture, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::geometry::size_3d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glInvalidateTexSubImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateTexSubImage(C_OCAST<libc_types::u32>(texture), level, xoffset.x, xoffset.y, xoffset.z, width.width, width.height, width.depth);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void MultiDrawArraysIndirect(DrwMd const& mode, libc_types::uintptr indirect, libc_types::i32 drawcount, libc_types::i32 stride)
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
STATICINLINE void MultiDrawElementsIndirect(DrwMd const& mode, TypeEnum type, libc_types::uintptr indirect, libc_types::i32 drawcount, libc_types::i32 stride)
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
STATICINLINE void ObjectLabel(GLenum identifier, libc_types::u32 name, libc_types::i32 length, const GLchar * label)
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
STATICINLINE void ObjectPtrLabel(const void * ptr, libc_types::i32 length, const GLchar * label)
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
STATICINLINE void PushDebugGroup(GLenum source, glhnd const& id, libc_types::i32 length, const GLchar * message)
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
    glPushDebugGroup(source, C_OCAST<libc_types::u32>(id), length, message);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void SSBOBinding(glhnd const& program, libc_types::u32 storageBlockIndex, libc_types::u32 storageBlockBinding)
{
#ifndef NDEBUG
    if(!glShaderStorageBlockBinding)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glShaderStorageBlockBinding(C_OCAST<libc_types::u32>(program), storageBlockIndex, storageBlockBinding);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void TexBufferRange(TexComp::tex_flag const& target, typing::pixels::PixFmt internalformat, glhnd const& buffer, libc_types::ptroff offset, libc_types::ptroff size)
{
#ifndef NDEBUG
    if(!glTexBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexBufferRange(texture_to_enum(target), to_enum(internalformat), C_OCAST<libc_types::u32>(buffer), offset, size);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void TexStorage2DMultisample(TexComp::tex_flag const& target, libc_types::i32 samples, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width, libc_types::u8 fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTexStorage2DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage2DMultisample(texture_to_enum(target), samples, to_enum(internalformat), width.w, width.h, fixedsamplelocations);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void TexStorage3DMultisample(TexComp::tex_flag const& target, libc_types::i32 samples, typing::pixels::PixFmt internalformat, typing::geometry::size_3d<libc_types::i32> const& width, libc_types::u8 fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTexStorage3DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage3DMultisample(texture_to_enum(target), samples, to_enum(internalformat), width.width, width.height, width.depth, fixedsamplelocations);
}
#endif


#if GL_VERSION_VERIFY(0x430, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_43)
STATICINLINE void TexView(glhnd const& texture, TexComp::tex_flag const& target, libc_types::u32 origtexture, typing::pixels::PixFmt internalformat, libc_types::u32 minlevel, libc_types::u32 numlevels, libc_types::u32 minlayer, libc_types::u32 numlayers)
{
#ifndef NDEBUG
    if(!glTextureView)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureView(C_OCAST<libc_types::u32>(texture), texture_to_enum(target), origtexture, to_enum(internalformat), minlevel, numlevels, minlayer, numlayers);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VAOBinding(libc_types::u32 attribindex, libc_types::u32 bindingindex)
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
STATICINLINE void VAOFormat(libc_types::u32 attribindex, libc_types::i32 size, GLenum type, libc_types::u8 normalized, libc_types::u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexAttribFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribFormat(attribindex, size, type, normalized, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VAOIFormat(libc_types::u32 attribindex, libc_types::i32 size, GLenum type, libc_types::u32 relativeoffset)
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
STATICINLINE void VAOLFormat(libc_types::u32 attribindex, libc_types::i32 size, GLenum type, libc_types::u32 relativeoffset)
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
STATICINLINE void VAOBindingDivisor(libc_types::u32 bindingindex, libc_types::u32 divisor)
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
struct v44
    : v43<ReqVer>
{
using Parent = v43<ReqVer>;
using Parent::TexBind;
using Parent::BufBindBase;
using Parent::BufBindRange;
using Parent::SamplerBind;
using Parent::TexBindImage;
using Parent::VertBufBind;

#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_44)
STATICINLINE void BufBindBase(BufferComp::buf_flag const& target, libc_types::u32 first, libc_types::i32 count, const libc_types::u32 * buffers)
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
STATICINLINE void BufBindRange(BufferComp::buf_flag const& target, libc_types::u32 first, libc_types::i32 count, const libc_types::u32 * buffers, const libc_types::ptroff * offsets, const libc_types::ptroff * sizes)
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
STATICINLINE void TexBindImage(libc_types::u32 first, libc_types::i32 count, const libc_types::u32 * textures)
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
STATICINLINE void SamplerBind(libc_types::u32 first, libc_types::i32 count, const libc_types::u32 * samplers)
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
STATICINLINE void TexBind(libc_types::u32 first, libc_types::i32 count, const libc_types::u32 * textures)
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
STATICINLINE void VertBufBind(libc_types::u32 first, libc_types::i32 count, const libc_types::u32 * buffers, const libc_types::ptroff * offsets, const libc_types::i32 * strides)
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
STATICINLINE void BufStorage(BufferComp::buf_flag const& target, semantic::BytesConst const& data, semantic::RSCA flags)
{
#ifndef NDEBUG
    if(!glBufferStorage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBufferStorage(buffer_to_enum(target), C_FCAST<libc_types::ptroff>(data.size), data.data, to_enum2(flags));
}
#endif


#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_44)
STATICINLINE void TexClearImage(glhnd const& texture, libc_types::i32 level, typing::PixCmp format, typing::pixels::BitFmt type, const void * data)
{
#ifndef NDEBUG
    if(!glClearTexImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearTexImage(C_OCAST<libc_types::u32>(texture), level, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), data);
}
#endif


#if GL_VERSION_VERIFY(0x440, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_44)
STATICINLINE void TexClearSubImage(glhnd const& texture, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::geometry::size_3d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, const void * data)
{
#ifndef NDEBUG
    if(!glClearTexSubImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearTexSubImage(C_OCAST<libc_types::u32>(texture), level, xoffset.x, xoffset.y, xoffset.z, width.width, width.height, width.depth, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), data);
}
#endif

};
template<typename ReqVer>
struct v45
    : v44<ReqVer>
{
using Parent = v44<ReqVer>;
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
STATICINLINE void TexBindUnit(libc_types::u32 unit, glhnd const& texture)
{
#ifndef NDEBUG
    if(!glBindTextureUnit)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindTextureUnit(unit, C_OCAST<libc_types::u32>(texture));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBBlitNamed(libc_types::u32 readFramebuffer, libc_types::u32 drawFramebuffer, libc_types::i32 srcX0, libc_types::i32 srcY0, libc_types::i32 srcX1, libc_types::i32 srcY1, libc_types::i32 dstX0, libc_types::i32 dstY0, libc_types::i32 dstX1, libc_types::i32 dstY1, libc_types::u32 mask, GLenum filter)
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
STATICINLINE GLenum FBCheckNamedStatus(glhnd const& framebuffer, FramebufferT target)
{
#ifndef NDEBUG
    if(!glCheckNamedFramebufferStatus)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (GLenum)glCheckNamedFramebufferStatus(C_OCAST<libc_types::u32>(framebuffer), to_enum(target));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufClearData(glhnd const& buffer, typing::pixels::PixFmt internalformat, typing::PixCmp format, typing::pixels::BitFmt type, const void * data)
{
#ifndef NDEBUG
    if(!glClearNamedBufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearNamedBufferData(C_OCAST<libc_types::u32>(buffer), to_enum(internalformat), to_enum(format, typing::pixels::PixFmt::None), to_enum(type), data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufClearSubData(glhnd const& buffer, typing::pixels::PixFmt internalformat, libc_types::ptroff offset, typing::PixCmp format, typing::pixels::BitFmt type, semantic::BytesConst const& data)
{
#ifndef NDEBUG
    if(!glClearNamedBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearNamedBufferSubData(C_OCAST<libc_types::u32>(buffer), to_enum(internalformat), offset, C_FCAST<libc_types::ptroff>(data.size), to_enum(format, typing::pixels::PixFmt::None), to_enum(type), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBClearNamedfi(glhnd const& framebuffer, GLenum buffer, libc_types::i32 drawbuffer, libc_types::f32 depth, libc_types::i32 stencil)
{
#ifndef NDEBUG
    if(!glClearNamedFramebufferfi)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearNamedFramebufferfi(C_OCAST<libc_types::u32>(framebuffer), buffer, drawbuffer, depth, stencil);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBClearNamedfv(glhnd const& framebuffer, GLenum buffer, libc_types::i32 drawbuffer, semantic::Span<const libc_types::f32> const& value)
{
#ifndef NDEBUG
    if(!glClearNamedFramebufferfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearNamedFramebufferfv(C_OCAST<libc_types::u32>(framebuffer), buffer, drawbuffer, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBClearNamediv(glhnd const& framebuffer, GLenum buffer, libc_types::i32 drawbuffer, semantic::Span<const libc_types::i32> const& value)
{
#ifndef NDEBUG
    if(!glClearNamedFramebufferiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearNamedFramebufferiv(C_OCAST<libc_types::u32>(framebuffer), buffer, drawbuffer, value.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBClearNameduiv(glhnd const& framebuffer, GLenum buffer, libc_types::i32 drawbuffer, semantic::Span<const libc_types::u32> const& value)
{
#ifndef NDEBUG
    if(!glClearNamedFramebufferuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glClearNamedFramebufferuiv(C_OCAST<libc_types::u32>(framebuffer), buffer, drawbuffer, value.data);
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
STATICINLINE void TexCompressedSubImage1D(glhnd const& texture, libc_types::i32 level, libc_types::i32 xoffset, libc_types::i32 width, typing::pixels::CompFmt format, libc_types::i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTextureSubImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTextureSubImage1D(C_OCAST<libc_types::u32>(texture), level, xoffset, width, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexCompressedSubImage2D(glhnd const& texture, libc_types::i32 level, typing::geometry::point_2d<libc_types::i32> const& xoffset, typing::geometry::size_2d<libc_types::i32> const& width, typing::pixels::CompFmt format, libc_types::i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTextureSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTextureSubImage2D(C_OCAST<libc_types::u32>(texture), level, xoffset.x, xoffset.y, width.w, width.h, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexCompressedSubImage3D(glhnd const& texture, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::geometry::size_3d<libc_types::i32> const& width, typing::pixels::CompFmt format, libc_types::i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTextureSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTextureSubImage3D(C_OCAST<libc_types::u32>(texture), level, xoffset.x, xoffset.y, xoffset.z, width.width, width.height, width.depth, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufCopySubData(libc_types::u32 readBuffer, libc_types::u32 writeBuffer, libc_types::ptroff readOffset, libc_types::ptroff writeOffset, libc_types::ptroff size)
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
STATICINLINE void TexCopySubImage1D(glhnd const& texture, libc_types::i32 level, libc_types::i32 xoffset, typing::vectors::tvector<libc_types::i32, 2> x, libc_types::i32 width)
{
#ifndef NDEBUG
    if(!glCopyTextureSubImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTextureSubImage1D(C_OCAST<libc_types::u32>(texture), level, xoffset, x[0], x[1], width);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexCopySubImage2D(glhnd const& texture, libc_types::i32 level, typing::geometry::point_2d<libc_types::i32> const& xoffset, typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glCopyTextureSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTextureSubImage2D(C_OCAST<libc_types::u32>(texture), level, xoffset.x, xoffset.y, x[0], x[1], width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexCopySubImage3D(glhnd const& texture, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glCopyTextureSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTextureSubImage3D(C_OCAST<libc_types::u32>(texture), level, xoffset.x, xoffset.y, xoffset.z, x[0], x[1], width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufAllocEx(semantic::Span<libc_types::u32>&& buffers)
{
#ifndef NDEBUG
    if(!glCreateBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateBuffers(C_FCAST<libc_types::i32>(buffers.elements), buffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBAllocEx(semantic::Span<libc_types::u32>&& framebuffers)
{
#ifndef NDEBUG
    if(!glCreateFramebuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateFramebuffers(C_FCAST<libc_types::i32>(framebuffers.elements), framebuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void PipelineAllocEx(semantic::Span<libc_types::u32>&& pipelines)
{
#ifndef NDEBUG
    if(!glCreateProgramPipelines)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateProgramPipelines(C_FCAST<libc_types::i32>(pipelines.elements), pipelines.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void QueryAllocEx(QueryComp::query_flag const& target, semantic::Span<libc_types::u32>&& ids)
{
#ifndef NDEBUG
    if(!glCreateQueries)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateQueries(query_to_enum(target), C_FCAST<libc_types::i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void RBufAllocEx(semantic::Span<libc_types::u32>&& renderbuffers)
{
#ifndef NDEBUG
    if(!glCreateRenderbuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateRenderbuffers(C_FCAST<libc_types::i32>(renderbuffers.elements), renderbuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void SamplerAllocEx(semantic::Span<libc_types::u32>&& samplers)
{
#ifndef NDEBUG
    if(!glCreateSamplers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateSamplers(C_FCAST<libc_types::i32>(samplers.elements), samplers.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexAllocEx(TexComp::tex_flag const& target, semantic::Span<libc_types::u32>&& textures)
{
#ifndef NDEBUG
    if(!glCreateTextures)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateTextures(texture_to_enum(target), C_FCAST<libc_types::i32>(textures.elements), textures.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void XFAllocEx(semantic::Span<libc_types::u32>&& ids)
{
#ifndef NDEBUG
    if(!glCreateTransformFeedbacks)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateTransformFeedbacks(C_FCAST<libc_types::i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOAllocEx(semantic::Span<libc_types::u32>&& arrays)
{
#ifndef NDEBUG
    if(!glCreateVertexArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCreateVertexArrays(C_FCAST<libc_types::i32>(arrays.elements), arrays.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAODisable(glhnd const& vaobj, libc_types::u32 index)
{
#ifndef NDEBUG
    if(!glDisableVertexArrayAttrib)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDisableVertexArrayAttrib(C_OCAST<libc_types::u32>(vaobj), index);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOEnable(glhnd const& vaobj, libc_types::u32 index)
{
#ifndef NDEBUG
    if(!glEnableVertexArrayAttrib)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glEnableVertexArrayAttrib(C_OCAST<libc_types::u32>(vaobj), index);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufFlushMappedRange(glhnd const& buffer, libc_types::ptroff offset, libc_types::ptroff length)
{
#ifndef NDEBUG
    if(!glFlushMappedNamedBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFlushMappedNamedBufferRange(C_OCAST<libc_types::u32>(buffer), offset, length);
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
    glGenerateTextureMipmap(C_OCAST<libc_types::u32>(texture));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetCompressedImage(glhnd const& texture, libc_types::i32 level, libc_types::i32 bufSize, void * pixels)
{
#ifndef NDEBUG
    if(!glGetCompressedTextureImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetCompressedTextureImage(C_OCAST<libc_types::u32>(texture), level, bufSize, pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetCompressedSubImage(glhnd const& texture, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::geometry::size_3d<libc_types::i32> const& width, libc_types::i32 bufSize, void * pixels)
{
#ifndef NDEBUG
    if(!glGetCompressedTextureSubImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetCompressedTextureSubImage(C_OCAST<libc_types::u32>(texture), level, xoffset.x, xoffset.y, xoffset.z, width.width, width.height, width.depth, bufSize, pixels);
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
    return (GLenum)glGetGraphicsResetStatus();
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufGetParameteri64v(glhnd const& buffer, GLenum pname, libc_types::i64 * params)
{
#ifndef NDEBUG
    if(!glGetNamedBufferParameteri64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedBufferParameteri64v(C_OCAST<libc_types::u32>(buffer), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufGetParameteriv(glhnd const& buffer, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetNamedBufferParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedBufferParameteriv(C_OCAST<libc_types::u32>(buffer), pname, params);
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
    glGetNamedBufferPointerv(C_OCAST<libc_types::u32>(buffer), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufGetSubData(glhnd const& buffer, libc_types::ptroff offset, semantic::Bytes const& data)
{
#ifndef NDEBUG
    if(!glGetNamedBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedBufferSubData(C_OCAST<libc_types::u32>(buffer), offset, C_FCAST<libc_types::ptroff>(data.size), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBGetNamedAttachmentParameteriv(glhnd const& framebuffer, GLenum attachment, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetNamedFramebufferAttachmentParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedFramebufferAttachmentParameteriv(C_OCAST<libc_types::u32>(framebuffer), attachment, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBGetNamedParameteriv(glhnd const& framebuffer, GLenum pname, libc_types::i32 * param)
{
#ifndef NDEBUG
    if(!glGetNamedFramebufferParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedFramebufferParameteriv(C_OCAST<libc_types::u32>(framebuffer), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void RBufGetParameteriv(glhnd const& renderbuffer, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetNamedRenderbufferParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetNamedRenderbufferParameteriv(C_OCAST<libc_types::u32>(renderbuffer), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void QueryGetBufferObjecti64v(glhnd const& id, glhnd const& buffer, GLenum pname, libc_types::ptroff offset)
{
#ifndef NDEBUG
    if(!glGetQueryBufferObjecti64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryBufferObjecti64v(C_OCAST<libc_types::u32>(id), C_OCAST<libc_types::u32>(buffer), pname, offset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void QueryGetBufferObjectiv(glhnd const& id, glhnd const& buffer, GLenum pname, libc_types::ptroff offset)
{
#ifndef NDEBUG
    if(!glGetQueryBufferObjectiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryBufferObjectiv(C_OCAST<libc_types::u32>(id), C_OCAST<libc_types::u32>(buffer), pname, offset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void QueryGetBufferObjectui64v(glhnd const& id, glhnd const& buffer, GLenum pname, libc_types::ptroff offset)
{
#ifndef NDEBUG
    if(!glGetQueryBufferObjectui64v)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryBufferObjectui64v(C_OCAST<libc_types::u32>(id), C_OCAST<libc_types::u32>(buffer), pname, offset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void QueryGetBufferObjectuiv(glhnd const& id, glhnd const& buffer, GLenum pname, libc_types::ptroff offset)
{
#ifndef NDEBUG
    if(!glGetQueryBufferObjectuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryBufferObjectuiv(C_OCAST<libc_types::u32>(id), C_OCAST<libc_types::u32>(buffer), pname, offset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetImage(glhnd const& texture, libc_types::i32 level, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 bufSize, void * pixels)
{
#ifndef NDEBUG
    if(!glGetTextureImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureImage(C_OCAST<libc_types::u32>(texture), level, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), bufSize, pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetLevelParameterfv(glhnd const& texture, libc_types::i32 level, GLenum pname, libc_types::f32 * params)
{
#ifndef NDEBUG
    if(!glGetTextureLevelParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureLevelParameterfv(C_OCAST<libc_types::u32>(texture), level, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetLevelParameteriv(glhnd const& texture, libc_types::i32 level, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetTextureLevelParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureLevelParameteriv(C_OCAST<libc_types::u32>(texture), level, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetParameterIiv(glhnd const& texture, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetTextureParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureParameterIiv(C_OCAST<libc_types::u32>(texture), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetParameterIuiv(glhnd const& texture, GLenum pname, libc_types::u32 * params)
{
#ifndef NDEBUG
    if(!glGetTextureParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureParameterIuiv(C_OCAST<libc_types::u32>(texture), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetParameterfv(glhnd const& texture, GLenum pname, libc_types::f32 * params)
{
#ifndef NDEBUG
    if(!glGetTextureParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureParameterfv(C_OCAST<libc_types::u32>(texture), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetParameteriv(glhnd const& texture, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetTextureParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureParameteriv(C_OCAST<libc_types::u32>(texture), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetSubImage(glhnd const& texture, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::geometry::size_3d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 bufSize, void * pixels)
{
#ifndef NDEBUG
    if(!glGetTextureSubImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTextureSubImage(C_OCAST<libc_types::u32>(texture), level, xoffset.x, xoffset.y, xoffset.z, width.width, width.height, width.depth, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), bufSize, pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void XFGeti64_v(libc_types::u32 xfb, GLenum pname, libc_types::u32 index, libc_types::i64 * param)
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
STATICINLINE void XFGeti_v(libc_types::u32 xfb, GLenum pname, libc_types::u32 index, libc_types::i32 * param)
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
STATICINLINE void XFGetiv(libc_types::u32 xfb, GLenum pname, libc_types::i32 * param)
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
STATICINLINE void VAOGetIndexed64iv(glhnd const& vaobj, libc_types::u32 index, GLenum pname, libc_types::i64 * param)
{
#ifndef NDEBUG
    if(!glGetVertexArrayIndexed64iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexArrayIndexed64iv(C_OCAST<libc_types::u32>(vaobj), index, pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOGetIndexediv(glhnd const& vaobj, libc_types::u32 index, GLenum pname, libc_types::i32 * param)
{
#ifndef NDEBUG
    if(!glGetVertexArrayIndexediv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexArrayIndexediv(C_OCAST<libc_types::u32>(vaobj), index, pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOGetiv(glhnd const& vaobj, GLenum pname, libc_types::i32 * param)
{
#ifndef NDEBUG
    if(!glGetVertexArrayiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetVertexArrayiv(C_OCAST<libc_types::u32>(vaobj), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexGetnCompressedImage(TexComp::tex_flag const& target, libc_types::i32 lod, libc_types::i32 bufSize, void * pixels)
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
STATICINLINE void TexGetnImage(TexComp::tex_flag const& target, libc_types::i32 level, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 bufSize, void * pixels)
{
#ifndef NDEBUG
    if(!glGetnTexImage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnTexImage(texture_to_enum(target), level, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), bufSize, pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void UnifGetndv(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::f64 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformdv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformdv(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void UnifGetnfv(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::f32 * params)
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
    glGetnUniformfv(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void UnifGetniv(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::i32 * params)
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
    glGetnUniformiv(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void UnifGetnuiv(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::u32 * params)
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
    glGetnUniformuiv(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBInvalidateNamedData(glhnd const& framebuffer, libc_types::i32 numAttachments, const GLenum * attachments)
{
#ifndef NDEBUG
    if(!glInvalidateNamedFramebufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateNamedFramebufferData(C_OCAST<libc_types::u32>(framebuffer), numAttachments, attachments);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBInvalidateNamedSubData(glhnd const& framebuffer, libc_types::i32 numAttachments, const GLenum * attachments, typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glInvalidateNamedFramebufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateNamedFramebufferSubData(C_OCAST<libc_types::u32>(framebuffer), numAttachments, attachments, x[0], x[1], width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void * BufMap(glhnd const& buffer, semantic::RSCA access)
{
#ifndef NDEBUG
    if(!glMapNamedBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (void *)glMapNamedBuffer(C_OCAST<libc_types::u32>(buffer), to_enum2(access));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void * BufMapRange(glhnd const& buffer, libc_types::ptroff offset, libc_types::ptroff length, semantic::RSCA access)
{
#ifndef NDEBUG
    if(!glMapNamedBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (void *)glMapNamedBufferRange(C_OCAST<libc_types::u32>(buffer), offset, length, to_enum2(access));
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x310)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_31)
STATICINLINE void MemoryBarrierByRegion(libc_types::u32 barriers)
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
STATICINLINE void BufData(glhnd const& buffer, semantic::BytesConst const& data, semantic::RSCA usage)
{
#ifndef NDEBUG
    if(!glNamedBufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedBufferData(C_OCAST<libc_types::u32>(buffer), C_FCAST<libc_types::ptroff>(data.size), data.data, to_enum1(usage));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufStorage(glhnd const& buffer, semantic::BytesConst const& data, semantic::RSCA flags)
{
#ifndef NDEBUG
    if(!glNamedBufferStorage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedBufferStorage(C_OCAST<libc_types::u32>(buffer), C_FCAST<libc_types::ptroff>(data.size), data.data, to_enum2(flags));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufSubData(glhnd const& buffer, libc_types::ptroff offset, semantic::BytesConst const& data)
{
#ifndef NDEBUG
    if(!glNamedBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedBufferSubData(C_OCAST<libc_types::u32>(buffer), offset, C_FCAST<libc_types::ptroff>(data.size), data.data);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void NamedFramebufferDrawBuffer(glhnd const& framebuffer, GLenum buf)
{
#ifndef NDEBUG
    if(!glNamedFramebufferDrawBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferDrawBuffer(C_OCAST<libc_types::u32>(framebuffer), buf);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void NamedFramebufferDrawBuffers(glhnd const& framebuffer, libc_types::i32 n, const GLenum * bufs)
{
#ifndef NDEBUG
    if(!glNamedFramebufferDrawBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferDrawBuffers(C_OCAST<libc_types::u32>(framebuffer), n, bufs);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBNamedParameteri(glhnd const& framebuffer, GLenum pname, libc_types::i32 param)
{
#ifndef NDEBUG
    if(!glNamedFramebufferParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferParameteri(C_OCAST<libc_types::u32>(framebuffer), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBNamedReadBuffer(glhnd const& framebuffer, GLenum src)
{
#ifndef NDEBUG
    if(!glNamedFramebufferReadBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferReadBuffer(C_OCAST<libc_types::u32>(framebuffer), src);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBNamedRenderbuffer(glhnd const& framebuffer, GLenum attachment, GLenum renderbuffertarget, glhnd const& renderbuffer)
{
#ifndef NDEBUG
    if(!glNamedFramebufferRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferRenderbuffer(C_OCAST<libc_types::u32>(framebuffer), attachment, renderbuffertarget, C_OCAST<libc_types::u32>(renderbuffer));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBNamedTexture(glhnd const& framebuffer, GLenum attachment, glhnd const& texture, libc_types::i32 level)
{
#ifndef NDEBUG
    if(!glNamedFramebufferTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferTexture(C_OCAST<libc_types::u32>(framebuffer), attachment, C_OCAST<libc_types::u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void FBNamedTextureLayer(glhnd const& framebuffer, GLenum attachment, glhnd const& texture, libc_types::i32 level, libc_types::i32 layer)
{
#ifndef NDEBUG
    if(!glNamedFramebufferTextureLayer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedFramebufferTextureLayer(C_OCAST<libc_types::u32>(framebuffer), attachment, C_OCAST<libc_types::u32>(texture), level, layer);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void RBufStorage(glhnd const& renderbuffer, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glNamedRenderbufferStorage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedRenderbufferStorage(C_OCAST<libc_types::u32>(renderbuffer), to_enum(internalformat), width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void RBufStorageMultisample(glhnd const& renderbuffer, libc_types::i32 samples, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glNamedRenderbufferStorageMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glNamedRenderbufferStorageMultisample(C_OCAST<libc_types::u32>(renderbuffer), samples, to_enum(internalformat), width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void ReadnPixels(typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 bufSize, void * data)
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
    glReadnPixels(x[0], x[1], width.w, width.h, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), bufSize, data);
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
STATICINLINE void TexBuffer(glhnd const& texture, typing::pixels::PixFmt internalformat, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glTextureBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureBuffer(C_OCAST<libc_types::u32>(texture), to_enum(internalformat), C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexBufferRange(glhnd const& texture, typing::pixels::PixFmt internalformat, glhnd const& buffer, libc_types::ptroff offset, libc_types::ptroff size)
{
#ifndef NDEBUG
    if(!glTextureBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureBufferRange(C_OCAST<libc_types::u32>(texture), to_enum(internalformat), C_OCAST<libc_types::u32>(buffer), offset, size);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexParameterIiv(glhnd const& texture, GLenum pname, const libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glTextureParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureParameterIiv(C_OCAST<libc_types::u32>(texture), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexParameterIuiv(glhnd const& texture, GLenum pname, const libc_types::u32 * params)
{
#ifndef NDEBUG
    if(!glTextureParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureParameterIuiv(C_OCAST<libc_types::u32>(texture), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexParameterf(glhnd const& texture, GLenum pname, libc_types::f32 param)
{
#ifndef NDEBUG
    if(!glTextureParameterf)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureParameterf(C_OCAST<libc_types::u32>(texture), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexParameterfv(glhnd const& texture, GLenum pname, const libc_types::f32 * param)
{
#ifndef NDEBUG
    if(!glTextureParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureParameterfv(C_OCAST<libc_types::u32>(texture), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexParameteri(glhnd const& texture, GLenum pname, libc_types::i32 param)
{
#ifndef NDEBUG
    if(!glTextureParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureParameteri(C_OCAST<libc_types::u32>(texture), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexParameteriv(glhnd const& texture, GLenum pname, const libc_types::i32 * param)
{
#ifndef NDEBUG
    if(!glTextureParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureParameteriv(C_OCAST<libc_types::u32>(texture), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexStorage1D(glhnd const& texture, libc_types::i32 levels, typing::pixels::PixFmt internalformat, libc_types::i32 width)
{
#ifndef NDEBUG
    if(!glTextureStorage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureStorage1D(C_OCAST<libc_types::u32>(texture), levels, to_enum(internalformat), width);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexStorage2D(glhnd const& texture, libc_types::i32 levels, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glTextureStorage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureStorage2D(C_OCAST<libc_types::u32>(texture), levels, to_enum(internalformat), width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexStorage2DMultisample(glhnd const& texture, libc_types::i32 samples, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width, libc_types::u8 fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTextureStorage2DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureStorage2DMultisample(C_OCAST<libc_types::u32>(texture), samples, to_enum(internalformat), width.w, width.h, fixedsamplelocations);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexStorage3D(glhnd const& texture, libc_types::i32 levels, typing::pixels::PixFmt internalformat, typing::geometry::size_3d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glTextureStorage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureStorage3D(C_OCAST<libc_types::u32>(texture), levels, to_enum(internalformat), width.width, width.height, width.depth);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexStorage3DMultisample(glhnd const& texture, libc_types::i32 samples, typing::pixels::PixFmt internalformat, typing::geometry::size_3d<libc_types::i32> const& width, libc_types::u8 fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTextureStorage3DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureStorage3DMultisample(C_OCAST<libc_types::u32>(texture), samples, to_enum(internalformat), width.width, width.height, width.depth, fixedsamplelocations);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexSubImage1D(glhnd const& texture, libc_types::i32 level, libc_types::i32 xoffset, libc_types::i32 width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTextureSubImage1D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureSubImage1D(C_OCAST<libc_types::u32>(texture), level, xoffset, width, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexSubImage2D(glhnd const& texture, libc_types::i32 level, typing::geometry::point_2d<libc_types::i32> const& xoffset, typing::geometry::size_2d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTextureSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureSubImage2D(C_OCAST<libc_types::u32>(texture), level, xoffset.x, xoffset.y, width.w, width.h, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void TexSubImage3D(glhnd const& texture, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::geometry::size_3d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTextureSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTextureSubImage3D(C_OCAST<libc_types::u32>(texture), level, xoffset.x, xoffset.y, xoffset.z, width.width, width.height, width.depth, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufTransformFeedbackBase(libc_types::u32 xfb, libc_types::u32 index, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glTransformFeedbackBufferBase)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTransformFeedbackBufferBase(xfb, index, C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void BufTransformFeedbackRange(libc_types::u32 xfb, libc_types::u32 index, glhnd const& buffer, libc_types::ptroff offset, libc_types::ptroff size)
{
#ifndef NDEBUG
    if(!glTransformFeedbackBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTransformFeedbackBufferRange(xfb, index, C_OCAST<libc_types::u32>(buffer), offset, size);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE libc_types::u8 BufUnmap(glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glUnmapNamedBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glUnmapNamedBuffer(C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOBinding(glhnd const& vaobj, libc_types::u32 attribindex, libc_types::u32 bindingindex)
{
#ifndef NDEBUG
    if(!glVertexArrayAttribBinding)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayAttribBinding(C_OCAST<libc_types::u32>(vaobj), attribindex, bindingindex);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOFormat(glhnd const& vaobj, libc_types::u32 attribindex, libc_types::i32 size, GLenum type, libc_types::u8 normalized, libc_types::u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexArrayAttribFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayAttribFormat(C_OCAST<libc_types::u32>(vaobj), attribindex, size, type, normalized, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOIFormat(glhnd const& vaobj, libc_types::u32 attribindex, libc_types::i32 size, GLenum type, libc_types::u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexArrayAttribIFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayAttribIFormat(C_OCAST<libc_types::u32>(vaobj), attribindex, size, type, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOLFormat(glhnd const& vaobj, libc_types::u32 attribindex, libc_types::i32 size, GLenum type, libc_types::u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexArrayAttribLFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayAttribLFormat(C_OCAST<libc_types::u32>(vaobj), attribindex, size, type, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOBindingDivisor(glhnd const& vaobj, libc_types::u32 bindingindex, libc_types::u32 divisor)
{
#ifndef NDEBUG
    if(!glVertexArrayBindingDivisor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayBindingDivisor(C_OCAST<libc_types::u32>(vaobj), bindingindex, divisor);
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
    glVertexArrayElementBuffer(C_OCAST<libc_types::u32>(vaobj), C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOVertexBuffer(glhnd const& vaobj, libc_types::u32 bindingindex, glhnd const& buffer, libc_types::ptroff offset, libc_types::i32 stride)
{
#ifndef NDEBUG
    if(!glVertexArrayVertexBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayVertexBuffer(C_OCAST<libc_types::u32>(vaobj), bindingindex, C_OCAST<libc_types::u32>(buffer), offset, stride);
}
#endif


#if GL_VERSION_VERIFY(0x450, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_45)
STATICINLINE void VAOVertexBuffers(glhnd const& vaobj, libc_types::u32 first, libc_types::i32 count, const libc_types::u32 * buffers, const libc_types::ptroff * offsets, const libc_types::i32 * strides)
{
#ifndef NDEBUG
    if(!glVertexArrayVertexBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexArrayVertexBuffers(C_OCAST<libc_types::u32>(vaobj), first, count, buffers, offsets, strides);
}
#endif

};
template<typename ReqVer>
struct v46
    : v45<ReqVer>
{
using Parent = v45<ReqVer>;

#if GL_VERSION_VERIFY(0x460, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_46)
STATICINLINE void MultiDrawArraysIndirectCount(DrwMd const& mode, libc_types::uintptr indirect, libc_types::ptroff drawcount, libc_types::i32 maxdrawcount, libc_types::i32 stride)
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
STATICINLINE void MultiDrawElementsIndirectCount(DrwMd const& mode, TypeEnum type, libc_types::uintptr indirect, libc_types::ptroff drawcount, libc_types::i32 maxdrawcount, libc_types::i32 stride)
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
    glMultiDrawElementsIndirectCount(to_enum(mode.t, mode.c), to_enum(type), C_RCAST<const void *>(indirect), drawcount, maxdrawcount, stride);
}
#endif


#if GL_VERSION_VERIFY(0x460, GL_VERSION_NONE)
GL_VERSION_REQ_DESKTOP(GLVER_46)
STATICINLINE void PolygonOffsetClamp(libc_types::f32 factor, libc_types::f32 units, libc_types::f32 clamp)
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
STATICINLINE void ShaderSpecialize(glhnd const& shader, const GLchar * pEntryPoint, libc_types::u32 numSpecializationConstants, const libc_types::u32 * pConstantIndex, const libc_types::u32 * pConstantValue)
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
    glSpecializeShader(C_OCAST<libc_types::u32>(shader), pEntryPoint, numSpecializationConstants, pConstantIndex, pConstantValue);
}
#endif

};
#endif
template<typename ReqVer>
struct v20es
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
    glAttachShader(C_OCAST<libc_types::u32>(program), C_OCAST<libc_types::u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void AttribBindLocation(glhnd const& program, libc_types::u32 index, const GLchar * name)
{
#ifndef NDEBUG
    if(!glBindAttribLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindAttribLocation(C_OCAST<libc_types::u32>(program), index, name);
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
    glBindBuffer(buffer_to_enum(target), C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBBind(FramebufferT target, glhnd const& framebuffer)
{
#ifndef NDEBUG
    if(!glBindFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindFramebuffer(to_enum(target), C_OCAST<libc_types::u32>(framebuffer));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufBind(GLenum target, glhnd const& renderbuffer)
{
#ifndef NDEBUG
    if(!glBindRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindRenderbuffer(target, C_OCAST<libc_types::u32>(renderbuffer));
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
    glBindTexture(texture_to_enum(target), C_OCAST<libc_types::u32>(texture));
}
#endif


#if GL_VERSION_VERIFY(0x140, 0x200)
GL_VERSION_REQ_COMBO(GLVER_14, GLESVER_20)
STATICINLINE void BlendColor(typing::vectors::tvector<libc_types::f32,4> const& color)
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
STATICINLINE void BufData(BufferComp::buf_flag const& target, semantic::BytesConst const& data, semantic::RSCA usage)
{
#ifndef NDEBUG
    if(!glBufferData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBufferData(buffer_to_enum(target), C_FCAST<libc_types::ptroff>(data.size), data.data, to_enum1(usage));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x200)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_20)
STATICINLINE void BufSubData(BufferComp::buf_flag const& target, libc_types::ptroff offset, semantic::BytesConst const& data)
{
#ifndef NDEBUG
    if(!glBufferSubData)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBufferSubData(buffer_to_enum(target), offset, C_FCAST<libc_types::ptroff>(data.size), data.data);
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
    return (GLenum)glCheckFramebufferStatus(to_enum(target));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Clear(libc_types::u32 mask)
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
STATICINLINE void ClearColor(typing::vectors::tvector<libc_types::f32,4> const& color)
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
STATICINLINE void ClearDepthf(libc_types::f32 d)
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
STATICINLINE void ClearStencil(libc_types::i32 s)
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
STATICINLINE void ColorMask(typing::vectors::tvector<libc_types::u8,4> const& color)
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
    glCompileShader(C_OCAST<libc_types::u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x200)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_20)
STATICINLINE void TexCompressedImage2D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::CompFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width, libc_types::i32 imageSize, const void * data)
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
    glCompressedTexImage2D(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), width.w, width.h, 0, imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x200)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_20)
STATICINLINE void TexCompressedSubImage2D(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_2d<libc_types::i32> const& xoffset, typing::geometry::size_2d<libc_types::i32> const& width, typing::pixels::CompFmt format, libc_types::i32 imageSize, const void * data)
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
    glCompressedTexSubImage2D(texture_to_enum(target), level, xoffset.x, xoffset.y, width.w, width.h, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexCopyImage2D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::PixFmt internalformat, typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glCopyTexImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexImage2D(texture_to_enum(target), level, to_enum(internalformat), x[0], x[1], width.w, width.h, 0);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexCopySubImage2D(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_2d<libc_types::i32> const& xoffset, typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glCopyTexSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexSubImage2D(texture_to_enum(target), level, xoffset.x, xoffset.y, x[0], x[1], width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE libc_types::u32 ProgramAllocEx()
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
    return (libc_types::u32)glCreateProgram();
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE libc_types::u32 ShaderAllocEx(ShaderStage type)
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
    return (libc_types::u32)glCreateShader(to_enum1(type));
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
STATICINLINE void BufFree(semantic::Span<const libc_types::u32>&& buffers)
{
#ifndef NDEBUG
    if(!glDeleteBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteBuffers(C_FCAST<libc_types::i32>(buffers.elements), buffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBFree(semantic::Span<const libc_types::u32>&& framebuffers)
{
#ifndef NDEBUG
    if(!glDeleteFramebuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteFramebuffers(C_FCAST<libc_types::i32>(framebuffers.elements), framebuffers.data);
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
    glDeleteProgram(C_OCAST<libc_types::u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufFree(semantic::Span<const libc_types::u32>&& renderbuffers)
{
#ifndef NDEBUG
    if(!glDeleteRenderbuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteRenderbuffers(C_FCAST<libc_types::i32>(renderbuffers.elements), renderbuffers.data);
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
    glDeleteShader(C_OCAST<libc_types::u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexFree(semantic::Span<const libc_types::u32>&& textures)
{
#ifndef NDEBUG
    if(!glDeleteTextures)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteTextures(C_FCAST<libc_types::i32>(textures.elements), textures.data);
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
STATICINLINE void DepthMask(libc_types::u8 flag)
{
#ifndef NDEBUG
    if(!glDepthMask)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthMask(flag);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void DepthRangef(libc_types::f32 n, libc_types::f32 f)
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
    glDetachShader(C_OCAST<libc_types::u32>(program), C_OCAST<libc_types::u32>(shader));
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
STATICINLINE void VAODisableArray(libc_types::u32 index)
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
STATICINLINE void DrawArrays(DrwMd const& mode, libc_types::i32 first, libc_types::i32 count)
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
STATICINLINE void DrawElements(DrwMd const& mode, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices)
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
STATICINLINE void VAOEnableArray(libc_types::u32 index)
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
STATICINLINE void FBRenderbuffer(FramebufferT target, GLenum attachment, GLenum renderbuffertarget, glhnd const& renderbuffer)
{
#ifndef NDEBUG
    if(!glFramebufferRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferRenderbuffer(to_enum(target), attachment, renderbuffertarget, C_OCAST<libc_types::u32>(renderbuffer));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBTexture2D(FramebufferT target, GLenum attachment, TexComp::tex_flag const& textarget, glhnd const& texture, libc_types::i32 level)
{
#ifndef NDEBUG
    if(!glFramebufferTexture2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTexture2D(to_enum(target), attachment, texture_to_enum(textarget), C_OCAST<libc_types::u32>(texture), level);
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
STATICINLINE void BufAlloc(semantic::Span<libc_types::u32>&& buffers)
{
#ifndef NDEBUG
    if(!glGenBuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenBuffers(C_FCAST<libc_types::i32>(buffers.elements), buffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void FBAlloc(semantic::Span<libc_types::u32>&& framebuffers)
{
#ifndef NDEBUG
    if(!glGenFramebuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenFramebuffers(C_FCAST<libc_types::i32>(framebuffers.elements), framebuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufAlloc(semantic::Span<libc_types::u32>&& renderbuffers)
{
#ifndef NDEBUG
    if(!glGenRenderbuffers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenRenderbuffers(C_FCAST<libc_types::i32>(renderbuffers.elements), renderbuffers.data);
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE void TexAlloc(semantic::Span<libc_types::u32>&& textures)
{
#ifndef NDEBUG
    if(!glGenTextures)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenTextures(C_FCAST<libc_types::i32>(textures.elements), textures.data);
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
STATICINLINE void ActiveAttribGet(glhnd const& program, libc_types::u32 index, libc_types::i32 bufSize, libc_types::i32 * length, libc_types::i32 * size, GLenum * type, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetActiveAttrib)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveAttrib(C_OCAST<libc_types::u32>(program), index, bufSize, length, size, type, name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ActiveUnifGet(glhnd const& program, libc_types::u32 index, libc_types::i32 bufSize, libc_types::i32 * length, libc_types::i32 * size, GLenum * type, GLchar * name)
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
    glGetActiveUniform(C_OCAST<libc_types::u32>(program), index, bufSize, length, size, type, name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void AttachedShadersGet(glhnd const& program, libc_types::i32 maxCount, libc_types::i32 * count, libc_types::u32 * shaders)
{
#ifndef NDEBUG
    if(!glGetAttachedShaders)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetAttachedShaders(C_OCAST<libc_types::u32>(program), maxCount, count, shaders);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE libc_types::i32 AttribGetLocation(glhnd const& program, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetAttribLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::i32)glGetAttribLocation(C_OCAST<libc_types::u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void BoolGetv(GLenum pname, libc_types::u8 * data)
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
STATICINLINE void BufGetParameteriv(BufferComp::buf_flag const& target, GLenum pname, libc_types::i32 * params)
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
    return (GLenum)glGetError();
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void ScalarfGetv(GLenum pname, libc_types::f32 * data)
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
STATICINLINE void FBGetAttachmentParameteriv(FramebufferT target, GLenum attachment, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void IntegerGetv(GLenum pname, libc_types::i32 * data)
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
STATICINLINE void ProgramGetInfoLog(glhnd const& program, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * infoLog)
{
#ifndef NDEBUG
    if(!glGetProgramInfoLog)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramInfoLog(C_OCAST<libc_types::u32>(program), bufSize, length, infoLog);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ProgramGetiv(glhnd const& program, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramiv(C_OCAST<libc_types::u32>(program), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE void RBufGetParameteriv(GLenum target, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void ShaderGetInfoLog(glhnd const& shader, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * infoLog)
{
#ifndef NDEBUG
    if(!glGetShaderInfoLog)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderInfoLog(C_OCAST<libc_types::u32>(shader), bufSize, length, infoLog);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void ShaderGetPrecisionFormat(ShaderStage shadertype, GLenum precisiontype, libc_types::i32 * range, libc_types::i32 * precision)
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
STATICINLINE void ShaderGetSource(glhnd const& shader, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * source)
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
    glGetShaderSource(C_OCAST<libc_types::u32>(shader), bufSize, length, source);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void ShaderGetiv(glhnd const& shader, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetShaderiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetShaderiv(C_OCAST<libc_types::u32>(shader), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE libc_types::cstring  GetString(GLenum name)
{
#ifndef NDEBUG
    if(!glGetString)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::cstring )glGetString(name);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexGetParameterfv(TexComp::tex_flag const& target, GLenum pname, libc_types::f32 * params)
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
STATICINLINE void TexGetParameteriv(TexComp::tex_flag const& target, GLenum pname, libc_types::i32 * params)
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
STATICINLINE libc_types::i32 UnifGetLocation(glhnd const& program, const GLchar * name)
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
    return (libc_types::i32)glGetUniformLocation(C_OCAST<libc_types::u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void UnifGetfv(glhnd const& program, libc_types::i32 location, libc_types::f32 * params)
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
    glGetUniformfv(C_OCAST<libc_types::u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void UnifGetiv(glhnd const& program, libc_types::i32 location, libc_types::i32 * params)
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
    glGetUniformiv(C_OCAST<libc_types::u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAOGetPointerv(libc_types::u32 index, GLenum pname, void * * pointer)
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
STATICINLINE void VAOGetfv(libc_types::u32 index, GLenum pname, libc_types::f32 * params)
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
STATICINLINE void VAOGetiv(libc_types::u32 index, GLenum pname, libc_types::i32 * params)
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
STATICINLINE libc_types::u8 IsBuffer(glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glIsBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsBuffer(C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE libc_types::u8 IsEnabled(Feature cap)
{
#ifndef NDEBUG
    if(!glIsEnabled)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsEnabled(to_enum(cap));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE libc_types::u8 IsFramebuffer(glhnd const& framebuffer)
{
#ifndef NDEBUG
    if(!glIsFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsFramebuffer(C_OCAST<libc_types::u32>(framebuffer));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE libc_types::u8 IsProgram(glhnd const& program)
{
#ifndef NDEBUG
    if(!glIsProgram)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsProgram(C_OCAST<libc_types::u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x200)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_20)
STATICINLINE libc_types::u8 IsRenderbuffer(glhnd const& renderbuffer)
{
#ifndef NDEBUG
    if(!glIsRenderbuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsRenderbuffer(C_OCAST<libc_types::u32>(renderbuffer));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE libc_types::u8 IsShader(glhnd const& shader)
{
#ifndef NDEBUG
    if(!glIsShader)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsShader(C_OCAST<libc_types::u32>(shader));
}
#endif


#if GL_VERSION_VERIFY(0x110, 0x200)
GL_VERSION_REQ_COMBO(GLVER_11, GLESVER_20)
STATICINLINE libc_types::u8 IsTexture(glhnd const& texture)
{
#ifndef NDEBUG
    if(!glIsTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsTexture(C_OCAST<libc_types::u32>(texture));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void LineWidth(libc_types::f32 width)
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
    glLinkProgram(C_OCAST<libc_types::u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void PixelStorei(GLenum pname, libc_types::i32 param)
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
STATICINLINE void PolygonOffset(libc_types::f32 factor, libc_types::f32 units)
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
STATICINLINE void ReadPixels(typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, void * pixels)
{
#ifndef NDEBUG
    if(!glReadPixels)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReadPixels(x[0], x[1], width.w, width.h, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
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
STATICINLINE void RBufStorage(GLenum target, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glRenderbufferStorage)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glRenderbufferStorage(target, to_enum(internalformat), width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x200)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_20)
STATICINLINE void SampleCoverage(libc_types::f32 value, libc_types::u8 invert)
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
    glSampleCoverage(value, invert);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Scissor(typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glScissor)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glScissor(x[0], x[1], width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x200)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_20)
STATICINLINE void ShaderBinary(libc_types::i32 count, const libc_types::u32 * shaders, GLenum binaryformat, const void * binary, libc_types::i32 length)
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
STATICINLINE void ShaderSource(glhnd const& shader, libc_types::i32 count, const GLchar * *const string, const libc_types::i32 * length)
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
    glShaderSource(C_OCAST<libc_types::u32>(shader), count, string, length);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void StencilFunc(GLenum func, libc_types::i32 ref, libc_types::u32 mask)
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
STATICINLINE void StencilFuncSeparate(GLenum face, GLenum func, libc_types::i32 ref, libc_types::u32 mask)
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
STATICINLINE void StencilMask(libc_types::u32 mask)
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
STATICINLINE void StencilMaskSeparate(GLenum face, libc_types::u32 mask)
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
STATICINLINE void TexImage2D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage2D(texture_to_enum(target), level, to_enum(internalformat), width.w, width.h, 0, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void TexParameterf(TexComp::tex_flag const& target, GLenum pname, libc_types::f32 param)
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
STATICINLINE void TexParameterfv(TexComp::tex_flag const& target, GLenum pname, const libc_types::f32 * params)
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
STATICINLINE void TexParameteri(TexComp::tex_flag const& target, GLenum pname, libc_types::i32 param)
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
STATICINLINE void TexParameteriv(TexComp::tex_flag const& target, GLenum pname, const libc_types::i32 * params)
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
STATICINLINE void TexSubImage2D(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_2d<libc_types::i32> const& xoffset, typing::geometry::size_2d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexSubImage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexSubImage2D(texture_to_enum(target), level, xoffset.x, xoffset.y, width.w, width.h, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif1f(libc_types::i32 location, libc_types::f32 v0)
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
STATICINLINE void Uniffv(libc_types::i32 location, semantic::Span<const libc_types::f32> const& value)
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
    glUniform1fv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif1i(libc_types::i32 location, libc_types::i32 v0)
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
STATICINLINE void Unifiv(libc_types::i32 location, semantic::Span<const libc_types::i32> const& value)
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
    glUniform1iv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif2f(libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 2> v0)
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
    glUniform2f(location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecf2> const& value)
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
    glUniform2fv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif2i(libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 2> v0)
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
    glUniform2i(location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(libc_types::i32 location, semantic::Span<typing::vector_types::Veci2> const& value)
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
    glUniform2iv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif3f(libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 3> v0, libc_types::f32 v2)
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
    glUniform3f(location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecf3> const& value)
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
    glUniform3fv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif3i(libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 3> v0, libc_types::i32 v2)
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
    glUniform3i(location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(libc_types::i32 location, semantic::Span<typing::vector_types::Veci3> const& value)
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
    glUniform3iv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif4f(libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 4> v0, libc_types::f32 v2)
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
    glUniform4f(location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecf4> const& value)
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
    glUniform4fv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unif4i(libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 4> v0, libc_types::i32 v2)
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
    glUniform4i(location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Unifiv(libc_types::i32 location, semantic::Span<typing::vector_types::Veci4> const& value)
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
    glUniform4iv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf2> const& value)
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
    glUniformMatrix2fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf3> const& value)
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
    glUniformMatrix3fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf4> const& value)
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
    glUniformMatrix4fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
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
    glUseProgram(C_OCAST<libc_types::u32>(program));
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
    glValidateProgram(C_OCAST<libc_types::u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO1f(libc_types::u32 index, libc_types::f32 x)
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
STATICINLINE void VAO1fv(libc_types::u32 index, const libc_types::f32 * v)
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
STATICINLINE void VAO2f(libc_types::u32 index, typing::vectors::tvector<libc_types::f32, 2> x)
{
#ifndef NDEBUG
    if(!glVertexAttrib2f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib2f(index, x[0], x[1]);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO2fv(libc_types::u32 index, const libc_types::f32 * v)
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
STATICINLINE void VAO3f(libc_types::u32 index, typing::vectors::tvector<libc_types::f32, 3> x, libc_types::f32 z)
{
#ifndef NDEBUG
    if(!glVertexAttrib3f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib3f(index, x[0], x[1], z);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO3fv(libc_types::u32 index, const libc_types::f32 * v)
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
STATICINLINE void VAO4f(libc_types::u32 index, typing::vectors::tvector<libc_types::f32, 4> x, libc_types::f32 z)
{
#ifndef NDEBUG
    if(!glVertexAttrib4f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttrib4f(index, x[0], x[1], z, x[3]);
}
#endif


#if GL_VERSION_VERIFY(0x200, 0x200)
GL_VERSION_REQ_COMBO(GLVER_20, GLESVER_20)
STATICINLINE void VAO4fv(libc_types::u32 index, const libc_types::f32 * v)
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
STATICINLINE void VAOPointer(libc_types::u32 index, libc_types::i32 size, GLenum type, libc_types::u8 normalized, libc_types::i32 stride, const void * pointer)
{
#ifndef NDEBUG
    if(!glVertexAttribPointer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x200)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_20)
STATICINLINE void Viewport(typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glViewport)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glViewport(x[0], x[1], width.w, width.h);
}
#endif

};
template<typename ReqVer>
struct v30es
    : v20es<ReqVer>
{
using Parent = v20es<ReqVer>;
using Parent::Uniffv;

#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryBegin(QueryComp::query_flag const& target, glhnd const& id)
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
    glBeginQuery(query_to_enum(target), C_OCAST<libc_types::u32>(id));
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
STATICINLINE void BufBindBase(BufferComp::buf_flag const& target, libc_types::u32 index, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glBindBufferBase)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindBufferBase(buffer_to_enum(target), index, C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufBindRange(BufferComp::buf_flag const& target, libc_types::u32 index, glhnd const& buffer, libc_types::ptroff offset, libc_types::ptroff size)
{
#ifndef NDEBUG
    if(!glBindBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindBufferRange(buffer_to_enum(target), index, C_OCAST<libc_types::u32>(buffer), offset, size);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerBind(libc_types::u32 unit, glhnd const& sampler)
{
#ifndef NDEBUG
    if(!glBindSampler)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindSampler(unit, C_OCAST<libc_types::u32>(sampler));
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFBind(GLenum target, glhnd const& id)
{
#ifndef NDEBUG
    if(!glBindTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindTransformFeedback(target, C_OCAST<libc_types::u32>(id));
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
    glBindVertexArray(C_OCAST<libc_types::u32>(array));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void FBBlit(libc_types::i32 srcX0, libc_types::i32 srcY0, libc_types::i32 srcX1, libc_types::i32 srcY1, libc_types::i32 dstX0, libc_types::i32 dstY0, libc_types::i32 dstX1, libc_types::i32 dstY1, libc_types::u32 mask, GLenum filter)
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
STATICINLINE void BufClearfi(GLenum buffer, libc_types::i32 drawbuffer, libc_types::f32 depth, libc_types::i32 stencil)
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
STATICINLINE void BufClearfv(GLenum buffer, libc_types::i32 drawbuffer, semantic::Span<const libc_types::f32> const& value)
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
STATICINLINE void BufCleariv(GLenum buffer, libc_types::i32 drawbuffer, semantic::Span<const libc_types::i32> const& value)
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
STATICINLINE void BufClearuiv(GLenum buffer, libc_types::i32 drawbuffer, semantic::Span<const libc_types::u32> const& value)
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
STATICINLINE GLenum SyncClientWait(GLsync sync, semantic::RSCA flags, libc_types::u64 timeout)
{
#ifndef NDEBUG
    if(!glClientWaitSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (GLenum)glClientWaitSync(sync, to_enum2(flags), timeout);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x300)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_30)
STATICINLINE void TexCompressedImage3D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::CompFmt internalformat, typing::geometry::size_3d<libc_types::i32> const& width, libc_types::i32 imageSize, const void * data)
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
    glCompressedTexImage3D(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), width.width, width.height, width.depth, 0, imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x130, 0x300)
GL_VERSION_REQ_COMBO(GLVER_13, GLESVER_30)
STATICINLINE void TexCompressedSubImage3D(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::geometry::size_3d<libc_types::i32> const& width, typing::pixels::CompFmt format, libc_types::i32 imageSize, const void * data)
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
    glCompressedTexSubImage3D(texture_to_enum(target), level, xoffset.x, xoffset.y, xoffset.z, width.width, width.height, width.depth, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void BufCopySubData(GLenum readTarget, GLenum writeTarget, libc_types::ptroff readOffset, libc_types::ptroff writeOffset, libc_types::ptroff size)
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
STATICINLINE void TexCopySubImage3D(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glCopyTexSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCopyTexSubImage3D(texture_to_enum(target), level, xoffset.x, xoffset.y, xoffset.z, x[0], x[1], width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryFree(semantic::Span<const libc_types::u32>&& ids)
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
    glDeleteQueries(C_FCAST<libc_types::i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerFree(semantic::Span<const libc_types::u32>&& samplers)
{
#ifndef NDEBUG
    if(!glDeleteSamplers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteSamplers(C_FCAST<libc_types::i32>(samplers.elements), samplers.data);
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
STATICINLINE void XFFree(semantic::Span<const libc_types::u32>&& ids)
{
#ifndef NDEBUG
    if(!glDeleteTransformFeedbacks)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteTransformFeedbacks(C_FCAST<libc_types::i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOFree(semantic::Span<const libc_types::u32>&& arrays)
{
#ifndef NDEBUG
    if(!glDeleteVertexArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteVertexArrays(C_FCAST<libc_types::i32>(arrays.elements), arrays.data);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void DrawArraysInstanced(DrwMd const& mode, libc_types::i32 first, libc_types::i32 count, libc_types::i32 instancecount)
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
STATICINLINE void DrawBuffers(libc_types::i32 n, const GLenum * bufs)
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
STATICINLINE void DrawElementsInstanced(DrwMd const& mode, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 instancecount)
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
STATICINLINE void DrawRangeElements(DrwMd const& mode, libc_types::u32 start, libc_types::u32 end, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices)
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
STATICINLINE GLsync SyncFence(GLenum condition, semantic::RSCA flags)
{
#ifndef NDEBUG
    if(!glFenceSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (GLsync)glFenceSync(condition, to_enum2(flags));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void BufFlushMappedRange(BufferComp::buf_flag const& target, libc_types::ptroff offset, libc_types::ptroff length)
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
STATICINLINE void FBTextureLayer(FramebufferT target, GLenum attachment, glhnd const& texture, libc_types::i32 level, libc_types::i32 layer)
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
    glFramebufferTextureLayer(to_enum(target), attachment, C_OCAST<libc_types::u32>(texture), level, layer);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryAlloc(semantic::Span<libc_types::u32>&& ids)
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
    glGenQueries(C_FCAST<libc_types::i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerAlloc(semantic::Span<libc_types::u32>&& samplers)
{
#ifndef NDEBUG
    if(!glGenSamplers)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenSamplers(C_FCAST<libc_types::i32>(samplers.elements), samplers.data);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE void XFAlloc(semantic::Span<libc_types::u32>&& ids)
{
#ifndef NDEBUG
    if(!glGenTransformFeedbacks)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenTransformFeedbacks(C_FCAST<libc_types::i32>(ids.elements), ids.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOAlloc(semantic::Span<libc_types::u32>&& arrays)
{
#ifndef NDEBUG
    if(!glGenVertexArrays)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenVertexArrays(C_FCAST<libc_types::i32>(arrays.elements), arrays.data);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void ActiveUnifBlockGetName(glhnd const& program, libc_types::u32 uniformBlockIndex, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * uniformBlockName)
{
#ifndef NDEBUG
    if(!glGetActiveUniformBlockName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformBlockName(C_OCAST<libc_types::u32>(program), uniformBlockIndex, bufSize, length, uniformBlockName);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void ActiveUnifBlockGetiv(glhnd const& program, libc_types::u32 uniformBlockIndex, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetActiveUniformBlockiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformBlockiv(C_OCAST<libc_types::u32>(program), uniformBlockIndex, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void ActiveUnifGetsiv(glhnd const& program, libc_types::i32 uniformCount, const libc_types::u32 * uniformIndices, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetActiveUniformsiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetActiveUniformsiv(C_OCAST<libc_types::u32>(program), uniformCount, uniformIndices, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void BufGetParameteri64v(BufferComp::buf_flag const& target, GLenum pname, libc_types::i64 * params)
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
STATICINLINE libc_types::i32 GetFragDataLocation(glhnd const& program, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetFragDataLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::i32)glGetFragDataLocation(C_OCAST<libc_types::u32>(program), name);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void IntegerGet64i_v(GLenum target, libc_types::u32 index, libc_types::i64 * data)
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
STATICINLINE void IntegerGet64v(GLenum pname, libc_types::i64 * data)
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
STATICINLINE void IntegerGeti_v(GLenum target, libc_types::u32 index, libc_types::i32 * data)
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
STATICINLINE void GetInternalformativ(TexComp::tex_flag const& target, typing::pixels::PixFmt internalformat, GLenum pname, libc_types::i32 count, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetInternalformativ)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetInternalformativ(texture_to_enum(target), to_enum(internalformat), pname, count, params);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x300)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_30)
STATICINLINE void ProgramGetBinary(glhnd const& program, libc_types::i32 * length, GLenum * binaryFormat, semantic::Bytes const& binary)
{
#ifndef NDEBUG
    if(!glGetProgramBinary)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramBinary(C_OCAST<libc_types::u32>(program), C_FCAST<libc_types::i32>(binary.size), length, binaryFormat, binary.data);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryGetObjectuiv(glhnd const& id, GLenum pname, libc_types::u32 * params)
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
    glGetQueryObjectuiv(C_OCAST<libc_types::u32>(id), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE void QueryGetiv(QueryComp::query_flag const& target, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void SamplerGetParameterfv(glhnd const& sampler, GLenum pname, libc_types::f32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameterfv(C_OCAST<libc_types::u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerGetParameteriv(glhnd const& sampler, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameteriv(C_OCAST<libc_types::u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE libc_types::cstring  GetStringi(GLenum name, libc_types::u32 index)
{
#ifndef NDEBUG
    if(!glGetStringi)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::cstring )glGetStringi(name, index);
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE void SyncGetiv(GLsync sync, GLenum pname, libc_types::i32 count, libc_types::i32 * length, libc_types::i32 * values)
{
#ifndef NDEBUG
    if(!glGetSynciv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSynciv(sync, pname, count, length, values);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void XFGetVarying(glhnd const& program, libc_types::u32 index, libc_types::i32 bufSize, libc_types::i32 * length, libc_types::i32 * size, GLenum * type, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetTransformFeedbackVarying)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetTransformFeedbackVarying(C_OCAST<libc_types::u32>(program), index, bufSize, length, size, type, name);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE libc_types::u32 UnifBlockGetIndex(glhnd const& program, const GLchar * uniformBlockName)
{
#ifndef NDEBUG
    if(!glGetUniformBlockIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u32)glGetUniformBlockIndex(C_OCAST<libc_types::u32>(program), uniformBlockName);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void UnifGetIndices(glhnd const& program, libc_types::i32 uniformCount, const GLchar * *const uniformNames, libc_types::u32 * uniformIndices)
{
#ifndef NDEBUG
    if(!glGetUniformIndices)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformIndices(C_OCAST<libc_types::u32>(program), uniformCount, uniformNames, uniformIndices);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void UnifGetuiv(glhnd const& program, libc_types::i32 location, libc_types::u32 * params)
{
#ifndef NDEBUG
    if(!glGetUniformuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformuiv(C_OCAST<libc_types::u32>(program), location, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOGetIiv(libc_types::u32 index, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void VAOGetIuiv(libc_types::u32 index, GLenum pname, libc_types::u32 * params)
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
STATICINLINE void FBInvalidate(FramebufferT target, libc_types::i32 numAttachments, const GLenum * attachments)
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
STATICINLINE void FBInvalidateSub(FramebufferT target, libc_types::i32 numAttachments, const GLenum * attachments, typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glInvalidateSubFramebuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glInvalidateSubFramebuffer(to_enum(target), numAttachments, attachments, x[0], x[1], width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE libc_types::u8 IsQuery(glhnd const& id)
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
    return (libc_types::u8)glIsQuery(C_OCAST<libc_types::u32>(id));
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE libc_types::u8 IsSampler(glhnd const& sampler)
{
#ifndef NDEBUG
    if(!glIsSampler)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsSampler(C_OCAST<libc_types::u32>(sampler));
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x300)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_30)
STATICINLINE libc_types::u8 IsSync(GLsync sync)
{
#ifndef NDEBUG
    if(!glIsSync)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsSync(sync);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x300)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_30)
STATICINLINE libc_types::u8 IsTransformFeedback(glhnd const& id)
{
#ifndef NDEBUG
    if(!glIsTransformFeedback)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsTransformFeedback(C_OCAST<libc_types::u32>(id));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE libc_types::u8 IsVertexArray(glhnd const& array)
{
#ifndef NDEBUG
    if(!glIsVertexArray)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsVertexArray(C_OCAST<libc_types::u32>(array));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void * BufMapRange(BufferComp::buf_flag const& target, libc_types::ptroff offset, libc_types::ptroff length, semantic::RSCA access)
{
#ifndef NDEBUG
    if(!glMapBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (void *)glMapBufferRange(buffer_to_enum(target), offset, length, to_enum2(access));
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
STATICINLINE void ProgramBinary(glhnd const& program, GLenum binaryFormat, const void * binary, libc_types::i32 length)
{
#ifndef NDEBUG
    if(!glProgramBinary)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramBinary(C_OCAST<libc_types::u32>(program), binaryFormat, binary, length);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x300)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_30)
STATICINLINE void ProgramParameteri(glhnd const& program, GLenum pname, libc_types::i32 value)
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
    glProgramParameteri(C_OCAST<libc_types::u32>(program), pname, value);
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
STATICINLINE void RBufStorageMultisample(GLenum target, libc_types::i32 samples, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glRenderbufferStorageMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glRenderbufferStorageMultisample(target, samples, to_enum(internalformat), width.w, width.h);
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
STATICINLINE void SamplerParameterf(glhnd const& sampler, GLenum pname, libc_types::f32 param)
{
#ifndef NDEBUG
    if(!glSamplerParameterf)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterf(C_OCAST<libc_types::u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameterfv(glhnd const& sampler, GLenum pname, const libc_types::f32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameterfv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterfv(C_OCAST<libc_types::u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameteri(glhnd const& sampler, GLenum pname, libc_types::i32 param)
{
#ifndef NDEBUG
    if(!glSamplerParameteri)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameteri(C_OCAST<libc_types::u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void SamplerParameteriv(glhnd const& sampler, GLenum pname, const libc_types::i32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameteriv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameteriv(C_OCAST<libc_types::u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void TexImage3D(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::PixFmt internalformat, typing::geometry::size_3d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexImage3D(texture_to_enum(target), level, to_enum(internalformat), width.width, width.height, width.depth, 0, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x300)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_30)
STATICINLINE void TexStorage2D(TexComp::tex_flag const& target, libc_types::i32 levels, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glTexStorage2D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage2D(texture_to_enum(target), levels, to_enum(internalformat), width.w, width.h);
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x300)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_30)
STATICINLINE void TexStorage3D(TexComp::tex_flag const& target, libc_types::i32 levels, typing::pixels::PixFmt internalformat, typing::geometry::size_3d<libc_types::i32> const& width)
{
#ifndef NDEBUG
    if(!glTexStorage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage3D(texture_to_enum(target), levels, to_enum(internalformat), width.width, width.height, width.depth);
}
#endif


#if GL_VERSION_VERIFY(0x120, 0x300)
GL_VERSION_REQ_COMBO(GLVER_12, GLESVER_30)
STATICINLINE void TexSubImage3D(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::geometry::size_3d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, const void * pixels)
{
#ifndef NDEBUG
    if(!glTexSubImage3D)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexSubImage3D(texture_to_enum(target), level, xoffset.x, xoffset.y, xoffset.z, width.width, width.height, width.depth, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), pixels);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void XFVaryings(glhnd const& program, libc_types::i32 count, const GLchar * *const varyings, GLenum bufferMode)
{
#ifndef NDEBUG
    if(!glTransformFeedbackVaryings)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTransformFeedbackVaryings(C_OCAST<libc_types::u32>(program), count, varyings, bufferMode);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif1ui(libc_types::i32 location, libc_types::u32 v0)
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
STATICINLINE void Unifuiv(libc_types::i32 location, semantic::Span<const libc_types::u32> const& value)
{
#ifndef NDEBUG
    if(!glUniform1uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1uiv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif2ui(libc_types::i32 location, typing::vectors::tvector<libc_types::u32, 2> v0)
{
#ifndef NDEBUG
    if(!glUniform2ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2ui(location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecui2> const& value)
{
#ifndef NDEBUG
    if(!glUniform2uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2uiv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif3ui(libc_types::i32 location, typing::vectors::tvector<libc_types::u32, 3> v0, libc_types::u32 v2)
{
#ifndef NDEBUG
    if(!glUniform3ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3ui(location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecui3> const& value)
{
#ifndef NDEBUG
    if(!glUniform3uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3uiv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unif4ui(libc_types::i32 location, typing::vectors::tvector<libc_types::u32, 4> v0, libc_types::u32 v2)
{
#ifndef NDEBUG
    if(!glUniform4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4ui(location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void Unifuiv(libc_types::i32 location, semantic::Span<typing::vector_types::Vecui4> const& value)
{
#ifndef NDEBUG
    if(!glUniform4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4uiv(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x300)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_30)
STATICINLINE void UnifBlockBinding(glhnd const& program, libc_types::u32 uniformBlockIndex, libc_types::u32 uniformBlockBinding)
{
#ifndef NDEBUG
    if(!glUniformBlockBinding)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformBlockBinding(C_OCAST<libc_types::u32>(program), uniformBlockIndex, uniformBlockBinding);
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf2_3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2x3fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf2_4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2x4fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf3_2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3x2fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf3_4> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3x4fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf4_2> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4x2fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x210, 0x300)
GL_VERSION_REQ_COMBO(GLVER_21, GLESVER_30)
STATICINLINE void Uniffv(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf4_3> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4x3fv(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x150, 0x300)
GL_VERSION_REQ_COMBO(GLVER_15, GLESVER_30)
STATICINLINE libc_types::u8 BufUnmap(BufferComp::buf_flag const& target)
{
#ifndef NDEBUG
    if(!glUnmapBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glUnmapBuffer(buffer_to_enum(target));
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x300)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_30)
STATICINLINE void VAODivisor(libc_types::u32 index, libc_types::u32 divisor)
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
STATICINLINE void VAOI4i(libc_types::u32 index, typing::vectors::tvector<libc_types::i32, 4> x, libc_types::i32 z)
{
#ifndef NDEBUG
    if(!glVertexAttribI4i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4i(index, x[0], x[1], z, x[3]);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOI4iv(libc_types::u32 index, const libc_types::i32 * v)
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
STATICINLINE void VAOI4ui(libc_types::u32 index, typing::vectors::tvector<libc_types::u32, 4> x, libc_types::u32 z)
{
#ifndef NDEBUG
    if(!glVertexAttribI4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribI4ui(index, x[0], x[1], z, x[3]);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x300)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_30)
STATICINLINE void VAOI4uiv(libc_types::u32 index, const libc_types::u32 * v)
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
STATICINLINE void VAOIPointer(libc_types::u32 index, libc_types::i32 size, GLenum type, libc_types::i32 stride, const void * pointer)
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
STATICINLINE void SyncWait(GLsync sync, semantic::RSCA flags, libc_types::u64 timeout)
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
struct v31es
    : v30es<ReqVer>
{
using Parent = v30es<ReqVer>;
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
    glActiveShaderProgram(C_OCAST<libc_types::u32>(pipeline), C_OCAST<libc_types::u32>(program));
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x310)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_31)
STATICINLINE void TexBindImage(libc_types::u32 unit, glhnd const& texture, libc_types::i32 level, libc_types::u8 layered, libc_types::i32 layer, semantic::RSCA access, typing::pixels::PixFmt format)
{
#ifndef NDEBUG
    if(!glBindImageTexture)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindImageTexture(unit, C_OCAST<libc_types::u32>(texture), level, layered, layer, to_enum2(access), to_enum(format));
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
    glBindProgramPipeline(C_OCAST<libc_types::u32>(pipeline));
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VertBufBind(libc_types::u32 bindingindex, glhnd const& buffer, libc_types::ptroff offset, libc_types::i32 stride)
{
#ifndef NDEBUG
    if(!glBindVertexBuffer)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBindVertexBuffer(bindingindex, C_OCAST<libc_types::u32>(buffer), offset, stride);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE libc_types::u32 ShaderProgramvAllocEx(ShaderStage type, semantic::Span<const GLchar *>&& strings)
{
#ifndef NDEBUG
    if(!glCreateShaderProgramv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u32)glCreateShaderProgramv(to_enum1(type), C_FCAST<libc_types::i32>(strings.elements), strings.data);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineFree(semantic::Span<const libc_types::u32>&& pipelines)
{
#ifndef NDEBUG
    if(!glDeleteProgramPipelines)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDeleteProgramPipelines(C_FCAST<libc_types::i32>(pipelines.elements), pipelines.data);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void DispatchCompute(libc_types::u32 num_groups_x, libc_types::u32 num_groups_y, libc_types::u32 num_groups_z)
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
STATICINLINE void DispatchComputeIndirect(libc_types::ptroff indirect)
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
STATICINLINE void DrawArraysIndirect(DrwMd const& mode, libc_types::uintptr indirect)
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
STATICINLINE void DrawElementsIndirect(DrwMd const& mode, TypeEnum type, libc_types::uintptr indirect)
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
STATICINLINE void FBParameteri(FramebufferT target, GLenum pname, libc_types::i32 param)
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
STATICINLINE void PipelineAlloc(semantic::Span<libc_types::u32>&& pipelines)
{
#ifndef NDEBUG
    if(!glGenProgramPipelines)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGenProgramPipelines(C_FCAST<libc_types::i32>(pipelines.elements), pipelines.data);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x310)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_31)
STATICINLINE void BoolGeti_v(BufferComp::buf_flag const& target, libc_types::u32 index, libc_types::u8 * data)
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
STATICINLINE void FBGetParameteriv(FramebufferT target, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void GetMultisamplefv(GLenum pname, libc_types::u32 index, libc_types::f32 * val)
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
STATICINLINE void ProgramGetInterfaceiv(glhnd const& program, GLenum programInterface, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramInterfaceiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramInterfaceiv(C_OCAST<libc_types::u32>(program), programInterface, pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineGetInfoLog(glhnd const& pipeline, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * infoLog)
{
#ifndef NDEBUG
    if(!glGetProgramPipelineInfoLog)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramPipelineInfoLog(C_OCAST<libc_types::u32>(pipeline), bufSize, length, infoLog);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void PipelineGetiv(glhnd const& pipeline, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramPipelineiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramPipelineiv(C_OCAST<libc_types::u32>(pipeline), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE libc_types::u32 ProgramGetResourceIndex(glhnd const& program, GLenum programInterface, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceIndex)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u32)glGetProgramResourceIndex(C_OCAST<libc_types::u32>(program), programInterface, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE libc_types::i32 ProgramGetResourceLocation(glhnd const& program, GLenum programInterface, const GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceLocation)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::i32)glGetProgramResourceLocation(C_OCAST<libc_types::u32>(program), programInterface, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void ProgramGetResourceName(glhnd const& program, GLenum programInterface, libc_types::u32 index, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * name)
{
#ifndef NDEBUG
    if(!glGetProgramResourceName)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramResourceName(C_OCAST<libc_types::u32>(program), programInterface, index, bufSize, length, name);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void ProgramGetResourceiv(glhnd const& program, GLenum programInterface, libc_types::u32 index, libc_types::i32 propCount, const GLenum * props, libc_types::i32 count, libc_types::i32 * length, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetProgramResourceiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetProgramResourceiv(C_OCAST<libc_types::u32>(program), programInterface, index, propCount, props, count, length, params);
}
#endif


#if GL_VERSION_VERIFY(0x100, 0x310)
GL_VERSION_REQ_COMBO(GLVER_10, GLESVER_31)
STATICINLINE void TexGetLevelParameterfv(TexComp::tex_flag const& target, libc_types::i32 level, GLenum pname, libc_types::f32 * params)
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
STATICINLINE void TexGetLevelParameteriv(TexComp::tex_flag const& target, libc_types::i32 level, GLenum pname, libc_types::i32 * params)
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
STATICINLINE libc_types::u8 IsProgramPipeline(glhnd const& pipeline)
{
#ifndef NDEBUG
    if(!glIsProgramPipeline)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsProgramPipeline(C_OCAST<libc_types::u32>(pipeline));
}
#endif


#if GL_VERSION_VERIFY(0x420, 0x310)
GL_VERSION_REQ_COMBO(GLVER_42, GLESVER_31)
STATICINLINE void MemoryBarrier(libc_types::u32 barriers)
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
STATICINLINE void MemoryBarrierByRegion(libc_types::u32 barriers)
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
STATICINLINE void Unif1f(glhnd const& program, libc_types::i32 location, libc_types::f32 v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1f(C_OCAST<libc_types::u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::f32> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif1i(glhnd const& program, libc_types::i32 location, libc_types::i32 v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1i(C_OCAST<libc_types::u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::i32> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1iv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif1ui(glhnd const& program, libc_types::i32 location, libc_types::u32 v0)
{
#ifndef NDEBUG
    if(!glProgramUniform1ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1ui(C_OCAST<libc_types::u32>(program), location, v0);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::u32> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1uiv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif2f(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 2> v0)
{
#ifndef NDEBUG
    if(!glProgramUniform2f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2f(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecf2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif2i(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 2> v0)
{
#ifndef NDEBUG
    if(!glProgramUniform2i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2i(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Veci2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2iv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif2ui(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::u32, 2> v0)
{
#ifndef NDEBUG
    if(!glProgramUniform2ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2ui(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1]);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecui2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2uiv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif3f(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 3> v0, libc_types::f32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3f(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecf3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif3i(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 3> v0, libc_types::i32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3i(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Veci3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3iv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif3ui(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::u32, 3> v0, libc_types::u32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform3ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3ui(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecui3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3uiv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif4f(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 4> v0, libc_types::f32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform4f)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4f(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecf4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif4i(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 4> v0, libc_types::i32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform4i)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4i(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifiv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Veci4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4iv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4iv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unif4ui(glhnd const& program, libc_types::i32 location, typing::vectors::tvector<libc_types::u32, 4> v0, libc_types::u32 v2)
{
#ifndef NDEBUG
    if(!glProgramUniform4ui)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4ui(C_OCAST<libc_types::u32>(program), location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Unifuiv(glhnd const& program, libc_types::i32 location, semantic::Span<typing::vector_types::Vecui4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4uiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4uiv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf2_3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2x3fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf2_4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix2x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix2x4fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf3_2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3x2fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf3_4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix3x4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix3x4fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf4> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf4_2> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4x2fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4x2fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x410, 0x310)
GL_VERSION_REQ_COMBO(GLVER_41, GLESVER_31)
STATICINLINE void Uniffv(glhnd const& program, libc_types::i32 location, libc_types::u8 transpose, semantic::Span<typing::vector_types::Matf4_3> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniformMatrix4x3fv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformMatrix4x3fv(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if GL_VERSION_VERIFY(0x320, 0x310)
GL_VERSION_REQ_COMBO(GLVER_32, GLESVER_31)
STATICINLINE void SampleMaski(libc_types::u32 maskNumber, libc_types::u32 mask)
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
STATICINLINE void TexStorage2DMultisample(TexComp::tex_flag const& target, libc_types::i32 samples, typing::pixels::PixFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width, libc_types::u8 fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTexStorage2DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage2DMultisample(texture_to_enum(target), samples, to_enum(internalformat), width.w, width.h, fixedsamplelocations);
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
    glUseProgramStages(C_OCAST<libc_types::u32>(pipeline), to_enum2(stages), C_OCAST<libc_types::u32>(program));
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
    glValidateProgramPipeline(C_OCAST<libc_types::u32>(pipeline));
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VAOBinding(libc_types::u32 attribindex, libc_types::u32 bindingindex)
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
STATICINLINE void VAOFormat(libc_types::u32 attribindex, libc_types::i32 size, GLenum type, libc_types::u8 normalized, libc_types::u32 relativeoffset)
{
#ifndef NDEBUG
    if(!glVertexAttribFormat)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glVertexAttribFormat(attribindex, size, type, normalized, relativeoffset);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x310)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_31)
STATICINLINE void VAOIFormat(libc_types::u32 attribindex, libc_types::i32 size, GLenum type, libc_types::u32 relativeoffset)
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
STATICINLINE void VAOBindingDivisor(libc_types::u32 bindingindex, libc_types::u32 divisor)
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
struct v32es
    : v31es<ReqVer>
{
using Parent = v31es<ReqVer>;

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
STATICINLINE void BlendEquationSeparatei(libc_types::u32 buf, GLenum modeRGB, GLenum modeAlpha)
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
STATICINLINE void BlendEquationi(libc_types::u32 buf, GLenum mode)
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
STATICINLINE void BlendFuncSeparatei(libc_types::u32 buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
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
STATICINLINE void BlendFunci(libc_types::u32 buf, GLenum src, GLenum dst)
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
STATICINLINE void ColorMaski(libc_types::u32 index, libc_types::u8 r, libc_types::u8 g, libc_types::u8 b, libc_types::u8 a)
{
#ifndef NDEBUG
    if(!glColorMaski)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glColorMaski(index, r, g, b, a);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void CopyImageSubData(libc_types::u32 srcName, GLenum srcTarget, libc_types::i32 srcLevel, libc_types::i32 srcX, libc_types::i32 srcY, libc_types::i32 srcZ, libc_types::u32 dstName, GLenum dstTarget, libc_types::i32 dstLevel, libc_types::i32 dstX, libc_types::i32 dstY, libc_types::i32 dstZ, libc_types::i32 srcWidth, libc_types::i32 srcHeight, libc_types::i32 srcDepth)
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
STATICINLINE void DebugMessageControl(GLenum source, GLenum type, GLenum severity, semantic::Span<const libc_types::u32>&& ids, libc_types::u8 enabled)
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
    glDebugMessageControl(source, type, severity, C_FCAST<libc_types::i32>(ids.elements), ids.data, enabled);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void DebugMessageInsert(GLenum source, GLenum type, glhnd const& id, GLenum severity, libc_types::i32 length, const GLchar * buf)
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
    glDebugMessageInsert(source, type, C_OCAST<libc_types::u32>(id), severity, length, buf);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void Disablei(Feature target, libc_types::u32 index)
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
STATICINLINE void DrawElementsBaseVertex(DrwMd const& mode, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 basevertex)
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
STATICINLINE void DrawElementsInstancedBaseVertex(DrwMd const& mode, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 instancecount, libc_types::i32 basevertex)
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
STATICINLINE void DrawRangeElementsBaseVertex(DrwMd const& mode, libc_types::u32 start, libc_types::u32 end, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 basevertex)
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
STATICINLINE void Enablei(Feature target, libc_types::u32 index)
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
STATICINLINE void FBTexture(FramebufferT target, GLenum attachment, glhnd const& texture, libc_types::i32 level)
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
    glFramebufferTexture(to_enum(target), attachment, C_OCAST<libc_types::u32>(texture), level);
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE libc_types::u32 GetDebugMessageLog(libc_types::u32 count, libc_types::i32 bufSize, GLenum * sources, GLenum * types, libc_types::u32 * ids, GLenum * severities, libc_types::i32 * lengths, GLchar * messageLog)
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
    return (libc_types::u32)glGetDebugMessageLog(count, bufSize, sources, types, ids, severities, lengths, messageLog);
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
    return (GLenum)glGetGraphicsResetStatus();
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void GetObjectLabel(GLenum identifier, libc_types::u32 name, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * label)
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
STATICINLINE void GetObjectPtrLabel(const void * ptr, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * label)
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
STATICINLINE void SamplerGetParameterIiv(glhnd const& sampler, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameterIiv(C_OCAST<libc_types::u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerGetParameterIuiv(glhnd const& sampler, GLenum pname, libc_types::u32 * params)
{
#ifndef NDEBUG
    if(!glGetSamplerParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetSamplerParameterIuiv(C_OCAST<libc_types::u32>(sampler), pname, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void TexGetParameterIiv(TexComp::tex_flag const& target, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void TexGetParameterIuiv(TexComp::tex_flag const& target, GLenum pname, libc_types::u32 * params)
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
STATICINLINE void UnifGetnfv(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::f32 * params)
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
    glGetnUniformfv(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void UnifGetniv(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::i32 * params)
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
    glGetnUniformiv(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void UnifGetnuiv(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::u32 * params)
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
    glGetnUniformuiv(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE libc_types::u8 IsEnabledi(Feature target, libc_types::u32 index)
{
#ifndef NDEBUG
    if(!glIsEnabledi)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsEnabledi(to_enum(target), index);
}
#endif


#if GL_VERSION_VERIFY(0x400, 0x320)
GL_VERSION_REQ_COMBO(GLVER_40, GLESVER_32)
STATICINLINE void MinSampleShading(libc_types::f32 value)
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
STATICINLINE void ObjectLabel(GLenum identifier, libc_types::u32 name, libc_types::i32 length, const GLchar * label)
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
STATICINLINE void ObjectPtrLabel(const void * ptr, libc_types::i32 length, const GLchar * label)
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
STATICINLINE void PatchParameteri(PatchProperty pname, libc_types::i32 value)
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
STATICINLINE void PrimitiveBoundingBox(libc_types::f32 minX, libc_types::f32 minY, libc_types::f32 minZ, libc_types::f32 minW, libc_types::f32 maxX, libc_types::f32 maxY, libc_types::f32 maxZ, libc_types::f32 maxW)
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
STATICINLINE void PushDebugGroup(GLenum source, glhnd const& id, libc_types::i32 length, const GLchar * message)
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
    glPushDebugGroup(source, C_OCAST<libc_types::u32>(id), length, message);
}
#endif


#if GL_VERSION_VERIFY(0x450, 0x320)
GL_VERSION_REQ_COMBO(GLVER_45, GLESVER_32)
STATICINLINE void ReadnPixels(typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 bufSize, void * data)
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
    glReadnPixels(x[0], x[1], width.w, width.h, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), bufSize, data);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerParameterIiv(glhnd const& sampler, GLenum pname, const libc_types::i32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameterIiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterIiv(C_OCAST<libc_types::u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x330, 0x320)
GL_VERSION_REQ_COMBO(GLVER_33, GLESVER_32)
STATICINLINE void SamplerParameterIuiv(glhnd const& sampler, GLenum pname, const libc_types::u32 * param)
{
#ifndef NDEBUG
    if(!glSamplerParameterIuiv)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSamplerParameterIuiv(C_OCAST<libc_types::u32>(sampler), pname, param);
}
#endif


#if GL_VERSION_VERIFY(0x310, 0x320)
GL_VERSION_REQ_COMBO(GLVER_31, GLESVER_32)
STATICINLINE void TexBuffer(TexComp::tex_flag const& target, typing::pixels::PixFmt internalformat, glhnd const& buffer)
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
    glTexBuffer(texture_to_enum(target), to_enum(internalformat), C_OCAST<libc_types::u32>(buffer));
}
#endif


#if GL_VERSION_VERIFY(0x430, 0x320)
GL_VERSION_REQ_COMBO(GLVER_43, GLESVER_32)
STATICINLINE void TexBufferRange(TexComp::tex_flag const& target, typing::pixels::PixFmt internalformat, glhnd const& buffer, libc_types::ptroff offset, libc_types::ptroff size)
{
#ifndef NDEBUG
    if(!glTexBufferRange)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexBufferRange(texture_to_enum(target), to_enum(internalformat), C_OCAST<libc_types::u32>(buffer), offset, size);
}
#endif


#if GL_VERSION_VERIFY(0x300, 0x320)
GL_VERSION_REQ_COMBO(GLVER_30, GLESVER_32)
STATICINLINE void TexParameterIiv(TexComp::tex_flag const& target, GLenum pname, const libc_types::i32 * params)
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
STATICINLINE void TexParameterIuiv(TexComp::tex_flag const& target, GLenum pname, const libc_types::u32 * params)
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
STATICINLINE void TexStorage3DMultisample(TexComp::tex_flag const& target, libc_types::i32 samples, typing::pixels::PixFmt internalformat, typing::geometry::size_3d<libc_types::i32> const& width, libc_types::u8 fixedsamplelocations)
{
#ifndef NDEBUG
    if(!glTexStorage3DMultisample)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexStorage3DMultisample(texture_to_enum(target), samples, to_enum(internalformat), width.width, width.height, width.depth, fixedsamplelocations);
}
#endif

};

namespace ext {
#if defined(GL_ARB_ES3_2_compatibility) && GL_ARB_ES3_2_compatibility
template<typename ReqVer>
struct ARB_ES3_2_compatibility
{

#if 0
/* No template check available */
STATICINLINE void PrimitiveBoundingBoxARB(libc_types::f32 minX, libc_types::f32 minY, libc_types::f32 minZ, libc_types::f32 minW, libc_types::f32 maxX, libc_types::f32 maxY, libc_types::f32 maxZ, libc_types::f32 maxW)
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
struct ARB_bindless_texture
{

#if 0
/* No template check available */
STATICINLINE libc_types::u64 GetImageHandleARB(glhnd const& texture, libc_types::i32 level, libc_types::u8 layered, libc_types::i32 layer, GLenum format)
{
#ifndef NDEBUG
    if(!glGetImageHandleARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u64)glGetImageHandleARB(C_OCAST<libc_types::u32>(texture), level, layered, layer, format);
}
#endif


#if 0
/* No template check available */
STATICINLINE libc_types::u64 TexGetHandleARB(glhnd const& texture)
{
#ifndef NDEBUG
    if(!glGetTextureHandleARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u64)glGetTextureHandleARB(C_OCAST<libc_types::u32>(texture));
}
#endif


#if 0
/* No template check available */
STATICINLINE libc_types::u64 TexGetSamplerHandleARB(glhnd const& texture, glhnd const& sampler)
{
#ifndef NDEBUG
    if(!glGetTextureSamplerHandleARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u64)glGetTextureSamplerHandleARB(C_OCAST<libc_types::u32>(texture), C_OCAST<libc_types::u32>(sampler));
}
#endif


#if 0
/* No template check available */
STATICINLINE void VAOGetLui64vARB(libc_types::u32 index, GLenum pname, libc_types::u64 * params)
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
STATICINLINE libc_types::u8 IsImageHandleResidentARB(libc_types::u64 handle)
{
#ifndef NDEBUG
    if(!glIsImageHandleResidentARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsImageHandleResidentARB(handle);
}
#endif


#if 0
/* No template check available */
STATICINLINE libc_types::u8 IsTextureHandleResidentARB(libc_types::u64 handle)
{
#ifndef NDEBUG
    if(!glIsTextureHandleResidentARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsTextureHandleResidentARB(handle);
}
#endif


#if 0
/* No template check available */
STATICINLINE void MakeImageHandleNonResidentARB(libc_types::u64 handle)
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
STATICINLINE void MakeImageHandleResidentARB(libc_types::u64 handle, semantic::RSCA access)
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
STATICINLINE void TexMakeHandleNonResidentARB(libc_types::u64 handle)
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
STATICINLINE void TexMakeHandleResidentARB(libc_types::u64 handle)
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
STATICINLINE void UnifHandleui64vARB(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::u64> const& values)
{
#ifndef NDEBUG
    if(!glProgramUniformHandleui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniformHandleui64vARB(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(values.elements), C_RCAST<const libc_types::u64*>(values.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifHandleui64vARB(libc_types::i32 location, semantic::Span<const libc_types::u64> const& value)
{
#ifndef NDEBUG
    if(!glUniformHandleui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformHandleui64vARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void VAOL1ui64ARB(libc_types::u32 index, libc_types::u64 x)
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
STATICINLINE void VAOL1ui64vARB(libc_types::u32 index, const libc_types::u64 * v)
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
struct ARB_compute_variable_group_size
{

#if 0
/* No template check available */
STATICINLINE void DispatchComputeGroupSizeARB(libc_types::u32 num_groups_x, libc_types::u32 num_groups_y, libc_types::u32 num_groups_z, libc_types::u32 group_size_x, libc_types::u32 group_size_y, libc_types::u32 group_size_z)
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
struct ARB_debug_output
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
STATICINLINE void DebugMessageControlARB(GLenum source, GLenum type, GLenum severity, semantic::Span<const libc_types::u32>&& ids, libc_types::u8 enabled)
{
#ifndef NDEBUG
    if(!glDebugMessageControlARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageControlARB(source, type, severity, C_FCAST<libc_types::i32>(ids.elements), ids.data, enabled);
}
#endif


#if 0
/* No template check available */
STATICINLINE void DebugMessageInsertARB(GLenum source, GLenum type, glhnd const& id, GLenum severity, libc_types::i32 length, const GLchar * buf)
{
#ifndef NDEBUG
    if(!glDebugMessageInsertARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageInsertARB(source, type, C_OCAST<libc_types::u32>(id), severity, length, buf);
}
#endif


#if 0
/* No template check available */
STATICINLINE libc_types::u32 GetDebugMessageLogARB(libc_types::u32 count, libc_types::i32 bufSize, GLenum * sources, GLenum * types, libc_types::u32 * ids, GLenum * severities, libc_types::i32 * lengths, GLchar * messageLog)
{
#ifndef NDEBUG
    if(!glGetDebugMessageLogARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u32)glGetDebugMessageLogARB(count, bufSize, sources, types, ids, severities, lengths, messageLog);
}
#endif

};
#endif // GL_ARB_debug_output

#if defined(GL_ARB_draw_buffers) && GL_ARB_draw_buffers
template<typename ReqVer>
struct ARB_draw_buffers
{

#if 0
/* No template check available */
STATICINLINE void DrawBuffersARB(libc_types::i32 n, const GLenum * bufs)
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
struct ARB_draw_buffers_blend
{

#if 0
/* No template check available */
STATICINLINE void BlendEquationSeparateiARB(libc_types::u32 buf, GLenum modeRGB, GLenum modeAlpha)
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
STATICINLINE void BlendEquationiARB(libc_types::u32 buf, GLenum mode)
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
STATICINLINE void BlendFuncSeparateiARB(libc_types::u32 buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
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
STATICINLINE void BlendFunciARB(libc_types::u32 buf, GLenum src, GLenum dst)
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
struct ARB_draw_instanced
{

#if 0
/* No template check available */
STATICINLINE void DrawArraysInstancedARB(DrwMd const& mode, libc_types::i32 first, libc_types::i32 count, libc_types::i32 primcount)
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
STATICINLINE void DrawElementsInstancedARB(DrwMd const& mode, libc_types::i32 count, TypeEnum type, libc_types::uintptr indices, libc_types::i32 primcount)
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
struct ARB_geometry_shader4
{

#if 0
/* No template check available */
STATICINLINE void FBTextureARB(FramebufferT target, GLenum attachment, glhnd const& texture, libc_types::i32 level)
{
#ifndef NDEBUG
    if(!glFramebufferTextureARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTextureARB(to_enum(target), attachment, C_OCAST<libc_types::u32>(texture), level);
}
#endif


#if 0
/* No template check available */
STATICINLINE void FBTextureFaceARB(FramebufferT target, GLenum attachment, glhnd const& texture, libc_types::i32 level, TexComp::tex_flag const& face)
{
#ifndef NDEBUG
    if(!glFramebufferTextureFaceARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTextureFaceARB(to_enum(target), attachment, C_OCAST<libc_types::u32>(texture), level, texture_to_enum(face));
}
#endif


#if 0
/* No template check available */
STATICINLINE void FBTextureLayerARB(FramebufferT target, GLenum attachment, glhnd const& texture, libc_types::i32 level, libc_types::i32 layer)
{
#ifndef NDEBUG
    if(!glFramebufferTextureLayerARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glFramebufferTextureLayerARB(to_enum(target), attachment, C_OCAST<libc_types::u32>(texture), level, layer);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ProgramParameteriARB(glhnd const& program, GLenum pname, libc_types::i32 value)
{
#ifndef NDEBUG
    if(!glProgramParameteriARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramParameteriARB(C_OCAST<libc_types::u32>(program), pname, value);
}
#endif

};
#endif // GL_ARB_geometry_shader4

#if defined(GL_ARB_gl_spirv) && GL_ARB_gl_spirv
template<typename ReqVer>
struct ARB_gl_spirv
{

#if 0
/* No template check available */
STATICINLINE void ShaderSpecializeARB(glhnd const& shader, const GLchar * pEntryPoint, libc_types::u32 numSpecializationConstants, const libc_types::u32 * pConstantIndex, const libc_types::u32 * pConstantValue)
{
#ifndef NDEBUG
    if(!glSpecializeShaderARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSpecializeShaderARB(C_OCAST<libc_types::u32>(shader), pEntryPoint, numSpecializationConstants, pConstantIndex, pConstantValue);
}
#endif

};
#endif // GL_ARB_gl_spirv

#if defined(GL_ARB_gpu_shader_int64) && GL_ARB_gpu_shader_int64
template<typename ReqVer>
struct ARB_gpu_shader_int64
{

#if 0
/* No template check available */
STATICINLINE void UnifGeti64vARB(glhnd const& program, libc_types::i32 location, libc_types::i64 * params)
{
#ifndef NDEBUG
    if(!glGetUniformi64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformi64vARB(C_OCAST<libc_types::u32>(program), location, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetui64vARB(glhnd const& program, libc_types::i32 location, libc_types::u64 * params)
{
#ifndef NDEBUG
    if(!glGetUniformui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetUniformui64vARB(C_OCAST<libc_types::u32>(program), location, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetni64vARB(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::i64 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformi64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformi64vARB(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnui64vARB(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::u64 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformui64vARB(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1i64vARB(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::i64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1i64vARB(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1ui64vARB(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::u64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform1ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform1ui64vARB(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2i64vARB(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::i64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2i64vARB(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2ui64vARB(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::u64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform2ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform2ui64vARB(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3i64vARB(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::i64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3i64vARB(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3ui64vARB(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::u64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform3ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform3ui64vARB(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4i64vARB(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::i64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4i64vARB(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4ui64vARB(glhnd const& program, libc_types::i32 location, semantic::Span<const libc_types::u64> const& value)
{
#ifndef NDEBUG
    if(!glProgramUniform4ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glProgramUniform4ui64vARB(C_OCAST<libc_types::u32>(program), location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1i64vARB(libc_types::i32 location, semantic::Span<const libc_types::i64> const& value)
{
#ifndef NDEBUG
    if(!glUniform1i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1i64vARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1ui64vARB(libc_types::i32 location, semantic::Span<const libc_types::u64> const& value)
{
#ifndef NDEBUG
    if(!glUniform1ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1ui64vARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2i64vARB(libc_types::i32 location, semantic::Span<const libc_types::i64> const& value)
{
#ifndef NDEBUG
    if(!glUniform2i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2i64vARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2ui64vARB(libc_types::i32 location, semantic::Span<const libc_types::u64> const& value)
{
#ifndef NDEBUG
    if(!glUniform2ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2ui64vARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3i64vARB(libc_types::i32 location, semantic::Span<const libc_types::i64> const& value)
{
#ifndef NDEBUG
    if(!glUniform3i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3i64vARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3ui64vARB(libc_types::i32 location, semantic::Span<const libc_types::u64> const& value)
{
#ifndef NDEBUG
    if(!glUniform3ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3ui64vARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4i64vARB(libc_types::i32 location, semantic::Span<const libc_types::i64> const& value)
{
#ifndef NDEBUG
    if(!glUniform4i64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4i64vARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i64*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4ui64vARB(libc_types::i32 location, semantic::Span<const libc_types::u64> const& value)
{
#ifndef NDEBUG
    if(!glUniform4ui64vARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4ui64vARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::u64*>(value.data));
}
#endif

};
#endif // GL_ARB_gpu_shader_int64

#if defined(GL_ARB_indirect_parameters) && GL_ARB_indirect_parameters
template<typename ReqVer>
struct ARB_indirect_parameters
{

#if 0
/* No template check available */
STATICINLINE void MultiDrawArraysIndirectCountARB(DrwMd const& mode, libc_types::uintptr indirect, libc_types::ptroff drawcount, libc_types::i32 maxdrawcount, libc_types::i32 stride)
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
STATICINLINE void MultiDrawElementsIndirectCountARB(DrwMd const& mode, TypeEnum type, libc_types::uintptr indirect, libc_types::ptroff drawcount, libc_types::i32 maxdrawcount, libc_types::i32 stride)
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
struct ARB_instanced_arrays
{

#if 0
/* No template check available */
STATICINLINE void VAODivisorARB(libc_types::u32 index, libc_types::u32 divisor)
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
struct ARB_multisample
{

#if 0
/* No template check available */
STATICINLINE void SampleCoverageARB(libc_types::f32 value, libc_types::u8 invert)
{
#ifndef NDEBUG
    if(!glSampleCoverageARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glSampleCoverageARB(value, invert);
}
#endif

};
#endif // GL_ARB_multisample

#if defined(GL_ARB_occlusion_query) && GL_ARB_occlusion_query
template<typename ReqVer>
struct ARB_occlusion_query
{

#if 0
/* No template check available */
STATICINLINE void QueryBeginARB(GLenum target, glhnd const& id)
{
#ifndef NDEBUG
    if(!glBeginQueryARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glBeginQueryARB(target, C_OCAST<libc_types::u32>(id));
}
#endif


#if 0
/* No template check available */
STATICINLINE void QueryDeleteARB(libc_types::i32 n, const libc_types::u32 * ids)
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
STATICINLINE void QueryGenARB(libc_types::i32 n, libc_types::u32 * ids)
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
STATICINLINE void QueryGetObjectivARB(glhnd const& id, GLenum pname, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetQueryObjectivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryObjectivARB(C_OCAST<libc_types::u32>(id), pname, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void QueryGetObjectuivARB(glhnd const& id, GLenum pname, libc_types::u32 * params)
{
#ifndef NDEBUG
    if(!glGetQueryObjectuivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetQueryObjectuivARB(C_OCAST<libc_types::u32>(id), pname, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void QueryGetivARB(QueryComp::query_flag const& target, GLenum pname, libc_types::i32 * params)
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
STATICINLINE libc_types::u8 IsQueryARB(glhnd const& id)
{
#ifndef NDEBUG
    if(!glIsQueryARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsQueryARB(C_OCAST<libc_types::u32>(id));
}
#endif

};
#endif // GL_ARB_occlusion_query

#if defined(GL_ARB_parallel_shader_compile) && GL_ARB_parallel_shader_compile
template<typename ReqVer>
struct ARB_parallel_shader_compile
{

#if 0
/* No template check available */
STATICINLINE void ShaderMaxCompilerThreadsARB(libc_types::u32 count)
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
struct ARB_point_parameters
{

#if 0
/* No template check available */
STATICINLINE void PointParameterfARB(GLenum pname, libc_types::f32 param)
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
STATICINLINE void PointParameterfvARB(GLenum pname, const libc_types::f32 * params)
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
struct ARB_robustness
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
    return (GLenum)glGetGraphicsResetStatusARB();
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnColorTableARB(GLenum target, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 bufSize, void * table)
{
#ifndef NDEBUG
    if(!glGetnColorTableARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnColorTableARB(target, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), bufSize, table);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexGetnCompressedImageARB(TexComp::tex_flag const& target, libc_types::i32 lod, libc_types::i32 bufSize, void * img)
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
STATICINLINE void GetnConvolutionFilterARB(GLenum target, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 bufSize, void * image)
{
#ifndef NDEBUG
    if(!glGetnConvolutionFilterARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnConvolutionFilterARB(target, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), bufSize, image);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnHistogramARB(GLenum target, libc_types::u8 reset, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 bufSize, void * values)
{
#ifndef NDEBUG
    if(!glGetnHistogramARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnHistogramARB(target, reset, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), bufSize, values);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnMapdvARB(GLenum target, GLenum query, libc_types::i32 bufSize, libc_types::f64 * v)
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
STATICINLINE void GetnMapfvARB(GLenum target, GLenum query, libc_types::i32 bufSize, libc_types::f32 * v)
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
STATICINLINE void GetnMapivARB(GLenum target, GLenum query, libc_types::i32 bufSize, libc_types::i32 * v)
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
STATICINLINE void GetnMinmaxARB(GLenum target, libc_types::u8 reset, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 bufSize, void * values)
{
#ifndef NDEBUG
    if(!glGetnMinmaxARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnMinmaxARB(target, reset, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), bufSize, values);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetnPixelMapfvARB(GLenum map, libc_types::i32 bufSize, libc_types::f32 * values)
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
STATICINLINE void GetnPixelMapuivARB(GLenum map, libc_types::i32 bufSize, libc_types::u32 * values)
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
STATICINLINE void GetnPixelMapusvARB(GLenum map, libc_types::i32 bufSize, libc_types::u16 * values)
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
STATICINLINE void GetnPolygonStippleARB(libc_types::i32 bufSize, libc_types::u8 * pattern)
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
STATICINLINE void GetnSeparableFilterARB(GLenum target, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 rowBufSize, void * row, libc_types::i32 columnBufSize, void * column, void * span)
{
#ifndef NDEBUG
    if(!glGetnSeparableFilterARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnSeparableFilterARB(target, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), rowBufSize, row, columnBufSize, column, span);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexGetnImageARB(TexComp::tex_flag const& target, libc_types::i32 level, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 bufSize, void * img)
{
#ifndef NDEBUG
    if(!glGetnTexImageARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnTexImageARB(texture_to_enum(target), level, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), bufSize, img);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetndvARB(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::f64 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformdvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformdvARB(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnfvARB(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::f32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformfvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformfvARB(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnivARB(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformivARB(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnuivARB(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::u32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformuivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformuivARB(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ReadnPixelsARB(typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 bufSize, void * data)
{
#ifndef NDEBUG
    if(!glReadnPixelsARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReadnPixelsARB(x[0], x[1], width.w, width.h, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), bufSize, data);
}
#endif

};
#endif // GL_ARB_robustness

#if defined(GL_ARB_shader_objects) && GL_ARB_shader_objects
template<typename ReqVer>
struct ARB_shader_objects
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
    return (GLhandleARB)glCreateProgramObjectARB();
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
    return (GLhandleARB)glCreateShaderObjectARB(to_enum1(shaderType));
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
STATICINLINE void ActiveUnifGetARB(GLhandleARB programObj, libc_types::u32 index, libc_types::i32 maxLength, libc_types::i32 * length, libc_types::i32 * size, GLenum * type, GLcharARB * name)
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
STATICINLINE void GetAttachedObjectsARB(GLhandleARB containerObj, libc_types::i32 maxCount, libc_types::i32 * count, GLhandleARB * obj)
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
    return (GLhandleARB)glGetHandleARB(pname);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetInfoLogARB(GLhandleARB obj, libc_types::i32 maxLength, libc_types::i32 * length, GLcharARB * infoLog)
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
STATICINLINE void GetObjectParameterfvARB(GLhandleARB obj, GLenum pname, libc_types::f32 * params)
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
STATICINLINE void GetObjectParameterivARB(GLhandleARB obj, GLenum pname, libc_types::i32 * params)
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
STATICINLINE void ShaderGetSourceARB(GLhandleARB obj, libc_types::i32 maxLength, libc_types::i32 * length, GLcharARB * source)
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
STATICINLINE libc_types::i32 UnifGetLocationARB(GLhandleARB programObj, const GLcharARB * name)
{
#ifndef NDEBUG
    if(!glGetUniformLocationARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::i32)glGetUniformLocationARB(programObj, name);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetfvARB(GLhandleARB programObj, libc_types::i32 location, libc_types::f32 * params)
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
STATICINLINE void UnifGetivARB(GLhandleARB programObj, libc_types::i32 location, libc_types::i32 * params)
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
STATICINLINE void ShaderSourceARB(GLhandleARB shaderObj, libc_types::i32 count, const GLcharARB * * string, const libc_types::i32 * length)
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
STATICINLINE void Unif1fARB(libc_types::i32 location, libc_types::f32 v0)
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
STATICINLINE void Unif1fvARB(libc_types::i32 location, semantic::Span<const libc_types::f32> const& value)
{
#ifndef NDEBUG
    if(!glUniform1fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1fvARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif1iARB(libc_types::i32 location, libc_types::i32 v0)
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
STATICINLINE void Unif1ivARB(libc_types::i32 location, semantic::Span<const libc_types::i32> const& value)
{
#ifndef NDEBUG
    if(!glUniform1ivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform1ivARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2fARB(libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 2> v0)
{
#ifndef NDEBUG
    if(!glUniform2fARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2fARB(location, v0[0], v0[1]);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2fvARB(libc_types::i32 location, semantic::Span<const libc_types::f32> const& value)
{
#ifndef NDEBUG
    if(!glUniform2fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2fvARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2iARB(libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 2> v0)
{
#ifndef NDEBUG
    if(!glUniform2iARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2iARB(location, v0[0], v0[1]);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif2ivARB(libc_types::i32 location, semantic::Span<const libc_types::i32> const& value)
{
#ifndef NDEBUG
    if(!glUniform2ivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform2ivARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3fARB(libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 3> v0, libc_types::f32 v2)
{
#ifndef NDEBUG
    if(!glUniform3fARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3fARB(location, v0[0], v0[1], v2);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3fvARB(libc_types::i32 location, semantic::Span<const libc_types::f32> const& value)
{
#ifndef NDEBUG
    if(!glUniform3fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3fvARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3iARB(libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 3> v0, libc_types::i32 v2)
{
#ifndef NDEBUG
    if(!glUniform3iARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3iARB(location, v0[0], v0[1], v2);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif3ivARB(libc_types::i32 location, semantic::Span<const libc_types::i32> const& value)
{
#ifndef NDEBUG
    if(!glUniform3ivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform3ivARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4fARB(libc_types::i32 location, typing::vectors::tvector<libc_types::f32, 4> v0, libc_types::f32 v2)
{
#ifndef NDEBUG
    if(!glUniform4fARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4fARB(location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4fvARB(libc_types::i32 location, semantic::Span<const libc_types::f32> const& value)
{
#ifndef NDEBUG
    if(!glUniform4fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4fvARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4iARB(libc_types::i32 location, typing::vectors::tvector<libc_types::i32, 4> v0, libc_types::i32 v2)
{
#ifndef NDEBUG
    if(!glUniform4iARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4iARB(location, v0[0], v0[1], v2, v0[3]);
}
#endif


#if 0
/* No template check available */
STATICINLINE void Unif4ivARB(libc_types::i32 location, semantic::Span<const libc_types::i32> const& value)
{
#ifndef NDEBUG
    if(!glUniform4ivARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniform4ivARB(location, C_FCAST<libc_types::i32>(value.elements), C_RCAST<const libc_types::i32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifMatrix2fvARB(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<const libc_types::f32> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix2fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix2fvARB(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifMatrix3fvARB(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<const libc_types::f32> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix3fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix3fvARB(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifMatrix4fvARB(libc_types::i32 location, libc_types::u8 transpose, semantic::Span<const libc_types::f32> const& value)
{
#ifndef NDEBUG
    if(!glUniformMatrix4fvARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glUniformMatrix4fvARB(location, C_FCAST<libc_types::i32>(value.elements), transpose, C_RCAST<const libc_types::f32*>(value.data));
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
struct ARB_shading_language_include
{

#if 0
/* No template check available */
STATICINLINE void ShaderCompileIncludeARB(glhnd const& shader, libc_types::i32 count, const GLchar * *const path, const libc_types::i32 * length)
{
#ifndef NDEBUG
    if(!glCompileShaderIncludeARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompileShaderIncludeARB(C_OCAST<libc_types::u32>(shader), count, path, length);
}
#endif


#if 0
/* No template check available */
STATICINLINE void DeleteNamedStringARB(libc_types::i32 namelen, const GLchar * name)
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
STATICINLINE void GetNamedStringARB(libc_types::i32 namelen, const GLchar * name, libc_types::i32 bufSize, libc_types::i32 * stringlen, GLchar * string)
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
STATICINLINE void GetNamedStringivARB(libc_types::i32 namelen, const GLchar * name, GLenum pname, libc_types::i32 * params)
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
STATICINLINE libc_types::u8 IsNamedStringARB(libc_types::i32 namelen, const GLchar * name)
{
#ifndef NDEBUG
    if(!glIsNamedStringARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u8)glIsNamedStringARB(namelen, name);
}
#endif


#if 0
/* No template check available */
STATICINLINE void NamedStringARB(GLenum type, libc_types::i32 namelen, const GLchar * name, libc_types::i32 stringlen, const GLchar * string)
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
struct ARB_sparse_texture
{

#if 0
/* No template check available */
STATICINLINE void TexPageCommitmentARB(GLenum target, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::geometry::size_3d<libc_types::i32> const& width, libc_types::u8 commit)
{
#ifndef NDEBUG
    if(!glTexPageCommitmentARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexPageCommitmentARB(target, level, xoffset.x, xoffset.y, xoffset.z, width.width, width.height, width.depth, commit);
}
#endif

};
#endif // GL_ARB_sparse_texture

#if defined(GL_ARB_texture_buffer_object) && GL_ARB_texture_buffer_object
template<typename ReqVer>
struct ARB_texture_buffer_object
{

#if 0
/* No template check available */
STATICINLINE void TexBufferARB(TexComp::tex_flag const& target, typing::pixels::PixFmt internalformat, glhnd const& buffer)
{
#ifndef NDEBUG
    if(!glTexBufferARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glTexBufferARB(texture_to_enum(target), to_enum(internalformat), C_OCAST<libc_types::u32>(buffer));
}
#endif

};
#endif // GL_ARB_texture_buffer_object

#if defined(GL_ARB_texture_compression) && GL_ARB_texture_compression
template<typename ReqVer>
struct ARB_texture_compression
{

#if 0
/* No template check available */
STATICINLINE void TexCompressedImage1DARB(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::CompFmt internalformat, libc_types::i32 width, libc_types::i32 imageSize, const void * data)
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
STATICINLINE void TexCompressedImage2DARB(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::CompFmt internalformat, typing::geometry::size_2d<libc_types::i32> const& width, libc_types::i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexImage2DARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexImage2DARB(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), width.w, width.h, 0, imageSize, data);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexCompressedImage3DARB(TexComp::tex_flag const& target, libc_types::i32 level, typing::pixels::CompFmt internalformat, typing::geometry::size_3d<libc_types::i32> const& width, libc_types::i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexImage3DARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexImage3DARB(texture_to_enum(target), level, to_enum(internalformat.base_fmt, internalformat.p_flags, internalformat.c_flags), width.width, width.height, width.depth, 0, imageSize, data);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexCompressedSubImage1DARB(TexComp::tex_flag const& target, libc_types::i32 level, libc_types::i32 xoffset, libc_types::i32 width, typing::pixels::CompFmt format, libc_types::i32 imageSize, const void * data)
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
STATICINLINE void TexCompressedSubImage2DARB(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_2d<libc_types::i32> const& xoffset, typing::geometry::size_2d<libc_types::i32> const& width, typing::pixels::CompFmt format, libc_types::i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexSubImage2DARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexSubImage2DARB(texture_to_enum(target), level, xoffset.x, xoffset.y, width.w, width.h, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexCompressedSubImage3DARB(TexComp::tex_flag const& target, libc_types::i32 level, typing::geometry::point_3d<libc_types::i32> const& xoffset, typing::geometry::size_3d<libc_types::i32> const& width, typing::pixels::CompFmt format, libc_types::i32 imageSize, const void * data)
{
#ifndef NDEBUG
    if(!glCompressedTexSubImage3DARB)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glCompressedTexSubImage3DARB(texture_to_enum(target), level, xoffset.x, xoffset.y, xoffset.z, width.width, width.height, width.depth, to_enum(format.base_fmt, format.p_flags, format.c_flags), imageSize, data);
}
#endif


#if 0
/* No template check available */
STATICINLINE void TexGetCompressedImageARB(TexComp::tex_flag const& target, libc_types::i32 level, void * img)
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

#if defined(GL_ARB_viewport_array) && GL_ARB_viewport_array
template<typename ReqVer>
struct ARB_viewport_array
{

#if 0
/* No template check available */
STATICINLINE void DepthRangeArraydvNV(libc_types::u32 first, libc_types::i32 count, const libc_types::f64 * v)
{
#ifndef NDEBUG
    if(!glDepthRangeArraydvNV)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthRangeArraydvNV(first, count, v);
}
#endif


#if 0
/* No template check available */
STATICINLINE void DepthRangeIndexeddNV(libc_types::u32 index, libc_types::f64 n, libc_types::f64 f)
{
#ifndef NDEBUG
    if(!glDepthRangeIndexeddNV)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDepthRangeIndexeddNV(index, n, f);
}
#endif

};
#endif // GL_ARB_viewport_array

#if defined(GL_KHR_blend_equation_advanced) && GL_KHR_blend_equation_advanced
template<typename ReqVer>
struct KHR_blend_equation_advanced
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
struct KHR_debug
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
STATICINLINE void DebugMessageControlKHR(GLenum source, GLenum type, GLenum severity, semantic::Span<const libc_types::u32>&& ids, libc_types::u8 enabled)
{
#ifndef NDEBUG
    if(!glDebugMessageControlKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageControlKHR(source, type, severity, C_FCAST<libc_types::i32>(ids.elements), ids.data, enabled);
}
#endif


#if 0
/* No template check available */
STATICINLINE void DebugMessageInsertKHR(GLenum source, GLenum type, glhnd const& id, GLenum severity, libc_types::i32 length, const GLchar * buf)
{
#ifndef NDEBUG
    if(!glDebugMessageInsertKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glDebugMessageInsertKHR(source, type, C_OCAST<libc_types::u32>(id), severity, length, buf);
}
#endif


#if 0
/* No template check available */
STATICINLINE libc_types::u32 GetDebugMessageLogKHR(libc_types::u32 count, libc_types::i32 bufSize, GLenum * sources, GLenum * types, libc_types::u32 * ids, GLenum * severities, libc_types::i32 * lengths, GLchar * messageLog)
{
#ifndef NDEBUG
    if(!glGetDebugMessageLogKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    return (libc_types::u32)glGetDebugMessageLogKHR(count, bufSize, sources, types, ids, severities, lengths, messageLog);
}
#endif


#if 0
/* No template check available */
STATICINLINE void GetObjectLabelKHR(GLenum identifier, libc_types::u32 name, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * label)
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
STATICINLINE void GetObjectPtrLabelKHR(const void * ptr, libc_types::i32 bufSize, libc_types::i32 * length, GLchar * label)
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
STATICINLINE void ObjectLabelKHR(GLenum identifier, libc_types::u32 name, libc_types::i32 length, const GLchar * label)
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
STATICINLINE void ObjectPtrLabelKHR(const void * ptr, libc_types::i32 length, const GLchar * label)
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
STATICINLINE void PushDebugGroupKHR(GLenum source, glhnd const& id, libc_types::i32 length, const GLchar * message)
{
#ifndef NDEBUG
    if(!glPushDebugGroupKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glPushDebugGroupKHR(source, C_OCAST<libc_types::u32>(id), length, message);
}
#endif

};
#endif // GL_KHR_debug

#if defined(GL_KHR_parallel_shader_compile) && GL_KHR_parallel_shader_compile
template<typename ReqVer>
struct KHR_parallel_shader_compile
{

#if 0
/* No template check available */
STATICINLINE void ShaderMaxCompilerThreadsKHR(libc_types::u32 count)
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
struct KHR_robustness
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
    return (GLenum)glGetGraphicsResetStatusKHR();
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnfvKHR(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::f32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformfvKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformfvKHR(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnivKHR(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::i32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformivKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformivKHR(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void UnifGetnuivKHR(glhnd const& program, libc_types::i32 location, libc_types::i32 bufSize, libc_types::u32 * params)
{
#ifndef NDEBUG
    if(!glGetnUniformuivKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glGetnUniformuivKHR(C_OCAST<libc_types::u32>(program), location, bufSize, params);
}
#endif


#if 0
/* No template check available */
STATICINLINE void ReadnPixelsKHR(typing::vectors::tvector<libc_types::i32, 2> x, typing::geometry::size_2d<libc_types::i32> const& width, typing::PixCmp format, typing::pixels::BitFmt type, libc_types::i32 bufSize, void * data)
{
#ifndef NDEBUG
    if(!glReadnPixelsKHR)
    {
        Throw(undefined_behavior("function not loaded!"));
    }
#endif
    glReadnPixelsKHR(x[0], x[1], width.w, width.h, to_enum(format, typing::pixels::PixFmt::None), to_enum(type), bufSize, data);
}
#endif

};
#endif // GL_KHR_robustness


} // ext
} // glwrap

#if defined(COFFEE_GCC)
#pragma GCC diagnostic pop
#elif defined(COFFEE_CLANG)
#pragma clang diagnostic pop
#endif
