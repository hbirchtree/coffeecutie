#pragma once

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_font.h>

#include <coffee/comp_app/AppWrap>
#include <coffee/comp_app/gl_config.h>
#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/EventHandlers>
#include <coffee/core/Scene>

#include <peripherals/semantic/chunk_ops.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_compat.h>

#include <fmt_extensions/vector_types.h>

#include <coffee/core/CDebug>

#include "bitmap_cache.h"
#include "blam_files.h"
#include "caching_item.h"
#include "data.h"
#include "data_cache.h"
#include "materials.h"
#include "shader_cache.h"

using gfx_api = gleam::api;
using libc_types::byte_t;
using libc_types::u64;
using semantic::Bytes;
using semantic::BytesConst;
using semantic::Span;

using compo::time_point;

using ModelManifest =
    compo::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

template<typename V>
struct ModelCache
    : DataCache<
          ModelItem<V>,
          std::tuple<u32, blam::mod2::mod2_lod>,
          blam::tagref_t const&,
          blam::mod2::mod2_lod>
    , compo::RestrictedSubsystem<ModelCache<V>, ModelManifest>
{
    using type    = ModelCache<V>;
    using Variant = typename std::conditional_t<
        std::is_same_v<V, blam::xbox_version_t>,
        blam::xbox_t,
        blam::grbx_t>;
    using Proxy = compo::proxy_of<ModelManifest>;

    ModelCache(
        BitmapCache<V>& bitm_cache,
        ShaderCache<V>& shader_cache,
        gfx_api*        allocator)
        : bitm_cache(bitm_cache)
        , shader_cache(shader_cache)
        , allocator(allocator)
    {
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        version      = V::version_v;
        tags         = map.tags;
        index        = blam::tag_index_view(map);
        magic        = map.magic;
        vertex_magic = V::version_v == blam::version_t::xbox
                           ? map.magic
                           : tags->vertex_magic(magic);
        vert_ptr = 0, element_ptr = 0;
        this->evict_all();
    }

    blam::version_t             version;
    blam::tag_index_t<V> const* tags;
    blam::tag_index_view<V>     index;
    blam::magic_data_t          magic;
    blam::magic_data_t          vertex_magic;
    BitmapCache<V>&             bitm_cache;
    ShaderCache<V>&             shader_cache;
    gfx_api*                    allocator;

    Bytes vert_buffer, element_buffer;
    u32   vert_ptr, element_ptr;

    blam::mod2::header<V> const* get_header(blam::tagref_t const& mod2)
    {
        auto header_it = index.find(mod2);

        if(header_it == index.end())
            return nullptr;

        auto header =
            (*header_it).template data<blam::mod2::header<V>>(magic).value();

        return &header[0];
    }

    inline auto vertex_data(blam::mod2::part const& model)
    {
        return model.vertex_segment(*tags, magic).data(vertex_magic);
    }

    inline auto index_data(blam::mod2::part const& model)
    {
        return model.index_segment(*tags).data(vertex_magic);
    }

    virtual ModelItem<V> predict_impl(
        blam::tagref_t const& mod2, blam::mod2::mod2_lod lod) override
    {
        auto _ = allocator->debug().scope("ModelCache");

        using namespace blam::mod2;

        blam::mod2::header<V> const* header = get_header(mod2);

        if(!header)
            return {};

        ModelItem<V> out;
        out.mesh   = {};
        out.header = header;
        out.tag    = &(*index.find(mod2));

        Span<const blam::shader::shader_desc> shaders;
        if(auto const& shaders_ = header->shaders.data(magic);
           shaders_.has_error())
            Throw(undefined_behavior("no shaders found"));
        else
            shaders = shaders_.value();

        blam::mod2::model_data_t model_data;
        if(auto model_data_ = header->model_at(lod, magic);
           model_data_.has_value())
            model_data = std::move(model_data_.value());
        else
            return {};
        //        auto const& geom =
        //        header[0].geometries.data(magic).value()[geom_idx];
        {
            //            out.mesh.header = &geom;

            //            for(auto const& model : geom.meshes(magic))
            for(auto const* part : model_data.parts)
            {
                auto elements_ = index_data(*part);
                auto vertices_ = vertex_data(*part);

                if(!elements_.has_value() || !vertices_.has_value())
                {
                    cWarning(
                        "Invalid element/vertex data for model: {0}",
                        mod2.to_name().to_string(magic));
                    continue;
                }
                auto elements = elements_.value();
                auto vertices = vertices_.value();

                if(elements.empty() || vertices.empty())
                {
                    cWarning(
                        "Failed to get element/vertex data for model: {0}",
                        mod2.to_name().to_string(magic));
                    continue;
                }

                using element_type = typename std::remove_const<
                    typename decltype(elements)::value_type>::type;
                using vertex_type = typename std::remove_const<
                    typename decltype(vertices)::value_type>::type;

                out.mesh.sub.emplace_back();
                auto& draw_data         = out.mesh.sub.back();
                draw_data.header        = part;
                draw_data.draw.elements = {
                    .count         = static_cast<u32>(elements.size()),
                    .offset        = element_ptr,
                    .vertex_offset = vert_ptr / sizeof(vertex_type),
                    .type          = semantic::TypeEnum::UShort,
                };
                draw_data.draw.instances.count = 1;
                draw_data.shader =
                    shader_cache.predict(shaders[part->shader_idx].ref);

                auto vert_dest =
                    (*vert_buffer.at(vert_ptr)).template as<vertex_type>();
                auto element_dest = (*element_buffer.at(element_ptr))
                                        .template as<element_type>();

                std::copy(vertices.begin(), vertices.end(), vert_dest.begin());
                std::copy(
                    elements.begin(), elements.end(), element_dest.begin());

                //                Array<u16, 2> last_indices = {{elements[0],
                //                elements[1]}};

                vert_ptr += vertices.size_bytes();
                element_ptr += elements.size_bytes();
            }
        }

        return out;
    }

    virtual void evict_impl() override
    {
        vert_ptr    = 0;
        element_ptr = 0;
    }

    virtual std::tuple<u32, blam::mod2::mod2_lod> get_id(
        blam::tagref_t const& tag, blam::mod2::mod2_lod lod) override
    {
        return std::make_tuple(tag.tag_id, lod);
    }

    ModelAssembly predict_regions(
        blam::tagref_t const& tag,
        blam::mod2::mod2_lod  max_lod = blam::mod2::lod_high_ext)
    {
        ModelAssembly assem  = {};
        auto          header = get_header(tag);

        if(!header)
            return {};

        assem.models.push_back(this->predict(tag, max_lod));

        //        for(auto const& region : header->regions.data(magic).value())
        //        {
        //            assem.models.emplace_back();
        //            auto& mod = assem.models.back();
        //            for(auto const& perm :
        //            region.permutations.data(magic).value())
        //            {
        //                auto const& lod = perm.meshindex_lod.at(max_lod);
        //                mod = this->predict(tag, lod);
        //                break;
        //            }
        //        }

        return assem;
    }

    void start_restricted(Proxy&, time_point const&)
    {
    }

    void end_restricted(Proxy&, time_point const&)
    {
    }
};

using BSPManifest =
    compo::SubsystemManifest<empty_list_t, empty_list_t, empty_list_t>;

template<typename V>
struct BSPCache
    : DataCache<BSPItem, blam::bsp::info const*, blam::bsp::info const&>
    , compo::RestrictedSubsystem<BSPCache<V>, BSPManifest>
{
    using type  = BSPCache<V>;
    using Proxy = compo::proxy_of<BSPManifest>;

    BSPCache(BitmapCache<V>& bitm_cache, ShaderCache<V>& shader_cache)
        : version(V::version_v)
        , bitm_cache(bitm_cache)
        , shader_cache(shader_cache)
    {
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        index    = blam::tag_index_view(map);
        magic    = map.magic;
        vert_ptr = 0, element_ptr = 0, light_ptr = 0, portal_ptr = 12,
        portal_color_ptr = 4;
        evict_all();
    }

    blam::version_t         version;
    BitmapCache<V>&         bitm_cache;
    ShaderCache<V>&         shader_cache;
    blam::tag_index_view<V> index;
    blam::magic_data_t      magic;

    Span<byte_t>           vert_buffer;
    Span<byte_t>           light_buffer;
    Span<blam::vert::face> element_buffer;

    Span<Vecf3> portal_buffer, portal_color_buffer;

    u32 vert_ptr, element_ptr, light_ptr, portal_ptr, portal_color_ptr;

    virtual BSPItem predict_impl(blam::bsp::info const& bsp) override
    {
        using namespace semantic::chunk_ops;

        //        GFX::DBG::SCOPE _("BSPCache");

        auto        bsp_magic = bsp.bsp_magic(magic);
        auto const& section_  = bsp.to_bsp(bsp_magic).to_header().data(
            bsp_magic, blam::single_value);
        if(section_.has_error())
        {
            cDebug("Failed to find BSP section");
            return {};
        }
        auto const& section = *section_.value();

        BSPItem out;
        out.mesh = &section;
        out.tag  = &(*index.find(bsp.tag));

        if(!out.tag->valid())
            return {};

        //        auto shader = section.shaders.data(bsp_magic);

        Span<blam::bsp::cluster_portal const> portals =
            section.cluster_portals.data(bsp_magic).value();

        //        for(auto const& portal : portals)
        //        {
        //            auto vertices = portal.vertices.data(bsp_magic).value();
        //            std::copy(
        //                vertices.begin(),
        //                vertices.end(),
        //                portal_buffer.begin() + portal_ptr);
        //            portal_color_buffer[portal_color_ptr] = Vecf3(1);
        //            out.portals.push_back({
        //                .arrays = {
        //                     .count  = static_cast<u32>(vertices.size()),
        //                     .offset = static_cast<u32>(portal_ptr),
        //                },
        //            });
        //            out.portal_color_ptrs.push_back(portal_color_ptr);

        //            portal_ptr += vertices.size();
        //            portal_color_ptr++;
        //        }

        auto bclusters = section.clusters.data(bsp_magic).value();
        for(blam::bsp::cluster const& cluster : bclusters)
        {
            out.clusters.push_back({
                .cluster = &cluster,
            });
            BSPItem::Cluster& it = out.clusters.back();
            auto portal_idxs     = cluster.portals.data(bsp_magic).value();
            auto subclusters     = cluster.sub_clusters.data(bsp_magic).value();
            for(auto const& portal_idx : portal_idxs)
                it.portals.push_back(&portals[portal_idx]);
            for(blam::bsp::subcluster const& sub : subclusters)
            {
                auto indices    = sub.indices.data(bsp_magic).value();
                auto [min, max] = sub.bounds.points();
                std::array<Vecf3, 8> vertices = {{
                    min,
                    Vecf3(max.x, min.y, min.z),
                    Vecf3(max.x, max.y, min.z),
                    Vecf3(min.x, max.y, min.z),
                    Vecf3(min.x, max.y, max.z),
                    Vecf3(min.x, min.y, max.z),
                    Vecf3(max.x, min.y, max.z),
                    max,
                }};
                std::copy(
                    vertices.begin(),
                    vertices.end(),
                    portal_buffer.begin() + portal_ptr);
                portal_color_buffer[portal_color_ptr] = Vecf3(0, 1, 0);
                out.portals.push_back({
                    .arrays =
                        {
                            .count  = static_cast<u32>(8),
                            .offset = static_cast<u32>(portal_ptr),
                        },
                });
                it.sub.push_back(BSPItem::Subcluster{
                    .cluster = &sub,
                    .indices = indices,
                });
                portal_ptr += 8;
                portal_color_ptr++;
            }
        }

        /* TODO: Find link between indices in cluster and submeshes */

        //        auto submeshes      = section.lightmaps.data(bsp_magic);
        auto surfaces_      = section.surfaces.data(bsp_magic);
        auto lightmaps_     = section.lightmaps.data(bsp_magic);
        auto leaves_        = section.leaves.data(bsp_magic);
        auto leaf_surfaces_ = section.leaf_surfaces.data(bsp_magic);
        auto nodes_         = section.nodes.data(bsp_magic);
        auto clusters_      = section.clusters.data(bsp_magic);

        if(surfaces_.has_error())
        {
            cDebug("Error finding submeshes");
            return {};
        }

        auto surfaces      = surfaces_.value();
        auto lightmaps     = lightmaps_.value();
        auto leaves        = leaves_.value();
        auto leaf_surfaces = leaf_surfaces_.value();
        auto nodes         = nodes_.value();
        auto clusters      = clusters_.value();

        std::map<i32, std::vector<blam::vert::face const*>> node_surfaces;

        for(auto const& assoc : leaf_surfaces)
        {
            if(assoc.surface > surfaces.size())
                Throw(std::out_of_range("surface idx out of bounds"));
            node_surfaces[assoc.node].push_back(&surfaces[assoc.surface]);
        }

        std::map<i32, std::vector<std::pair<u16, u16>>> cluster_surfaces;
        u16                                             max_reference = 0;

        for(auto const& leaf : leaves)
        {
            if(leaf.surface_reference_count < 1)
                continue;
            max_reference = std::max<u16>(
                max_reference,
                leaf.surface_reference_index + leaf.surface_reference_count);
            cluster_surfaces[leaf.cluster].push_back(
                {leaf.surface_reference_index, leaf.surface_reference_count});
            //            cDebug(
            //                "Leaf {} contains {}+{} surfaces in cluster {}",
            //                &leaf - leaves.data(),
            //                leaf.surface_reference_index,
            //                leaf.surface_reference_count,
            //                leaf.cluster);
        }

        cDebug("Max referenced surface index: {}", max_reference);
        cDebug("Nodes with surfaces: {}", node_surfaces.size());
        cDebug("Clusters with surfaces: {}", cluster_surfaces.size());

        /* First, load up the vertices into the vertex buffer
         * We leave references to where they are in the vertex_ranges map
         * Later we want to point to them from each of the leaves
         */
        std::map<u64, std::pair<u16, u16>> vertex_ranges;
        std::map<u64, std::pair<u16, u16>> index_ranges;
        std::map<u64, std::pair<generation_idx_t, generation_idx_t>>
            lightmap_refs;
        for(auto const& lightmap : lightmaps)
        {
            auto light_bitm =
                bitm_cache.predict(section.lightmap_, lightmap.lightmap_idx);

            auto materials = lightmap.materials.data(bsp_magic).value();
            out.groups.emplace_back();
            auto& group = out.groups.back();

            for(blam::bsp::material const& mat : materials)
            {
                auto shader    = shader_cache.predict(mat.shader.to_plain());
                auto vertex_id = lightmap.lightmap_idx |
                                 (static_cast<u64>(mat.shader.tag_id) << 32);
                auto vertices       = mat.vertices().data(bsp_magic).value();
                auto light_vertices = mat.light_verts().data(bsp_magic).value();

                vertex_ranges[vertex_id] = {vert_ptr, vertices.size()};
                index_ranges[vertex_id]  = {
                    mat.surfaces.count, mat.surfaces.offset};
                lightmap_refs[vertex_id] = {light_bitm, shader};

                std::copy(
                    vertices.begin(),
                    vertices.end(),
                    vert_buffer.begin() + vert_ptr);
                std::copy(
                    light_vertices.begin(),
                    light_vertices.end(),
                    light_buffer.begin() + light_ptr);

                auto indices = mat.indices(section).data(bsp_magic).value();
                std::copy(
                    indices.begin(),
                    indices.end(),
                    element_buffer.begin() + element_ptr);
                group.meshes.emplace_back();
                auto& mesh = group.meshes.back();
                mesh.mesh  = &mat;
                mesh.draw  = {
                     .elements =
                        {
                             .count  = static_cast<u32>(indices.size() * 3),
                             .offset = element_ptr * sizeof(blam::vert::face),
                             .vertex_offset = vert_ptr / mat.vertex_size(),
                             .type          = semantic::TypeEnum::UShort,
                        },
                     .instances =
                        {
                             .count = 1,
                        },
                };
                mesh.shader     = shader_cache.predict(mat.shader);
                mesh.light_bitm = light_bitm;

                vert_ptr += vertices.size_bytes();
                light_ptr += light_vertices.size_bytes();
                element_ptr += indices.size();
            }
        }

        cDebug("Vertex ranges: {}", vertex_ranges);
        cDebug("Index ranges: {}", index_ranges);

        //        for(auto const& cluster : cluster_surfaces)
        //        {
        //            out.groups.emplace_back();
        //            auto& group = out.groups.back();
        //            cDebug("Cluster: {}", cluster.second);
        //            for(auto const& surface : cluster.second)
        //            {
        //            }
        //        }

        //        for(auto const& leaf : leaves.value())
        //        {
        //            cDebug("Leaf of cluster {}", leaf.cluster);
        //        }
        //        for(auto const& surface : leaf_surfaces_.value())
        //        {
        //            cDebug(
        //                "Leaf surface {} in node {}",
        //                surface.surface,
        //                surface.node);
        //        }

        //        for(auto const& group : submeshes.value())
        //        {
        //            auto meshes = group.materials.data(bsp_magic);
        //            if(meshes.has_error())
        //                continue;
        //            out.groups.emplace_back();
        //            auto& group_data = out.groups.back();
        //            group_data.group = &group;
        //            for(auto const& mesh : meshes.value())
        //            {
        //                group_data.meshes.emplace_back();
        //                auto& mesh_data = group_data.meshes.back();

        //                /* Just dig up the textures, long process */
        //                mesh_data.shader = shader_cache.predict(mesh.shader);
        //                if(group.lightmap_idx != -1)
        //                    mesh_data.light_bitm = bitm_cache.resolve(
        //                        section.lightmap_, group.lightmap_idx);

        //                /* ... and moving on */

        //                if(version == blam::version_t::xbox)
        //                {
        //                    auto indices
        //                        =
        //                        mesh.indices(section).data(bsp_magic).value();
        //                    auto vertices
        //                        =
        //                        mesh.xbox_vertices().data(bsp_magic).value();
        //                    auto light_verts
        //                        =
        //                        mesh.xbox_light_verts().data(bsp_magic).value();

        //                    if(vertices.empty() || indices.empty())
        //                    {
        //                        group_data.meshes.erase(--group_data.meshes.end());
        //                        continue;
        //                    }

        //                    using vertex_type = typename std::remove_const<
        //                        typename
        //                        decltype(vertices)::value_type>::type;
        //                    using light_type = typename std::remove_const<
        //                        typename
        //                        decltype(light_verts)::value_type>::type;
        //                    using element_type = typename std::remove_const<
        //                        typename decltype(indices)::value_type>::type;

        //                    mesh_data.mesh          = &mesh;
        //                    mesh_data.draw.elements = {
        //                        .count         = C_FCAST<u32>(indices.size()),
        //                        .offset        = element_ptr,
        //                        .vertex_offset = vert_ptr /
        //                        sizeof(vertex_type), .type          =
        //                        semantic::TypeEnum::UShort,
        //                    };
        //                    mesh_data.draw.instances.count = 1;

        //                    MemCpy(
        //                        vertices,
        //                        (*vert_buffer.at(vert_ptr)).template
        //                        as<vertex_type>());
        //                    MemCpy(
        //                        indices,
        //                        (*element_buffer.at(element_ptr))
        //                            .template as<element_type>());
        //                    MemCpy(
        //                        light_verts,
        //                        (*light_buffer.at(light_ptr))
        //                            .template as<light_type>());

        //                    vert_ptr += vertices.size_bytes();
        //                    element_ptr += indices.size_bytes();
        //                    light_ptr += light_verts.size_bytes();
        //                } else
        //                {
        //                    auto indices
        //                        =
        //                        mesh.indices(section).data(bsp_magic).value();
        //                    auto vertices =
        //                    mesh.pc_vertices().data(bsp_magic).value(); auto
        //                    light_verts
        //                        =
        //                        mesh.pc_light_verts().data(bsp_magic).value();

        //                    using vertex_type = typename std::remove_const<
        //                        typename
        //                        decltype(vertices)::value_type>::type;
        //                    using light_type = typename std::remove_const<
        //                        typename
        //                        decltype(light_verts)::value_type>::type;
        //                    using element_type = typename std::remove_const<
        //                        typename decltype(indices)::value_type>::type;

        //                    mesh_data.mesh          = &mesh;
        //                    mesh_data.draw.elements = {
        //                        .count         = C_FCAST<u32>(indices.size()),
        //                        .offset        = element_ptr,
        //                        .vertex_offset = vert_ptr /
        //                        sizeof(vertex_type), .type          =
        //                        semantic::TypeEnum::UShort,
        //                    };
        //                    mesh_data.draw.instances.count = 1;

        //                    MemCpy(
        //                        vertices,
        //                        (*vert_buffer.at(vert_ptr)).template
        //                        as<vertex_type>());
        //                    MemCpy(
        //                        indices,
        //                        (*element_buffer.at(element_ptr))
        //                            .template as<element_type>());
        //                    MemCpy(
        //                        light_verts,
        //                        (*light_buffer.at(light_ptr))
        //                            .template as<light_type>());

        //                    vert_ptr += vertices.size_bytes();
        //                    element_ptr += indices.size_bytes();
        //                    light_ptr += light_verts.size_bytes();
        //                }
        //            }
        //        }

        return out;
    }

    virtual blam::bsp::info const* get_id(blam::bsp::info const& bsp) override
    {
        return &bsp;
    }

    void start_restricted(Proxy&, time_point const&)
    {
    }

    void end_restricted(Proxy&, time_point const&)
    {
    }
};
