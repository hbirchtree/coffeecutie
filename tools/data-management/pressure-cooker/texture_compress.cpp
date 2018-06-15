#include <coffee/core/base/threading/job_system.h>

#include "texture_common.h"

#include <coffee/core/CDebug>

/* Image decoders */
#include "tiff_decoder.h"

/* Image compressors */
#include "dxt_compression.h"
#include "etc_compression.h"

struct TextureCooker : FileProcessor
{
    virtual void process(
        Vector<VirtFS::VirtDesc>& files, TerminalCursor& cursor);

    virtual void setBaseDirectories(const Vector<CString>&);
};

bool StbDecode(
    FileProcessor*,
    Pair<CString, ImageProcessor> const&,
    PixCmp&   cmp,
    BitFmt&   bfmt,
    CSize&    size,
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
        PixCmp&,
        BitFmt&,
        CSize&,
        Bytes&,
        Resource&)>
    image_decoders = {{ImageProc_stb, StbDecode},
#if defined(HAVE_LIBTIFF)
                      {ImageProc_tiff, TiffDecode}
#endif
};

static Tup<Bytes, Size> UpscaleImage(
    Bytes&& image, Size const& size, i32 target_size)
{
    Size target_size_ = {target_size, target_size};

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
                                C_CAST<i32>(scalar_size.w / scalar_size.h),
                            target_size};
    }

    auto temp_img = stb::image_rw::From(image, size, 4);
    auto res      = stb::Resize(temp_img, target_size_, 4);

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
    CResources::Resource r(MkUrl(file.first.c_str()));

    auto outName = Path(file.first).removeExt();

    PixCmp cmp = PixCmp::RGBA;
    BitFmt bfmt;
    Bytes  data;
    CSize  size;

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

    settings.max_size = max_texture_size;
    settings.min_size = min_texture_size;
    settings.channels = 4;

    settings.parse(outName);

    if(size.w < settings.max_size && size.h < settings.max_size)
    {
        std::tie(data, size) =
            UpscaleImage(std::move(data), size, settings.max_size);
    }

    common_tools_t tools = {cooker, cursor, settings, files};

    CompressDXT(tools, file, size, data, outName);
    CompressETC12(tools, size, data, outName);
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
            if(StrICmp(path.extension().c_str(), ext.c_str()))
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

    if(Env::ExistsVar(TEX_MAX_SIZE))
        max_texture_size = cast_string<i32>(Env::GetVar(TEX_MAX_SIZE));

    if(Env::ExistsVar(TEX_MIN_SIZE))
        min_texture_size = cast_string<i32>(Env::GetVar(TEX_MIN_SIZE));

    cursor.progress(
        TEXCOMPRESS_API "Compressable textures found: {0}", targets.size());

    Map<ThreadId::Hash, Vector<VirtFS::VirtDesc>> threadFiles;

    Threads::ParallelForEach<FileContainer, FileElement>(
        targets, [&](FileElement& e) {
            CompressTextureSet(threadFiles[ThreadId().hash()], e, this, cursor);
        });

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
