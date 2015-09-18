#ifndef COFFEE_CRENDERING_CMULTIDRAWGRAPH_H
#define COFFEE_CRENDERING_CMULTIDRAWGRAPH_H

#include "coffee_impl/assimp/cassimpimporters.h"
#include "coffee_impl/graphics/cgraphicsdata.h"
#include "coffee_impl/graphics/cshader.h"

namespace Coffee {
namespace CRendering {

using namespace CGraphicsWrappers;
using namespace CGraphicsData;

struct CMultiDrawDescriptor{
    CVertexArrayObject vao;
    std::vector<CVertexAttribute> attributes;
};

class CMultiDrawGraph
{
public:
    CMultiDrawGraph();

    void addDrawCalls(CGLDrawCall *dc,szptr numCalls);
    void render();

private:
    CBuffer drawcalls;
    szptr numPrimitives;
};

} // namespace CRendering
} // namespace Coffee

#endif // COFFEE_CRENDERING_CMULTIDRAWGRAPH_H
