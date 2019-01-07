/* Image decoders, TIFF needs to avoid uint* typedefs */
#include "tiff_decoder.h"

#include <coffee/core/CEnvironment>
#include <peripherals/stl/threads/job_system.h>

#include "texture_common.h"

/* Image compressors */
#include "dxt_compression.h"
#include "etc_compression.h"

namespace TexCompress {

struct TextureCooker : FileProcessor
{
    virtual void process(
        Vector<VirtFS::VirtDesc>& files, TerminalCursor& cursor);

    virtual void setBaseDirectories(const Vector<CString>&);

    virtual cstring name() const
    {
        return "TextureCooker"
        #if defined(HAVE_LIBTIFF)
            "+libtiff"
        #endif
        #if defined(HAVE_ETC2COMP)
            "+etc2comp"
        #endif
                ;
    }
};

bool StbDecode(
    FileProcessor*,
    Pair<CString, ImageProcessor> const&,
    PixCmp    cmp,
    BitFmt&   bfmt,
    Size&     size,
    Bytes&    data,
    Resource& r)
{
    return IMG::Load(std::move(r), cmp, bfmt, data, size);
}

static Map<
    ImageProcessor,
    bool (*)(
        FileProcessor*,
        Pair<CString, ImageProcessor> const&,
        PixCmp,
        BitFmt&,
        Size&,
        Bytes&,
        Resource&)>
    image_decoders = {{ImageProc_stb, StbDecode},
#if defined(HAVE_LIBTIFF)
                      {ImageProc_tiff, TiffDecode}
#endif
};

static Size ScaleSize(Size const& size, i32 target_size)
{
    size_2d<i32> target_size_ = {target_size, target_size};

    /* Non-quadratic, clamp largest size, scale the other */
    if(size.w != size.h)
    {
        auto scalar_size = size.convert<scalar>();

        if(size.w > size.h)
            target_size_ = {target_size,
                            target_size *
                                C_CAST<i32>(scalar_size.w / scalar_size.h)};
        else
            target_size_ = {target_size *
                                C_CAST<i32>(scalar_size.h / scalar_size.w),
                            target_size};
    }

    return target_size_.convert<u32>();
}

static Tup<Bytes, Size> ScaleImage(
    Bytes&& image, Size const& size, i32 target_size, stb::ImageHint hint)
{
    auto target_size_ = ScaleSize(size, target_size);

    auto temp_img = stb::image_rw::From(image, size, 4);
    auto res      = stb::Resize(temp_img, target_size_, 4, hint);

    Bytes output_data = std::move(res.data_owner);

    res.disown();

    return std::make_tuple(std::move(output_data), target_size_);
}

static void CompressTextureSet(
    Vector<VirtFS::VirtDesc>&            files,
    Pair<CString, ImageProcessor> const& file,
    FileProcessor*                       cooker,
    TerminalCursor&                      cursor)
{
    Resource r(MkUrl(file.first.c_str()));

    auto outName = Path(file.first).removeExt();

    PixCmp cmp = PixCmp::RGBA;
    BitFmt bfmt;
    Bytes  data;
    Size   size;

    auto decoder = image_decoders.find(file.second);

    if(decoder == image_decoders.end())
        return;

    if(!decoder->second(cooker, file, cmp, bfmt, size, data, r))
        return;

    if(size.area() == 0)
    {
        return;
    }

    texture_settings_t settings;

    settings.flags    = stb::ImageHint::Undefined;
    settings.max_size = max_texture_size;
    settings.min_size = min_texture_size;
    settings.channels = 4;
    settings.formats  = Compress_ALL; /*!< Use all formats by default */

    settings.parse(outName);

    if(size.w < settings.max_size && size.h < settings.max_size)
    {
        std::tie(data, size) = ScaleImage(
            std::move(data), size, settings.max_size, settings.flags);
    } else if(size.w > settings.max_size && size.h > settings.max_size)
    {
        std::tie(data, size) = ScaleImage(
            std::move(data), size, settings.max_size, settings.flags);
    }

    common_tools_t tools = {cooker, cursor, settings, files};

    if(settings.formats & Compress_DXT)
        CompressDXT(tools, file, size, data, outName);

#if defined(HAVE_ETC2COMP)
    if(settings.formats & Compress_ETC)
        CompressETC12(tools, size, data, outName);
#endif

    if(settings.formats & Compress_RAW)
    {
        /* Just export the raw image as RGBA8 */
        IMG::serial_image img;
        img.size               = size;
        img.v2.bit_fmt         = BitFmt::UByte;
        img.v2.format.base_fmt = PixFmt::RGBA8;
        img.v2.format.c_flags  = CompFlags::CompressionNone;
        img.v2.format.p_flags  = PixFlg::None;

        auto rawName = outName.addExtension("raw");

        files.emplace_back(rawName, Bytes(), 0);

        auto& rawData = files.back().data;

        rawData = Bytes::Alloc(sizeof(img) + data.size);
        MemCpy(Bytes::From(img), rawData.at(0, sizeof(img)));
        MemCpy(data, rawData.at(sizeof(img)));

        cursor.progress(
            TEXCOMPRESS_API "Exporting raw RGBA for {0}", file.first);
    }
}

void TextureCooker::process(
    Vector<VirtFS::VirtDesc>& files, TerminalCursor& cursor)
{
    using FileElement   = Pair<CString, ImageProcessor>;
    using FileContainer = Vector<FileElement>;

    FileContainer targets;

    for(auto& desc : files)
    {
        auto path = Path(desc.filename);

        for(auto ext : imageExtensions)
            /* Check if we recognize the extension */
            if(libc::str::cmp<libc::str::comp_nocase>(
                   path.extension().c_str(), ext.c_str()))
            {
                if(ext == "")
                    continue;
#if defined(HAVE_LIBTIFF)
                else if(ext == "TIFF" || ext == "TIF")
                    targets.push_back({desc.filename, ImageProc_tiff});
#endif
                else
                    targets.push_back({desc.filename, ImageProc_stb});

                continue;
            }
    }

    /* Set default texture sizes, for the cases where nothing is specified.
     * There is also a program-defined default to fallback to. */
    if(Env::ExistsVar(TEX_MAX_SIZE))
        max_texture_size = cast_string<i32>(Env::GetVar(TEX_MAX_SIZE));
    if(Env::ExistsVar(TEX_MIN_SIZE))
        min_texture_size = cast_string<i32>(Env::GetVar(TEX_MIN_SIZE));

    cursor.progress(
        TEXCOMPRESS_API "Compressable textures found: {0}", targets.size());

    Map<ThreadId::Hash, Vector<VirtFS::VirtDesc>> threadFiles;

    /* Each thread works on a texture and its mipmaps. */
    threads::ParallelForEach<FileContainer, FileElement>(
        targets,
        [&](FileElement& e) {
            CompressTextureSet(threadFiles[ThreadId().hash()], e, this, cursor);
        },
        this->numWorkers);

    for(auto& thread : threadFiles)
    {
        files.reserve(files.size() + thread.second.size());

        for(auto& file : thread.second)
            files.push_back(std::move(file));
    }

#if defined(HAVE_LIBTIFF)
    auto removePred = [&](VirtFS::VirtDesc const& desc) {
        for(auto const& e : targets)
            if(e.first == desc.filename)
                return true;

        return false;
    };

    auto removeIt = std::remove_if(files.begin(), files.end(), removePred);

    for(auto it = removeIt; it != files.end(); it++)
        it->data = Bytes();

    files.erase(removeIt, files.end());
#endif
}

void TextureCooker::setBaseDirectories(const Vector<CString>&)
{
}

COFFAPI FileProcessor* CoffeeLoader()
{
    return new TextureCooker;
}

} // namespace TexCompress
