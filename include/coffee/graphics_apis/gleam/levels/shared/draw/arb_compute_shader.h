#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_ComputeShader
{
    /* Compute */
    /*!
     * \brief ComputeDispatch
     * \param x Number of X workgroups
     * \param y Number of Y workgroups
     * \param z Number of Z workgroups
     */
    STATICINLINE void ComputeDispatch(uint32 x,uint32 y,uint32 z){glDispatchCompute(x,y,z);}
    /*!
     * \brief ComputeDispatchIndirect
     * \param o Offset into dispatch buffer for command
     */
    STATICINLINE void ComputeDispatchIndirect(uint64 o){glDispatchComputeIndirect(o);}
};
}
}
