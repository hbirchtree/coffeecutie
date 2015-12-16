#include <rendering/cmultidrawgraph.h>

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
        coffee_graphics_vao_attribute_bind_buffer(*set.vao,*bnd);
}

void coffee_multidraw_render(const CMultiDrawDataSet &set)
{
    coffee_graphics_multidraw(CPrimitiveMode::Triangles,
                              *set.drawcalls->drawbuffer);
}

void coffee_multidraw_render_safe(const CMultiDrawDataSet &set)
{
    for(const CGLDrawCall& call : set.drawcalls->drawcalls)
    {
        if(call.count<1)
            continue;
        coffee_graphics_draw_indexed(CPrimitiveMode::Triangles,call);
    }
}

void coffee_multidraw_load_drawcalls(
        const CMultiDrawDataSet &set, const CBufferPFN& bfun)
{
    set.drawcalls->drawbuffer->type = CBufferType::DrawIndirect;
    _coffee_bufferload_vector<CGLDrawCall>(set.drawcalls->drawcalls,
                                           *set.drawcalls->drawbuffer,
                                           bfun);
}

void coffee_multidraw_load_indices(
        const CMultiDrawDataSet &set, const CBufferPFN& bfun)
{
    set.index->buffer->type = CBufferType::Index;
    _coffee_bufferload_vector<CGuint>(set.index->indices,*set.index->buffer,bfun);
}

void coffee_multidraw_load_buffer(
        CBuffer *buffer, const std::vector<byte_t> &data, const CBufferPFN& bfun)
{
    buffer->type = CBufferType::Array;
    _coffee_bufferload_vector<byte_t>(data,*buffer,bfun);
}

void coffee_multidraw_load_vao(CMultiDrawDataSet &set, CMultiDrawDescriptor &desc)
{
    coffee_graphics_activate(*set.vao);
    for(CVertexAttribute& attr : desc.attributes){
        coffee_graphics_vao_attribute_buffer(*set.vao,attr,*attr.bnd);
        coffee_graphics_vao_attribute_format(*set.vao,attr,*attr.fmt);

        coffee_graphics_vao_attribute_bind_buffer(*set.vao,*attr.bnd);
        set.bindings.push_back(attr.bnd);
    }
    coffee_graphics_activate(*set.index->buffer);
    coffee_graphics_vao_attribute_index_buffer(*set.vao,*set.index->buffer);
}

bool coffee_multidraw_create_call(CMultiDrawDataSet &set, CAssimpMesh *mesh)
{
    CGLDrawCall call;

    const uint32* indices = nullptr;
    szptr numIndices = 0;
    const assimp_reflexive* ref;
    for(uint8 i=0;i<mesh->numBuffers;i++)
    {
        ref = &mesh->buffers[i];
        if(ref->type==CAssimpMesh::IndexType){
            indices = (uint32*)coffee_assimp_get_reflexive_ptr(mesh,ref);
            numIndices = ref->size/sizeof(uint32);
            break;
        }
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

    for(CGint i=0;i<fmt.size;i++)
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

void coffee_mesh_fill_vertexdata(std::vector<byte_t> &data, const void *rsrc, szptr roffset, szptr size)
{
    const byte_t* bytes = (const byte_t*)rsrc;

    std::copy(&bytes[roffset],&bytes[roffset+size],std::back_inserter(data));
}

} // namespace CRendering
} // namespace Coffee

