#ifndef CTRANSFORMCOMPUTER
#define CTRANSFORMCOMPUTER

#include "glbinding.h"
#include "cshader.h"
#include "cvertexarrayobject.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CTransformFeedback
{
    GLenum transformType = GL_TRANSFORM_FEEDBACK;
    GLuint handle = 0;

    void create(){
        glCreateTransformFeedbacks(1,&handle);
    }
    void free(){
        glDeleteTransformFeedbacks(1,&handle);
        handle = 0;
    }

    void bind(){
        glBindTransformFeedback(transformType,handle);
    }
    void unbind(){
        glBindTransformFeedback(transformType,0);
    }
};

struct CTransformData
{
    //Used as vertex spec and transform spec, allows iteration
    uint32          numVaryings     = 0;
    cstring*        varyings        = nullptr;

    uint32*         datasizes       = nullptr;
    uint16*         sizes           = nullptr;
    GLenum*         types           = nullptr;
    uint16*         locations       = nullptr;
};

struct CTransformPair
{
    CVertexArrayObject dataDescriptor;
    CSubBuffer dataBuffer;
    CTransformFeedback transform;
};

static void coffee_setup_transform_shader(CShaderProgram* shader, CTransformData* spec)
{
    glTransformFeedbackVaryings(shader->handle,
                                spec->numVaryings,
                                spec->varyings,
                                GL_SEPARATE_ATTRIBS);
}

static CTransformPair* coffee_create_transform_vao(CTransformData* spec, szptr numElements)
{
    CTransformPair *tf =
            reinterpret_cast<CTransformPair*>(calloc(sizeof(CTransformPair),1));
    tf->dataDescriptor.create();
    tf->transform.create();

    tf->dataBuffer.bufferType = GL_TRANSFORM_FEEDBACK_BUFFER;

    uint32 i;
    szptr total = 0;
    szptr stride = 0;

    for(i=0;i<spec->numVaryings;i++)
        total+=spec->datasizes[i];

    tf->dataDescriptor.bind();
    tf->dataBuffer.bindRange();
    for(i=0;i<spec->numVaryings;i++){
        tf->dataDescriptor.addAttribute(spec->locations[i],spec->types[i],GL_FALSE,
                                        spec->sizes[i],total,stride);
        stride+=spec->datasizes[i];
    }
    tf->dataDescriptor.unbind();

    stride = 0;
    tf->transform.bind();
    tf->dataBuffer.bindRange();

    tf->transform.unbind();

    tf->dataBuffer.unbind();

    return tf;
}

struct CTransformComputer
{
    CPipeline* program      = nullptr;
    GLenum primitiveMode    = GL_NONE;

    void simulate(CTransformPair* source, CTransformPair* target){
        cWarning("TODO: Finish transform computer!");
        glEnable(GL_RASTERIZER_DISCARD); //Avoid drawing to framebuffer

        source->dataDescriptor.bind();
        program->bind(); //Bind transform program
//        target->transform.bind();
        glBeginTransformFeedback(primitiveMode);

        glDrawArrays(primitiveMode,0,0); //Draws TFB, first buffer filling
        glDrawTransformFeedback(primitiveMode,0); //Draws TFB usually

        glEndTransformFeedback();

        glDisable(GL_RASTERIZER_DISCARD); //Allow drawing to framebuffer again
    }

private:
    uint32 numActiveTFs = 0;
};

}
}

#endif // CTRANSFORMCOMPUTER

