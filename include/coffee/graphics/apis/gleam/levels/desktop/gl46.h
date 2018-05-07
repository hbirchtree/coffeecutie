#pragma once

#include "gl45.h"

namespace Coffee{
namespace CGL{

struct CGL46 : CGL45
{
    STATICINLINE bool DrawParametersSupported()
    {return Debug::CheckExtensionSupported("GL_ARB_shader_draw_parameters");}

};

}
}
