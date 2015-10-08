#include "cmultidrawgraph.h"

namespace Coffee {
namespace CRendering {

CMultiDrawDataSet coffee_multidraw_create(){
    CMultiDrawDataSet multidraw;

    multidraw.index = new CMultiIndexStorage;
    multidraw.drawcalls = new CMultiDrawCalls;

    return multidraw;
}

void coffee_multidraw_free(CMultiDrawDataSet *md){
    delete md->index;
    delete md->drawcalls;
}

void coffee_multidraw_bind_states(const CMultiDrawDataSet &set)
{
    for(CVertexBufferBinding* bnd : set.bindings)
        coffee_vao_attribute_bind_buffer(set.vao,*bnd);
}

void coffee_multidraw_render(const CMultiDrawDataSet &set)
{
    glMultiDrawElementsIndirect(GL_TRIANGLES,GL_UNSIGNED_INT,
                                0,set.drawcalls->drawcalls.size(),
                                sizeof(CGLDrawCall));
}

void coffee_multidraw_render_safe(const CMultiDrawDataSet &set)
{
    for(const CGLDrawCall& call : set.drawcalls->drawcalls)
    {
        if(call.count<1)
            continue;
        glDrawElementsInstancedBaseVertexBaseInstance(
                    GL_TRIANGLES,
                    call.count,
                    GL_UNSIGNED_INT,
                    (void*)(sizeof(GLuint)*call.firstIndex),
                    call.instanceCount,
                    call.baseVertex,
                    call.baseInstance);
    }
}

void coffee_multidraw_load_drawcalls(const CMultiDrawDataSet &set)
{
    set.drawcalls->drawbuffer->bufferType = GL_DRAW_INDIRECT_BUFFER;
    _coffee_bufferload_vector<CGLDrawCall>(set.drawcalls->drawcalls,set.drawcalls->drawbuffer);
}

void coffee_multidraw_load_indices(const CMultiDrawDataSet &set)
{
    set.index->buffer->bufferType = GL_ELEMENT_ARRAY_BUFFER;
    _coffee_bufferload_vector<GLuint>(set.index->indices,set.index->buffer);
}

void coffee_multidraw_load_buffer(CBuffer *buffer, const std::vector<byte> &data)
{
    buffer->bufferType = GL_ARRAY_BUFFER;
    _coffee_bufferload_vector<byte>(data,buffer);
}

void coffee_multidraw_load_vao(CMultiDrawDataSet &set, CMultiDrawDescriptor &desc)
{
    set.vao->bind();
    for(CVertexAttribute& attr : desc.attributes){
        coffee_vao_attribute_buffer(set.vao,attr,*attr.bnd);
        coffee_vao_attribute_format(set.vao,attr,*attr.fmt);

        coffee_vao_attribute_bind_buffer(set.vao,*attr.bnd);
        set.bindings.push_back(attr.bnd);
    }

    set.index->buffer->bind();
    set.vao->unbind();
}

bool coffee_multidraw_create_call(CMultiDrawDataSet &set, CAssimpMesh *mesh)
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
    call.baseVertex = set.drawcalls->vertexoffset;
    call.count = numIndices;
    call.instanceCount = 1;

    std::copy(&indices[0],&indices[call.count],std::back_inserter(set.index->indices));

    set.drawcalls->drawcalls.push_back(call);
    return true;
}

void coffee_mesh_define_matrix_attribs(CBuffer *instanceBuffer, CVertexFormat &fmt, CMultiDrawDescriptor &desc, uint16 baseIdx, uint16 baseBind)
{
    //Define dimensions, we use vertex format for amount of rows
    //We assume TxT matrix

    CVertexAttribute attr;
    attr.fmt = &fmt;

    for(GLuint i=0;i<fmt.size;i++)
    {
        //TODO: Store binding somewhere convenient
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

void coffee_mesh_free_matrix_attribs(std::vector<CVertexAttribute>::iterator start, std::vector<CVertexAttribute>::iterator end)
{
    for(auto it=start; it!=end; it++)
    {
        free((*it).bnd);
    }
}

void coffee_mesh_fill_vertexdata(std::vector<byte> &data, const void *rsrc, szptr roffset, szptr size)
{
    const byte* bytes = reinterpret_cast<const byte*>(rsrc);

    std::copy(&bytes[roffset],&bytes[roffset+size],std::back_inserter(data));
}

} // namespace CRendering
} // namespace Coffee

