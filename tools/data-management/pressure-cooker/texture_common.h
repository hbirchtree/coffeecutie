#pragma once

#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>
#include <coffee/core/datastorage/text/json/cjsonparser.h>
#include <coffee/core/terminal/terminal_cursor.h>
#include <coffee/core/type_safety.h>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/image/cimage.h>
#include <coffee/interfaces/content_pipeline.h>

#define TEXCOMPRESS_API "TextureCooker::"

using namespace CoffeePipeline;

/* Environment variables */
static const constexpr cstring TEX_MAX_SIZE = "TEXCOOK_MAX_SIZE";
static const constexpr cstring TEX_MIN_SIZE = "TEXCOOK_MIN_SIZE";

static i32 max_texture_size = 1024;
static i32 min_texture_size = 16;

struct texture_settings_t
{
    i32 max_size, min_size;
    u32 channels;

    void parse(Bytes&& data)
    {
        if(!data.data)
            return;

        auto doc = JSON::Read(data.as<char>().data);

        for(auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it)
        {
            auto member = CString((*it).name.GetString());

            if(member == "max_size")
                max_size = (*it).value.GetInt();
            if(member == "min_size")
                min_size = (*it).value.GetInt();
            if(member == "channels")
                channels = (*it).value.GetUint();
        }
    }

    void parse(Path basePath)
    {
        Path generalDesc = basePath.dirname() + "ALL.texture.json";
        Path specificDesc =
            basePath.addExtension("texture").addExtension("json");

        auto generalRsc = Resource(MkUrl(generalDesc, RSCA::AssetFile));
        if(FileExists(generalRsc))
            parse(C_OCAST<Bytes>(generalRsc));

        auto specificRsc = Resource(MkUrl(specificDesc, RSCA::AssetFile));
        if(FileExists(specificRsc))
            parse(C_OCAST<Bytes>(specificRsc));
    }
};

struct common_tools_t
{
    FileProcessor* cooker;
    TerminalCursor& cursor;
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
