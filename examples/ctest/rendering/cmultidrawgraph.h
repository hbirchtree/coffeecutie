#ifndef COFFEE_CRENDERING_CMULTIDRAWGRAPH_H
#define COFFEE_CRENDERING_CMULTIDRAWGRAPH_H

#include <coffee/assimp/cassimpimporters.h>
#include <coffee/core/graphics/cgraphicsdata.h>
#include <coffee/core/graphics_apis/COpenGL>
#include <iterator>

namespace Coffee {
namespace CRendering {

using namespace CGraphicsWrappers;
using namespace CResourceTypes::CAssimp;
using namespace CGraphicsData;

struct CMultiIndexStorage
{
    CBuffer*                buffer  = nullptr;
    std::vector<CGuint>     indices;
};

struct CMultiDrawCalls
{
    std::vector<CGLDrawCall>    drawcalls;
    CBuffer*                    drawbuffer  = nullptr;
    szptr                       vertexoffset = 0;
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

extern CMultiDrawDataSet coffee_multidraw_create();

extern void coffee_multidraw_free(CMultiDrawDataSet* md);

extern void coffee_multidraw_bind_states(const CMultiDrawDataSet& set);
extern void coffee_multidraw_render(const CMultiDrawDataSet& set);
extern void coffee_multidraw_render_safe(const CMultiDrawDataSet& set);

template<typename DataType>
static void _coffee_bufferload_vector(
        const std::vector<DataType>& source, CBuffer* buffer,
        const CBufferFunctionBinds& fun){
    szptr datasz = source.size()*sizeof(DataType);
    fun.store_immutable(buffer,source.data(),datasz,buffer->flags);
}
//Load drawcalls into GPU memory
extern void coffee_multidraw_load_drawcalls(
        const CMultiDrawDataSet& set, const CBufferFunctionBinds& bfun);
//Load indices into GPU memory
extern void coffee_multidraw_load_indices(
        const CMultiDrawDataSet& set, const CBufferFunctionBinds& bfun);
//Load buffer data into GPU memory
extern void coffee_multidraw_load_buffer(CBuffer* buffer,
        const std::vector<byte_t>& data, const CBufferFunctionBinds &bfun);
//Load up VAO
extern void coffee_multidraw_load_vao(CMultiDrawDataSet& set, CMultiDrawDescriptor& desc);
//Copy indices, create drawcall
extern bool coffee_multidraw_create_call(CMultiDrawDataSet& set,CAssimpMesh* mesh);
//For creating instance buffer data
extern void coffee_mesh_define_matrix_attribs(CBuffer* instanceBuffer,
                                              CVertexFormat& fmt,
                                              CMultiDrawDescriptor& desc,
                                              uint16 baseIdx,
                                              uint16 baseBind);
extern void coffee_mesh_free_matrix_attribs(std::vector<CVertexAttribute>::iterator start, std::vector<CVertexAttribute>::iterator end);
//Copy mesh data
extern void coffee_mesh_fill_vertexdata(std::vector<byte_t>& data, const void* rsrc,
                                        szptr roffset, szptr size);

} // namespace CRendering
} // namespace Coffee

#endif // COFFEE_CRENDERING_CMULTIDRAWGRAPH_H
