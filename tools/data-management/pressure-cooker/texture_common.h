#pragma once

#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>
#include <coffee/core/terminal/terminal_cursor.h>
#include <coffee/image/cimage.h>
#include <coffee/interfaces/content_pipeline.h>
#include <coffee/interfaces/content_settings.h>
#include <peripherals/stl/type_safety.h>
#include <peripherals/stl/types.h>

#define TEXCOMPRESS_API "TextureCooker::"

namespace TexCompress {

using namespace CoffeePipeline;

/* Environment variables */
static const constexpr cstring TEX_MAX_SIZE = "TEXCOOK_MAX_SIZE";
static const constexpr cstring TEX_MIN_SIZE = "TEXCOOK_MIN_SIZE";

static i32 max_texture_size = 1024;
static i32 min_texture_size = 16;

enum CompressFormats
{
    Compress_DXT   = 0x0001, /*!< For desktop */
    Compress_ETC   = 0x0002, /*!< For Android */
    Compress_PVRTC = 0x0004, /*!< For Maemo, iOS and Android */
    Compress_ATC   = 0x0008, /*!< For old Adreno */
    Compress_ASTC  = 0x0010, /*!< For high-end desktop and iOS */
    Compress_BC7   = 0x0020, /*!< For high-end desktop */
    Compress_ALL   = 0xFFFF,

    /* Special formats */
    Compress_RAW       = 0x10000, /*!< Export raw RGBA u8 */
    Compress_RAW_Float = 0x20000, /*!< Export raw RGBA f32 */
};

struct texture_settings_t : settings_visitor
{
    i32            max_size, min_size;
    u32            channels;
    u32            formats;
    stb::ImageHint flags;

    virtual CString type()
    {
        return "texture";
    }
    virtual void visit(CString const& member, JSON::Value const& value)
    {
        if(member == "max_size")
            max_size = value.GetInt();
        else if(member == "min_size")
            min_size = value.GetInt();
        else if(member == "channels")
            channels = value.GetUint();
        else if(member == "formats")
        {
            formats = 0x0;

            for(auto const& m : value.GetArray())
            {
                if(m.GetString() == CString("DXT"))
                    formats |= Compress_DXT;
                else if(m.GetString() == CString("BC7"))
                    formats |= Compress_BC7;
                else if(m.GetString() == CString("ASTC"))
                    formats |= Compress_ASTC;
                else if(m.GetString() == CString("ETC"))
                    formats |= Compress_ETC;
                else if(m.GetString() == CString("ATC"))
                    formats |= Compress_ATC;
                else if(m.GetString() == CString("PVRTC"))
                    formats |= Compress_PVRTC;
                else if(m.GetString() == CString("RAW"))
                    formats |= Compress_RAW;
            }
        } else if(member == "hints")
        {
            flags = stb::ImageHint::Undefined;

            for(auto const& m : value.GetArray())
            {
                if(m.GetString() == CString("normal"))
                    flags |= stb::ImageHint::NormalMap;
            }
        }
    }
};

struct common_tools_t
{
    FileProcessor*            cooker;
    TerminalCursor&           cursor;
    texture_settings_t const& compress;
    Vector<VirtFS::VirtDesc>& files;
};

static Vector<CString> imageExtensions = {"PNG",
                                          "JPG",
                                          "JPEG",
                                          "TGA",
#if defined(HAVE_LIBTIFF)
                                          "TIF",
                                          "TIFF"
#endif
};

enum ImageProcessor
{
    ImageProc_stb,
#if defined(HAVE_LIBTIFF)
    ImageProc_tiff,
#endif
};

} // namespace TexCompress
