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

static CMultiDrawDataSet coffee_multidraw_create(){
    CMultiDrawDataSet multidraw;

    multidraw.index = new CMultiIndexStorage;
    multidraw.index->buffer = new CBuffer;
    multidraw.index->buffer->create();
    multidraw.drawcalls = new CMultiDrawCalls;
    multidraw.drawcalls->drawbuffer = new CBuffer;
    multidraw.drawcalls->drawbuffer->create();
    multidraw.vao = new CVertexArrayObject;
    multidraw.vao->create();

    return multidraw;
}


static void coffee_multidraw_bind_states(const CMultiDrawDataSet& set)
{
    for(CVertexBufferBinding* bnd : set.bindings)
        bnd->bindBuffer(set.vao);
}
static void coffee_multidraw_render(const CMultiDrawDataSet& set)
{
    set.drawcalls->drawbuffer->bind();
    set.vao->bind();
    glMultiDrawElementsIndirect(GL_TRIANGLES,GL_UNSIGNED_INT,
                                0,set.drawcalls->drawcalls.size(),
                                sizeof(CGLDrawCall));
    set.drawcalls->drawbuffer->unbind();
    set.vao->unbind();
}
template<typename DataType>
static void _coffee_bufferload_vector(const std::vector<DataType>& source, CBuffer* buffer){
    szptr datasz = source.size()*sizeof(DataType);

    buffer->bind();
    buffer->store(datasz,source.data());
    buffer->unbind();
}
//Load drawcalls into GPU memory
static void coffee_multidraw_load_drawcalls(const CMultiDrawDataSet& set)
{
    set.drawcalls->drawbuffer->bufferType = GL_DRAW_INDIRECT_BUFFER;
    _coffee_bufferload_vector<CGLDrawCall>(set.drawcalls->drawcalls,set.drawcalls->drawbuffer);
}
//Load indices into GPU memory
static void coffee_multidraw_load_indices(const CMultiDrawDataSet& set)
{
    set.index->buffer->bufferType = GL_ELEMENT_ARRAY_BUFFER;
    _coffee_bufferload_vector<GLuint>(set.index->indices,set.index->buffer);
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

    std::copy(&indices[0],&indices[call.count],std::back_inserter(set.index->indices));

    set.drawcalls->drawcalls.push_back(call);
    return true;
}
//For creating instance buffer data
static void coffee_mesh_define_matrix_attribs(CBuffer* instanceBuffer,
                                              CVertexFormat& fmt,
                                              CMultiDrawDescriptor& desc,
                                              uint16 baseIdx,
                                              uint16 baseBind)
{
    //Define dimensions, we use vertex format for amount of rows
    //We assume TxT matrix

    CVertexAttribute attr;
    attr.fmt = &fmt;

    for(int i=0;i<fmt.size;i++)
    {
        CVertexBufferBinding* bind = new CVertexBufferBinding;
        bind->binding = baseBind+i;
        bind->buffer = instanceBuffer;
        bind->divisor = 1;
        bind->stride = sizeof(float)*fmt.size*fmt.size;
        bind->offset = sizeof(float)*fmt.size*i;

        attr.attribIdx = baseIdx+i;
        attr.bnd = bind;

        desc.attributes.push_back(attr);
    }
}
static void coffee_mesh_free_matrix_attribs(std::vector<CVertexAttribute>::iterator start, std::vector<CVertexAttribute>::iterator end)
{
    for(auto it=start; it!=end; it++)
    {
        free((*it).bnd);
    }
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
