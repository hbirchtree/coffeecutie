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

struct CMultiDrawDescriptor{
    CVertexArrayObject              vao;
    std::vector<CVertexAttribute>   attributes;
    std::vector<uint8_t>            attribute_mapping;
};

class CMultiDrawGraph
{
public:
    CMultiDrawGraph(CMultiDrawDescriptor* desc);

    void addMesh(CAssimpMesh* mesh);
    void render();

    std::vector<CGLDrawCall> m_calls;

private:
    CVertexArrayObject vao;
    std::map<uint8_t,CBuffer*> m_vbuffers;
    CBuffer* m_idxbuf;
    CBuffer drawcalls;
    szptr numPrimitives = 0;
};

} // namespace CRendering
} // namespace Coffee

#endif // COFFEE_CRENDERING_CMULTIDRAWGRAPH_H
