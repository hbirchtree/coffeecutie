#ifndef CGAME_FUNCTION_BINDS
#define CGAME_FUNCTION_BINDS

#include <coffee/core/graphics_apis/COpenGL>
#include "../rendering/cmultidrawgraph.h"

namespace Coffee{
namespace CMemoryManagement{
using namespace CGraphicsWrappers;
using namespace CRendering;

typedef void(*RenderFun)(const CMultiDrawDataSet&);

struct game_function_binds
{
    RenderFun renderfun = nullptr;

    CTextureFunctionBinds textures;
    CBufferFunctionBinds  buffers;
};

}
}

#endif //CGAME_FUNCTION_BINDS
