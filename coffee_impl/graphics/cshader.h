#ifndef COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
#define COFFEE_CGRAPHICSWRAPPERS_CSHADER_H

#include "cgraphicswrappers.h"

namespace Coffee {
namespace CGraphicsWrappers {

struct CShader
{
    GLuint handle = 0;

    uint16_t numAttributes      = 0;
    uint16_t numUniforms        = 0;
    uint16_t numUniformBlocks   = 0;

    bool addShader(){
        return true;
    }
};

} // namespace CGraphicsWrappers
} // namespace Coffee

#endif // COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
