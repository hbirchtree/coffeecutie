#include <coffee/core/types/type_enum.h>
#include <coffee/core/unit_tests/graphics_framework.h>
#include <coffee/interfaces/cgraphics_api.h>
#include <coffee/interfaces/cgraphics_types.h>

#include <coffee/graphics/apis/CGLeamRHI>

using namespace Coffee;
using namespace RHI::Datatypes;

template<typename BUFAPI, typename... Args>
bool test_buffer_api(Args... args)
{
    Vecd3 test_vec;

    BUFAPI buffer(args...);

    buffer.alloc();
    buffer.commit(sizeof(Vecf3), &test_vec);

    C_UNUSED(auto mapped_ptr = buffer.map(0, sizeof(scalar)));

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

    vert_attr.m_bassoc    = 0;
    vert_attr.m_boffset   = 0;
    vert_attr.m_flags     = 0;
    vert_attr.m_idx       = 0;
    vert_attr.m_instanced = false;
    vert_attr.m_off       = 0;
    vert_attr.m_size      = 3;
    vert_attr.m_stride    = sizeof(Vecf3);
    vert_attr.m_type      = TypeEnum::Scalar;

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
        Size             s = {512, 512};
        Point            p = {0, 0};

        s_2d.allocate(s, PixCmp::RGBA);
        s = {0, 0};

        Bytes data;
        data.data = nullptr;
        data.size = 0;
        s_2d.upload({BitFmt::Byte, PixCmp::RGBA}, s, data, p, 0);

        C_UNUSED(u32 size = s_2d.arraySize());
        C_UNUSED(bool is_array = s_2d.isArray());
        C_UNUSED(auto fmt = s_2d.format());
        C_UNUSED(u32 mips = s_2d.mipmaps());

        typename A::SM_2D sample_2d;
        sample_2d.alloc();

        sample_2d.attach(&s_2d);
        sample_2d.setFiltering(
            Filtering::Linear, Filtering::Linear, Filtering::Nearest);
        sample_2d.enableShadowSampler();
        sample_2d.setLODBias(0.f);
        Vecf2 lrng = {0.f, 0.2f};
        sample_2d.setLODRange(lrng);
        sample_2d.setEdgePolicy(0, WrapPolicy::Clamp);
        C_UNUSED(typename A::UNIFSMP hand = sample_2d.handle());

        sample_2d.bind(0);

        s_2d.dealloc();
        sample_2d.dealloc();
    }

    {
        typename A::S_3D s_2d(PixFmt::RGBA8, 1, 0);
        Size3            s = {512, 512, 2};
        Point3           p = {0, 0, 0};

        s_2d.allocate(s, PixCmp::RGBA);
        s = {0, 0, 0};

        Bytes data;
        data.data = nullptr;
        data.size = 0;
        s_2d.upload({BitFmt::Byte, PixCmp::RGBA}, s, data, p, 0);

        C_UNUSED(u32 size = s_2d.arraySize());
        C_UNUSED(bool is_array = s_2d.isArray());
        C_UNUSED(auto fmt = s_2d.format());
        C_UNUSED(u32 mips = s_2d.mipmaps());

        s_2d.dealloc();
    }

    {
        typename A::S_2DA s_2d(PixFmt::RGBA8, 1, 0);
        Size3             s = {512, 512, 2};
        Point3            p = {0, 0, 0};

        s_2d.allocate(s, PixCmp::RGBA);
        s = {0, 0, 0};

        Bytes data;
        data.data = nullptr;
        data.size = 0;
        s_2d.upload({BitFmt::Byte, PixCmp::RGBA}, s, data, p, 0);

        C_UNUSED(u32 size = s_2d.arraySize());
        C_UNUSED(bool is_array = s_2d.isArray());
        C_UNUSED(auto fmt = s_2d.format());
        C_UNUSED(u32 mips = s_2d.mipmaps());

        s_2d.dealloc();
    }

    {
        typename A::S_Cube s_2d(PixFmt::RGBA8, 1, 0);
        Size               s = {512, 512};
        Point              p = {0, 0};

        s_2d.allocate(s, PixCmp::RGBA);
        s = {0, 0};

        Bytes data;
        data.data = nullptr;
        data.size = 0;
        s_2d.upload({BitFmt::Byte, PixCmp::RGBA}, s, data, p, 0);

        C_UNUSED(u32 size = s_2d.arraySize());
        C_UNUSED(bool is_array = s_2d.isArray());
        C_UNUSED(auto fmt = s_2d.format());
        C_UNUSED(u32 mips = s_2d.mipmaps());

        s_2d.dealloc();
    }

    {
        typename A::S_CubeA s_2d(PixFmt::RGBA8, 1, 0);
        Size3               s = {512, 512, 2};
        Point3              p = {0, 0, 0};

        s_2d.allocate(s, PixCmp::RGBA);
        s = {0, 0, 0};

        Bytes data;
        data.data = nullptr;
        data.size = 0;
        s_2d.upload({BitFmt::Byte, PixCmp::RGBA}, s, data, p, 0);

        C_UNUSED(u32 size = s_2d.arraySize());
        C_UNUSED(bool is_array = s_2d.isArray());
        C_UNUSED(auto fmt = s_2d.format());
        C_UNUSED(u32 mips = s_2d.mipmaps());

        s_2d.dealloc();
    }

    A::UnloadAPI();

    return true;
}

COFFEE_TEST_SUITE(2) = {
    {test_api<RHI::NullAPI>, "Null API", nullptr, false, false},
    {test_api<RHI::GLEAM::GLEAM_API>,
     "GLEAM OpenGL API",
     nullptr,
     false,
     false}};

COFFEE_GFX_RUN_TESTS(_tests);
