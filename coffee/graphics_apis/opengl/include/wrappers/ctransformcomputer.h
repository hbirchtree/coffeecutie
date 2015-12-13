#ifndef COFFEE_GRAPHICS_APIS_OPENGL_CTRANSFORMCOMPUTER_H
#define COFFEE_GRAPHICS_APIS_OPENGL_CTRANSFORMCOMPUTER_H

#include "copengl_types.h"

namespace Coffee{
namespace CGraphicsWrappers{

/*!
 * \brief XFB object resource
 */
struct CXFBuffer : _cbasic_graphics_resource {};

/*!
 * \brief XFB variable output description
 */
struct CXFBufferOutput
{
    size_t numVariables;
    cstring *variables;
};

extern void coffee_graphics_alloc(size_t count, CXFBuffer* obj);

inline void coffee_graphics_alloc(CXFBuffer& obj)
{
    coffee_graphics_alloc(1,&obj);
}

extern void coffee_graphics_free(size_t count, CXFBuffer* obj);

inline void coffee_graphics_free(CXFBuffer& obj)
{
    coffee_graphics_free(1,&obj);
}

extern void coffee_graphics_bind(CXFBuffer& obj);
extern void coffee_graphics_unbind(CXFBuffer&);

/*!
 * \brief Draw primitives according to XFB data
 * \param obj
 * \param mode
 * \param stream
 * \param instances
 */
extern void coffee_graphics_xfb_draw(
        CXFBuffer& obj,
        const CPrimitiveMode& mode,
        const CGuint& stream,
        const CGsize& instances);

/*!
 * \brief Start capture of XFB primitives
 * \param mode
 */
extern void coffee_graphics_xfb_begin(const CPrimitiveMode& mode);
/*!
 * \brief Pause XFB capture, allow switching XFB object
 */
extern void coffee_graphics_xfb_pause();
/*!
 * \brief Resume from a pause
 */
extern void coffee_graphics_xfb_resume();
/*!
 * \brief End capture of XFB primitives
 */
extern void coffee_graphics_xfb_end();

/*!
 * \brief Set XFB capture variables from vertex shader, geometry shader or tessellation shader. This commands needs to be run *before* shader program linkage. (Optionally, you can enumerate them in the shader. See GLSL documentation for details.)
 * \param shader
 * \param output
 * \param mode
 */
extern void coffee_graphics_xfb_set_output(
        _cbasic_graphics_shader_program& shader,
        const CXFBufferOutput& output,
        const CAttributeMode& mode);

}
}

#endif
