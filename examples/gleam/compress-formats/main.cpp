#include <coffee/core/CApplication>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/interfaces/full_launcher.h>

#include <coffee/core/CDebug>

using namespace Coffee;

using API = RHI::GLEAM::GLEAM_API;
using Ren = Display::RendererInterface;

struct Empty
{
};

i32 coffee_main(i32, cstring_w*)
{
    RuntimeQueue::CreateNewQueue("Main");

    AutoExec<API, Ren, Empty>(
        [](Ren& renderer, Empty*) {


        auto api = API::GetLoadAPI();

        if(!api(true))
        {
            renderer.closeWindow();
            return;
        }

        auto empty = CompFlags::CompressionNone;

        cBasicPrint(R"(
Hardware texture compression support:

Universal formats:
 - ASTC:          {6}
 - ETC2:          {8}

Desktop formats:
 - BC1/S3TC/DXT1: {0}
 - BC3/S3TC/DXT5: {1}
 - BC4/RGTC1:     {2}
 - BC5/RGTC2:     {3}
 - BC6H/BPTC:     {4}
 - BC7/BPTC:      {5}

Mobile formats:
 - ETC1:          {7}
 - ATC:           {9}

 - PVRTC (2bpp):  {10}
 - PVRTC (4bpp):  {11}
 - PVRTC2 (2bpp): {12}
 - PVRTC2 (4bpp): {13}
)",
            /* S3TC/BCn/DXTn formats */
            API::TextureFormatSupport(PixFmt::S3TC, CompFlags::BC1),
            API::TextureFormatSupport(PixFmt::S3TC, CompFlags::BC3),
            API::TextureFormatSupport(PixFmt::S3TC, CompFlags::BC4),
            API::TextureFormatSupport(PixFmt::S3TC, CompFlags::BC5),
            API::TextureFormatSupport(PixFmt::S3TC, CompFlags::BC6H),
            API::TextureFormatSupport(PixFmt::S3TC, CompFlags::BC7),

            /* ASTC can be tested without flags */
            API::TextureFormatSupport(PixFmt::ASTC, empty),

            /* ETC1 and ETC2 are package-deals */
            API::TextureFormatSupport(PixFmt::ETC1, empty),
            API::TextureFormatSupport(PixFmt::ETC2, empty),

            /* ATC is mobile only */
            API::TextureFormatSupport(PixFmt::ATC, empty),

            API::TextureFormatSupport(PixFmt::PVRTC, CompFlags::bpp_2),
            API::TextureFormatSupport(PixFmt::PVRTC, CompFlags::bpp_4),
            API::TextureFormatSupport(PixFmt::PVRTC2, CompFlags::bpp_2),
            API::TextureFormatSupport(PixFmt::PVRTC2, CompFlags::bpp_4)
                    );

        API::UnloadAPI();

        renderer.closeWindow();

    }, [](Ren&, Empty*) {}, [](Ren&, Empty*) {});

        return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
