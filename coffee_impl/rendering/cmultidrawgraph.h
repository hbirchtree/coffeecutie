#ifndef COFFEE_CRENDERING_CMULTIDRAWGRAPH_H
#define COFFEE_CRENDERING_CMULTIDRAWGRAPH_H

#include "coffee_impl/assimp/cassimpimporters.h"
#include "coffee_impl/graphics/cgraphicsdata.h"
#include "coffee_impl/graphics/cshader.h"

namespace Coffee {
namespace CRendering {

using namespace CGraphicsWrappers;
using namespace CResourceTypes::CAssimp;
using namespace CGraphicsData;

struct CMultiIndexStorage
{
    CBuffer*                buffer  = nullptr;
    std::vector<GLuint>     indices;
};

struct CMultiDrawCalls
{
    std::vector<CGLDrawCall>    drawcalls;
    CBuffer*                    drawbuffer  = nullptr;
};

struct CMultiDrawDataSet
{
    CMultiIndexStorage*         index       = nullptr;
    CMultiDrawCalls*            drawcalls   = nullptr;
    CVertexArrayObject*         vao         = nullptr;
    std::vector<CVertexBufferBinding*> bindings;
};

struct CMultiDrawDescriptor{
    std::vector<CVertexAttribute>   attributes;
};

static void coffee_multidraw_render(const CMultiDrawDataSet& set)
{
    for(CVertexBufferBinding* bnd : set.bindings)
        bnd->bindBuffer(set.vao);

    set.drawcalls->drawbuffer->bind();
    set.vao->bind();
    glMultiDrawElementsIndirect(GL_TRIANGLES,GL_UNSIGNED_INT,
                                0,set.drawcalls->drawcalls.size(),
                                sizeof(CGLDrawCall));
    set.drawcalls->drawbuffer->unbind();
    set.vao->unbind();
}
//Load drawcalls into GPU memory
static void coffee_multidraw_load_drawcalls(const CMultiDrawDataSet& set)
{
    set.drawcalls->drawbuffer->bufferType = GL_DRAW_INDIRECT_BUFFER;
    szptr datasz = set.drawcalls->drawcalls.size()*sizeof(set.drawcalls->drawcalls.data()[0]);

    set.drawcalls->drawbuffer->bind();
    set.drawcalls->drawbuffer->store(datasz,set.drawcalls->drawcalls.data());
    set.drawcalls->drawbuffer->unbind();
}
//Load indices into GPU memory
static void coffee_multidraw_load_indices(const CMultiDrawDataSet& set)
{
    set.index->buffer->bufferType = GL_ELEMENT_ARRAY_BUFFER;
    szptr datasz = set.index->indices.size()*sizeof(set.index->indices.data()[0]);

    set.index->buffer->bind();
    set.index->buffer->store(datasz,set.index->indices.data());
    set.index->buffer->unbind();
}
//Load up VAO
static void coffee_multidraw_load_vao(CMultiDrawDataSet& set, CMultiDrawDescriptor& desc)
{
    set.vao->bind();
    for(CVertexAttribute& attr : desc.attributes){
        attr.setBuffer(set.vao,*attr.bnd);
        attr.setFormat(set.vao,*attr.fmt);

        attr.bnd->bindBuffer(set.vao);
        set.bindings.push_back(attr.bnd);
    }

    set.index->buffer->create();
    set.index->buffer->bufferType = GL_ELEMENT_ARRAY_BUFFER;
    set.index->buffer->bind();
    set.vao->unbind();
}
//Copy indices, create drawcall
static bool coffee_multidraw_create_call(CMultiDrawDataSet& set,CAssimpMesh* mesh)
{
    CGLDrawCall call;
    call.baseInstance = 0;
    call.baseVertex = 0;
    call.count = 0;
    call.firstIndex = 0;
    call.instanceCount = 0;

    const GLuint* indices = nullptr;
    csize_t numIndices = 0;
    for(int i=0;i<mesh->numBuffers;i++)
        if(mesh->bufferType[i]==CAssimpMesh::IndexType){
            indices = reinterpret_cast<const GLuint*>(mesh->buffers[i]);
            numIndices = mesh->bufferSize[i];
        }

    if(numIndices<1)
        return false;

    call.firstIndex = set.index->indices.size();
    call.count = numIndices;
    call.instanceCount = 1;

    std::copy(&indices[0],&indices[numIndices],std::back_inserter(set.index->indices));

    set.drawcalls->drawcalls.push_back(call);
    return true;
}
//Copy mesh data
static void coffee_mesh_load_vertexdata(std::vector<byte>& data, const void* rsrc,
                                        szptr roffset, szptr size)
{
    const byte* bytes = reinterpret_cast<const byte*>(rsrc);

    std::copy(&bytes[roffset],&bytes[roffset+size],std::back_inserter(data));
}

} // namespace CRendering
} // namespace Coffee

#endif // COFFEE_CRENDERING_CMULTIDRAWGRAPH_H
