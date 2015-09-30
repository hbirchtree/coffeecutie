#ifndef CIMAGE_LIB
#define CIMAGE_LIB

#include "coffee_types.h"
#include "coffee/cfiles.h"
#include "stb/stb_image.h"

namespace Coffee{
namespace CStbImageLib{

using namespace CResources;

struct CStbImage
{
    ubyte* data = nullptr;
    uint32 w	= 0;
    uint32 h	= 0;
};

static bool coffee_stb_image_load(CResource* src, CStbImage* target)
{
    int n = 0;
    target->data = stbi_loadf_from_memory(
		(const ubyte*)src->data,src->size,
		&target->w,&target->h,
		&n,0);
}

} //CStbImageLib
} //Coffee

#endif
