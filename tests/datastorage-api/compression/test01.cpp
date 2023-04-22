#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/rng.h>

#if defined(COFFEE_COMPRESS_LZ4)
#include <coffee/compression/lz4.h>
#endif

#include <coffee/compression/libz.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/unit_tests/framework.h>

using namespace Coffee::DebugFun;

static const char uncompressed[] =
    R"(Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam elit ex, accumsan at cursus eu, sodales non risus. Proin pellentesque nibh quis nisi malesuada rhoncus. Etiam fringilla augue nec aliquet sagittis. Vivamus augue sapien, tempor sit amet purus a, convallis placerat ligula. In et hendrerit lectus. Phasellus auctor et nisl vel sollicitudin. Sed eget ante vehicula, rhoncus nibh eget, aliquet mauris. Mauris a leo lobortis, commodo justo quis, mattis sapien. Duis fermentum convallis augue sit amet convallis. Donec in cursus risus.
    Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam elit ex, accumsan at cursus eu, sodales non risus. Proin pellentesque nibh quis nisi malesuada rhoncus. Etiam fringilla augue nec aliquet sagittis. Vivamus augue sapien, tempor sit amet purus a, convallis placerat ligula. In et hendrerit lectus. Phasellus auctor et nisl vel sollicitudin. Sed eget ante vehicula, rhoncus nibh eget, aliquet mauris. Mauris a leo lobortis, commodo justo quis, mattis sapien. Duis fermentum convallis augue sit amet convallis. Donec in cursus risus.
    Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam elit ex, accumsan at cursus eu, sodales non risus. Proin pellentesque nibh quis nisi malesuada rhoncus. Etiam fringilla augue nec aliquet sagittis. Vivamus augue sapien, tempor sit amet purus a, convallis placerat ligula. In et hendrerit lectus. Phasellus auctor et nisl vel sollicitudin. Sed eget ante vehicula, rhoncus nibh eget, aliquet mauris. Mauris a leo lobortis, commodo justo quis, mattis sapien. Duis fermentum convallis augue sit amet convallis. Donec in cursus risus.
    Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam elit ex, accumsan at cursus eu, sodales non risus. Proin pellentesque nibh quis nisi malesuada rhoncus. Etiam fringilla augue nec aliquet sagittis. Vivamus augue sapien, tempor sit amet purus a, convallis placerat ligula. In et hendrerit lectus. Phasellus auctor et nisl vel sollicitudin. Sed eget ante vehicula, rhoncus nibh eget, aliquet mauris. Mauris a leo lobortis, commodo justo quis, mattis sapien. Duis fermentum convallis augue sit amet convallis. Donec in cursus risus.
)";

using namespace Coffee;

static std::unique_ptr<Resource> shared_source;
static semantic::Bytes shared_input;

bool alloc_shared_data()
{
    shared_source = std::make_unique<Resource>("tree.blend"_rsc);
    shared_input  = C_OCAST<semantic::Bytes>(*shared_source);

    return true;
}

bool zlib_api()
{
    using namespace libc_types;
    using namespace stl_types;

    PrintingVerbosityLevel() = 5;

    //    auto input = semantic::Bytes::From(uncompressed,
    //    sizeof(uncompressed));
    auto& input = shared_input;

    using Chrono::system_clock;

    system_clock::duration   compress_time;
    system_clock::duration   decompress_time;
    system_clock::time_point start;

    for(auto i : Range<>(10))
    {
        semantic::Bytes compressed;

        Zlib::Opts opts;
        opts.level      = 9;
        opts.chunk_size = 2 * 1024;

        Compression::zlib_error_code ec;
        {
            start = system_clock::now();
            Zlib::Compress(input, &compressed, opts, ec);
            compress_time += system_clock::now() - start;
        }
        C_ERROR_CHECK(ec);

        if(i == 0)
            cDebug(
                "Zlib compression: {0} => {1}, ratio = {2}",
                input.size,
                compressed.size,
                scalar(input.size) / compressed.size);

        semantic::Bytes output;
        {
            start = system_clock::now();
            Zlib::Decompress(compressed, &output, opts, ec);
            decompress_time += system_clock::now() - start;
        }
        C_ERROR_CHECK(ec);

        assertTrue(std::equal(
            input.begin(), input.end(), output.begin(), output.end()));
    }

    cDebug(
        "- Zlib compression time: {0} us",
        Chrono::duration_cast<Chrono::microseconds>(compress_time).count());

    cDebug(
        "- Zlib decompression time: {0} us",
        Chrono::duration_cast<Chrono::microseconds>(decompress_time).count());

    return true;
}

bool lz4_api()
{
#if defined(COFFEE_COMPRESS_LZ4)
    PrintingVerbosityLevel() = 5;

    //    auto input = semantic::Bytes::From(uncompressed,
    //    sizeof(uncompressed));
    auto& input = shared_input;

    using namespace libc_types;
    using namespace stl_types;
    using comp_mode = lz4::compressor::compression_mode;
    using Chrono::system_clock;

    for(auto const& opt :
        Vector<lz4::compressor::opts>{{comp_mode::high, 0, 9},
                                      {comp_mode::fast, 1, 0},
                                      {comp_mode::fast, 9, 0},
                                      {comp_mode::default_, 0, 0}})
    {
        switch(opt.mode)
        {
        case comp_mode::fast:
            cDebug("LZ4 fast mode (level = {0})", opt.fast_acceleration);
            break;
        case comp_mode::high:
            cDebug(
                "LZ4 high compression mode (level = {0})",
                opt.high_compression);
            break;
        case comp_mode::default_:
            cDebug("LZ4 default mode");
            break;
        }

        system_clock::duration   compress_time   = {};
        system_clock::duration   decompress_time = {};
        system_clock::time_point start;

        for(auto i : Range<>(10))
        {
            semantic::Bytes output;

            lz4::error_code ec;
            {
                start = system_clock::now();
                lz4::compressor::Compress(input, &output, opt, ec);
                compress_time += system_clock::now() - start;
            }
            C_ERROR_CHECK(ec);

            if(i == 0)
                cDebug(
                    "LZ4 compression: {0} => {1}, ratio = {2}",
                    input.size,
                    output.size,
                    scalar(input.size) / output.size);

            semantic::Bytes decompressed;
            {
                start = system_clock::now();
                lz4::compressor::Decompress(output, &decompressed, opt, ec);
                decompress_time += system_clock::now() - start;
            }
            C_ERROR_CHECK(ec);

            assertTrue(std::equal(
                input.begin(),
                input.end(),
                decompressed.begin(),
                decompressed.end()));
        }

        cDebug(
            "- LZ4 compression time: {0} us",
            Chrono::duration_cast<Chrono::microseconds>(compress_time).count());

        cDebug(
            "- LZ4 decompression time: {0} us",
            Chrono::duration_cast<Chrono::microseconds>(decompress_time)
                .count());
    }

    return true;

#else
    return true;
#endif
}

bool free_shared_data()
{
    shared_source.release();
    shared_input = {};
    return true;
}

COFFEE_TESTS_BEGIN(4)

    {alloc_shared_data, "Allocate shared test data"},

    {zlib_api, "zlib API"},

    {lz4_api, "LZ4 API"},

    {free_shared_data, "Free shared test data"}

COFFEE_TESTS_END()
