#include <coffee/blam/cblam.h>
#include <coffee/blam/volta/blam_stl.h>
#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/stl_types.h>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/image/cimage.h>
#include <coffee/interfaces/full_launcher.h>
#include <coffee/strings/libc_types.h>
#include <coffee/windowing/renderer/renderer.h>
#include <peripherals/stl/string_ops.h>

#include <coffee/core/CDebug>

using namespace Coffee;
using namespace Blam;

struct texture_data_t
{
    bitm_texture_t      tex;
    const index_item_t* tag;
    const bitm_image_t* img;
    cstring             tag_name;
};

using rgba_color_t = rgba_t;

#define bmpEndianSwap64(x)                                                 \
    (((x & 0xFF00000000000000) >> 56) | ((x & 0x00FF000000000000) >> 40) | \
     ((x & 0x0000FF0000000000) >> 24) | ((x & 0x000000FF00000000) >> 8) |  \
     ((x & 0x00000000FF000000) << 8) | ((x & 0x0000000000FF0000) << 24) |  \
     ((x & 0x000000000000FF00) << 40) | ((x & 0x00000000000000FF) << 56))
#define bmpEndianSwap32(x)                                \
    (((x & 0xFF000000) >> 24) | ((x & 0x00FF0000) >> 8) | \
     ((x & 0x0000FF00) << 8) | ((x & 0x000000FF) << 24))
#define bmpEndianSwap16(x) (((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8))

rgba_color_t GradientColorsHalf(rgba_color_t Col1, rgba_color_t Col2)
{
    rgba_color_t ret;
    ret.r = (Col1.r / 2 + Col2.r / 2);
    ret.g = (Col1.g / 2 + Col2.g / 2);
    ret.b = (Col1.b / 2 + Col2.b / 2);
    ret.a = 255;
    return ret;
}

rgba_color_t GradientColors(rgba_color_t Col1, rgba_color_t Col2)
{
    rgba_color_t ret;
    ret.r = ((Col1.r * 2 + Col2.r)) / 3;
    ret.g = ((Col1.g * 2 + Col2.g)) / 3;
    ret.b = ((Col1.b * 2 + Col2.b)) / 3;
    ret.a = 255;
    return ret;
}

unsigned int rgba_to_int(rgba_color_t color)
{
    return (color.r << 24) | (color.g << 16) | (color.b << 8) | (color.a);
}

rgba_color_t short_to_rgba(unsigned short color)
{
    rgba_color_t rc;
    rc.r = (((color >> 11) & 0x1F) * 0xFF) / 31;
    rc.g = (((color >> 5) & 0x3F) * 0xFF) / 63;
    rc.b = (((color >> 0) & 0x1F) * 0xFF) / 31;
    rc.a = 255;

    return rc;
}

void DecodeDXT1(int Height, int Width, const char* IData, unsigned int* PData)
{
    rgba_color_t        Color[4];
    int                 i;
    int                 dptr;
    rgba_color_t        CColor;
    unsigned int        CData;
    unsigned int        ChunksPerHLine = Width / 4;
    bool                trans;
    static rgba_color_t zeroColor = {0, 0, 0, 0};

    for(i = 0, dptr = 0; i < (Width * Height); i += 16, dptr += 8)
    {
        unsigned short c1, c2;

        c1 = *(unsigned short*)&IData[dptr];
        c1 = bmpEndianSwap16(c1);
        c2 = *(unsigned short*)&IData[dptr + 2];
        c2 = bmpEndianSwap16(c2);
        if(c1 > c2)
            trans = false;
        else
            trans = true;

        Color[0] = short_to_rgba(c1);
        Color[1] = short_to_rgba(c2);
        if(!trans)
        {
            Color[2] = GradientColors(Color[0], Color[1]);
            Color[3] = GradientColors(Color[1], Color[0]);
        } else
        {
            Color[2] = GradientColorsHalf(Color[0], Color[1]);
            Color[3] = zeroColor;
        }

        CData                 = *(unsigned int*)&(IData[dptr + 4]);
        CData                 = bmpEndianSwap32(CData);
        unsigned int ChunkNum = i / 16;
        unsigned int XPos     = ChunkNum % ChunksPerHLine;
        unsigned int YPos     = (ChunkNum - XPos) / ChunksPerHLine;

        int sizew, sizeh;

        sizeh = (Height < 4) ? Height : 4;
        sizew = (Width < 4) ? Width : 4;
        int x, y;
        for(x = 0; x < sizeh; x++)
        {
            for(y = 0; y < sizew; y++)
            {
                CColor = Color[CData & 0x03];
                CData >>= 2;
                PData[(YPos * 4 + x) * Width + XPos * 4 + y] =
                    rgba_to_int(CColor);
            }
        }
    }
}

/*================================
 * DecodeDXT2And3
================================*/
void DecodeDXT2And3(
    int Height, int Width, const char* IData, unsigned int* PData)
{
    rgba_color_t   Color[4];
    int            i;
    rgba_color_t   CColor;
    unsigned int   CData;
    unsigned int   ChunksPerHLine = Width / 4;
    unsigned short Alpha;

    if(!ChunksPerHLine)
        ChunksPerHLine = 1;

    for(i = 0; i < (Width * Height); i += 16)
    {
        Color[0] =
            short_to_rgba(bmpEndianSwap16(*(unsigned short*)&IData[i + 8]));
        Color[1] =
            short_to_rgba(bmpEndianSwap16(*(unsigned short*)&IData[i + 10]));
        Color[2] = GradientColors(Color[0], Color[1]);
        Color[3] = GradientColors(Color[1], Color[0]);

        CData                 = *(unsigned int*)&IData[i + 12];
        CData                 = bmpEndianSwap32(CData);
        unsigned int ChunkNum = i / 16;
        unsigned int XPos     = ChunkNum % ChunksPerHLine;
        unsigned int YPos     = (ChunkNum - XPos) / ChunksPerHLine;

        int sizew, sizeh;

        sizeh = (Height < 4) ? Height : 4;
        sizew = (Width < 4) ? Width : 4;
        int x, y;
        for(x = 0; x < sizeh; x++)
        {
            Alpha = *(short*)&IData[i + (2 * x)];
            Alpha = bmpEndianSwap16(Alpha);
            for(y = 0; y < sizew; y++)
            {
                CColor = Color[CData & 0x03];
                CData >>= 2;

                CColor.a = (Alpha & 0x0F) * 16;
                Alpha >>= 4;

                PData[(YPos * 4 + x) * Width + XPos * 4 + y] =
                    rgba_to_int(CColor);
            }
        }
    }
}

/*================================
 * DecodeDXT4And5
================================*/
void DecodeDXT4And5(
    int Height, int Width, const char* IData, unsigned int* PData)
{
    rgba_color_t  Color[4];
    int           i;
    rgba_color_t  CColor;
    unsigned int  CData;
    unsigned int  ChunksPerHLine = Width / 4;
    unsigned char Alpha[8];
    uint64_t      AlphaDat;

    if(!ChunksPerHLine)
        ChunksPerHLine = 1;

    for(i = 0; i < (Width * Height); i += 16)
    {
        Color[0] =
            short_to_rgba(bmpEndianSwap16(*(unsigned short*)&IData[i + 8]));
        Color[1] =
            short_to_rgba(bmpEndianSwap16(*(unsigned short*)&IData[i + 10]));
        Color[2] = GradientColors(Color[0], Color[1]);
        Color[3] = GradientColors(Color[1], Color[0]);

        CData    = (int)*(unsigned int*)&IData[i + 12];
        CData    = bmpEndianSwap32(CData);
        Alpha[0] = IData[i];
        Alpha[1] = IData[i + 1];

        // Do the alphas
        if(Alpha[0] > Alpha[1])
        {
            // 8-alpha block:  derive the other six alphas.
            // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
            Alpha[2] = (6 * Alpha[0] + 1 * Alpha[1] + 3) / 7; // bit code 010
            Alpha[3] = (5 * Alpha[0] + 2 * Alpha[1] + 3) / 7; // bit code 011
            Alpha[4] = (4 * Alpha[0] + 3 * Alpha[1] + 3) / 7; // bit code 100
            Alpha[5] = (3 * Alpha[0] + 4 * Alpha[1] + 3) / 7; // bit code 101
            Alpha[6] = (2 * Alpha[0] + 5 * Alpha[1] + 3) / 7; // bit code 110
            Alpha[7] = (1 * Alpha[0] + 6 * Alpha[1] + 3) / 7; // bit code 111
        } else
        {
            // 6-alpha block.
            // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
            Alpha[2] = (4 * Alpha[0] + 1 * Alpha[1] + 2) / 5; // Bit code 010
            Alpha[3] = (3 * Alpha[0] + 2 * Alpha[1] + 2) / 5; // Bit code 011
            Alpha[4] = (2 * Alpha[0] + 3 * Alpha[1] + 2) / 5; // Bit code 100
            Alpha[5] = (1 * Alpha[0] + 4 * Alpha[1] + 2) / 5; // Bit code 101
            Alpha[6] = 0;                                     // Bit code 110
            Alpha[7] = 255;                                   // Bit code 111
        }

        // Byte	Alpha
        // 0	Alpha_0
        // 1	Alpha_1
        // 2	[0][2] (2 LSBs), [0][1], [0][0]
        // 3	[1][1] (1 LSB), [1][0], [0][3], [0][2] (1 MSB)
        // 4	[1][3], [1][2], [1][1] (2 MSBs)
        // 5	[2][2] (2 LSBs), [2][1], [2][0]
        // 6	[3][1] (1 LSB), [3][0], [2][3], [2][2] (1 MSB)
        // 7	[3][3], [3][2], [3][1] (2 MSBs)
        // [0

        // Read an int and a short
        unsigned int   tmpdword;
        unsigned short tmpword;

        tmpword  = bmpEndianSwap16((short)*(unsigned short*)&IData[i + 2]);
        tmpdword = bmpEndianSwap32((int)*(unsigned int*)&IData[i + 4]);

        AlphaDat = tmpword | bmpEndianSwap64((uint64_t)tmpdword << 16);

        unsigned int ChunkNum = i / 16;
        unsigned int XPos     = ChunkNum % ChunksPerHLine;
        unsigned int YPos     = (ChunkNum - XPos) / ChunksPerHLine;

        int sizew, sizeh;

        sizeh = (Height < 4) ? Height : 4;
        sizew = (Width < 4) ? Width : 4;
        int x, y;
        for(x = 0; x < sizeh; x++)
        {
            for(y = 0; y < sizew; y++)
            {
                CColor = Color[CData & 0x03];
                CData >>= 2;

                CColor.a = Alpha[AlphaDat & 0x07];
                AlphaDat >>= 3;

                PData[(YPos * 4 + x) * Width + XPos * 4 + y] =
                    rgba_to_int(CColor);
            }
        }
    }
}

// static int num_channels(bitm_format fmt)
//{
//    switch(fmt)
//    {
//    case bitm_format::A8Y8:
//    case bitm_format::AY8:
//        return 2;
//    case bitm_format::R5G6B5:
//        return 3;
//    case bitm_format::A1R5G5B5:
//    case bitm_format::A4R4G4B4:
//    case bitm_format::A8R8G8B8:
//    case bitm_format::X8R8G8B8:
//        return 4;
//    default:
//        return 1;
//    }
//}

static int texture_size(bitm_image_t const* img)
{
    auto isize = img->isize;

    switch(img->format)
    {
    case bitm_format::DXT1:
    case bitm_format::DXT2AND3:
    case bitm_format::DXT4AND5:
        if(isize.area() == 0)
            isize = {4, 4};
        break;
    default:
        if(isize.area() == 0)
            isize = {1, 1};
        break;
    }

    switch(img->format)
    {
    case bitm_format::A8:
    case bitm_format::Y8:
    case bitm_format::P8:
    case bitm_format::AY8:
        return isize.area();
    case bitm_format::A8Y8:
    case bitm_format::R5G6B5:
    case bitm_format::A1R5G5B5:
    case bitm_format::A4R4G4B4:
        return isize.area() * 2;
    case bitm_format::A8R8G8B8:
    case bitm_format::X8R8G8B8:
        return isize.area() * 4;
    case bitm_format::DXT1:
        return (isize.w >> 2) * (isize.h >> 2) * 8;
    case bitm_format::DXT2AND3:
    case bitm_format::DXT4AND5:
        return (isize.w >> 2) * (isize.h >> 2) * 16;
    }
}

struct Empty
{
};

/*!
 * \brief This example employs the COFFEE_APPLICATION_MAIN macro to redirect the
 * main function. The purpose of this is platform abstraction such that the rest
 * of the code works consistently across platforms.
 */
int coffee_main(i32, cstring_w*)
{
    //    CResources::FileResourcePrefix("sample_data/cblam_data/");

    //    cstring mapstring = GetArgument(argv,argc,"halomap");
    cstring mapstring = "bloodgulch.map";
    if(!mapstring)
        return 1;
    Resource mapfile(mapstring);
    if(!FileExists(mapfile))
        return 1;

    Resource bitmfile("bitmaps.map");
    if(!FileMap(bitmfile))
        return 1;
    if(!FileMap(mapfile))
        return 1;

    if(bitmfile.size < 5_kB || mapfile.size < 5_kB)
        return 1;

    map_container map(mapfile.data, version_t::pc);

    cDebug(
        "Opening map: {0}, build {1}",
        C_CAST<cstring>(map),
        map.map->buildDate);

    tag_index_view index_view(map);

    {
        /* Extracting scenario data */
        const scenario* scn = scn_get(index_view);

        const scn_bsp_header* sbsp =
            scn->struct_bsp.data(map.map, map.tags.index_magic);
        for(int i = 0; i < scn->struct_bsp.count; i++)
        {
            const scn_bsp_header* s_bsp = &sbsp[i];
            //        const blam_scn_chunk* bsp =
            //                (const blam_scn_chunk*)
            //                blam_mptr(map,0,s_bsp->offset);
            cDebug(
                "BSP: {0}",
                (cstring)blam_mptr(
                    map.map, map.tags.index_magic, s_bsp->name_ptr));
        }
    }

    auto debigend = [](cstring src, u32 len) {
        return str::transform::reverse(str::encapsulate(src, len));
    };
    auto pred = [&](index_item_t const* e) {
        return debigend(e->tagclass[0], 4) == "scnr";
    };

    auto scenario = *std::find_if(index_view.begin(), index_view.end(), pred);

    cDebug(
        "Predicate test: {0} -> {1}", scenario->tagId, map.get_name(scenario));

    Vector<index_item_t const*> texture_vec;

    texture_vec.insert(
        texture_vec.begin(),
        std::find_if(
            index_view.begin(),
            index_view.end(),
            [](index_item_t const* e) {
                return e->tagclass_e[0] == tag_class_t::bitm;
            }),
        index_view.end());

    using API = RHI::GLEAM::GLEAM_API;
    using C   = CompFlags;

    AutoExec<API, Display::RendererInterface, Empty>(
        [&](Display::RendererInterface& r, Empty*) {
            /* OpenGL context time! */

            auto context = API::GetLoadAPI();

            if(!context(true))
            {
                r.closeWindow();
                return;
            }

            for(auto bitm : texture_vec)
            {
                if(bitm->offset == 0)
                    continue;

                auto head = bitm->to_reflexive<bitm_header_t>().data(
                    mapfile.data, index_view.tags()->index_magic);

                if(head->imageCount > 10 || head->imageCount < 0 ||
                   head->imageOffset == 0)
                    continue;

                auto label = map.get_name(bitm);

                API::DBG::SCOPE img_scope(label);

                //            for(auto i : Range<i32>(head->imageCount))
                {
                    auto image = head->image_headers().data(
                        mapfile.data, index_view.tags()->index_magic);

                    auto texture = bitm_get_texture(image, bitmfile.data);

                    u32 flgs = 0;

                    switch(image->format)
                    {
                    case bitm_format::DXT1:
                        flgs = C_CAST<u32>(C::S3TC_1);
                        break;
                    case bitm_format::DXT2AND3:
                        flgs = C_CAST<u32>(C::S3TC_3);
                        break;
                    case bitm_format::DXT4AND5:
                        flgs = C_CAST<u32>(C::S3TC_5);
                        break;
                    default:
                        break;
                    }

                    flgs <<= 10;

                    API::S_2D tex_handle(texture.cformat, 1, flgs);

                    tex_handle.allocate(
                        image->isize.convert<i32>(), texture.format);

                    tex_handle.upload(
                        {texture.cformat, texture.dformat, texture.format},
                        image->isize.convert<i32>(),
                        {(byte_t*)texture.data,
                         (szptr)texture_size(image),
                         (szptr)texture_size(image)});

                    tex_handle.dealloc();
                }
            }

            RHI::GLEAM::GLEAM_API::UnloadAPI();

            r.closeWindow();

            exit(0);
        },
        [](Display::RendererInterface&, Empty*) {},
        [](Display::RendererInterface&, Empty*) {});

    //    for(auto e : texture_vec)
    //    {
    //        auto r = e->to_reflexive<bitm_header_t>()
    //                .data(mapfile.data, index_view.tags()->index_magic);
    //        for(i32 i=0; i< r->imageCount; i++)
    //        {
    //            auto img_h = r->image_headers()
    //                    .data(mapfile.data, index_view.tags()->index_magic);
    //            if(img_h && r->imageCount < 10 && r->imageCount > 0
    //                    && img_h->isize.area() != 0)
    //            {
    //                auto img_data = r->image_ptr(img_h).data(bitmfile.data,
    //                0);

    //                cDebug("Texture: {0} -> {1}", e->tagId, map.get_name(e));
    //                cDebug("Pixel data: {0}", StrUtil::pointerify(img_data));

    //                Vector<byte_t> dxt_store = {};

    //                CStbImageLib::CStbImageConst img;
    //                img.data = (byte_t*)img_data;
    //                img.size = img_h->isize;
    //                img.bpp = num_channels(img_h->format);

    //                if(img_h->format == bitm_format::DXT2AND3
    //                        || img_h->format == bitm_format::DXT4AND5
    //                        || img_h->format == bitm_format::DXT1)
    //                {
    //                    auto tsize = texture_size(img_h);
    //                    dxt_store.resize(tsize * 4);
    //                    img.data = dxt_store.data();

    //                    switch(img_h->format)
    //                    {
    //                    case bitm_format::DXT1:
    //                        DecodeDXT1(img_h->isize.h, img_h->isize.w,
    //                                   img_data, (u32*)img.data);
    //                        break;
    //                    case bitm_format::DXT2AND3:
    //                        DecodeDXT2And3(img_h->isize.h, img_h->isize.w,
    //                                       img_data, (u32*)img.data);
    //                        break;
    //                    case bitm_format::DXT4AND5:
    //                        DecodeDXT4And5(img_h->isize.h, img_h->isize.w,
    //                                       img_data, (u32*)img.data);
    //                        break;
    //                    default:
    //                        break;
    //                    }
    //                }

    //                auto fname = cStringFormat("texture_{0}_{1}.png",
    //                                           map.get_name(e),
    //                                           i);

    //                fname = CStrReplace(fname, "\\", "-");

    //                Resource rsc(MkUrl(fname.c_str(),
    //                                   RSCA::AssetFile));

    //                CStbImageLib::SavePNG(&rsc, &img);

    //                FileCommit(rsc);
    //            }
    //        }
    //    }

    return 0;

    for(auto e : index_view)
    {
        cDebug("Values: {0}", map.get_name(e));
    }

    //    {
    //        /* Extracting texture data */
    //        texture_data_t img;
    //        const index_item_t* it = nullptr;
    //        while(it = fun(tags, map) /*fun_filter(fun, tags, map, bitmfile,
    //        img)*/)
    //        {
    //            cDebug("Index item: {0} -> {1}, {2}, {3}",
    //                   it->tagId,
    //                   debigend(it->tagclass[0], 4),
    //                    (it->tagclass[1][0] != -1) ? debigend(it->tagclass[1],
    //                    4) : CString(), blam_index_item_get_string(it, map,
    //                    &tags));
    ////            cDebug("Texture:
    ///{0},res={1},mip={2}",img.tag_name,img.tex.resolution,img.tex.mipmaps);
    //        }
    //    }

    FileUnmap(bitmfile);
    FileUnmap(mapfile);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
