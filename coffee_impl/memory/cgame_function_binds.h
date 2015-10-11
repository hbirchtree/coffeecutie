#ifndef CGAME_FUNCTION_BINDS
#define CGAME_FUNCTION_BINDS

#include "coffee_impl/graphics/cbuffer.h"
#include "coffee_impl/graphics/ctexture.h"
#include "coffee_impl/rendering/cmultidrawgraph.h"

namespace Coffee{
namespace CMemoryManagement{
using namespace CGraphicsWrappers;
using namespace CRendering;

typedef std::function<void(const CMultiDrawDataSet&)> RenderFun;

typedef std::function
<void(const CTexture* tex, const CTextureTools::CTextureData* data)> TexDefineFun;
typedef std::function
<void(const CTexture* tex, const CTextureTools::CTextureData* data, GLint level)> TexStoreFun;

typedef std::function<void(const CTexture* tex)> TexLoadFun;
typedef std::function<void(const CTexture* tex)> TexUnloadFun;


struct game_function_binds
{
    RenderFun       renderfun = nullptr;

    TexDefineFun    tex_define = nullptr;
    TexStoreFun     tex_store = nullptr;
    TexLoadFun      tex_load = nullptr;
    TexUnloadFun    tex_unload = nullptr;

    CBufferFunctionBinds buffers = {0};
};

}
}

#endif //CGAME_FUNCTION_BINDS
