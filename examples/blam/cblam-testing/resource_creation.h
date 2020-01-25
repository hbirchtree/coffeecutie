#pragma once

#include "data.h"

struct memory_budget
{
    static constexpr auto bsp_buffer      = 35_MB;
    static constexpr auto bsp_elements    = 15_MB;
    static constexpr auto mesh_buffer     = 25_MB;
    static constexpr auto mesh_elements   = 15_MB;
    static constexpr auto matrix_buffer   = 4_MB;
    static constexpr auto material_buffer = 4_MB;

    static constexpr auto grand_total = bsp_buffer + bsp_elements +
                                        mesh_buffer + mesh_elements +
                                        matrix_buffer + material_buffer;
};

template<typename Version>
void create_resources(EntityContainer& e, BlamData<Version>& data)
{
    ProfContext _(__PRETTY_FUNCTION__);

    {
        using namespace Display::EventHandlers;
        auto eventhandler =
            e.service<comp_app::BasicEventBus<Input::CIEvent>>();

        eventhandler->addEventHandler(
            0,
            ExitOn<OnKey<Input::CK_Escape>>(
                e.service_ref<comp_app::Windowing>()));

        eventhandler->addEventHandler(
            0, std_camera_t::KeyboardInput(data.std_camera));
        eventhandler->addEventHandler(
            0, std_camera_t::MouseInput(&data.camera));

        auto eventhandler_w =
            e.service<comp_app::BasicEventBus<Display::Event>>();

        eventhandler_w->addEventHandler(0, WindowResize<GFX>());
    }

    /* Set up graphics data */
    GFX::OPTS options;
    options.crash_on_error        = true;
    options.old_shader_processing = true;
    auto& gfx = e.register_subsystem_inplace<GFX_ALLOC::tag_type, GFX_ALLOC>(
        options, true);

    cDebug("Currently loaded with {0}", GFX::Level());

    data.bsp_buf = gfx.alloc_buffer<GFX::BUF_A>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);
    data.bsp_index = gfx.alloc_buffer<GFX::BUF_E>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);
    data.bsp_light_buf = gfx.alloc_buffer<GFX::BUF_A>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);

    data.model_buf = gfx.alloc_buffer<GFX::BUF_A>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);
    data.model_index = gfx.alloc_buffer<GFX::BUF_E>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, 0);

    data.model_matrix_store = gfx.alloc_buffer<GFX::BUF_S>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable, sizeof(Matf4), 0);

    data.material_store = gfx.alloc_buffer<GFX::BUF_S>(
        RSCA::ReadWrite | RSCA::Persistent | RSCA::Immutable,
        sizeof(Vecf2) + sizeof(Veci2) + sizeof(u32),
        0);

    data.bsp_buf->commit(memory_budget::bsp_buffer);
    data.bsp_index->commit(memory_budget::bsp_elements);
    data.bsp_light_buf->commit(memory_budget::bsp_buffer / 2);

    data.model_buf->commit(memory_budget::mesh_buffer);
    data.model_index->commit(memory_budget::mesh_elements);

    data.model_matrix_store->commit(memory_budget::matrix_buffer);
    data.material_store->commit(memory_budget::material_buffer);

    GFX::V_ATTR pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr, ltex_attr;
    pos_attr.m_idx  = 0;
    tex_attr.m_idx  = 1;
    nrm_attr.m_idx  = 2;
    bin_attr.m_idx  = 3;
    tan_attr.m_idx  = 4;
    ltex_attr.m_idx = 5;
    if(data.map_container.map->is_xbox())
    {
        pos_attr.m_off  = offsetof(xbox_vertex_type, position);
        tex_attr.m_off  = offsetof(xbox_vertex_type, texcoord);
        nrm_attr.m_off  = offsetof(xbox_vertex_type, normal);
        bin_attr.m_off  = offsetof(xbox_vertex_type, binorm);
        tan_attr.m_off  = offsetof(xbox_vertex_type, tangent);
        ltex_attr.m_off = offsetof(light_xbox_vertex_type, texcoord);
    } else
    {
        pos_attr.m_off  = offsetof(vertex_type, position);
        tex_attr.m_off  = offsetof(vertex_type, texcoord);
        nrm_attr.m_off  = offsetof(vertex_type, normal);
        bin_attr.m_off  = offsetof(vertex_type, binorm);
        tan_attr.m_off  = offsetof(vertex_type, tangent);
        ltex_attr.m_off = offsetof(light_vertex_type, texcoord);
    }
    tex_attr.m_size = ltex_attr.m_size = 2;

    pos_attr.m_size = nrm_attr.m_size = bin_attr.m_size = tan_attr.m_size = 3;
    if(data.map_container.map->is_xbox())
    {
        pos_attr.m_type = tex_attr.m_type = semantic::TypeEnum::Scalar;
        nrm_attr.m_type = bin_attr.m_type = tan_attr.m_type =
            semantic::TypeEnum::Packed_UFloat;
        ltex_attr.m_type  = semantic::TypeEnum::UShort;
        ltex_attr.m_flags = GFX::AttributeNormalization;

        pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
            bin_attr.m_stride = tan_attr.m_stride = sizeof(xbox_vertex_type);
        ltex_attr.m_stride = sizeof(light_xbox_vertex_type);
    } else
    {
        pos_attr.m_type = tex_attr.m_type = nrm_attr.m_type = bin_attr.m_type =
            tan_attr.m_type = ltex_attr.m_type = semantic::TypeEnum::Scalar;
        pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
            bin_attr.m_stride = tan_attr.m_stride = sizeof(vertex_type);
        ltex_attr.m_stride                        = sizeof(light_vertex_type);
    }
    pos_attr.m_bassoc = tex_attr.m_bassoc = nrm_attr.m_bassoc =
        bin_attr.m_bassoc = tan_attr.m_bassoc = 0;
    ltex_attr.m_bassoc                        = 1;

    data.bsp_attr = gfx.alloc_desc<6>(
        {{pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr, ltex_attr}});
    data.bsp_attr->setIndexBuffer(data.bsp_index.get());
    data.bsp_attr->bindBuffer(0, *data.bsp_buf);
    data.bsp_attr->bindBuffer(1, *data.bsp_light_buf);

    using model_vertex_type = blam::vert::mod2_vertex<blam::vert::uncompressed>;
    using xbox_model_vertex_type =
        blam::vert::mod2_vertex<blam::vert::compressed>;

    if(data.map_container.map->is_xbox())
    {
        tex_attr.m_type = semantic::TypeEnum::Short;
        tex_attr.m_flags =
            GLEAMAPI::AttributePacked | GLEAMAPI::AttributeNormalization;

        pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
            bin_attr.m_stride                 = tan_attr.m_stride =
                sizeof(xbox_model_vertex_type);
    } else
    {
        pos_attr.m_stride = tex_attr.m_stride = nrm_attr.m_stride =
            bin_attr.m_stride = tan_attr.m_stride = sizeof(model_vertex_type);
    }

    data.model_attr =
        gfx.alloc_desc<5>({{pos_attr, tex_attr, nrm_attr, bin_attr, tan_attr}});
    data.model_attr->setIndexBuffer(data.model_index.get());
    data.model_attr->bindBuffer(0, *data.model_buf);

    data.bitm_cache.allocator = &gfx;
}
