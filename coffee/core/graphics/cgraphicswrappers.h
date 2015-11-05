#ifndef CGRAPHICSWRAPPERS_H
#define CGRAPHICSWRAPPERS_H

#include "cvertexarrayobject.h"
#include "cuniformtypes.h"

namespace Coffee{
namespace CGraphicsWrappers{

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

    GLuint count; /*!< Number of indices*/
    GLuint instanceCount; /*!< Number of instances*/
    GLuint firstIndex; /*!< First index in buffer*/
    GLuint baseVertex; /*!< First vertex in buffer*/
    GLuint baseInstance; /*!< First instance to render*/
};

}
}

#endif // CGRAPHICSWRAPPERS_H
