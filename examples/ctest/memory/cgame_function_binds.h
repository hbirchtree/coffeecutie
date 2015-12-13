#ifndef CGAME_FUNCTION_BINDS
#define CGAME_FUNCTION_BINDS

#include <coffee/graphics_apis/COpenGL>
#include "rendering/cmultidrawgraph.h"

namespace Coffee{
namespace CMemoryManagement{
using namespace CGraphicsWrappers;
using namespace CRendering;

typedef void(*RenderFun)(const CMultiDrawDataSet&);

struct game_function_binds
{
    RenderFun renderfun = nullptr;

    CTexturePFN textures;
    CBufferPFN  buffers;
};

}
}

#endif //CGAME_FUNCTION_BINDS
