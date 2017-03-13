#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_MultiDrawIndirect
{
    /*!
     * \brief DrawMultiArraysIndirect
     * \param p Primitive type
     * \param c Primitive creation method
     * \param off Offset into indirect buffer
     * \param dc Number of indirect calls to process
     * \param s Stride of draw parameter structure
     */
    STATICINLINE void DrawMultiArraysIndirect(DrwMd const& md,uint64 off,uint32 dc,uint64 s)
    {glMultiDrawArraysIndirect(to_enum(md.t,md.c),(c_cptr)off,dc,s);}
    /*!
     * \brief DrawMultiElementsIndirect
     * \param p Primitive type
     * \param c Primitive creation method
     * \param d Element data type
     * \param off Offset into indirect buffer
     * \param dc Number of indirect calls to process
     * \param s Stride of draw parameter structure
     */
    STATICINLINE void DrawMultiElementsIndirect(
            DrwMd const& md,TypeEnum d,uint64 off,uint32 dc,uint32 s)
    {glMultiDrawElementsIndirect(to_enum(md.t,md.c),to_enum(d),(c_cptr)off,dc,s);}
};
}
}
