#include "ctransformcomputer.h"

namespace Coffee{
namespace CGraphicsWrappers{

void coffee_graphics_alloc(size_t count, CXFBuffer *obj)
{
    CGuint *handles = new CGuint[count];
    glGenTransformFeedbacks(count,handles);
    for(size_t i=0;i<count;i++)
    {
        obj[i].handle = handles[i];
        coffee_graphics_bind(obj[i]);
    }
    coffee_graphics_unbind(*obj);
    delete[] handles;
}

void coffee_graphics_free(size_t count, CXFBuffer *obj)
{
    CGuint *handles = new CGuint[count];
    for(size_t i=0;i<count;i++)
    {
        handles[i] = obj[i].handle;
        obj[i].handle = 0;
    }
    glDeleteTransformFeedbacks(count,handles);
    delete[] handles;
}

void coffee_graphics_bind(CXFBuffer &obj)
{
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,obj.handle);
}

void coffee_graphics_unbind(CXFBuffer &)
{
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,0);
}

void coffee_graphics_xfb_begin(const CPrimitiveMode &mode)
{
    glBeginTransformFeedback(gl_get(mode));
}

void coffee_graphics_xfb_pause()
{
    glPauseTransformFeedback();
}

void coffee_graphics_xfb_resume()
{
    glResumeTransformFeedback();
}

void coffee_graphics_xfb_end()
{
    glEndTransformFeedback();
}

void coffee_graphics_xfb_set_output(
        _cbasic_graphics_shader_program &shader,
        const CXFBufferOutput &output,
        const CAttributeMode &mode)
{
    glTransformFeedbackVaryings(
                shader.handle,
                output.numVariables,
                output.variables,
                gl_get(mode));
}

void coffee_graphics_xfb_draw(
        CXFBuffer &obj,
        const CPrimitiveMode &mode,
        const CGuint &stream,
        const CGsize &instances)
{
    glDrawTransformFeedbackStreamInstanced(gl_get(mode),
                                           obj.handle,
                                           stream,
                                           instances);
}

}
}

#include "glfunctions.h"
