#ifndef COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
#define COFFEE_CGRAPHICSWRAPPERS_CSHADER_H

#include "cgraphicswrappers.h"
#include "coffee/cfiles.h"

namespace Coffee {
namespace CGraphicsWrappers {

struct CPipeline
{
    GLuint handle = 0;
    UseProgramStageMask stages;

    bool createPipeline();
    void attachShader(CShader* shader);
};

struct CShaderProgram
{
    GLuint handle = 0;

    uint16_t numAttributes      = 0;
    uint16_t numUniforms        = 0;
    uint16_t numUniformBlocks   = 0;
};

struct CShader
{
    GLuint handle = 0;
    UseProgramStageMask stage;

    uint16_t numAttributes      = 0;
    uint16_t numUniforms        = 0;
    uint16_t numUniformBlocks   = 0;

    bool compile(CResources::CResource* res, GLenum stage, UseProgramStageMask stageMask);
};

} // namespace CGraphicsWrappers
} // namespace Coffee

#endif // COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
