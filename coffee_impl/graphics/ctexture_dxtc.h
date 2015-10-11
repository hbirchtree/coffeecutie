#ifndef CTEXTURE_DXTC
#define CTEXTURE_DXTC

#include "glbinding.h"
#include "coffee/cfiles.h"
#include "ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

extern bool coffee_graphics_tex_dxtc_store();
extern bool coffee_graphics_tex_dxtc_define();

extern bool coffee_graphics_tex_dxtc_store_safe();
extern bool coffee_graphics_tex_dxtc_define_safe();

extern CTexture* coffee_graphics_tex_dxtc_load(CResources::CResource* rsc);

}
}

#endif
