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
                coffee_get(mode,cpritype_map,cpritype_size),
                drawcall->count,
                coffee_get(CDataType::UInt,cdtypes_map,cdtypes_size),
                (void*)(sizeof(CGint)*drawcall->firstIndex),
                drawcall->instanceCount,
                drawcall->baseVertex,
                drawcall->baseInstance);
}

void coffee_graphics_multidraw(const CPrimitiveMode &mode, const CBuffer *buf)
{
    glMultiDrawElementsIndirect(
                coffee_get(mode,cpritype_map,cpritype_size),
                coffee_get(CDataType::UInt,cdtypes_map,cdtypes_size),
                0, buf->size/sizeof(CGLDrawCall),
                sizeof(CGLDrawCall));
}

}
}
