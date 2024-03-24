#include "caching.h"

#include "materials.h"
#include "selected_version.h"

template<typename V>
BSPItem BSPCache<V>::predict_impl(const blam::bsp::info& bsp)
{
    using namespace semantic::chunk_ops;

    //        GFX::DBG::SCOPE _("BSPCache");

    auto        bsp_magic = bsp.bsp_magic(magic);
    auto const& section_ =
        bsp.to_bsp(bsp_magic).to_header().data(bsp_magic, blam::single_value);
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

    {
        auto bg_sound = section.background_sound.data(bsp_magic);
        if(bg_sound.has_value())
        {
            auto snds = bg_sound.value();
            for(auto const& snd : snds)
            {
                cDebug("BG sound tag: {}", snd.name.str());
                sound_cache.predict(snd.bg_sound.to_plain());
            }
        }
    }

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
        BSPItem::Cluster& it          = out.clusters.back();
        auto              portal_idxs = cluster.portals.data(bsp_magic).value();
        auto subclusters = cluster.sub_clusters.data(bsp_magic).value();
        for(auto const& portal_idx : portal_idxs)
            it.portals.push_back(&portals[portal_idx]);
        for(blam::bsp::subcluster const& sub : subclusters)
        {
            auto indices                  = sub.indices.data(bsp_magic).value();
            auto [min, max]               = sub.bounds.points();
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
    std::map<u64, std::pair<u16, u16>>                           vertex_ranges;
    std::map<u64, std::pair<u16, u16>>                           index_ranges;
    std::map<u64, std::pair<generation_idx_t, generation_idx_t>> lightmap_refs;
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
            index_ranges[vertex_id] = {mat.surfaces.count, mat.surfaces.offset};
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
                         .count         = static_cast<u32>(indices.size() * 3),
                         .offset        = element_ptr * sizeof(blam::vert::face),
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

template BSPItem BSPCache<halo_version>::predict_impl(
    const blam::bsp::info& bsp);

template<typename V>
ModelItem<V> ModelCache<V>::predict_impl(
    const blam::tagref_t& mod2, blam::mod2::mod2_lod lod)
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
    if(auto const& shaders_ = header->shaders.data(magic); shaders_.has_error())
        Throw(undefined_behavior("no shaders found"));
    else
        shaders = shaders_.value();

    blam::mod2::model_data_t model_data;
    if(auto model_data_ = header->model_at(lod, magic); model_data_.has_value())
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
            auto element_dest =
                (*element_buffer.at(element_ptr)).template as<element_type>();

            std::copy(vertices.begin(), vertices.end(), vert_dest.begin());
            std::copy(elements.begin(), elements.end(), element_dest.begin());

            //                Array<u16, 2> last_indices = {{elements[0],
            //                elements[1]}};

            vert_ptr += vertices.size_bytes();
            element_ptr += elements.size_bytes();
        }
    }

    return out;
}

template ModelItem<halo_version> ModelCache<halo_version>::
    predict_impl(const blam::tagref_t& mod2, blam::mod2::mod2_lod lod);

template<typename V>
ShaderItem ShaderCache<V>::predict_impl(const blam::tagref_t& shader)
{
    auto _ = bitm_cache.allocator->debug().scope("ShaderCache");

    using blam::tag_class_t;
    using namespace blam::shader;

    if(!shader.valid())
        return {};

    auto it                    = index.find(shader);
    C_UNUSED(auto shader_name) = shader.to_name().to_string(magic);

    if(it == index.end())
        return {};

    ShaderItem out = {};
    out.tag        = &(*it);
    out.tag_class  = shader.tag_class;
    out.header     = extract_shader<blam::shader::radiosity_properties>(it);

    switch(shader.tag_class)
    {
    case tag_class_t::senv: {
        auto const& shader_model = *extract_shader<shader_env>(it);

        out.senv.base_bitm    = get_bitm_idx(shader_model.diffuse.base);
        out.senv.primary_bitm = get_bitm_idx(shader_model.diffuse.primary.map);
        out.senv.secondary_bitm =
            get_bitm_idx(shader_model.diffuse.secondary.map);
        out.senv.micro_bitm = get_bitm_idx(shader_model.diffuse.micro.map);

        //            out.senv.self_illum =
        //            get_bitm_idx(shader_model.self_illum.map.map);
        out.senv.bump = get_bitm_idx(shader_model.bump.map);
        out.senv.reflection_bitm =
            get_bitm_idx(shader_model.reflection.reflection);

        break;
    }
    case tag_class_t::soso: {
        blam::shader::shader_model const& shader_model =
            *extract_shader<blam::shader::shader_model>(it);

        out.soso.base_bitm   = get_bitm_idx(shader_model.maps.base);
        out.soso.multi_bitm  = get_bitm_idx(shader_model.maps.multipurpose);
        out.soso.detail_bitm = get_bitm_idx(shader_model.maps.detail.map);
        out.soso.reflection_bitm =
            get_bitm_idx(shader_model.reflection.reflection);

        break;
    }
    case tag_class_t::schi: {
        auto const& shader_model = *extract_shader<shader_chicago<V>>(it);

        if(auto maps = shader_model.maps.data(magic); maps.has_value())
        {
            u8 i = 0;
            for(chicago::map_t const& map : maps.value())
            {
                out.schi.maps.at(i++) = get_bitm_idx(map.map.map);
            }
        }
        if(auto layers = shader_model.layers.data(magic); layers.has_value())
        {
            u8 i = 0;
            for(auto const& layer : layers.value())
                out.schi.layers.at(i++) = predict(layer.to_plain());
        }

        break;
    }
    case tag_class_t::scex: {
        auto const& shader_model =
            *extract_shader<shader_chicago_extended<V>>(it);

        if(auto maps4 = shader_model.maps_4stage.data(magic); maps4.has_value())
        {
            u8 i = 0;
            for(chicago::map_t const& map : maps4.value())
                out.scex.maps.at(i++) = get_bitm_idx(map.map.map);
        }
        if(auto layers = shader_model.layers.data(magic); layers.has_value())
        {
            u8 i = 0;
            for(auto const& layer : layers.value())
                out.scex.layers.at(i++) = predict(layer.to_plain());
        }

        break;
    }
    case tag_class_t::sgla: {
        shader_glass const& shader_model = *extract_shader<shader_glass>(it);

        out.color_bitm = get_bitm_idx(shader_model.diffuse.map.map);

        break;
    }
    case tag_class_t::swat: {
        shader_water const& shader_model = *extract_shader<shader_water>(it);

        out.swat.base       = get_bitm_idx(shader_model.base);
        out.swat.reflection = get_bitm_idx(shader_model.reflection_map);
        out.swat.bump       = get_bitm_idx(shader_model.ripple.map);

        break;
    }
    case tag_class_t::spla: {
        auto const& shader_model = *extract_shader<shader_plasma>(it);

        out.color_bitm = get_bitm_idx(shader_model.primary_noise.noise.map);

        break;
    }
    case tag_class_t::smet: {
        auto const& shader_model = *extract_shader<shader_meter>(it);

        out.color_bitm = get_bitm_idx(shader_model.map);

        break;
    }
    case tag_class_t::sotr: {
        shader_transparent const& shader_model =
            *extract_shader<shader_transparent>(it);

        if(auto maps = shader_model.maps.data(magic); maps.has_value())
        {
            u32 i = 0;
            for(shader_transparent::map_t const& map : maps.value())
                out.sotr.maps.at(i++) = get_bitm_idx(map.map.map);
        }

        break;
    }
    default:
        Throw(undefined_behavior("unhandled shader type"));
    }

    return out;
}

template ShaderItem ShaderCache<halo_version>::predict_impl(
    const blam::tagref_t& shader);

template<typename V>
void ShaderCache<V>::populate_material(
    materials::shader_data&            mat,
    const generation_idx_t&            shader_id,
    const typing::vector_types::Vecf2& base_map_scale)
{
    using blam::tag_class_t;
    using namespace blam::shader;

    ShaderItem const& shader = find(shader_id)->second;

    mat = {};

    switch(shader.tag_class)
    {
    case tag_class_t::scex: {
        shader_chicago_extended<V> const* info =
            shader.header->as<blam::shader::shader_chicago_extended<V>>();

        auto maps = info->maps_4stage.data(magic).value();
        for(auto i : range<>(4))
        {
            auto id = shader.scex.maps.at(i);
            if(!shader.schi.maps.at(i).valid())
                continue;
            BitmapItem const& bitm =
                *bitm_cache.assign_atlas_data(mat.maps[i], id);
            chicago::map_t const& map = maps[i];
            mat.maps[i].uv_scale      = map.map.uv_scale * base_map_scale;
            mat.maps[i].bias          = bitm.image.bias;

            u16 flags = static_cast<u8>(map.color_func) |
                        (static_cast<u8>(map.alpha_func) << 4);
            mat.lightmap.meta1 |= flags << (i * 8);
        }

        mat.material.material = materials::id::scex;
        break;
    }
    case tag_class_t::schi: {
        shader_chicago<V> const* info =
            shader.header->as<blam::shader::shader_chicago<V>>();

        auto maps = info->maps.data(magic).value();
        for(auto i : range<>(4))
        {
            auto id = shader.schi.maps.at(i);
            if(!shader.schi.maps.at(i).valid())
                continue;
            BitmapItem const& bitm =
                *bitm_cache.assign_atlas_data(mat.maps[i], id);
            chicago::map_t const& map = maps[i];
            mat.maps[i].uv_scale      = map.map.uv_scale * base_map_scale;
            mat.maps[i].bias          = bitm.image.bias;

            u16 flags = static_cast<u8>(map.color_func) |
                        (static_cast<u8>(map.alpha_func) << 4);
            mat.lightmap.meta1 |= flags << (i * 8);
        }

        mat.material.material = materials::id::schi;
        break;
    }
    case tag_class_t::senv: {
        shader_env const* info = shader.header->as<blam::shader::shader_env>();

        auto base =
            bitm_cache.assign_atlas_data(mat.maps[0], shader.senv.base_bitm);
        if(base)
        {
            mat.maps[0].uv_scale = Vecf2{1};
            mat.maps[0].bias     = base->image.bias;
        }

        auto* micro =
            bitm_cache.assign_atlas_data(mat.maps[1], shader.senv.micro_bitm);
        if(micro)
        {
            mat.maps[1].uv_scale = Vecf2(info->diffuse.micro.scale);
            mat.maps[1].bias     = micro->image.bias;
        }

        mat.material.flags |= (micro ? 1 : 0) << 10;

        auto* primary =
            bitm_cache.assign_atlas_data(mat.maps[2], shader.senv.primary_bitm);
        if(primary)
        {
            mat.maps[2].uv_scale = Vecf2(info->diffuse.primary.scale);
            mat.maps[2].bias     = primary->image.bias;
        }

        auto* secondary = bitm_cache.assign_atlas_data(
            mat.maps[3], shader.senv.secondary_bitm);
        if(secondary)
        {
            mat.maps[3].uv_scale = Vecf2(info->diffuse.secondary.scale);
            mat.maps[3].bias     = secondary->image.bias;
        }

        mat.material.flags |= (primary && secondary ? 1 : 0) << 9;

        mat.lightmap.meta1 = bitm_cache.get_atlas_layer(shader.senv.self_illum);

        auto* bump =
            bitm_cache.assign_atlas_data(mat.maps[4], shader.senv.bump);
        if(bump)
        {
            mat.maps[4].uv_scale = Vecf2(info->bump.scale);
            mat.maps[4].bias     = bump->image.bias;
        }

        mat.material.material = materials::id::senv;
        mat.material.flags    = static_cast<u32>(info->flags) |
                             static_cast<u32>(info->shader_type) << 4;
        mat.material.inputs1[0] = info->reflection.lightmap_brightness;
        mat.material.inputs1[1] = info->specular.brightness;

        auto& reflection = info->reflection;
        if(shader.senv.reflection_bitm.valid())
        {
            mat.material.inputs[0] = Vecf4(
                info->specular.perpendicular_color,
                reflection.perpendicular_brightness);
            mat.material.inputs[1] = Vecf4(
                info->specular.parallel_color, reflection.parallel_brightness);
            mat.lightmap.reflection =
                bitm_cache.get_atlas_layer(shader.senv.reflection_bitm);
            mat.material.flags |= 1 << 6; /* Flag for reflection */
            mat.material.flags |= static_cast<u32>(reflection.type) << 7;
        }

        /* Allocation of flag bits:
         * 0-3:   Top-level flags (senv::flags)
         * 4-5:   Shader type
         *          * Reflection properties:
         * 6:     Reflection toggle
         * 7-8:   Reflection map type
         *          * Blending properties:
         * 9-10:  Detail map function
         * 10-11: Micro map function
         *          * Texture scrolling animation:
         * 12-15: U-animation function
         * 16-19: V-animation function
         *          *          */

        break;
    }
    case tag_class_t::swat: {
        shader_water const* info = shader.header->as<shader_water>();
        bitm_cache.assign_atlas_data(mat.maps[0], shader.swat.base);
        mat.maps[0].uv_scale = base_map_scale;
        mat.maps[0].bias     = 0;

        bitm_cache.assign_atlas_data(mat.maps[1], shader.swat.reflection);
        mat.lightmap.reflection = mat.maps[1].layer;

        bitm_cache.assign_atlas_data(mat.maps[1], shader.swat.bump);
        mat.maps[1].uv_scale = base_map_scale * info->ripple.scale;
        mat.maps[1].bias     = 2.f;

        mat.material.material = materials::id::swat;
        mat.material.flags    = static_cast<u32>(info->flags);
        mat.material.inputs1  = Vecf2{
            glm::radians(info->ripple.anim_angle), info->ripple.anim_velocity};
        mat.material.inputs[0] =
            Vecf4(info->parallel.tint_color, info->parallel.brightness);
        mat.material.inputs[1] = Vecf4(
            info->perpendicular.tint_color, info->perpendicular.brightness);
        break;
    }
    case tag_class_t::sgla: {
        //            auto const* info =
        //            shader.header->as<blam::shader::shader_glass>();
        bitm_cache.assign_atlas_data(mat.maps[0], shader.color_bitm);
        mat.maps[0].uv_scale = Vecf2(1);
        mat.maps[0].bias     = 0;

        mat.material.material = materials::id::sgla;
        break;
    }
    case tag_class_t::smet: {
        mat.material.material = materials::id::smet;
        break;
    }
    case tag_class_t::sotr: {
        shader_transparent const* info =
            shader.header->as<shader_transparent>();
        auto maps = info->maps.data(magic).value();
        //            auto stages = info->stages.data(magic).value();
        //            auto layers = info->layers.data(magic).value();
        //            cDebug(
        //                "{}: {} maps, {} stages, {} layers",
        //                shader.tag->to_name().to_string(magic),
        //                maps.size(),
        //                stages.size(),
        //                layers.size());

        for(auto i : range<>(maps.size()))
        {
            bitm_cache.assign_atlas_data(mat.maps[i], shader.sotr.maps[i]);
            mat.maps[i].uv_scale = maps[i].map.uv_scale;
            mat.maps[i].bias     = maps[i].map.mip_bias;
        }

        mat.material.material = materials::id::sotr;
        break;
    }
    case tag_class_t::soso: {
        shader_model const* info =
            shader.header->as<blam::shader::shader_model>();
        bitm_cache.assign_atlas_data(mat.maps[0], shader.soso.base_bitm);
        mat.maps[0].uv_scale = base_map_scale;
        mat.maps[0].bias     = 2;

        bitm_cache.assign_atlas_data(mat.maps[1], shader.soso.multi_bitm);
        mat.maps[1].uv_scale = Vecf2(1);
        mat.maps[1].bias     = 2;

        auto* detail =
            bitm_cache.assign_atlas_data(mat.maps[2], shader.soso.detail_bitm);
        if(detail)
        {
            mat.maps[2].uv_scale = Vecf2(info->maps.detail.scale);
            mat.maps[2].bias     = detail->image.bias;
        }

        mat.lightmap.reflection =
            bitm_cache.get_atlas_layer(shader.soso.reflection_bitm);
        if(mat.lightmap.reflection != 0)
            mat.material.flags |= 0x1;

        mat.material.material = materials::id::soso;
        break;
    }
    default: {
        cDebug(
            "Material not set up: {0}/{1}",
            shader.tag->tagclass.at(0).str(),
            shader.tag->to_name().to_string(magic));
        break;
    }
    }
}

template void ShaderCache<halo_version>::populate_material(
    materials::shader_data&            mat,
    const generation_idx_t&            shader_id,
    const typing::vector_types::Vecf2& base_map_scale);

template<typename V>
void BitmapCache<V>::allocate_storage()
{
    using size_bucket = std::tuple<u32, u32>;

    struct pool_size
    {
        u32   num    = 0;
        u32   layers = 0;
        Veci2 max    = {};

        std::map<cache_id_t, size_bucket> images;
    };

    ProfContext _("Building texture atlases");

    std::map<bitm_format_hash, pool_size> fmt_count;

    /* Find final pool sizes */
    for(auto& bitm : m_cache)
    {
        auto const& fmt  = bitm.second.image.fmt;
        auto        hash = std::make_tuple(
            bitm.second.image.mip->type,
            fmt.pixfmt,
            fmt.comp,
            fmt.bfmt,
            fmt.cmpflg);
        auto& pool   = fmt_count[hash];
        auto  imsize = bitm.second.image.mip->isize;
        if(bitm.second.image.mip->mipmaps > params->mipmap_bias)
            imsize >>= params->mipmap_bias;
        //            auto&       surface =
        //            tex_buckets[bitm.second.image.bucket].surface;

        //            u32 mipmaps = surface->m_mipmaps;
        u32 pad = 0;
        //                = surface->m_format.pixfmt != PixFmt::RGB565 ? 4
        //                << mipmaps : 0;

        pool.num++;
        pool.max.x = std::max<u32>(pool.max.x, imsize.x + pad);
        pool.max.y = std::max<u32>(pool.max.y, imsize.y + pad);
        pool.images.insert(
            {bitm.first, std::make_tuple(imsize.x + pad, imsize.y + pad)});
    }

    /*
     * Generate metadata for use in shaders, like layer, scale and offset
     * This requires knowledge of the size of the pool and number of layers
     * in array textures
     */
    for(auto& pool_ : fmt_count)
    {
        auto& pool = pool_.second;

        Veci2 offset = {0, 0};

        //            auto& surface = tex_buckets[pool_.first].surface;

        u32 layer = 0;
        //            u32 mipmaps = surface->m_mipmaps;
        u32 max_pad = 0; // surface->m_format.pixfmt != PixFmt::RGB565 ? 4
                         // << mipmaps : 0;

        for(auto [id, fmt] : pool.images)
        {
            BitmapItem* img    = &m_cache.find(id)->second;
            auto        imsize = img->image.mip->isize;

            if(params->mipmap_bias > 0 &&
               img->image.mip->mipmaps > params->mipmap_bias)
            {
                imsize >>= params->mipmap_bias;
                img->mipmaps.base = params->mipmap_bias;
                img->mipmaps.last =
                    params->mipmap_bias +
                    std::min<i32>(
                        5, img->image.mip->mipmaps - params->mipmap_bias);
            } else
            {
                img->mipmaps.base = 0;
                img->mipmaps.last = img->image.mip->mipmaps;
            }

            if(img->header->type == blam::bitm::bitmap_type_t::cube)
            {
                img->image.layer = layer++;
                continue;
            }

            imsize.x += max_pad;
            imsize.y += max_pad;

            auto img_offset = offset;
            //                auto img_layer  = layer;

            [[maybe_unused]] Veci2 slack = {
                pool.max.x - offset.x,
                pool.max.y - offset.y,
            };

            bool fits_width = (offset.x + imsize.x) <= pool.max.x;

            if(fits_width)
            {
                img_offset.x = offset.x;
                img_offset.y = offset.y /*- imsize.h*/;
                offset.x += imsize.x;
            } else
            {
                layer++;
                offset.x   = imsize.x;
                img_offset = {};
                //                    img_layer  = layer;
            }

            img->image.layer  = layer;
            img->image.offset = {
                C_CAST<f32>(img_offset.x + max_pad / 2),
                C_CAST<f32>(img_offset.y + max_pad / 2),
            };
            img->image.scale = {
                C_CAST<f32>(imsize.x - max_pad),
                C_CAST<f32>(imsize.y - max_pad),
            };
        }

        pool.layers = layer + 1;
    }

    /* Allocate the surfaces */
    for(auto& bucket : tex_buckets)
    {
        auto& props  = bucket.second;
        auto& pool   = fmt_count[bucket.first];
        i32   layers = C_CAST<i32>(pool.layers);
        auto size = size_3d<i32>{pool.max.x, pool.max.y, layers}.convert<u32>();
        props.surface->alloc(size);

        auto [type, fmt, _, __, comp] = bucket.first;
        std::string bucket_name       = fmt::format(
            "cache_{0}_{1}_{2}",
            magic_enum::enum_name(type),
            magic_enum::enum_name(fmt),
            magic_enum::enum_name(comp));
        allocator->debug().annotate(*props.surface, bucket_name.data());
    }

    /* Commit the textures */
    for(auto& bitm : m_cache)
        commit_bitmap<gfx::compat::texture_2da_t>(bitm.second);

    for(auto& [_, pool] : fmt_count)
        for(auto [image_id, fmt] : pool.images)
        {
            auto  image_it = m_cache.find(image_id);
            auto* image    = &image_it->second;
            image->image.offset[0] /= pool.max.x;
            image->image.offset[1] /= pool.max.y;
            image->image.scale[0] /= pool.max.x;
            image->image.scale[1] /= pool.max.y;
        }
}

template void BitmapCache<halo_version>::allocate_storage();

template<typename V>
BitmapItem BitmapCache<V>::predict_impl(const blam::tagref_t& bitmap, i16 idx)
{
    using namespace typing::pixels;

    auto _ = allocator->debug().scope("BitmapCache");

    using blam::tag_class_t;

    if(idx == -1)
        return {};

    if(!bitmap.valid() || bitmap.tag_class != tag_class_t::bitm)
        Throw(std::runtime_error("non-bitm tag passed to BitmapCache"));

    auto it          = index.find(bitmap);
    auto shader_name = bitmap.to_name().to_string(magic);

    if(it == index.end())
    {
        cDebug("Failed to find bitmap: {0}", shader_name);
        return {};
    }

    blam::tag_t const* bitm_tag = &(*it);

    auto [bitm_ptr, curr_magic] = bitm_tag->image(magic, bitm_header).value();

    if(!bitm_ptr)
        Throw(undefined_behavior("failed to get bitmap header"));

    auto const& bitm = *bitm_ptr;

    BitmapItem out;
    out.header = &bitm;
    out.tag    = bitm_tag;

    auto sequences = bitm.sequences.data(curr_magic).value();
    for(auto const& sequence : sequences)
    {
        //            if(sequence.name.str().empty())
        continue;
        cDebug(
            "Sequence: {0} : {1} bitmaps",
            sequence.name.str(),
            sequence.bitmap_count);
        for(auto const& sprite : sequence.sprites.data(curr_magic).value())
        {
            cDebug(
                " - Sprite {0}+{1}",
                sequence.first_bitmap,
                sprite.bitmap_index);
        }
    }

    if(auto image_ = bitm.images.data(curr_magic); image_.has_value())
    {
        auto& im = image_.value();
        if(static_cast<u16>(idx) >= im.size())
            return {};

        auto const& image = im[idx];

        auto& img = out.image;
        img.mip   = &image;
        img.layer = 0;

        PixDesc fmt;
        if(image.compressed())
        {
            std::tie(fmt.pixfmt, fmt.cmpflg) = image.to_compressed_fmt();

            fmt.comp = convert::to<PixCmp>(fmt.c);
        } else
        {
            fmt.pixfmt                   = image.to_pixfmt();
            std::tie(fmt.bfmt, fmt.comp) = image.to_fmt();
        }

        img.bucket = create_hash(fmt, img.mip->type);
        img.fmt    = fmt;

        switch(im[0].type)
        {
        case blam::bitm::type_t::tex_2d: {
            auto& bucket =
                get_bucket<gfx::compat::texture_2da_t>(fmt, img.mip->type);
            img.layer = bucket.ptr++;
            break;
        }
#if GLEAM_MAX_VERSION >= 0x400 || GLEAM_MAX_VERSION_ES >= 0x320
        case blam::bitm::type_t::tex_cube: {
            if(!allocator->feature_info().texture.cube_array)
                return {};
            auto& bucket =
                get_bucket<gfx::texture_cube_array_t>(fmt, img.mip->type);
            img.layer = bucket.ptr++;
            break;
        }
#endif
        default:
            cDebug(
                "unimplemented texture type: {0}",
                magic_enum::enum_name(im[0].type));
            return {};
        }

    } else
        return {};

    return out;
}

template BitmapItem BitmapCache<halo_version>::predict_impl(
    const blam::tagref_t& bitmap, i16 idx);
