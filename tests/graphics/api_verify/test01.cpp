#include <coffee/core/unit_tests/graphics_framework.h>
#include <coffee/interfaces/cgraphics_api.h>

#include <coffee/graphics/apis/CGLeamRHI>

using namespace Coffee;

template<typename BUFAPI, typename... Args>
bool test_buffer_api(Args... args)
{
    Vecd3 test_vec;

    BUFAPI buffer(args...);

    buffer.alloc();
    buffer.commit(sizeof(Vecf3), &test_vec);

    auto mapped_ptr = buffer.map(0, sizeof(scalar));
    C_UNUSED(mapped_ptr);

    buffer.unmap();
    buffer.dealloc();

    return true;
}

template<typename GFXAPI>
bool test_api()
{
    using A = GFXAPI;

    auto ctxt = A::GetLoadAPI();

    if(!ctxt(true))
        return false;

    auto default_access = RSCA::WriteOnly | RSCA::Persistent;

    /* Vertex buffers */
    test_buffer_api<typename A::BUF_A>(default_access, 100);
    test_buffer_api<typename A::BUF_E>(default_access, 100);

    /* Shader buffers */
    test_buffer_api<typename A::BUF_S>(default_access, sizeof(Vecf3), 100);
    test_buffer_api<typename A::BUF_U>(default_access, sizeof(Vecf3), 100);

    /* Pixel buffer */
    test_buffer_api<typename A::BUF_P>(default_access, 100);

    /* Indirect draw buffer */
    test_buffer_api<typename A::BUF_DRAW>(
                default_access, 0x0, sizeof(Vecf3), 100);

    typename A::V_ATTR vert_attr;

    vert_attr.m_bassoc = 0;
    vert_attr.m_boffset = 0;
    vert_attr.m_flags = 0;
    vert_attr.m_idx = 0;
    vert_attr.m_instanced = false;
    vert_attr.m_off = 0;
    vert_attr.m_size = 3;
    vert_attr.m_stride = sizeof(Vecf3);
    vert_attr.m_type = TypeEnum::Scalar;

    typename A::V_DESC vert_desc;

    typename A::BUF_A test_buffer(default_access, 100);
    typename A::BUF_E test_buffer_e(default_access, 100);
    test_buffer.alloc();
    test_buffer_e.alloc();

    vert_desc.alloc();
    vert_desc.addAttribute(vert_attr);
    vert_desc.bind();

    vert_desc.bindBuffer(0, test_buffer);
    vert_desc.setIndexBuffer(&test_buffer_e);

    vert_desc.dealloc();

    test_buffer.dealloc();
    test_buffer_e.dealloc();

    {
        typename A::S_2D s_2d(PixFmt::RGBA8, 1, 0);
        CSize s = {512, 512};
        CPoint p = {0, 0};

        s_2d.allocate(s, PixCmp::RGBA);
        s = {0, 0};

        Bytes data;
        data.data = nullptr;
        data.size = 0;
        s_2d.upload({BitFmt::Byte, PixCmp::RGBA}, s, data, p, 0);

        u32 size = s_2d.arraySize();
        bool is_array = s_2d.isArray();
        auto fmt = s_2d.format();
        u32 mips = s_2d.mipmaps();

        C_UNUSED(size);
        C_UNUSED(is_array);
        C_UNUSED(fmt);
        C_UNUSED(mips);

        typename A::SM_2D sample_2d;
        sample_2d.alloc();

        sample_2d.attach(&s_2d);
        sample_2d.setFiltering(Filtering::Linear, Filtering::Linear,
                               Filtering::Nearest);
        sample_2d.enableShadowSampler();
        sample_2d.setLODBias(0.f);
        Vecf2 lrng = {0.f, 0.2f};
        sample_2d.setLODRange(lrng);
        sample_2d.setEdgePolicy(0, WrapPolicy::Clamp);
        typename A::UNIFSMP hand = sample_2d.handle();

        C_UNUSED(hand);

        sample_2d.bind(0);

        s_2d.dealloc();
    }

    {
        typename A::S_3D s_2d(PixFmt::RGBA8, 1, 0);
        CSize3 s = {512, 512, 2};
        CPoint3 p = {0, 0, 0};

        s_2d.allocate(s, PixCmp::RGBA);
        s = {0, 0, 0};

        Bytes data;
        data.data = nullptr;
        data.size = 0;
        s_2d.upload({BitFmt::Byte, PixCmp::RGBA}, s, data, p, 0);

        u32 size = s_2d.arraySize();
        bool is_array = s_2d.isArray();
        auto fmt = s_2d.format();
        u32 mips = s_2d.mipmaps();

        C_UNUSED(size);
        C_UNUSED(is_array);
        C_UNUSED(fmt);
        C_UNUSED(mips);

        s_2d.dealloc();
    }

    {
        typename A::S_2DA s_2d(PixFmt::RGBA8, 1, 0);
        CSize3 s = {512, 512, 2};
        CPoint3 p = {0, 0, 0};

        s_2d.allocate(s, PixCmp::RGBA);
        s = {0, 0, 0};

        Bytes data;
        data.data = nullptr;
        data.size = 0;
        s_2d.upload({BitFmt::Byte, PixCmp::RGBA}, s, data, p, 0);

        u32 size = s_2d.arraySize();
        bool is_array = s_2d.isArray();
        auto fmt = s_2d.format();
        u32 mips = s_2d.mipmaps();

        C_UNUSED(size);
        C_UNUSED(is_array);
        C_UNUSED(fmt);
        C_UNUSED(mips);

        s_2d.dealloc();
    }

    {
        typename A::S_Cube s_2d(PixFmt::RGBA8, 1, 0);
        CSize s = {512, 512};
        CPoint p = {0, 0};

        s_2d.allocate(s, PixCmp::RGBA);
        s = {0, 0};

        Bytes data;
        data.data = nullptr;
        data.size = 0;
        s_2d.upload({BitFmt::Byte, PixCmp::RGBA}, s, data, p, 0);

        u32 size = s_2d.arraySize();
        bool is_array = s_2d.isArray();
        auto fmt = s_2d.format();
        u32 mips = s_2d.mipmaps();

        C_UNUSED(size);
        C_UNUSED(is_array);
        C_UNUSED(fmt);
        C_UNUSED(mips);

        s_2d.dealloc();
    }

    {
        typename A::S_CubeA s_2d(PixFmt::RGBA8, 1, 0);
        CSize3 s = {512, 512, 2};
        CPoint3 p = {0, 0, 0};

        s_2d.allocate(s, PixCmp::RGBA);
        s = {0, 0, 0};

        Bytes data;
        data.data = nullptr;
        data.size = 0;
        s_2d.upload({BitFmt::Byte, PixCmp::RGBA}, s, data, p, 0);

        u32 size = s_2d.arraySize();
        bool is_array = s_2d.isArray();
        auto fmt = s_2d.format();
        u32 mips = s_2d.mipmaps();

        C_UNUSED(size);
        C_UNUSED(is_array);
        C_UNUSED(fmt);
        C_UNUSED(mips);

        s_2d.dealloc();
    }

    A::UnloadAPI();

    return true;
}

#if defined(COFFEE_GLEAM_DESKTOP)

#include <coffee/graphics/apis/CGLeam>

bool test_texture_formats()
{
    u32 pfmt_max = C_CAST<u32>(PixFmt::MAX_PIXFMT);

    bool status = true;

    /* First, test all normal formats, excluding compressed ones */
    for(u32 fmt=1; fmt<pfmt_max; fmt++)
    {
        PixFmt f = C_CAST<PixFmt>(fmt);

        auto enval = CGL::to_enum(f);

        if(enval == GL_NONE && !IsPixFmtCompressed(f))
            status = false;
    }

    /* Second, test compressed formats. These have special flags. */

    using P = PixFmt;
    using F = PixelFlags;
    using C = CompFlags;

    using namespace CGL;

    static const constexpr struct comp_fmt_t {
        P fmt;
        F flg;
        C cfl;
        bool expect;
    } compressed_formats[20] = {
        /* S3TC tests */
    { P::S3TC, F::RGB, C::S3TC_1, false },
    { P::S3TC, F::RGBA, C::S3TC_1, false },
    { P::S3TC, F::RGBA, C::S3TC_3, false },
    { P::S3TC, F::RGBA, C::S3TC_5, false },

    { P::S3TC, F::RGB, C::S3TC_3, true },
    { P::S3TC, F::RGB, C::S3TC_5, true },

    { P::BCn, F::RGBA|F::Unormalized, C::BC7, false },
    { P::BCn, F::RGBA|F::sRGB|F::Unormalized, C::BC7, false },
    { P::BCn, F::RGB|PixFlg::Unsigned|F::FloatingPoint, C::BC6H, false },
    { P::BCn, F::RGB|F::FloatingPoint, C::BC6H, false },

    { P::BCn, F::R|F::Unsigned, C::BC4, false },
    { P::BCn, F::R|F::Signed, C::BC4, false },
    { P::BCn, F::RG|F::Unsigned, C::BC5, false },
    { P::BCn, F::RG|F::Signed, C::BC5, false },

    };

    for(auto i : Range<>(20))
    {
        auto const& f = compressed_formats[i];

        auto v = CGL::to_enum(f.fmt, f.flg, f.cfl);

        if((v == GL_NONE) != f.expect)
            status = false;
    }

    /* ASTC is a bit different to test */
    static const constexpr F astc_flags[2] = {
        F::RGBA, F::RGBA | F::sRGB
    };
    for(auto i : Range<>(2))
        for(auto j : Range<>(C_CAST<u32>(C::ASTC_12x12) - 1))
        {
            auto v = CGL::to_enum(PixFmt::ASTC, astc_flags[i],
                                  C_CAST<CompFlags>(j + 1));
            if(v == GL_NONE)
                status = false;
        }

    return status;
}

#endif

#if defined(COFFEE_GLEAM_DESKTOP)
#define TEST_COUNT 3
#else
#define TEST_COUNT 2
#endif

COFFEE_TEST_SUITE(TEST_COUNT) = {
{test_api<RHI::NullAPI>,"Null API", nullptr, false, false},
{test_api<RHI::GLEAM::GLEAM_API>,"GLEAM OpenGL API", nullptr, false, false},
        #if defined(COFFEE_GLEAM_DESKTOP)
{test_texture_formats, "GLEAM pixel format verification"}
        #endif
};

COFFEE_GFX_RUN_TESTS(_tests);
