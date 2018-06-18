#pragma once

#include "../gl_shared_include.h"
#include "../gl_shared_enum_convert.h"
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/edef/resenum.h>

namespace Coffee{
namespace CGL{

struct CGL_Drawing_43
{
#if GL_VERSION_VERIFY(0x330, 0x320)
    /*!
     * \brief DrawArraysIndirect
     * \param p Primitive type
     * \param c Primitive creation method
     * \param off Offset into buffer
     */
    STATICINLINE void DrawArraysIndirect(DrwMd c,uint64 off)
    {glDrawArraysIndirect(to_enum(c),(c_cptr)off);}

    /*!
     * \brief DrawElementsIndirect
     * \param p Primitive type
     * \param c Primitive creation method
     * \param d Element data type
     * \param off Offset into indirect buffer
     */
    STATICINLINE void DrawElementsIndirect(DrwMd c,
                                     TypeEnum d,
                                     uint64 off)
    {glDrawElementsIndirect(to_enum(c),to_enum(d),(c_cptr)off);}

    /*!
     * \brief DrawRangeElementsBaseVertex
     * \param p Primitive type
     * \param c Primitive creation method
     * \param f First element in range to draw
     * \param e Last element in range to draw
     * \param vc Number of elements to draw
     * \param d Element data type
     * \param off Offset into index buffer
     * \param bv Index of first vertex to draw
     */
    STATICINLINE void DrawRangeElementsBaseVertex(
            DrwMd c,uint32 f,uint32 e,uint32 vc,TypeEnum d,uint64 off,int32 bv)
    {glDrawRangeElementsBaseVertex(to_enum(c),f,e,vc,to_enum(d),(c_cptr)off,bv);}
#endif

    /*!
     * \brief DrawRangeElements
     * \param p Primitive type
     * \param c Primitive creation method
     * \param f First element in range to draw
     * \param e Last element in range to draw
     * \param vc Number of elements to draw
     * \param d Element data type
     * \param off Offset into index buffer
     */
    STATICINLINE void DrawRangeElements(
            DrwMd c,uint32 f,uint32 e,uint32 vc,TypeEnum d,uint64 off)
    {glDrawRangeElements(to_enum(c),f,e,vc,to_enum(d),(c_cptr)off);}

#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
    /*!
     * \brief DrawElementsInstancedBaseInstance
     * \param p Primitive type
     * \param c Primitive creation method
     * \param ec Number of elements to draw per instance
     * \param d Element data type
     * \param off Offset into element buffer
     * \param bc Number of instances to draw
     * \param bi Base instance applied to all instances for attribute fetching
     */
    STATICINLINE void DrawElementsInstancedBaseInstance(
            DrwMd c,uint32 ec,TypeEnum d,uint64 off,uint32 bc,uint32 bi)
    {glDrawElementsInstancedBaseInstance(to_enum(c),ec,to_enum(d),(c_cptr)off,bc,bi);}
    /*!
     * \brief DrawElementsInstancedBaseVertexBaseInstance
     * \param p Primitive type
     * \param c Primitive creation method
     * \param ec Number of elements to draw
     * \param d Element data type
     * \param off Offset into element buffer
     * \param ic Number of instances to draw from base instance
     * \param bv Value added to each element in the element buffer for this call
     * \param bi Index of first instance to draw
     */
    STATICINLINE void DrawElementsInstancedBaseVertexBaseInstance(
            DrwMd c,u32 ec,TypeEnum d,u64 off,u32 ic,i32 bv,u32 bi)
    {glDrawElementsInstancedBaseVertexBaseInstance(
                    to_enum(c),ec,to_enum(d),(c_cptr)off,ic,bv,bi);}
    /*!
     * \brief DrawArraysInstancedBaseInstance
     * \param p Primitive type
     * \param c Primitive creation method
     * \param vf First vertex
     * \param vc Number of vertices
     * \param ic Number of instances
     * \param bi Index of first instance to draw
     */
    STATICINLINE void DrawArraysInstancedBaseInstance(DrwMd c,int32 vf,
                                                uint32 vc,uint32 ic,uint32 bi)
    {glDrawArraysInstancedBaseInstance(to_enum(c),vf,vc,ic,bi);}
#endif

#if GL_VERSION_VERIFY(0x400, GL_VERSION_NONE)
    /*!
     * \brief DrawXF
     * \param p Primitive type
     * \param c Primitive creation method
     * \param h Transform feedback handle
     */
    STATICINLINE void DrawXF(DrwMd c,CGhnd h)
    {glDrawTransformFeedback(to_enum(c),h);}
    /*!
     * \brief DrawXFStream
     * \param p Primitive type
     * \param c Primitive creation method
     * \param h Transform feedback handle
     * \param s Stream index
     */
    STATICINLINE void DrawXFStream(DrwMd c,CGhnd h,uint32 s)
    {glDrawTransformFeedbackStream(to_enum(c),h,s);}
#endif

#if GL_VERSION_VERIFY(0x420, GL_VERSION_NONE)
    /*!
     * \brief DrawXFInstanced
     * \param p Primitive type
     * \param c Primitive creation method
     * \param h Transform feedback handle
     * \param ic Instance count
     */
    STATICINLINE void DrawXFInstanced(DrwMd c,CGhnd h,uint32 ic)
    {glDrawTransformFeedbackInstanced(to_enum(c),h,ic);}
    /*!
     * \brief DrawXFStreamInstanced
     * \param p Primitive type
     * \param c Primitive creation method
     * \param h Transform feedback handle
     * \param s Stream index
     * \param ic Instance count
     */
    STATICINLINE void DrawXFStreamInstanced(DrwMd c,CGhnd h,uint32 s,uint32 ic)
    {glDrawTransformFeedbackStreamInstanced(to_enum(c),h,s,ic);}
#endif
};
}
}
