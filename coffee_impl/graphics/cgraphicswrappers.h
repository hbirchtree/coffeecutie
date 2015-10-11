#ifndef CGRAPHICSWRAPPERS_H
#define CGRAPHICSWRAPPERS_H

#include "cvertexarrayobject.h"
#include "cuniformtypes.h"

namespace Coffee{
namespace CGraphicsWrappers{

/*!
 * \brief Contains a GL message
 */
struct CGLReport
{
    GLenum source; /*!< Source of message*/
    GLenum type; /*!< Type of message*/
    GLuint id; /*!< ID of message*/
    GLenum severity; /*!< Severity of message*/
    const char* message; /*!< Textual message*/
};

/*!
 * \brief GL multidraw indirect call
 */
struct CGLDrawCall
{
    GLuint count            = 0; /*!< Number of indices*/
    GLuint instanceCount    = 0; /*!< Number of instances*/
    GLuint firstIndex       = 0; /*!< First index in buffer*/
    GLuint baseVertex       = 0; /*!< First vertex in buffer*/
    GLuint baseInstance     = 0; /*!< First instance to render*/
};

}
}

#endif // CGRAPHICSWRAPPERS_H
