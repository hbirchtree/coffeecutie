#ifndef CUNIFORMTYPES
#define CUNIFORMTYPES

#include "coffee/core/coffee.h"
#include "cbuffer.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CUniformValue{
    CUniformValue();

    enum UniformFlags{
        BlockFlag       = 0x1, //Part of a uniform block
        MatrixTranspose = 0x2,
    };

    cstring name;
    uint8   size;
    void*   data;
    uint8   flags;
    GLint   location;
};

/*!
 * \brief Corresponds to a GL uniform block. Contains the necessary information.
 */
struct CUniformBlock{
    CUniformBlock();

    GLuint  blockBinding; /*! The block binding, chosen by client*/
    GLuint  shaderIndex; /*! Shader index*/
    uint32  size; /*! Size of block*/
    cstring name;
    CSubBuffer* buffer; /*! Subbuffer associated with block*/
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

