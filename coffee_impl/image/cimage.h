#ifndef CIMAGE_LIB
#define CIMAGE_LIB

#include "coffee_types.h"
#include "coffee/cfiles.h"
#include "coffee/cmath.h"

namespace Coffee{
namespace CStbImageLib{

using namespace CResources;

struct CStbImage
{
    ubyte* data = nullptr;
    CSize size;
    int bpp = 0;
};

struct CStbImageConst
{
    const ubyte* data = nullptr;
    CSize size;
    int bpp = 0;
};

extern void coffee_stb_error();
extern bool coffee_stb_image_load(
        CStbImage* target,
        CResource* src);
extern void _stbi_write_data(
        void* ctxt,
        void* data,
        int size);
extern bool coffee_stb_image_resize(
        CStbImage* img,
        const CSize& target,
        int channels);
extern bool coffee_stb_image_save_png(
        CResource* target,
        CStbImageConst* src);
extern bool coffee_stb_image_save_png(
        CResource* target,
        CStbImage* src);
extern bool coffee_stb_image_save_tga(
        CResource* target,
        CStbImage* src);
extern void coffee_stb_image_flip_vertical(
        CStbImage* src);
extern void coffee_stb_image_flip_horizontal(
        CStbImage* src);

extern void coffee_stb_image_free(CStbImage* img);

} //CStbImageLib
} //Coffee

#endif
