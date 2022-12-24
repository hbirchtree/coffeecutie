#pragma once

#include "data.h"

#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>

struct memory_budget
{
    static constexpr auto bsp_buffer      = 35_MB;
    static constexpr auto bsp_elements    = 15_MB;
    static constexpr auto mesh_buffer     = 25_MB;
    static constexpr auto mesh_elements   = 15_MB;
    static constexpr auto matrix_buffer   = 16_MB;
    static constexpr auto material_buffer = 16_MB;

    static constexpr auto grand_total = bsp_buffer + bsp_elements + mesh_buffer
                                        + mesh_elements + matrix_buffer
                                        + material_buffer;
};

template<typename Version>
void create_resources(EntityContainer& e, BlamData<Version>& data)
{
    ProfContext _(__FUNCTION__);

    {
        using namespace Display::EventHandlers;
        auto eventhandler
            = e.service<comp_app::BasicEventBus<Input::CIEvent>>();

        eventhandler->addEventHandler(
            0,
            ExitOn<OnKey<Input::CK_Escape>>(
                [window = e.service_ref<comp_app::Windowing>()]() mutable {
                    window.lock()->close();
                }));

        eventhandler->addEventHandler(
            0, std_camera_t::KeyboardInput(data.std_camera));
        eventhandler->addEventHandler(
            0, std_camera_t::MouseInput(&data.camera));

        auto eventhandler_w
            = e.service<comp_app::BasicEventBus<Display::Event>>();

        //        eventhandler_w->addEventHandler(0, WindowResize());
    }

    gfx::api& api = e.subsystem_cast<gfx::system>();

    /* Set up graphics data */
    //    GFX::OPTS options;
    //    options.crash_on_error        = true;
    //    options.old_shader_processing = true;
    //    auto& gfx = e.register_subsystem_inplace<GFX_ALLOC::tag_type,
    //    GFX_ALLOC>(
    //        options, true);

    //    cDebug("Currently loaded with {0}", GFX::Level());

    //    data.bsp_buf = gfx.alloc_buffer<GFX::BUF_A>(
    //        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);
    //    data.bsp_index = gfx.alloc_buffer<GFX::BUF_E>(
    //        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);
    //    data.bsp_light_buf = gfx.alloc_buffer<GFX::BUF_A>(
    //        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);

    //    data.model_buf = gfx.alloc_buffer<GFX::BUF_A>(
    //        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);
    //    data.model_index = gfx.alloc_buffer<GFX::BUF_E>(
    //        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);

    //    data.model_matrix_store = gfx.alloc_buffer<GFX::BUF_S>(
    //        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable,
    //        sizeof(Matf4), 0);

    //    data.material_store = gfx.alloc_buffer<GFX::BUF_S>(
    //        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable,
    //        sizeof(Vecf2) + sizeof(Veci2) + sizeof(u32),
    //        0);

    auto access = RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable;

    data.bsp_buf = api.alloc_buffer(gfx::buffers::vertex, access);
    data.bsp_buf->alloc();
    data.bsp_buf->commit(memory_budget::bsp_buffer);
    data.bsp_index = api.alloc_buffer(gfx::buffers::vertex, access);
    data.bsp_index->alloc();
    data.bsp_index->commit(memory_budget::bsp_elements);
    data.bsp_light_buf = api.alloc_buffer(gfx::buffers::vertex, access);
    data.bsp_light_buf->alloc();
    data.bsp_light_buf->commit(memory_budget::bsp_buffer / 2);

    data.model_buf = api.alloc_buffer(gfx::buffers::vertex, access);
    data.model_buf->alloc();
    data.model_buf->commit(memory_budget::mesh_buffer);
    data.model_index = api.alloc_buffer(gfx::buffers::vertex, access);
    data.model_index->alloc();
    data.model_index->commit(memory_budget::mesh_elements);

    data.model_matrix_store
        = api.alloc_buffer(gfx::buffers::shader_writable, access);
    data.model_matrix_store->alloc();
    data.model_matrix_store->commit(memory_budget::matrix_buffer);
    data.material_store
        = api.alloc_buffer(gfx::buffers::shader_writable, access);
    data.material_store->alloc();
    data.material_store->commit(memory_budget::material_buffer);

    data.bsp_attr                  = api.alloc_vertex_array();
    gfx::vertex_array_t& bsp_array = *data.bsp_attr;
    bsp_array.alloc();

    using vertex_type = std::conditional_t<
        std::is_same_v<Version, blam::xbox_version_t>,
        ::xbox_vertex_type,
        ::vertex_type>;
    using light_vertex_type = std::conditional_t<
        std::is_same_v<Version, blam::xbox_version_t>,
        ::light_xbox_vertex_type,
        ::light_vertex_type>;

    std::array<gfx::vertex_attribute, 6> common_attributes = {{
        gfx::vertex_attribute::from_member(&vertex_type::position),
        gfx::vertex_attribute::from_member(&vertex_type::texcoord),
        gfx::vertex_attribute::from_member(&vertex_type::normal),
        gfx::vertex_attribute::from_member(&vertex_type::binorm),
        gfx::vertex_attribute::from_member(&vertex_type::tangent),
        gfx::vertex_attribute::from_member(
            &light_vertex_type::texcoord, gfx::vertex_float_type),
    }};

    {
        auto& light     = common_attributes.back();
        light.buffer.id = 1;
        if constexpr(std::is_same_v<Version, blam::xbox_version_t>)
            light.value.flags
                = gfx::vertex_attribute::attribute_flags::normalized
                  | gfx::vertex_attribute::attribute_flags::packed;
    }
    for(auto i : Range<u32>(6))
    {
        common_attributes.at(i).index = i;
        bsp_array.add(common_attributes.at(i));
    }
    bsp_array.set_buffer(gfx::buffers::vertex, data.bsp_buf, 0);
    bsp_array.set_buffer(gfx::buffers::vertex, data.bsp_light_buf, 1);
    bsp_array.set_buffer(gfx::buffers::element, data.bsp_index);

    using model_vertex_type = std::conditional_t<
        std::is_same_v<Version, blam::xbox_version_t>,
        blam::vert::mod2_vertex<blam::vert::compressed>,
        blam::vert::mod2_vertex<blam::vert::uncompressed>>;

    data.model_attr                 = api.alloc_vertex_array();
    gfx::vertex_array_t& mod2_array = *data.model_attr;
    mod2_array.alloc();

    common_attributes = {{
        gfx::vertex_attribute::from_member(&model_vertex_type::position),
        gfx::vertex_attribute::from_member(&model_vertex_type::texcoord),
        gfx::vertex_attribute::from_member(&model_vertex_type::normal),
        gfx::vertex_attribute::from_member(&model_vertex_type::binorm),
        gfx::vertex_attribute::from_member(&model_vertex_type::tangent),
    }};

    for(auto i : Range<u32>(5))
    {
        common_attributes.at(i).index = i;
        mod2_array.add(common_attributes.at(i));
    }
    mod2_array.set_buffer(gfx::buffers::vertex, data.model_buf, 0);
    mod2_array.set_buffer(gfx::buffers::element, data.model_index);

    //    data.bitm_cache.allocator = &gfx;
}
