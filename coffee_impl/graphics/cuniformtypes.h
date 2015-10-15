#ifndef CUNIFORMTYPES
#define CUNIFORMTYPES

#include "coffee.h"
#include "cbuffer.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CUniformValue{
    enum UniformFlags{
        BlockFlag       = 0x1, //Part of a uniform block
        MatrixTranspose = 0x2,
    };

    cstring name        = nullptr;
    uint8   size        = 0;
    void*   data        = nullptr;
    uint8   flags       = 0;
    GLint   location    = -1;
};

/*!
 * \brief Corresponds to a GL uniform block. Contains the necessary information.
 */
struct CUniformBlock{
    GLuint  blockBinding    = 0; /*! The block binding, chosen by client*/
    GLuint  shaderIndex     = 0; /*! Shader index*/
    uint32  size            = 0; /*! Size of block*/
    cstring name            = nullptr;
    CSubBuffer* buffer      = nullptr; /*! Subbuffer associated with block*/
};

/*!
 * \brief Apply a uniform value to a shader program
 * \param program Program to apply to
 * \param val Value to be applied
 */
extern void coffee_shader_apply_uniform(GLuint program, const CUniformValue& val);

}
}

#endif // CUNIFORMTYPES

