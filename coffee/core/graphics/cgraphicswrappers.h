#ifndef CGRAPHICSWRAPPERS_H
#define CGRAPHICSWRAPPERS_H

#include "cvertexarrayobject.h"
#include "cuniformtypes.h"

namespace Coffee{
namespace CGraphicsWrappers{

extern void coffee_graphics_clear(CClearFlag flg);

/*!
 * \brief Contains a GL message from the binding layer
 */
struct CGLReport
{
    CGLReport();

    GLenum source; /*!< Source of message*/
    GLenum type; /*!< Type of message*/
    GLuint id; /*!< ID of message*/
    GLenum severity; /*!< Severity of message*/
    const char* message; /*!< Textual message*/
};

/*!
 * \brief GL multidraw indirect call used to put triangles on the screen
 */
struct CGLDrawCall
{
    CGLDrawCall();

    CGidx count; /*!< Number of indices*/
    CGidx instanceCount; /*!< Number of instances*/
    CGidx firstIndex; /*!< First index in buffer*/
    CGidx baseVertex; /*!< First vertex in buffer*/
    CGidx baseInstance; /*!< First instance to render*/
};

}
}

#endif // CGRAPHICSWRAPPERS_H
