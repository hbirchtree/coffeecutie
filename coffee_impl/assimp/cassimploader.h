#ifndef CASSIMPLOADER
#define CASSIMPLOADER

#include "coffee_impl/assimp/cassimptypes.h"
#include "coffee_impl/graphics/cgraphicswrappers.h"

namespace Coffee{
namespace CGraphicsWrappers{

using namespace CResourceTypes::CAssimp;

struct CBufferedMesh
{
    void load(CAssimpMesh* mesh){
        m_vao = new CVertexArrayObject;
        m_vao->create();
        m_vao->bind();
        size_t i,j;
        for(i=0;i<mesh->numBuffers;i++){
            switch(mesh->bufferType[i]){
            case 1:{
                CBuffer buf;
                buf.create();
                buf.bind();
                buf.store(GL_ARRAY_BUFFER,mesh->bufferSize[i],
                          mesh->buffers[i],GL_DYNAMIC_STORAGE_BIT);
                m_vao->addAttribute(0,GL_FLOAT,GL_FALSE,3,0,0);
                break;
            }
            case 2:{
                CBuffer buf;
                buf.create();
                buf.bind();
                buf.store(GL_ARRAY_BUFFER,mesh->bufferSize[i],
                          mesh->buffers[i],GL_DYNAMIC_STORAGE_BIT);
                m_vao->addAttribute(0,GL_FLOAT,GL_FALSE,3,0,0);
                break;
            }
            case 3:{
                CBuffer buf;
                buf.create();
                buf.bind();
                buf.store(GL_ARRAY_BUFFER,mesh->bufferSize[i],
                          mesh->buffers[i],GL_DYNAMIC_STORAGE_BIT);
                m_vao->addAttribute(0,GL_FLOAT,GL_FALSE,2,0,0);
                break;
            }
            case 4:{
                CBuffer buf;
                buf.create();
                buf.bind();
                buf.store(GL_ARRAY_BUFFER,mesh->bufferSize[i],
                          mesh->buffers[i],GL_DYNAMIC_STORAGE_BIT);
                m_vao->addAttribute(0,GL_FLOAT,GL_FALSE,3,0,0);
                break;
            }
            case 5:{
                CBuffer buf;
                buf.create();
                buf.bind();
                buf.store(GL_ARRAY_BUFFER,mesh->bufferSize[i],
                          mesh->buffers[i],GL_DYNAMIC_STORAGE_BIT);
                m_vao->addAttribute(0,GL_FLOAT,GL_FALSE,3,0,0);
                break;
            }
            case 6:{
                CBuffer buf;
                buf.create();
                buf.bufferType = GL_ELEMENT_ARRAY_BUFFER;
                buf.bind();
                buf.store(GL_ELEMENT_ARRAY_BUFFER,mesh->bufferSize[i],
                          mesh->buffers[i],GL_DYNAMIC_STORAGE_BIT);
                elements = mesh->bufferSize[i];
                break;
            }
            }
        }
        m_vao->unbind();
    }
    CVertexArrayObject* vao(){
        return m_vao;
    }

    GLsizei elements = 0;

private:
    CVertexArrayObject* m_vao = nullptr;
};

}
}

#endif // CASSIMPLOADER

