#include "cmultidrawgraph.h"

namespace Coffee {
namespace CRendering {

CMultiDrawGraph::CMultiDrawGraph(CMultiDrawDescriptor *desc)
{
    drawcalls.create();
    drawcalls.bufferType = GL_DRAW_INDIRECT_BUFFER;
    drawcalls.flags = GL_DYNAMIC_STORAGE_BIT;

    vao.create();
    vao.bind();

    for(int j=0;j<desc->attributes.size();j++){
        CVertexAttribute* attr = &desc->attributes.at(j);
        CBuffer* buf = new CBuffer;
        buf->create();
        buf->bufferType = GL_ARRAY_BUFFER;
        buf->flags = GL_DYNAMIC_STORAGE_BIT;
        buf->bind();
        buf->store(0,nullptr);
        if(attr->flags&CVertexAttribute::MatrixType)
            for(int i=0;i<attr->rows;i++)
                vao.addAttributeDivided(attr->location+i,attr->type,attr->normalized,
                                        attr->size,attr->stride,attr->divisor,
                                        attr->offset*i);
        else
            vao.addAttribute(attr);

        m_vbuffers.insert(std::pair<uint8_t,CBuffer*>(desc->attribute_mapping.at(j),
                                                      buf));
    }
    m_idxbuf = new CBuffer;
    m_idxbuf->bufferType = GL_ELEMENT_ARRAY_BUFFER;
    m_idxbuf->flags = GL_DYNAMIC_STORAGE_BIT;
    m_idxbuf->create();
    m_idxbuf->bind();
    m_idxbuf->store(0,nullptr);

    vao.unbind();
}

void CMultiDrawGraph::addMesh(CAssimpMesh *mesh)
{
    CGLDrawCall call;
    call.baseInstance = 0;
    call.baseVertex = 0;
    for(const std::pair<uint8_t,CBuffer*>& b : m_vbuffers)
        for(int i=0;i<mesh->numBuffers;i++)
            if(mesh->bufferType[i]==b.first){
                szptr offset = b.second->size;
                b.second->resize(b.second->size
                                 +mesh->bufferSize[i]*mesh->elementSizes[i]);
                b.second->subStore(offset,mesh->bufferSize[i]*mesh->elementSizes[i],
                                   mesh->buffers[i]);
            }

    for(int i=0;i<mesh->numBuffers;i++)
        if(mesh->bufferType[i]==CAssimpMesh::IndexType){
            call.count = mesh->bufferSize[i];
            call.firstIndex = m_idxbuf->size/sizeof(GLuint);

            szptr offset = m_idxbuf->size;
            m_idxbuf->resize(m_idxbuf->size
                             +mesh->bufferSize[i]*mesh->elementSizes[i]);
            m_idxbuf->subStore(offset,mesh->bufferSize[i]*mesh->elementSizes[i],
                               mesh->buffers[i]);
        }
}

void CMultiDrawGraph::render()
{
    drawcalls.bind();
    glMultiDrawElementsIndirect(GL_TRIANGLES,GL_UNSIGNED_INT,
                                0,numPrimitives,
                                sizeof(CGLDrawCall));
}

} // namespace CRendering
} // namespace Coffee

