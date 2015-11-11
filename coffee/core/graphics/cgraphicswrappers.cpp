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

}
}
