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

    cstring name    = nullptr;
    uint8   size    = 0;
    void*   data      = nullptr;
    uint8   flags   = 0;
    GLint   location  =-1;

};

struct CUniformBlock{
    //A CBuffer is created to correspond with these objects
    //It is beneficial to create a larger buffer
    GLuint  blockBinding        = 0;
    GLuint  shaderIndex         = 0;

    uint32  size                = 0;
    cstring name                = nullptr;

    CSubBuffer* buffer          = nullptr;
};

extern void coffee_shader_apply_uniform(GLuint program, const CUniformValue& val);

}
}

#endif // CUNIFORMTYPES

