#ifndef CTEXTURE_DXTC
#define CTEXTURE_DXTC

#include "glbinding.h"
#include "coffee/cfiles.h"
#include "ctexture.h"

namespace Coffee{
namespace CGraphicsWrappers{

struct CDXTCHeader
{

};

extern CTexture* coffee_graphics_tex_dxtc_load(CResources::CResource* rsc);

}
}

#endif
