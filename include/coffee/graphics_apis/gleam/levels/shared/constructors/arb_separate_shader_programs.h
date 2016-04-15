#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_SeparableShaderPrograms_Allocators
{
    STATICINLINE bool PipelineAlloc(uint32 c,CGhnd* h)
    {
        glGenProgramPipelines(c,h);
        return true;
    }
    STATICINLINE bool PipelineFree(uint32 c,CGhnd* h)
    {
        glDeleteProgramPipelines(c,h);
        return true;
    }
};

}
}
