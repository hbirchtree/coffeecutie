#ifndef CASSIMPLOADER
#define CASSIMPLOADER

#include "coffee_impl/assimp/cassimptypes.h"
#include "coffee_impl/graphics/cgraphicswrappers.h"

namespace Coffee{
namespace CGraphicsWrappers{

using namespace CResourceTypes::CAssimp;

struct CBufferedMesh
{
    CVertexArrayObject* vao(){
        return m_vao;
    }
    CGLDrawCall drawcall(){
        CGLDrawCall call;
        call.count = elements;
        call.firstIndex = 0;
        call.instanceCount = 1;
        call.baseInstance = 0;
        call.baseVertex = 0;

        return call;
    }
    void draw(){
        glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES,elements,GL_UNSIGNED_INT,
                                                      0,1,0,0);
    }

    GLsizei elements = 0;

private:
    CVertexArrayObject* m_vao = nullptr;
};

}
}

#endif // CASSIMPLOADER

