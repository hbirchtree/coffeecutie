#include <coffee/windowing/renderer/renderer.h>

#include "renderer_internal.h"

namespace Coffee{
namespace Display{

CSDL2Renderer* CreateRenderer()
{
    return C_DCAST<CSDL2Renderer>(new CSDL2Renderer_Internal);
}

}
}
