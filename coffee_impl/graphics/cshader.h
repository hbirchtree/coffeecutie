#ifndef COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
#define COFFEE_CGRAPHICSWRAPPERS_CSHADER_H

#include "cgraphicswrappers.h"
#include "coffee/cfiles.h"

namespace Coffee {
namespace CGraphicsWrappers {

struct CShaderStageProgram;
struct CShaderProgram;
struct CShader;

static bool coffee_shader_compile_checklog(GLuint handle);
static bool coffee_program_link_checklog(GLuint handle);

struct CPipeline
{
    GLuint handle = 0;
    UseProgramStageMask stages;

    bool create();
    void free();
    void attachStages(CShaderStageProgram *shader, UseProgramStageMask bits);
    void attachProgram(CShaderProgram *shader, UseProgramStageMask bits);
};

struct CShaderProgram
{
    GLuint handle = 0;
    UseProgramStageMask stages;

    void create(bool separable = true);
    void attachShader(CShader* shader, UseProgramStageMask maskopt);
    void detachShader(CShader* shader);
    void link();
    void free();

    void storeProgram(CResources::CResource* out);
    bool fetchProgram(CResources::CResource* in);
};

struct CShaderStageProgram
{
    GLuint handle = 0;
    UseProgramStageMask stage;

    bool compile(CResources::CResource* res, GLenum stage, UseProgramStageMask stageMask);
    void free();
};

struct CShader
{
    GLuint handle = 0;
    GLenum stage = GL_NONE;

    bool compile(CResources::CResource* res, GLenum stage);
    void free();
};

} // namespace CGraphicsWrappers
} // namespace Coffee

#endif // COFFEE_CGRAPHICSWRAPPERS_CSHADER_H
