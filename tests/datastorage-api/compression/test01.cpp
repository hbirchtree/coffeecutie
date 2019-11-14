#include <coffee/core/CApplication>
#include <peripherals/semantic/chunk.h>

#if defined(COFFEE_COMPRESS_LZ4)
#include <coffee/compression/lz4.h>
#endif

#include <coffee/strings/libc_types.h>

#include <coffee/core/unit_tests/framework.h>

using namespace Coffee::DebugFun;

static const char uncompressed[] =
    R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam elit ex, accumsan at cursus eu, sodales non risus. Proin pellentesque nibh quis nisi malesuada rhoncus. Etiam fringilla augue nec aliquet sagittis. Vivamus augue sapien, tempor sit amet purus a, convallis placerat ligula. In et hendrerit lectus. Phasellus auctor et nisl vel sollicitudin. Sed eget ante vehicula, rhoncus nibh eget, aliquet mauris. Mauris a leo lobortis, commodo justo quis, mattis sapien. Duis fermentum convallis augue sit amet convallis. Donec in cursus risus.
    Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam elit ex, accumsan at cursus eu, sodales non risus. Proin pellentesque nibh quis nisi malesuada rhoncus. Etiam fringilla augue nec aliquet sagittis. Vivamus augue sapien, tempor sit amet purus a, convallis placerat ligula. In et hendrerit lectus. Phasellus auctor et nisl vel sollicitudin. Sed eget ante vehicula, rhoncus nibh eget, aliquet mauris. Mauris a leo lobortis, commodo justo quis, mattis sapien. Duis fermentum convallis augue sit amet convallis. Donec in cursus risus.
    Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam elit ex, accumsan at cursus eu, sodales non risus. Proin pellentesque nibh quis nisi malesuada rhoncus. Etiam fringilla augue nec aliquet sagittis. Vivamus augue sapien, tempor sit amet purus a, convallis placerat ligula. In et hendrerit lectus. Phasellus auctor et nisl vel sollicitudin. Sed eget ante vehicula, rhoncus nibh eget, aliquet mauris. Mauris a leo lobortis, commodo justo quis, mattis sapien. Duis fermentum convallis augue sit amet convallis. Donec in cursus risus.
    Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam elit ex, accumsan at cursus eu, sodales non risus. Proin pellentesque nibh quis nisi malesuada rhoncus. Etiam fringilla augue nec aliquet sagittis. Vivamus augue sapien, tempor sit amet purus a, convallis placerat ligula. In et hendrerit lectus. Phasellus auctor et nisl vel sollicitudin. Sed eget ante vehicula, rhoncus nibh eget, aliquet mauris. Mauris a leo lobortis, commodo justo quis, mattis sapien. Duis fermentum convallis augue sit amet convallis. Donec in cursus risus.
)";

bool zlib_api()
{
    Coffee::PrintingVerbosityLevel() = 5;

    auto input = semantic::Bytes::From(uncompressed, sizeof(uncompressed));

    return true;
}

bool lz4_api()
{
    using namespace libc_types;
    using namespace stl_types;

    Coffee::PrintingVerbosityLevel() = 5;

    using comp_mode = lz4::compressor::compression_mode;

#if defined(COFFEE_COMPRESS_LZ4)
    auto input = semantic::Bytes::From(uncompressed, sizeof(uncompressed));

    for(auto const& opt : Vector<lz4::compressor::opts>{
            {comp_mode::high, 0, 9}, {comp_mode::fast, 0, 9}})
    {
        semantic::Bytes output;

        lz4::lz4_error_code ec;
        assertTrue(lz4::compressor::Compress(input, &output, opt, ec));
        C_ERROR_CHECK(ec);

        cDebug(
            "LZ4 compression: {0} => {1}, ratio = {2}",
            input.size,
            output.size,
            scalar(input.size) / output.size);

        semantic::Bytes decompressed;
        assertTrue(lz4::compressor::Decompress(output, &decompressed, opt, ec));
        C_ERROR_CHECK(ec);

        assertTrue(std::equal(
            input.begin(),
            input.end(),
            decompressed.begin(),
            decompressed.end()));
    }

    return true;

#else
    return true;
#endif
}

COFFEE_TESTS_BEGIN(2)

    {zlib_api, "zlib API"},
    {lz4_api, "LZ4 API"}

COFFEE_TESTS_END()
