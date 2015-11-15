#include "cgraphicswrappers.h"
#include "glbinding.h"

namespace Coffee{
namespace CGraphicsWrappers{

CGLReport::CGLReport():
    source(GL_NONE),
    type(GL_NONE),
    id(0),
    severity(GL_NONE),
    message(nullptr)
{
}

CGLDrawCall::CGLDrawCall():
    count(0),
    instanceCount(0),
    firstIndex(0),
    baseVertex(0),
    baseInstance(0)
{
}

void coffee_graphics_clear(CClearFlag flg)
{
    auto clearflags = GL_COLOR_BUFFER_BIT;

    if(flag_eval(flg&CClearFlag::Depth))
        clearflags = clearflags|GL_DEPTH_BUFFER_BIT;
    if(flag_eval(flg&CClearFlag::Color))
        clearflags = clearflags|GL_COLOR_BUFFER_BIT;
    if(flag_eval(flg&CClearFlag::Stencil))
        clearflags = clearflags|GL_STENCIL_BUFFER_BIT;
    if(flag_eval(flg&CClearFlag::Accum))
        clearflags = clearflags|GL_ACCUM_BUFFER_BIT;

    glClear(clearflags);
}

void coffee_graphics_draw_indexed(const CPrimitiveMode &mode, const CGLDrawCall *drawcall)
{
    glDrawElementsInstancedBaseVertexBaseInstance(
                CG_GET(mode,cpritype_map),
                drawcall->count,
                CG_GET(CDataType::UInt,cdtypes_map),
                (void*)(sizeof(CGint)*drawcall->firstIndex),
                drawcall->instanceCount,
                drawcall->baseVertex,
                drawcall->baseInstance);
}

void coffee_graphics_multidraw(const CPrimitiveMode &mode, const CBuffer *buf)
{
    glMultiDrawElementsIndirect(
                CG_GET(mode,cpritype_map),
                CG_GET(CDataType::UInt,cdtypes_map),
                0, buf->size/sizeof(CGLDrawCall),
                sizeof(CGLDrawCall));
}

void coffee_graphics_depth(bool enable)
{
    if(enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void coffee_graphics_blend(bool enable,bool additive)
{
    if(enable)
    {
        glEnable(GL_BLEND);
        if(additive)
            glBlendFunc(GL_ONE,GL_ONE);
        else
            glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    }else
        glDisable(GL_BLEND);
}

void coffee_graphics_culling(bool enable, CCullMode mode)
{
    if(enable)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(CG_GET(mode,cculling_map));
    }
    else
        glDisable(GL_CULL_FACE);
}

void coffee_graphics_set_clearcolor(const CVec4 &color)
{
    glClearColor(color.r(),color.g(),color.b(),color.a());
}

void coffee_graphics_set_viewport(const CRectF &view)
{
    glViewport(view.x,view.y,view.w,view.h);
}

bool coffee_graphics_debug_filter_ignore_notifications(CGLReport *rep)
{
    if(rep->severity==GL_DEBUG_SEVERITY_NOTIFICATION)
        return false;
    return true;
}

bool coffee_graphics_debug_filter_all(CGLReport*)
{
    return true;
}

}
}
