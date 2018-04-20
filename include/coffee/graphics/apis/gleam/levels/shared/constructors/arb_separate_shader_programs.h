#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>
#include <coffee/core/types/cdef/memtypes.h>

namespace Coffee{
namespace CGL{

struct CGL_SeparableShaderPrograms_Allocators
{
    STATICINLINE bool PipelineAlloc(Span<CGhnd> const& handles)
    {
        glGenProgramPipelines(C_FCAST<i32>(handles.elements), handles.data);
        return true;
    }
    STATICINLINE bool PipelineFree(Span<CGhnd> const& handles)
    {
        glDeleteProgramPipelines(C_FCAST<i32>(handles.elements), handles.data);
        return true;
    }
};

}
}
