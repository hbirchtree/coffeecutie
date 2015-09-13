#ifndef COFFEE_CRENDERING_CMULTIDRAWGRAPH_H
#define COFFEE_CRENDERING_CMULTIDRAWGRAPH_H

#include "coffee_impl/assimp/cassimpimporters.h"
#include "coffee_impl/graphics/cgraphicsdata.h"
#include "coffee_impl/graphics/cshader.h"

namespace Coffee {
namespace CRendering {

using namespace CGraphicsWrappers;

struct CMultiDrawDescriptor{
    CVertexArrayObject vao;
    std::vector<CVertexAttribute> attributes;
};

class CMultiDrawGraph
{
public:
    CMultiDrawGraph(CMultiDrawDescriptor* desc);

    void addMesh(CResourceTypes::CAssimp::CAssimpMesh* mesh);

private:
    CBuffer m_drawcallBuffer;
    CVertexArrayObject *m_vao = nullptr;

    std::vector<CGLDrawCall> m_drawcalls;
    std::vector<CBuffer> m_vertexBuffers;
};

} // namespace CRendering
} // namespace Coffee

#endif // COFFEE_CRENDERING_CMULTIDRAWGRAPH_H
