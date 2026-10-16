#include "h3_scene.h"

#include "components.h"
#include "data.h"
#include "physics.h"
#include "resource_creation.h"
#include "sounds.h"

#include <blam/elegy/bitmap.h>
#include <blam/elegy/shader.h>
#include <blam/elegy/structure.h>
#include <blam/volta/blam_bitm.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include <coffee/components/proxy.h>
#include <coffee/components/subsystem.h>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_buffer.h>
#include <coffee/graphics/apis/gleam/rhi_system.h>
#include <coffee/graphics/apis/gleam/rhi_texture.h>

#include <glm/gtc/matrix_inverse.hpp>

#include <bit>
#include <limits>
#include <unordered_map>

void update_camera_aspect(compo::EntityContainer& e);

namespace h3 {

namespace el = blam::elegy;
using semantic::BytesConst;
using semantic::Span;

/* The pixel formats Halo 1/2 already upload, by Halo 3 format */
static std::optional<blam::bitm::format_t> h1_format(el::bitmap_format_t format)
{
    using blam::bitm::format_t;
    using enum el::bitmap_format_t;
    switch(format)
    {
    case dxt1:
        return format_t::BC1;
    case dxt3:
        return format_t::BC2;
    case dxt5:
        return format_t::BC3;
    case a8:
        return format_t::A8;
    case y8:
        return format_t::Y8;
    case ay8:
        return format_t::AY8;
    case a8y8:
        return format_t::A8Y8;
    case r5g6b5:
        return format_t::R5G6B5;
    case a4r4g4b4:
        return format_t::ARGB4;
    case a8r8g8b8:
        return format_t::ARGB8;
    case x8r8g8b8:
        return format_t::XRGB8;
    case v8u8:
        return format_t::V8U8;
    default:
        return std::nullopt;
    }
}

/* The 360's one- and two-channel block formats: BC4/BC5 plus where the
 * channels go */
struct rgtc_t
{
    typing::pixels::PixDesc                    desc;
    std::array<gfx::textures::swizzle_t, 4>    swizzle;
};

static std::optional<rgtc_t> rgtc_format(el::bitmap_format_t format)
{
    using enum el::bitmap_format_t;
    using gfx::textures::swizzle_t;
    using typing::pixels::CompFmt;
    using typing::pixels::comp_flags;
    using typing::pixels::pix_fmt;
    auto bc4 = CompFmt(pix_fmt::BCn, comp_flags::BC4);
    auto bc5 = CompFmt(pix_fmt::BCn, comp_flags::BC5);
    constexpr auto r = swizzle_t::red, g = swizzle_t::green, one = swizzle_t::one,
                   zero = swizzle_t::zero;
    switch(format)
    {
    case dxt5a:
    case dxt5a_mono:
        return rgtc_t{bc4, {r, r, r, one}};
    case dxt5a_alpha:
        return rgtc_t{bc4, {one, one, one, r}};
    case dxn:
        return rgtc_t{bc5, {r, g, zero, one}};
    case dxn_mono_alpha:
        return rgtc_t{bc5, {r, r, r, g}};
    default:
        return std::nullopt;
    }
}

/* Parses a Halo 3 cache into H3Scene: the starting structure BSP from the
 * scenario lightmap, its instances and the scenery placed in it. Event
 * driven, no frame work */
struct H3MapLoader : compo::SubsystemBase
{
    using type = H3MapLoader;

    H3MapLoader(compo::EntityContainer& e, H3Scene& scene)
        : m_container(e)
        , m_scene(scene)
    {
    }

    /* True if the file is a Halo 3 cache; it is then loaded (or at least
     * attempted) here and no other loader must see it */
    bool on_map_load(MapLoadEvent const& load)
    {
        if(!load.file)
            return false;
        /* Web files arrive asynchronously; not wired up for Halo 3 yet */
        if(compile_info::platform::is_emscripten && !load.file_data)
            return false;
        auto& mapper = m_container.subsystem_cast<comp_app::FileMapper>();
        auto  file   = load.file_data ? load.file_data : mapper.fetch(*load.file).get();
        auto  bytes  = file ? static_cast<BytesConst>(*file) : BytesConst{};
        if(!is_halo3_cache(bytes))
        {
            unload();
            return false;
        }
        m_file = file;
        return load_cache(load, bytes);
    }

    static bool is_halo3_cache(BytesConst const& bytes)
    {
        if(!bytes.data || bytes.size < sizeof(el::file_header_t))
            return false;
        auto const* header = reinterpret_cast<el::file_header_t const*>(bytes.data);
        return header->valid() && header->version.value() == el::halo3_version;
    }

    void unload()
    {
        m_scene.loaded = false;
        m_scene.materials.clear();
        m_vertices.clear();
        m_indices.clear();
        m_material_index.clear();
        m_textures.clear();
        m_side_maps.clear();
        m_side_files.clear();
        m_loader.reset();
        m_map.reset();
        m_file.reset();
    }

  private:
    bool load_cache(MapLoadEvent const& load, BytesConst const& bytes)
    {
        unload();
        /* Same teardown as the other loaders: the previous world goes away */
        m_container.remove_entity_if([](compo::Entity const& en) {
            return stl_types::any_flag_of(en.tags, ObjectGC);
        });
        if(auto* sound_bus = m_container.service<comp_app::EventBus<SoundEvent>>())
        {
            SoundEvent ev{.type = SoundEvent::clear_all};
            sound_bus->inject(ev, nullptr);
        }
        {
            Physics::Event reset_ev{Physics::Event::Reset};
            Physics::Reset reset{};
            m_container.subsystem_cast<PhysicsBus>().process(reset_ev, &reset);
        }
        ensure_gpu();
        m_scene.generation++;

        auto map = el::map_container::from_bytes(bytes);
        if(!map.has_value() || !map->has_tags())
        {
            cWarning("H3: not a loadable Halo 3 cache");
            return false;
        }
        m_map    = std::make_unique<el::map_container>(map.value());
        auto res = el::resource_loader::create(*m_map);
        if(!res)
        {
            cWarning("H3: no resource tables");
            return false;
        }
        m_loader = std::make_unique<el::resource_loader>(*res);
        attach_side_caches(load);

        bool ok = load_world();
        m_loader->trim();
        if(!ok)
        {
            cWarning("H3: nothing loaded from {}", load.file->internUrl);
            return false;
        }
        if(auto window = m_container.service<comp_app::WindowInfo>())
            window->setName(fmt::format("Blam! : {}", m_map->name()));
        m_scene.loaded = true;
        /* The Halo 1 systems key off LoadingStatus; only the app state says
         * "loaded" (which is what the dummy plug waits for) */
        if(auto* app_info = m_container.service<comp_app::AppInfo>())
            app_info->setState(comp_app::interfaces::AppInfo::loaded);
        return true;
    }

    /* Pages may live in mainmenu/shared/campaign.map next to the map */
    void attach_side_caches(MapLoadEvent const& load)
    {
        auto& mapper = m_container.subsystem_cast<comp_app::FileMapper>();
        for(auto const& cache : m_loader->external_caches())
        {
            auto name = std::string(cache.file_name());
            auto url  = (load.file->path().dirname() / name).url(load.file->flags);
            auto file = mapper.fetch(url).get();
            auto bytes = file ? static_cast<BytesConst>(*file) : BytesConst{};
            if(!bytes.data || !bytes.size)
            {
                cWarning("H3: {} not found, its pages are skipped", name);
                continue;
            }
            auto side = el::map_container::from_bytes(bytes);
            if(!side.has_value())
                continue;
            m_side_maps.push_back(std::make_unique<el::map_container>(side.value()));
            m_side_files.push_back(file);
            m_loader->attach(name, *m_side_maps.back());
        }
    }

    void ensure_gpu()
    {
        if(m_api)
            return;
        m_api     = &m_container.subsystem_cast<gfx::system>();
        auto& api = *m_api;
        m_scene.vertex_buf =
            api.alloc_buffer(gfx::buffers::vertex, semantic::RSCA::ReadOnly);
        m_scene.element_buf =
            api.alloc_buffer(gfx::buffers::element, semantic::RSCA::ReadOnly);
        m_scene.vertex_buf->alloc();
        m_scene.element_buf->alloc();
    }

    bool load_world()
    {
        auto const& map   = *m_map;
        auto        magic = map.magic();
        auto const* scnr_tag = map.find("scnr");
        auto const* scnr = scnr_tag ? map.tag_data<el::scenario_t>(*scnr_tag) : nullptr;
        if(!scnr)
            return false;

        /* Fallback slot for parts whose render method is not usable */
        m_scene.materials.push_back(Material{.name = "(fallback)"});

        Vecf3 eye{0.f, 0.f, 1.f};
        f32   yaw = 0.f;
        i16   bsp = 0;
        auto  starts = scnr->starting_locations.data(magic).value_or(
            Span<el::starting_location_t const>());
        if(!starts.empty())
        {
            eye = starts[0].position.value() + Vecf3{0.f, 0.f, 0.6f};
            yaw = starts[0].facing.value().x;
            bsp = std::max<i16>(starts[0].bsp.value(), 0);
        }

        if(!load_bsp(bsp))
            return false;
        load_scenery(*scnr, bsp);
        if(m_vertices.empty())
            return false;

        m_scene.vertex_buf->commit(
            Span<const Vertex>(m_vertices.data(), m_vertices.size()));
        m_scene.element_buf->commit(Span<const u16>(m_indices.data(), m_indices.size()));
        size_t draws = 0;
        for(auto const& mat : m_scene.materials)
            draws += mat.draws.size();
        cDebug(
            "H3: {}: bsp {}, {} vertices, {} indices, {} materials, {} draws, {} "
            "textures",
            map.name(),
            bsp,
            m_vertices.size(),
            m_indices.size(),
            m_scene.materials.size(),
            draws,
            m_textures.size());
        m_vertices = {};
        m_indices  = {};

        cDebug("H3: camera at ({}, {}, {}), yaw {}", eye.x, eye.y, eye.z, yaw);
        place_camera(eye, yaw);
        return true;
    }

    /* The structure BSP's render geometry lives in the scenario lightmap:
     * clusters in world space, instances placed from their definitions */
    bool load_bsp(i16 bsp_index)
    {
        auto const& map   = *m_map;
        auto        magic = map.magic();
        auto const* scnr  = map.tag_data<el::scenario_t>(*map.find("scnr"));
        auto bsps = scnr->bsps.data(magic).value_or(Span<el::scenario_bsp_t const>());
        if(static_cast<size_t>(bsp_index) >= bsps.size())
            return false;
        auto const* bsp_tag = map.tag_at(bsps[bsp_index].bsp.id);
        auto const* bsp = bsp_tag ? map.tag_data<el::structure_bsp_t>(*bsp_tag) : nullptr;
        if(!bsp)
            return false;

        el::lightmap_bsp_t const* lit = nullptr;
        for(auto const& tag : map.tags())
        {
            if(!tag.valid() || !map.is(tag, "sLdT"))
                continue;
            auto const* lightmap = map.tag_data<el::scenario_lightmap_t>(tag);
            for(auto const& entry : lightmap ? lightmap->bsps.data(magic).value_or(
                                                   Span<el::lightmap_bsp_t const>())
                                             : Span<el::lightmap_bsp_t const>())
                if(entry.bsp.value() == bsp_index)
                    lit = &entry;
            if(lit)
                break;
        }
        if(!lit)
        {
            cWarning("H3: no lightmap geometry for bsp {}", bsp_index);
            return false;
        }

        auto view = m_loader->load(lit->geometry.resource);
        if(!view.has_value())
        {
            cWarning("H3: bsp geometry resource failed: {}", static_cast<int>(view.error()));
            return false;
        }
        auto const& geometry    = lit->geometry;
        auto        meshes      = geometry.meshes.data(magic).value_or(Span<el::mesh_t const>());
        auto        compression = geometry.compression.data(magic).value_or(
            Span<el::compression_t const>());
        auto materials = bsp->materials.data(magic).value_or(
            Span<el::geometry_material_t const>());

        u32 clusters = 0, instances = 0;
        for(auto const& cluster : bsp->clusters.data(magic).value_or(
                Span<el::cluster_t const>()))
            if(auto const* mesh = cluster.mesh.resolve(meshes))
                clusters += append_mesh(view.value(), *mesh, nullptr, Matf4(1.f), materials) > 0;

        auto resources = m_loader->load(bsp->tag_resources);
        Span<el::instanced_geometry_definition_t const> definitions;
        if(resources.has_value())
            if(auto const* root = resources->root_as<el::structure_bsp_resources_t>())
                definitions = resources->block(root->instanced_geometry);
        for(auto const& instance : bsp->instances.data(magic).value_or(
                Span<el::instanced_geometry_t const>()))
        {
            auto const* definition = instance.definition.resolve(definitions);
            if(!definition)
                continue;
            auto const* mesh = definition->mesh.resolve(meshes);
            if(!mesh)
                continue;
            f32   scale = instance.scale;
            Matf4 transform(1.f);
            transform[0] = Vecf4(instance.forward.value() * scale, 0.f);
            transform[1] = Vecf4(instance.left.value() * scale, 0.f);
            transform[2] = Vecf4(instance.up.value() * scale, 0.f);
            transform[3] = Vecf4(instance.position.value(), 1.f);
            instances += append_mesh(
                             view.value(),
                             *mesh,
                             definition->compression.resolve(compression),
                             transform,
                             materials) > 0;
        }
        cDebug(
            "H3: {}: {} clusters, {} instances",
            map.name_of(*bsp_tag),
            clusters,
            instances);
        return clusters + instances > 0;
    }

    /* Scenery placed in this BSP, first permutation of every region */
    void load_scenery(el::scenario_t const& scnr, i16 bsp_index)
    {
        auto const& map   = *m_map;
        auto        magic = map.magic();
        auto palette = scnr.scenery_palette.data(magic).value_or(
            Span<el::palette_entry_t const>());
        u32 placed = 0, skipped = 0, hidden = 0;
        std::unordered_map<u32, std::optional<el::resource_view>> views;
        for(auto const& placement : scnr.scenery.data(magic).value_or(
                Span<el::scenery_placement_t const>()))
        {
            auto const& object = placement.object;
            u16 flags = object.bsp_flags;
            if(flags && !(flags & (1u << bsp_index)))
                continue;
            auto const* entry = object.palette.resolve(palette);
            auto const* mode_tag = entry ? render_model_of(entry->object) : nullptr;
            auto const* model = mode_tag ? map.tag_data<el::render_model_t>(*mode_tag) : nullptr;
            if(!model)
            {
                hidden++;
                continue;
            }
            u32 key = map.id_of(*mode_tag).raw.value();
            auto it = views.find(key);
            if(it == views.end())
            {
                auto view = m_loader->load(model->geometry.resource);
                it = views.emplace(key, view.has_value() ? std::optional(view.value())
                                                         : std::nullopt).first;
            }
            if(!it->second)
            {
                skipped++;
                continue;
            }

            Vecf3 rot   = object.rotation;
            Quatf q     = Quatf(Vecf3(0, 0, 1) * rot.x) * Quatf(Vecf3(0, 1, 0) * rot.y) *
                      Quatf(Vecf3(1, 0, 0) * rot.z);
            f32   scale = object.scale;
            if(scale <= 0.f)
                scale = 1.f;
            Matf4 transform = glm::translate(Matf4(1.f), object.position.value()) *
                              glm::mat4_cast(q) * glm::scale(Matf4(1.f), Vecf3(scale));

            auto meshes = model->geometry.meshes.data(magic).value_or(Span<el::mesh_t const>());
            auto compression = model->geometry.compression.data(magic).value_or(
                Span<el::compression_t const>());
            auto materials = model->materials.data(magic).value_or(
                Span<el::geometry_material_t const>());
            u32 emitted = 0;
            for(auto const& region : model->regions.data(magic).value_or(
                    Span<el::region_t const>()))
            {
                auto perms = region.permutations.data(magic).value_or(
                    Span<el::permutation_t const>());
                if(perms.empty() || !perms[0].mesh.valid())
                    continue;
                i16 first = perms[0].mesh.value();
                i16 count = std::max<i16>(perms[0].mesh_count.value(), 1);
                for(i16 m = first; m < first + count && static_cast<size_t>(m) < meshes.size(); m++)
                    emitted += append_mesh(
                        *it->second,
                        meshes[m],
                        compression.empty() ? nullptr : &compression[0],
                        transform,
                        materials);
            }
            if(emitted)
                placed++;
            else
                skipped++;
        }
        cDebug(
            "H3: scenery: {} placed, {} skipped, {} editor-only or modelless",
            placed,
            skipped,
            hidden);
    }

    el::tag_t const* render_model_of(el::tag_reference const& object_ref) const
    {
        auto const& map = *m_map;
        auto const* object_tag = map.tag_at(object_ref.id);
        auto const* object = object_tag ? map.tag_data<el::object_t>(*object_tag) : nullptr;
        if(object && object->editor_only(map.magic()))
            return nullptr;
        auto const* model_tag = object ? map.tag_at(object->model.id) : nullptr;
        auto const* model = model_tag ? map.tag_data<el::model_t>(*model_tag) : nullptr;
        auto const* mode_tag = model ? map.tag_at(model->render_model.id) : nullptr;
        return mode_tag && map.is(*mode_tag, "mode") ? mode_tag : nullptr;
    }

    /* A mesh's triangles, one draw per part, vertices moved by `transform` */
    u32 append_mesh(
        el::resource_view const&              view,
        el::mesh_t const&                     mesh,
        el::compression_t const*              compression,
        Matf4 const&                          transform,
        Span<el::geometry_material_t const>   materials)
    {
        auto magic   = m_map->magic();
        auto buffers = el::mesh_buffers::from(view, mesh);
        m_decoded.clear();
        if(!buffers.decode(compression, m_decoded) || m_decoded.empty() ||
           m_decoded.size() > std::numeric_limits<u16>::max())
            return 0;
        u32  vbase  = static_cast<u32>(m_vertices.size());
        auto normal = glm::mat3(glm::inverseTranspose(transform));
        for(auto const& v : m_decoded)
            m_vertices.push_back(Vertex{
                .position = Vecf3(transform * Vecf4(v.position, 1.f)),
                .texcoord = v.texcoord,
                .normal   = glm::normalize(normal * v.normal),
            });

        auto parts = mesh.parts.data(magic).value_or(Span<el::part_t const>());
        u32  emitted = 0;
        auto emit = [&](i16 material, u32 start, u32 count) {
            m_triangles.clear();
            buffers.triangles(mesh.index_type, start, count, m_triangles);
            if(m_triangles.empty())
                return;
            auto const* method = material >= 0 && static_cast<size_t>(material) < materials.size()
                                     ? &materials[material].render_method
                                     : nullptr;
            auto& mat = m_scene.materials[material_for(method)];
            mat.draws.push_back({
                .elements =
                    {
                        .count         = static_cast<u32>(m_triangles.size() * 3),
                        .offset        = m_indices.size() * sizeof(u16),
                        .vertex_offset = vbase,
                        .type          = semantic::type_t::u16,
                    },
                .instances = {.count = 1, .offset = 0},
            });
            for(auto const& t : m_triangles)
                m_indices.insert(m_indices.end(), t.begin(), t.end());
            emitted++;
        };
        if(parts.empty())
            emit(-1, 0, buffers.index_count());
        for(auto const& part : parts)
            emit(part.material, part.index_start, part.index_count);
        return emitted;
    }

    /* Material slot for a render method, built on first use */
    size_t material_for(el::tag_reference const* method_ref)
    {
        auto const& map   = *m_map;
        auto        magic = map.magic();
        auto const* tag   = method_ref ? map.tag_at(method_ref->id) : nullptr;
        auto const* method = tag ? map.tag_data<el::render_method_t>(*tag) : nullptr;
        if(!method)
            return 0;
        u32 key = method_ref->id.raw.value();
        if(auto it = m_material_index.find(key); it != m_material_index.end())
            return it->second;

        Material mat{.name = std::string(map.name_of(*tag))};
        auto posts = method->postprocess.data(magic).value_or(
            Span<el::render_method_postprocess_t const>());
        if(!posts.empty())
        {
            auto const& post = posts[0];
            Span<el::string_id const> names, float_names;
            if(auto const* tmpl_tag = map.tag_at(post.shader_template.id))
                if(auto const* tmpl = map.tag_data<el::render_method_template_t>(*tmpl_tag))
                {
                    names = tmpl->textures.data(magic).value_or(Span<el::string_id const>());
                    float_names = tmpl->float_constants.data(magic).value_or(
                        Span<el::string_id const>());
                }
            auto textures = post.textures.data(magic).value_or(
                Span<el::postprocess_texture_t const>());
            auto floats = post.float_constants.data(magic).value_or(
                Span<el::be<Vecf4> const>());
            for(size_t i = 0; i < floats.size() && i < float_names.size(); i++)
                if(map.string(float_names[i]) == "albedo_color")
                {
                    /* Halo 3 shades in linear space; the stand-in shader
                     * works on gamma-space texels */
                    Vecf4 c  = floats[i];
                    mat.tint = Vecf4(glm::pow(glm::max(Vecf3(c), Vecf3(0.f)), Vecf3(1.f / 2.2f)), c.w);
                }
            /* Terrain names its layers base_map_m_0 and so on */
            for(size_t i = 0; i < textures.size() && i < names.size(); i++)
            {
                if(!map.string(names[i]).starts_with("base_map"))
                    continue;
                auto const* bitmap = map.tag_at(textures[i].bitmap.id);
                auto        tex    = bitmap ? texture_for(*bitmap) : std::nullopt;
                if(!tex)
                    cDebug(
                        "H3: {}: base map {} not uploaded",
                        mat.name,
                        bitmap ? map.name_of(*bitmap) : std::string_view("(none)"));
                if(tex)
                {
                    mat.diffuse = *tex;
                    auto transform = textures[i].transform;
                    if(transform >= 0 && static_cast<size_t>(transform) < floats.size())
                    {
                        Vecf4 st     = floats[transform];
                        mat.uv_scale = {st.x, st.y};
                    }
                }
                break;
            }
        }
        option_flags(*method, mat);
        /* Helpers like z_invis_lightquad carry only an alpha test map */
        mat.visible = mat.diffuse.sampler || mat.constant_color;
        m_scene.materials.push_back(std::move(mat));
        size_t idx = m_scene.materials.size() - 1;
        m_material_index.emplace(key, idx);
        return idx;
    }

    /* Alpha test and additive blending from the definition's options */
    void option_flags(el::render_method_t const& method, Material& mat)
    {
        auto const& map   = *m_map;
        auto        magic = map.magic();
        auto const* def_tag = map.tag_at(method.definition.id);
        auto const* def = def_tag ? map.tag_data<el::render_method_definition_t>(*def_tag)
                                  : nullptr;
        if(!def)
            return;
        auto picked = method.options.data(magic).value_or(Span<el::be<i16> const>());
        auto categories = def->categories.data(magic).value_or(
            Span<el::render_method_category_t const>());
        for(size_t i = 0; i < categories.size() && i < picked.size(); i++)
        {
            auto options = categories[i].options.data(magic).value_or(
                Span<el::render_method_option_t const>());
            i16 pick = picked[i];
            if(pick < 0 || static_cast<size_t>(pick) >= options.size())
                continue;
            auto category = map.string(categories[i].name);
            auto option   = map.string(options[pick].name);
            if(category == "albedo")
                mat.constant_color = option == "constant_color";
            else if(category == "alpha_test")
                mat.alpha_test = option != "none";
            else if(category == "blend_mode")
            {
                mat.additive = option == "additive";
                mat.blended  = option == "alpha_blend" || option == "pre_multiplied_alpha";
            }
        }
    }

    /* Top level of a bitmap's first image, uploaded once per tag */
    std::optional<Texture> texture_for(el::tag_t const& bitmap_tag)
    {
        auto const& map = *m_map;
        auto        key = map.id_of(bitmap_tag).raw.value();
        if(auto it = m_textures.find(key); it != m_textures.end())
            return it->second;
        auto out = upload_bitmap(bitmap_tag);
        m_textures.emplace(key, out);
        return out;
    }

    std::optional<Texture> upload_bitmap(el::tag_t const& bitmap_tag)
    {
        auto const& map    = *m_map;
        auto        magic  = map.magic();
        auto const* bitmap = map.tag_data<el::bitmap_t>(bitmap_tag);
        if(!bitmap)
            return std::nullopt;
        auto images = bitmap->images.data(magic).value_or(Span<el::bitmap_image_t const>());
        auto hardware = bitmap->hardware_textures.data(magic).value_or(
            Span<el::hardware_texture_t const>());
        if(images.empty() || hardware.empty())
            return std::nullopt;
        auto const& image  = images[0];
        auto        format = image.format.value();
        auto        h1     = h1_format(format);
        auto        rgtc   = rgtc_format(format);
        if(image.type.value() != el::bitmap_type_t::texture_2d || (!h1 && !rgtc))
        {
            cDebug(
                "H3: bitmap {}: {} type {} not handled",
                map.name_of(bitmap_tag),
                el::to_string(format),
                static_cast<u16>(image.type.value()));
            return std::nullopt;
        }
        auto view = m_loader->load(hardware[0].resource);
        if(!view.has_value())
            return std::nullopt;
        u32 width  = static_cast<u32>(image.width.value());
        u32 height = static_cast<u32>(image.height.value());
        std::vector<u8> pixels;
        if(!el::linearize(
               el::top_level(view.value(), image),
               format,
               width,
               height,
               image.xbox_flags,
               pixels))
            return std::nullopt;
        using blam::bitm::format_t;
        using gfx::textures::swizzle_t;
        if(rgtc)
        {
            auto tex = m_api->alloc_texture(gfx::textures::d2, rgtc->desc, 1);
            tex->alloc(size_3d<u32>{width, height, 1});
            auto const& sw = rgtc->swizzle;
            tex->set_swizzle(sw[0], sw[1], sw[2], sw[3]);
            tex->upload(
                std::move(pixels),
                Veci2{0, 0},
                Veci2{static_cast<i32>(width), static_cast<i32>(height)},
                0);
            return make_texture(tex);
        }
        /* Little-endian A8R8G8B8 is B,G,R,A */
        if(*h1 == format_t::ARGB8 || *h1 == format_t::XRGB8)
            for(size_t i = 0; i + 3 < pixels.size(); i += 4)
            {
                std::swap(pixels[i], pixels[i + 2]);
                if(*h1 == format_t::XRGB8)
                    pixels[i + 3] = 0xFF;
            }
        if(*h1 == format_t::V8U8)
            for(auto& b : pixels)
                b = static_cast<u8>(b + 0x80);

        auto tex = m_api->alloc_texture(gfx::textures::d2, blam::bitm::to_pixdesc(*h1), 1);
        tex->alloc(size_3d<u32>{width, height, 1});
        switch(*h1)
        {
        case format_t::Y8:
            tex->set_swizzle(swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::one);
            break;
        case format_t::AY8:
            tex->set_swizzle(swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::red);
            break;
        case format_t::A8:
            tex->set_swizzle(swizzle_t::one, swizzle_t::one, swizzle_t::one, swizzle_t::red);
            break;
        case format_t::A8Y8:
            tex->set_swizzle(swizzle_t::red, swizzle_t::red, swizzle_t::red, swizzle_t::green);
            break;
        default:
            break;
        }
        tex->upload(
            std::move(pixels),
            Veci2{0, 0},
            Veci2{static_cast<i32>(width), static_cast<i32>(height)},
            0);
        return make_texture(tex);
    }

    static Texture make_texture(std::shared_ptr<gfx::texture_2d_t> const& tex)
    {
        Texture out{.texture = tex, .sampler = tex->sampler()};
        out.sampler->alloc();
        out.sampler->set_filtering(
            typing::Filtering::Linear, typing::Filtering::Linear, typing::Filtering::Linear);
        out.sampler->set_anisotropic(4);
        return out;
    }

    /* The Halo 1 loader creates the seats from the scenario; a Halo 3 map
     * has to do the same before anything can look at it */
    void place_camera(Vecf3 const& position, f32 yaw)
    {
        bool have_player = false;
        for([[maybe_unused]] auto const& p : m_container.select<PlayerInfo>())
        {
            have_player = true;
            break;
        }
        if(!have_player)
        {
            auto ref = m_container.create_entity(shared_recipes::player_recipe);
            for(size_t frame = 0; frame < Visibility::type::frame_count_v; frame++)
                m_container.get_at<Visibility>(ref.id(), frame)->skip_render_for = 0;
            auto& info      = ref.get<PlayerInfo>();
            info.player_idx = 0;
            info.seat_idx   = 0;
            auto& camera    = ref.get<PlayerCamera>();
            camera.keyboard.enabled = true;
            camera.controller.index = 0;
        }
        static const glm::mat3 bsp_basis_inv{{0, 1, 0}, {0, 0, 1}, {1, 0, 0}};
        for(auto entity : m_container.select<PlayerCamera, PlayerInfo, PlayerInput>())
        {
            auto [cam, info, input] = entity.components();
            if(info.is_remote())
                continue;
            cam.controller.opts.sens.move = {.1f, .1f};
            cam.controller.opts.curve     = 200.f;
            cam.camera_opts.accel.alt     = 50.f;
            cam.camera_opts.world_basis   = bsp_basis_inv;
            cam.camera.position           = position;
            cam.camera.rotation =
                glm::angleAxis(glm::pi<f32>() - yaw, Vecf3{0.f, 1.f, 0.f});
            /* No collision mesh to stand on: fly */
            info.mode.physics = false;
            input.input_mode  = PlayerInput::input_mode_t::game;
        }
        update_camera_aspect(m_container);
    }

    compo::EntityContainer& m_container;
    H3Scene&                m_scene;
    gfx::system*            m_api{nullptr};

    std::shared_ptr<comp_app::FileMapper::Resource>              m_file;
    std::vector<std::shared_ptr<comp_app::FileMapper::Resource>> m_side_files;
    std::vector<std::unique_ptr<el::map_container>>              m_side_maps;
    std::unique_ptr<el::map_container>                           m_map;
    std::unique_ptr<el::resource_loader>                         m_loader;

    std::vector<Vertex>                                m_vertices;
    std::vector<u16>                                   m_indices;
    std::vector<el::vertex_t>                          m_decoded;
    std::vector<std::array<u16, 3>>                    m_triangles;
    std::unordered_map<u32, size_t>                    m_material_index;
    std::unordered_map<u32, std::optional<Texture>>    m_textures;
};

void alloc_h3(compo::EntityContainer& e)
{
    auto& scene  = e.register_subsystem_inplace<H3Scene>();
    auto& loader = e.register_subsystem_inplace<H3MapLoader>(std::ref(e), std::ref(scene));
    e.subsystem_cast<GameEventBus>().addEventFunction<MapLoadEvent>(
        0, [&loader](GameEvent& ev, MapLoadEvent* load) {
            /* Consumed here: the Halo 1 loader never sees a Halo 3 map */
            if(loader.on_map_load(*load))
                ev.type = GameEvent::None;
        });
    alloc_h3_renderer(e);
}

} // namespace h3
