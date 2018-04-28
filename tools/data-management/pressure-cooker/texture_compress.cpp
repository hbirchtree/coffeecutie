#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/base/threading/job_system.h>
#include <coffee/core/terminal/terminal_cursor.h>
#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/image/cimage.h>
#include <coffee/interfaces/content_pipeline.h>

#include <squish.h>

#if defined(HAVE_LIBTIFF)
#include <tiffio.h>
#endif

#define TEXCOMPRESS_API "TextureCooker::"

using namespace CoffeePipeline;

/* Environment variables */
static const constexpr cstring TEX_MAX_SIZE = "TEXCOOK_MAX_SIZE";
static const constexpr cstring TEX_MIN_SIZE = "TEXCOOK_MIN_SIZE";

static i32 max_texture_size = 1024;
static i32 min_texture_size = 128;

static Vector<CString> imageExtensions = {"PNG",
                                          "JPG",
                                          "TGA",
                                          "JPEG",
#if defined(HAVE_LIBTIFF)
                                          "TIF",
                                          "TIFF"
#endif
};

enum ImageProcessor
{
    ImageProc_stb,
    ImageProc_stb_rgb,
    ImageProc_tga,
#if defined(HAVE_LIBTIFF)
    ImageProc_tiff,
#endif
};

struct TextureCooker : FileProcessor
{
    virtual void process(
        Vector<VirtFS::VirtDesc>& files, TerminalCursor& cursor);

    virtual void setBaseDirectories(const Vector<CString>&);
};

static cstring compression_extension(i32 format)
{
    switch(format)
    {
    case squish::kDxt1:
        return "dxt1";
    default:
        return "dxt5";
    }
}

static void CompressDXT(
    Vector<VirtFS::VirtDesc>&            files,
    Pair<CString, ImageProcessor> const& file,
    CSize const&                         size,
    Bytes const&                         inputData,
    Path const&                          outName,
    i32                                  compress,

    TerminalCursor& cursor,
    FileProcessor*  cooker)
{
    bool    fail   = false;
    cstring reason = nullptr;

    if(size.w != size.h)
    {
        fail   = true;
        reason = "{1}:0: Non-square texture is non-optimal: {0}";
    }

    if((size.w % 4) != 0 || (size.h % 4) != 0)
    {
        fail   = true;
        reason = "{1}:0: Inadequate size for S3TC texture: {0}";
    }

    if(fail)
    {
        auto offender = Path(GetFileResourcePrefix()) + file.first;
        cursor.print(reason, size, offender.internUrl);
        return;
    }

    Bytes data = Bytes();
    data.data  = inputData.data;
    data.size  = inputData.size;

    auto imsize  = size;
    auto newSize = imsize;
    while(newSize.w >= min_texture_size && newSize.h >= min_texture_size)
    {
        /* The step will generate a bitmap of size newSize,
         *  imsize is source size */

        auto sizeString = cast_pod(newSize.w);
        auto targetImg  = outName.addExtension(sizeString.c_str())
                             .addExtension(compression_extension(compress));
        auto mipImg = outName.addExtension(sizeString.c_str());

        if(compress == squish::kDxt1)
            mipImg = mipImg.addExtension("jpg");
        else
            mipImg = mipImg.addExtension("png");

        if(cooker->isCached(targetImg) && cooker->isCached(mipImg))
        {
            cursor.progress(
                TEXCOMPRESS_API "Using cache texture: {0}",
                targetImg.internUrl);
            files.emplace_back(targetImg, cooker->getCached(targetImg), 0);
            files.emplace_back(mipImg, cooker->getCached(mipImg), 0);

            imsize = newSize;
        } else if(newSize.w <= max_texture_size || newSize.h <= max_texture_size)
        {
            if(imsize.w != newSize.w)
            {
                auto newData = stb::Resize(
                    stb::CStbImage::From(inputData, size), newSize, 4);
                data    = std::move(newData);
                newData = Bytes();
                imsize  = newSize;
            }

            files.emplace_back(targetImg, Bytes(), 0);
            files.emplace_back(mipImg, Bytes(), 0);

            Bytes& output = files.at(files.size() - 2).data;
            Bytes& pngOutput = files.at(files.size() - 1).data;

            /* Allocate space for image as well as WxH parameters */
            output.size = C_FCAST<szptr>(squish::GetStorageRequirements(
                              imsize.w, imsize.h, squish::kDxt5)) +
                          sizeof(IMG::serial_image);
            output.data = C_RCAST<byte_t*>(Calloc(output.size, 1));
            Bytes::SetDestr(output, [](Bytes& b) { CFree(b.data); });

            cursor.progress(
                TEXCOMPRESS_API "Compressing texture: "
                                "{1}: {2}B (raw)"
                                " -> {0}B (compressed)",
                output.size,
                targetImg.internUrl,
                data.size);
            squish::CompressImage(
                data.data,
                imsize.w,
                imsize.h,
                &output[sizeof(IMG::serial_image)],
                squish::kDxt5);

            IMG::serial_image imgDesc = {};
            imgDesc.size              = imsize.convert<u32>();
            imgDesc.fmt               = PixFmt::S3TC;
            imgDesc.bit_fmt           = BitFmt::Byte;
            imgDesc.comp_fmt          = CompFlags::S3TC_5;

            MemCpy(output.data, &imgDesc, sizeof(IMG::serial_image));

            /* Cache DXT-compressed data */
            cooker->cacheFile(targetImg, output);

            if(compress == squish::kDxt1)
                pngOutput = JPG::Save(stb::image_const::From(data, imsize));
            else
                pngOutput = PNG::Save(stb::image_const::From(data, imsize));

            /* Cache regular image format */
            cooker->cacheFile(mipImg, pngOutput);
        }else
        {
            cursor.progress(
                        TEXCOMPRESS_API "Skipping texture, over max size: {0}",
                        targetImg.internUrl);
            imsize = newSize;
        }

        newSize = imsize / 2;
    }
}

static void CompressTextureSet(
    Vector<VirtFS::VirtDesc>&            files,
    Pair<CString, ImageProcessor> const& file,
    FileProcessor*                       cooker,
    TerminalCursor&                      cursor)
{
    CResources::Resource r(MkUrl(file.first.c_str()));

    auto compress = squish::kDxt5;

    /* Depending on color channels/source format, we opt for DXT1
     *  for storage efficiency. Especially when the image does
     *  not have alpha. */
    if(file.second == ImageProc_stb_rgb)
        compress = squish::kDxt1;

    auto outName = Path(file.first).removeExt();

    PixCmp cmp = PixCmp::RGBA;
    BitFmt bfmt;
    Bytes  data;
    CSize  size;
#if defined(HAVE_LIBTIFF)
    if(file.second != ImageProc_tiff)
#endif
        IMG::Load(std::move(r), cmp, bfmt, data, size);
#if defined(HAVE_LIBTIFF)
    else
        do
        {
            auto pngPath = Path(file.first).removeExt().addExtension("png");

            if(cooker->isCached(pngPath))
            {
                files.emplace_back(
                    pngPath.internUrl.c_str(), cooker->getCached(pngPath), 0);
                IMG::Load(files.back().data, cmp, bfmt, data, size);
                break;
            }

            /* libtiff is used for reading TIFF files, because it's a
             *  not part of stb. We use this opportunity to convert
             *  it into PNG format for packing. */
            auto timg = TIFFOpen(r.resource(), "r");

            if(!timg)
                return;

            TIFFRGBAImage rimg;
            char          emsg[1024] = {};

            if(!TIFFRGBAImageBegin(&rimg, timg, 0, emsg))
            {
                TIFFClose(timg);
                return;
            }

            ::uint32 w, h;
            TIFFGetField(timg, TIFFTAG_IMAGEWIDTH, &w);
            TIFFGetField(timg, TIFFTAG_IMAGELENGTH, &h);

            data.size = w * h * sizeof(u32);
            data.data = CallocT<byte_t>(data.size, 1);

            Bytes::SetDestr(data, [](Bytes& d) { CFree(d.data); });

            if(!TIFFRGBAImageGet(&rimg, C_RCAST<u32*>(data.data), w, h))
            {
                TIFFRGBAImageEnd(&rimg);
                TIFFClose(timg);
                return;
            }

            size.w = C_FCAST<i32>(w);
            size.h = C_FCAST<i32>(h);

            /* This is where we create the PNG file */
            files.emplace_back(
                pngPath.internUrl.c_str(),
                PNG::Save(stb::image_const::From(data, size)),
                0);

            cooker->cacheFile(pngPath, files.back().data);

            TIFFRGBAImageEnd(&rimg);
            TIFFClose(timg);
        } while(false);
#endif

    /* IMG::Load may fail silently, just ignore it */
    if(size.area() == 0)
        return;

    CompressDXT(files, file, size, data, outName, compress, cursor, cooker);
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
            if(StrICmp(path.extension().c_str(), ext.c_str()))
            {
                if(ext == "TGA")
                    targets.push_back({desc.filename, ImageProc_tga});
                else if(ext == "JPG")
                    targets.push_back({desc.filename, ImageProc_stb_rgb});
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
