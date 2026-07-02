#include "caching.h"

#include <coffee/core/CProfiling>

#include "map_marker.h"
#include "materials.h"
#include "selected_version.h"
#include <coffee/graphics/apis/gleam/rhi_texture_atlas.h>

#include <algorithm>

#include <magic_enum/magic_enum.hpp>
#include <peripherals/stl/magic_enum.hpp>

using Profiler = Coffee::Profiler;

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
    out.mesh        = &section;
    out.tag         = &(*index.find(bsp.tag));
    out.bsp_magic   = bsp_magic;
    out.section_idx = next_section_idx++;

    u32 const mcc_vert_base = bsp.to_bsp(bsp_magic).xbox_vertices.offset;

    if(!out.tag->valid())
        return {};

    //        auto shader = section.shaders.data(bsp_magic);

    Span<blam::bsp::cluster_portal const> portals =
        section.cluster_portals.data(bsp_magic).value();

    {
        auto bg_sound = section.background_sound.data(bsp_magic);
        if(bg_sound.has_value())
        {
            for(auto const& snd : bg_sound.value())
                out.bg_sound_palette.push_back(&snd);
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
        {
            auto const&     p     = portals[portal_idx];
            auto            verts = p.vertices.data(bsp_magic);
            BSPItem::Portal entry;
            entry.data = &p;
            if(verts.has_value())
                entry.vertices.assign(
                    verts.value().begin(), verts.value().end());
            it.portals.push_back(std::move(entry));
        }
        for(blam::bsp::subcluster const& sub : subclusters)
        {
            auto indices  = sub.indices.data(bsp_magic).value();
            auto [p1, p2] = sub.bounds.points();
            /* 16-vertex wire box tracing all 12 edges of the subcluster AABB,
             * appended through the shared debug-marker cursor. */
            DebugDraw box = debug_markers->create_box(p1, p2, Vecf3(0, 1, 0));
            out.portals.push_back(box.data);
            it.sub.push_back(
                BSPItem::Subcluster{
                    .cluster         = &sub,
                    .indices         = indices,
                    .debug_color_idx = box.color_ptr,
                });
        }
    }

    /* Build flat sorted subcluster list for find_cluster().
     * Sorted by volume ascending so find_cluster() can return on first hit. */
    {
        out.sorted_subclusters.reserve(out.clusters.size() * 4);
        for(u32 ci = 0; ci < static_cast<u32>(out.clusters.size()); ci++)
        {
            auto const& cluster = out.clusters[ci];
            for(u32 si = 0; si < static_cast<u32>(cluster.sub.size()); si++)
            {
                auto [p1, p2] = cluster.sub[si].cluster->bounds.points();
                out.sorted_subclusters.push_back({
                    .bmin        = glm::min(p1, p2),
                    .bmax        = glm::max(p1, p2),
                    .cluster_idx = ci,
                    .sub_idx     = si,
                });
            }
        }
        std::sort(
            out.sorted_subclusters.begin(),
            out.sorted_subclusters.end(),
            [](BSPItem::FlatSubcluster const& a,
               BSPItem::FlatSubcluster const& b) {
                auto da = a.bmax - a.bmin;
                auto db = b.bmax - b.bmin;
                return (da.x * da.y * da.z) < (db.x * db.y * db.z);
            });
    }

    /* NOTE: the `cluster_data` blob (cluster_data_size bytes after the cluster
     * array) was previously read as a precomputed PVS bitset. Halo CE has no
     * such PVS — visibility is portal-walked at runtime — and the bytes there
     * are not a per-cluster bitmask (and the read location was out of bounds).
     * Culling uses portal_visible_set; the blob is intentionally not decoded. */

    /* Collision surface mesh for the physics subsystem; tree queries
     * (point→cluster, hitscan) go through section/bsp_magic on demand. */
    if(auto collision =
           section.collision_header.data(bsp_magic, blam::single_value);
       collision.has_value())
    {
        auto const& coll = *collision.value();
        if(auto surfaces = coll.surfaces.data(bsp_magic); surfaces.has_value())
            out.coll_surfaces = surfaces.value();
        if(auto edges = coll.edges.data(bsp_magic); edges.has_value())
            out.coll_edges = edges.value();
        if(auto verts = coll.vertices.data(bsp_magic); verts.has_value())
            out.coll_vertices = verts.value();
        cDebug(
            "BSP tree: {} nodes, {} planes, {} render leaves",
            coll.nodes_3d.count,
            coll.planes.count,
            section.leaves.count);
    }
    {
        u32 total = 0, degenerate = 0;
        for(auto const& cluster : out.clusters)
            for(auto const& portal : cluster.portals)
            {
                total++;
                if(portal.vertices.size() < 3)
                    degenerate++;
            }
        cDebug(
            "BSP portals: {} in cluster lists, {} global, {} with <3 vertices",
            total,
            portals.size(),
            degenerate);
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

    cDebug(
        "Surfaces={} lightmaps={} leaves={} leaf_surfaces={} nodes={} "
        "clusters={}",
        surfaces.size(),
        lightmaps.size(),
        leaves.size(),
        leaf_surfaces.size(),
        nodes.size(),
        clusters.size());

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

    /* Build authoritative face → cluster map using the leaf hierarchy.
     * leaf.cluster names the cluster; leaf.surface_reference_{index,count}
     * index into leaf_surfaces[]; leaf_surface.surface is the face index
     * into header.surfaces (same index space as material::surfaces.count). */
    /* A face referenced from the leaves of more than one cluster straddles a
     * cluster boundary (large terrain faces do this constantly); it must stay
     * visible whenever ANY of those clusters is. Keep the full owner set per
     * face — assigning whichever cluster wrote last used to punch holes in
     * the ground next to the camera, and dropping the assignment entirely
     * left an always-visible checkerboard of terrain chunks when everything
     * around them was occluded. */
    std::vector<std::vector<u16>> face_clusters(surfaces.size());
    const auto add_face_cluster = [&face_clusters](u32 fi, u32 cid) {
        if(fi >= face_clusters.size())
            return;
        auto& owners = face_clusters[fi];
        u16   cid16  = static_cast<u16>(cid);
        if(std::find(owners.begin(), owners.end(), cid16) == owners.end())
            owners.push_back(cid16);
    };
    for(auto const& leaf : leaves)
    {
        if(leaf.cluster < 0)
            continue;
        u32 cid = static_cast<u32>(leaf.cluster);
        u16 ref_end =
            leaf.surface_reference_index + leaf.surface_reference_count;
        for(u16 ri = leaf.surface_reference_index; ri < ref_end; ri++)
        {
            if(ri >= leaf_surfaces.size())
                break;
            add_face_cluster(leaf_surfaces[ri].surface, cid);
        }
    }

    /* Build face → (cluster, subcluster) map from the subcluster index lists.
     * Each subcluster stores the global face indices (into header.surfaces)
     * that fall within its AABB.  This gives a finer split than the
     * cluster-only leaf map: one ECS entity per (material × subcluster) instead
     * of per (material × cluster), so the occluder can cull per-AABB rather
     * than per-cluster. */
    constexpr u32 kInvalid = std::numeric_limits<u32>::max();
    using SubclusterKey =
        std::pair<u32, u32>; /* (cluster_idx, subcluster_idx) */

    std::vector<SubclusterKey> face_subcluster(
        surfaces.size(), {kInvalid, kInvalid});
    for(u32 ci = 0; ci < out.clusters.size(); ci++)
    {
        for(u32 si = 0; si < out.clusters[ci].sub.size(); si++)
        {
            for(u32 fi : out.clusters[ci].sub[si].indices)
            {
                if(fi >= face_subcluster.size())
                    continue;
                auto& key = face_subcluster[fi];
                add_face_cluster(fi, ci);
                if(key.first == kInvalid)
                    key = {ci, si};
                else if(key.first != ci)
                    /* Straddles clusters: the owner set (face_clusters) does
                     * the culling; no single subcluster AABB applies. */
                    key = {kInvalid, kInvalid};
                else if(key.second != si)
                    /* Multiple subclusters of one cluster: cull by cluster
                     * PVS only, not by a single subcluster AABB. */
                    key.second = kInvalid;
            }
        }
    }

    for(auto& owners : face_clusters)
        std::sort(owners.begin(), owners.end());

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

        auto materials_r = lightmap.materials.data(bsp_magic);
        if(materials_r.has_error())
            return {};
        auto materials = materials_r.value();
        out.groups.emplace_back();
        auto& group = out.groups.back();

        for(blam::bsp::material const& mat : materials)
        {
            auto shader    = shader_cache.predict(mat.shader);
            auto vertex_id = lightmap.lightmap_idx |
                             (static_cast<u64>(mat.shader.tag_id) << 32);

            blam::reference<libc_types::byte_t, blam::xbox_t> vref{}, lref{};
            blam::map_ptr vmag = bsp_magic;
            if constexpr(std::is_same_v<V, blam::mcc_version_t>)
            {
                using lv    = blam::bsp::material::pc_light_vertex;
                u32 vsz     = mat.pc.count * mat.vertex_size();
                u32 vend    = mat.pc.padding2[0];
                vref.count  = vsz;
                vref.offset = mcc_vert_base + vend - vsz;
                lref.count  = mat.pc.count * sizeof(lv);
                lref.offset = mcc_vert_base + vend;
                vmag        = vertex_magic;
            } else
            {
                vref = mat.vertices();
                lref = mat.light_verts();
            }
            auto vr = vref.data(vmag);
            auto lr = lref.data(vmag);
            if(!vr.has_value() || !lr.has_value())
                continue;
            auto vertices       = vr.value();
            auto light_vertices = lr.value();

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

            /* Split material faces into sub-meshes by (owner cluster set ×
             * subcluster). Single-owner faces keep their subcluster split as
             * before; boundary-straddling faces group by their full owner set
             * so the occluder can hide them when no owner is visible.
             * Faces with no owner at all (empty set) are always visible. */
            auto faces     = mat.indices(section).data(bsp_magic).value();
            u32  mat_start = mat.surfaces.count;
            u32  mat_end   = mat_start + mat.surfaces.offset;
            u32  vert_base = vert_ptr / mat.vertex_size();
            u32  vert_size = mat.vertex_size();

            using ChunkKey = std::pair<std::vector<u16>, u32>;
            std::map<ChunkKey, std::vector<u32>> sub_faces;
            for(u32 fi = mat_start; fi < mat_end; fi++)
            {
                std::vector<u16> owners =
                    fi < face_clusters.size() ? face_clusters[fi]
                                              : std::vector<u16>{};
                u32 sid = kInvalid;
                if(owners.size() == 1 && fi < face_subcluster.size() &&
                   face_subcluster[fi].first == owners.front())
                    sid = face_subcluster[fi].second;
                sub_faces[{std::move(owners), sid}].push_back(fi);
            }

            for(auto const& [key, face_idxs] : sub_faces)
            {
                auto const& [owners, sid] = key;
                u32 cid =
                    owners.size() == 1 ? static_cast<u32>(owners.front())
                                       : kInvalid;
                u32 sub_start = element_ptr;
                /* Chunk AABB from the chunk's own vertices; both vertex
                 * layouts (compressed/uncompressed) start with Vecf3
                 * position. */
                Vecf3 bmin{std::numeric_limits<f32>::max()};
                Vecf3 bmax{std::numeric_limits<f32>::lowest()};
                bool  has_bounds = false;
                for(u32 fi : face_idxs)
                {
                    auto const& face            = faces[fi - mat_start];
                    element_buffer[element_ptr] = face;
                    element_ptr++;
                    for(u32 k = 0; k < 3; k++)
                    {
                        size_t voff = static_cast<size_t>(face[k]) * vert_size;
                        if(voff + sizeof(Vecf3) > vertices.size_bytes())
                            continue;
                        Vecf3 pos{};
                        std::memcpy(
                            &pos, vertices.data() + voff, sizeof(Vecf3));
                        bmin       = glm::min(bmin, pos);
                        bmax       = glm::max(bmax, pos);
                        has_bounds = true;
                    }
                }

                group.meshes.emplace_back();
                auto& mesh = group.meshes.back();
                mesh.mesh  = &mat;
                mesh.draw  = {
                     .elements =
                        {
                             .count  = static_cast<u32>(face_idxs.size() * 3),
                             .offset = sub_start * sizeof(blam::vert::face),
                             .vertex_offset = vert_base,
                             .type          = semantic::type_t::u16,
                        },
                     .instances =
                        {
                             .count = 1,
                        },
                };
                mesh.shader         = shader_cache.predict(mat.shader);
                mesh.light_bitm     = light_bitm;
                mesh.cluster_idx    = cid;
                mesh.subcluster_idx = sid;
                mesh.clusters       = owners;
                mesh.bmin           = bmin;
                mesh.bmax           = bmax;
                mesh.has_bounds     = has_bounds;
            }

            vert_ptr += vertices.size_bytes();
            light_ptr += light_vertices.size_bytes();
        }
    }

    {
        u32 single = 0, multi = 0, unassigned = 0;
        for(auto const& grp : out.groups)
            for(auto const& m : grp.meshes)
                (m.clusters.empty()  ? unassigned
                 : m.clusters.size() == 1 ? single
                                          : multi)++;
        cDebug(
            "BSP mesh cluster assignment: {} single-cluster, {} multi-cluster,"
            " {} unassigned of {}",
            single,
            multi,
            unassigned,
            single + multi + unassigned);
    }

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
                .type          = semantic::type_t::u16,
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

            /* Remap local node indices to global bone indices (PC only) */
            if constexpr(!std::is_same_v<V, blam::xbox_version_t>)
            {
                using flags_t = blam::mod2::model_flags_t;
                if(static_cast<u16>(header->flags) &
                   static_cast<u16>(flags_t::local_nodes))
                {
                    auto const* wrap = reinterpret_cast<
                        blam::mod2::part_wrap_header<V> const*>(part);
                    u8     node_count = wrap->unknown_2[3];
                    size_t vcount     = vertices.size();
                    if(node_count > 0)
                    {
                        /* Bound to this part's verts; vert_dest spans the rest
                         * of the shared buffer. */
                        for(size_t k = 0; k < vcount; k++)
                        {
                            auto& v = vert_dest[k];
                            if(v.weights.node0 < node_count)
                                v.weights.node0 =
                                    wrap->unknown_2[4 + v.weights.node0];
                            if(v.weights.node1 < node_count)
                                v.weights.node1 =
                                    wrap->unknown_2[4 + v.weights.node1];
                        }
                    }
                }
            }

            vert_ptr += vertices.size_bytes();
            element_ptr += elements.size_bytes();
        }
    }

    if(auto bones_opt = header->bones.data(magic); bones_opt.has_value())
    {
        auto bones = bones_opt.value();
        u32  n     = static_cast<u32>(bones.size());
        out.bone_matrices.resize(n, Matf4(1));
        out.inv_bind.resize(n);

        /* Compute world bind transforms by walking the parent chain.
         * Bones are stored in DFS order so parent index < child index. */
        std::vector<Matf4> world_bind(n);
        for(u32 i = 0; i < n; i++)
        {
            auto const& b = bones[i];
            /* Halo bone rotations are stored as the conjugate (inverse) of the
             * rotation GLM's forward kinematics expects, so conjugate them. */
            Quatf br    = glm::conjugate(b.rotation);
            Matf4 local = glm::translate(Matf4(1), b.translation) *
                          glm::mat4_cast(br);
            if(b.parent != blam::mod2::bone::invalid_bone && b.parent < i)
                world_bind[i] = world_bind[b.parent] * local;
            else
                world_bind[i] = local;

            /* inv_bind = inverse of world bind transform.
             * world_bind * inv(world_bind) = I, so bind-pose bone_matrices are identity. */
            out.inv_bind[i]      = glm::inverse(world_bind[i]);
            out.bone_matrices[i] = Matf4(1);
        }
    }

    return out;
}

template ModelItem<halo_version> ModelCache<halo_version>::predict_impl(
    const blam::tagref_t& mod2, blam::mod2::mod2_lod lod);

template<typename V>
void ModelCache<V>::apply_animation(
    generation_idx_t          model_id,
    blam::antr::header const* antr,
    u32                       anim_idx,
    u32                       frame_idx)
{
    ModelItem<V>& item = this->get(model_id);
    if(item.inv_bind.empty())
        return;

    u32 n = static_cast<u32>(item.inv_bind.size());

    auto anims_opt = antr->animations.data(magic);
    if(!anims_opt.has_value())
        return;
    auto anims = anims_opt.value();

    if(anim_idx >= static_cast<u32>(anims.size()))
        return;

    /* In PC cache files, antr::nodes is stripped — use mod2 bone parent chain */
    if(!item.header)
        return;
    auto bones_opt = item.header->bones.data(magic);
    if(!bones_opt.has_value() || bones_opt.value().size() < n)
        return;
    auto bones = bones_opt.value();

    auto const& anim = anims[anim_idx];
    if(anim.is_compressed())
        return;
    if(frame_idx >= static_cast<u32>(anim.frame_count))
        frame_idx = 0;

    auto default_bytes_opt = anim.default_data.data(magic);
    auto frame_bytes_opt   = anim.frame_data.data(magic);
    if(!default_bytes_opt.has_value() || !frame_bytes_opt.has_value())
        return;

    auto default_bytes = default_bytes_opt.value();
    auto frame_bytes   = frame_bytes_opt.value();

    std::vector<Quatf> rotations(n, Quatf(1, 0, 0, 0));
    std::vector<Vecf3> translations(n, Vecf3(0));

    /* Data is interleaved BY NODE: iterate nodes in order; each node emits its
     * [rotation][translation][scale] channels. Animated channels come from
     * frame_data (at the current frame), non-animated from default_data.
     * frame_info (root motion) is a SEPARATE block, not in frame_data.
     * Scale is a single float we don't use, but its bytes must be skipped. */
    size_t d = 0;                                            /* default cursor */
    size_t f = static_cast<size_t>(frame_idx) * anim.frame_size; /* frame cursor */

    auto read_quat = [](semantic::Span<const byte_t> buf, size_t off) {
        return reinterpret_cast<blam::antr::compressed_quat_t const*>(
                   buf.data() + off)
            ->decompress();
    };
    auto read_vec3 = [](semantic::Span<const byte_t> buf, size_t off) {
        return *reinterpret_cast<Vecf3 const*>(buf.data() + off);
    };

    for(u32 i = 0; i < n; i++)
    {
        /* Intra-node channel order: [rotation 8B][translation 12B][scale 4B] */
        /* rotation */
        if(anim.has_rotation(i))
        {
            if(f + 8 <= frame_bytes.size()) rotations[i] = read_quat(frame_bytes, f);
            f += 8;
        } else
        {
            if(d + 8 <= default_bytes.size()) rotations[i] = read_quat(default_bytes, d);
            d += 8;
        }
        /* translation */
        if(anim.has_translation(i))
        {
            if(f + 12 <= frame_bytes.size()) translations[i] = read_vec3(frame_bytes, f);
            f += 12;
        } else
        {
            if(d + 12 <= default_bytes.size()) translations[i] = read_vec3(default_bytes, d);
            d += 12;
        }
        /* scale (single float, skipped) */
        if(anim.has_scale(i))
            f += 4;
        else
            d += 4;
    }

    /* antr quats use the same conjugate convention as the mod2 bind quats. */
    for(u32 i = 0; i < n; i++)
        rotations[i] = glm::conjugate(rotations[i]);

    /* Build world transforms using mod2 bone parent chain (DFS order: parent<i) */
    std::vector<Matf4> world(n);
    for(u32 i = 0; i < n; i++)
    {
        Matf4 local = glm::translate(Matf4(1), translations[i]) *
                      glm::mat4_cast(rotations[i]);
        u16 parent = bones[i].parent;
        if(parent != blam::mod2::bone::invalid_bone && parent < i)
            world[i] = world[parent] * local;
        else
            world[i] = local;
    }

    for(u32 i = 0; i < n; i++)
        item.bone_matrices[i] = world[i] * item.inv_bind[i];
}

template void ModelCache<halo_version>::apply_animation(
    generation_idx_t, blam::antr::header const*, u32, u32);

template<typename V>
void ModelCache<V>::tick_animations(f32 time_s)
{
    for(auto& [raw_id, item] : this->m_cache)
    {
        if(!item.antr_hdr || item.anim_frame_count == 0 || item.inv_bind.empty())
            continue;
        u32 frame = static_cast<u32>(time_s * 30.f) % item.anim_frame_count;
        generation_idx_t gen_id = {raw_id, this->generation};
        apply_animation(gen_id, item.antr_hdr, item.anim_idx, frame);
    }
}
template void ModelCache<halo_version>::tick_animations(f32);

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

        out.senv.self_illum = get_bitm_idx(shader_model.self_illum.map.map);
        out.senv.bump       = get_bitm_idx(shader_model.bump.map);
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
        auto const& shader_model = *extract_shader<shader_chicago<blam::pc_version_t>>(it);

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
                out.schi.layers.at(i++) = predict(layer);
        }

        break;
    }
    case tag_class_t::scex: {
        auto const& shader_model =
            *extract_shader<shader_chicago_extended<blam::pc_version_t>>(it);

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
                out.scex.layers.at(i++) = predict(layer);
        }

        break;
    }
    case tag_class_t::sgla: {
        shader_glass const& shader_model = *extract_shader<shader_glass>(it);
        out.sgla.diffuse         = get_bitm_idx(shader_model.diffuse.map.map);
        out.sgla.reflection_cube = get_bitm_idx(shader_model.reflection.map);
        out.sgla.bump            = get_bitm_idx(shader_model.reflection.bump_map.map);
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
        auto const& shader_model   = *extract_shader<shader_plasma>(it);
        out.spla.primary_noise   = get_bitm_idx(shader_model.primary_noise.noise.map);
        out.spla.secondary_noise = get_bitm_idx(shader_model.secondary_noise.noise.map);
        break;
    }
    case tag_class_t::smet: {
        auto const& shader_model = *extract_shader<shader_meter>(it);
        out.smet.map             = get_bitm_idx(shader_model.map);
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
    const typing::vector_types::Vecf2& base_map_scale,
    std::optional<material_context>    context)
{
    using blam::tag_class_t;
    using namespace blam::shader;
    using enum_helpers::feval;

    ShaderItem const& shader = find(shader_id)->second;

    mat = {};

    switch(shader.tag_class)
    {
    case tag_class_t::scex: {
        shader_chicago_extended<blam::pc_version_t> const* info =
            shader.header->as<blam::shader::shader_chicago_extended<blam::pc_version_t>>();

        auto maps = info->maps_4stage.data(magic).value();
        for(auto i : range<>(4))
        {
            auto id = shader.scex.maps.at(i);
            if(!shader.scex.maps.at(i).valid())
                continue;
            BitmapItem const& bitm =
                *bitm_cache.assign_atlas_data(mat.maps[i], id);
            chicago::map_t const& map = maps[i];
            mat.maps[i].uv_scale      = map.map.uv_scale * base_map_scale;
            mat.maps[i].bias          = bitm.image.bias - 2.f;

            u16 flags = static_cast<u8>(map.color_func) |
                        (static_cast<u8>(map.alpha_func) << 4);
            mat.lightmap.meta1 |= flags << (i * 8);
        }

        mat.material.material = materials::id::scex;
        break;
    }
    case tag_class_t::schi: {
        shader_chicago<blam::pc_version_t> const* info =
            shader.header->as<blam::shader::shader_chicago<blam::pc_version_t>>();

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
            mat.maps[i].bias          = bitm.image.bias - 2.f;

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

        mat.material.flags |= (primary && secondary ? 1 : 0) << 9;

        // Self-illum takes map slot 1 when present; micro otherwise.
        if(shader.senv.self_illum.valid())
        {
            auto* si = bitm_cache.assign_atlas_data(
                mat.maps[1], shader.senv.self_illum);
            if(si)
                mat.maps[1].uv_scale = Vecf2(info->self_illum.map.scale);
            mat.lightmap.meta1 = 1;
        } else
        {
            auto* micro = bitm_cache.assign_atlas_data(
                mat.maps[1], shader.senv.micro_bitm);
            if(micro)
            {
                mat.maps[1].uv_scale = Vecf2(info->diffuse.micro.scale);
                mat.maps[1].bias     = micro->image.bias;
            }
            mat.material.flags |= (micro ? 1 : 0) << 10;
            mat.lightmap.meta1 = 0;
        }
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
         *
         * Allocation of material inputs:
         * i1.0: lightmap brightness
         * i1.1: specular brightness
         * 0: specular perp. color + brightness
         * 1: specular paral. color + brightness
         * 2: uv offset + ???
         * 3: power on/off/plasma in partitioned vector
         */

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
        mat.maps[1].bias     = 10.f; // Higher bias makes the water look better

        mat.material.material = materials::id::swat;
        mat.material.flags    = static_cast<u32>(info->flags);
        /* anim_velocity is in UV tiles/sec at 1x tiling. The bump map's
         * uvscale (= base_map_scale * ripple.scale) multiplies offsets,
         * so divide by ripple.scale to keep visual scroll rate independent
         * of texture tiling. Apply a floor so slow-velocity maps (e.g. c10)
         * still show perceptible animation; effective_rate = max(vel, floor).
         */
        constexpr f32 min_tiles_per_sec = 0.03f;
        f32           effective_vel =
            std::max(info->ripple.anim_velocity, min_tiles_per_sec);
        f32 norm_velocity = info->ripple.scale > 0.f
                                ? effective_vel / info->ripple.scale
                                : effective_vel;
        mat.material.inputs1 =
            Vecf2{glm::radians(info->ripple.anim_angle), norm_velocity};
        mat.material.inputs[0] =
            Vecf4(info->parallel.tint_color, info->parallel.brightness);
        mat.material.inputs[1] = Vecf4(
            info->perpendicular.tint_color, info->perpendicular.brightness);
        break;
    }
    case tag_class_t::sgla: {
        auto const* info = shader.header->as<blam::shader::shader_glass>();
        bitm_cache.assign_atlas_data(mat.maps[0], shader.sgla.diffuse);
        mat.maps[0].uv_scale = Vecf2(info->diffuse.map.scale);
        mat.maps[0].bias     = 0;
        if(shader.sgla.bump.valid())
        {
            bitm_cache.assign_atlas_data(mat.maps[1], shader.sgla.bump);
            mat.maps[1].uv_scale = Vecf2(info->reflection.bump_map.scale);
            mat.maps[1].bias     = 0;
        }
        if(shader.sgla.reflection_cube.valid())
            mat.lightmap.reflection =
                bitm_cache.get_atlas_layer(shader.sgla.reflection_cube);
        mat.material.flags    = static_cast<u32>(info->flags);
        mat.material.inputs[0] = Vecf4(info->background_tint.color, 1.f);
        mat.material.inputs[1] = Vecf4(
            info->reflection.perpendicular.tint_color,
            info->reflection.perpendicular.brightness);
        mat.material.inputs[2] = Vecf4(
            info->reflection.parallel.tint_color,
            info->reflection.parallel.brightness);
        mat.material.material = materials::id::sgla;
        break;
    }
    case tag_class_t::smet: {
        auto const* info = shader.header->as<blam::shader::shader_meter>();
        bitm_cache.assign_atlas_data(mat.maps[0], shader.smet.map);
        mat.maps[0].uv_scale = Vecf2(1);
        mat.maps[0].bias     = 0;
        mat.material.flags   = static_cast<u32>(info->flags);
        mat.material.inputs1 = Vecf2{1.f, info->colors.transparency};
        mat.material.inputs[0] =
            Vecf4(info->colors.gradient_min, info->colors.background_transparency);
        mat.material.inputs[1] = Vecf4(info->colors.gradient_max, 1.f);
        mat.material.inputs[2] = Vecf4(info->colors.background, 1.f);
        mat.material.inputs[3] = Vecf4(info->colors.flash, 1.f);
        mat.material.inputs[4] = Vecf4(info->colors.tint, 1.f);
        mat.material.material  = materials::id::smet;
        break;
    }
    case tag_class_t::spla: {
        auto const* info = shader.header->as<blam::shader::shader_plasma>();
        bitm_cache.assign_atlas_data(mat.maps[0], shader.spla.primary_noise);
        mat.maps[0].uv_scale =
            Vecf2(info->primary_noise.noise.scale);
        mat.maps[0].bias = 0;
        if(shader.spla.secondary_noise.valid())
        {
            bitm_cache.assign_atlas_data(mat.maps[1], shader.spla.secondary_noise);
            mat.maps[1].uv_scale =
                Vecf2(info->secondary_noise.noise.scale);
            mat.maps[1].bias = 0;
        }
        mat.material.inputs1   = Vecf2{info->intensity.exponent, 0.f};
        mat.material.inputs[0] = Vecf4(
            info->color.perpendicular_tint, info->color.perpendicular_brightness);
        mat.material.inputs[1] = Vecf4(
            info->color.parellel_tint, info->color.parallel_brightness);
        mat.material.inputs[2] = Vecf4(
            info->primary_noise.anim_dir,
            info->primary_noise.anim_period > 0.f
                ? 1.f / info->primary_noise.anim_period
                : 0.f);
        mat.material.inputs[3] = Vecf4(
            info->secondary_noise.anim_dir,
            info->secondary_noise.anim_period > 0.f
                ? 1.f / info->secondary_noise.anim_period
                : 0.f);
        mat.material.material = materials::id::spla;
        break;
    }
    case tag_class_t::sotr: {
        shader_transparent const* info = shader.header->as<shader_transparent>();
        auto maps_ = info->maps.data(magic);
        if(maps_.has_error())
        {
            mat.material.material = materials::id::sotr;
            break;
        }
        auto maps = maps_.value();

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
        using flags_t = blam::shader::shader_model::model_flags;
        using detail_func_t = blam::shader::shader_model::detail_function_t;
        using detail_mask_t = blam::shader::shader_model::detail_mask_t;

        mat.material.flags =
            (feval(info->flags & flags_t::detail_after_reflection) ? 0x1 : 0) |
            (info->maps.detail.function == detail_func_t::multiply ? 0x2 : 0) |
            (info->maps.detail.function == detail_func_t::double_biased_add ? 0x4 : 0) |
            (info->maps.detail.mask == detail_mask_t::reflection_mask ? 0x8 : 0) |
            (info->maps.detail.mask == detail_mask_t::reflection_mask_inverse ? 0x10 : 0) |
            (info->maps.detail.mask == detail_mask_t::change_color_mask ? 0x20 : 0) |
            (info->maps.detail.mask == detail_mask_t::change_color_mask_inverse ? 0x40 : 0) |
            (info->maps.detail.mask == detail_mask_t::self_illum_mask ? 0x80 : 0) |
            (info->maps.detail.mask == detail_mask_t::self_illum_mask_inverse ? 0x100 : 0) |
            (info->maps.detail.mask == detail_mask_t::multipurpose_map_alpha ? 0x200 : 0) |
            (info->maps.detail.mask == detail_mask_t::multipurpose_map_alpha_inverse ? 0x400 : 0);

        auto* soso_base =
            bitm_cache.assign_atlas_data(mat.maps[0], shader.soso.base_bitm);
        mat.maps[0].uv_scale = base_map_scale;
        mat.maps[0].bias     = soso_base ? soso_base->image.bias - 2.f : -2.f;

        auto* soso_multi =
            bitm_cache.assign_atlas_data(mat.maps[1], shader.soso.multi_bitm);
        mat.maps[1].uv_scale = base_map_scale;
        mat.maps[1].bias     = soso_multi ? soso_multi->image.bias - 2.f : -2.f;

        auto* detail =
            bitm_cache.assign_atlas_data(mat.maps[2], shader.soso.detail_bitm);
        if(detail)
        {
            mat.maps[2].uv_scale = Vecf2(info->maps.detail.scale);
            mat.maps[2].bias     = detail->image.bias;
        }

        mat.lightmap.reflection =
            bitm_cache.get_atlas_layer(shader.soso.reflection_bitm);

        mat.material.inputs[1] = Vecf4(
            info->reflection.perpendicular_tint,
            info->reflection.perpendicular_brightness);
        mat.material.inputs[2] = Vecf4(
            info->reflection.parallel_tint,
            info->reflection.parallel_brightness);

        mat.material.inputs[0] = Vecf4(Vecf3(1.f), 0.f);
        using change_color_t   = blam::shader::shader_model::change_color_t;
        if(context.has_value())
        {
            do
            {
                if(info->change_color_src == change_color_t::none)
                    break;
                auto unit   = std::get<blam::scn::unit const*>(*context);
                auto colors = unit->change_colors.data(magic).value();
                if(colors.empty())
                    break;
                auto i = static_cast<u32>(info->change_color_src) - 1u;
                if((i + 1) > colors.size())
                    break;
                auto perms = colors[i].permutations.data(magic).value();
                if(perms.empty())
                {
                    mat.material.inputs[0] = Vecf4(colors[i].lower_bound, 1.f);
                    break;
                }
                // TODO: Figure out what weights on permutations actually mean
                // The game doesn't use them, but chooses one that's not [0]
                mat.material.inputs[0] = Vecf4(perms[0].upper_bound, 1.f);
            } while(false);
        }

        mat.material.material = materials::id::soso;
        break;
    }
    default: {
        // cDebug(
        //     "Material not set up: {0}/{1}",
        //     shader.tag->tagclass.at(0).str(),
        //     shader.tag->to_name().to_string(magic));
        break;
    }
    }
}

template void ShaderCache<halo_version>::populate_material(
    materials::shader_data&            mat,
    const generation_idx_t&            shader_id,
    const typing::vector_types::Vecf2& base_map_scale,
    std::optional<material_context>    context);

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

    /* Effective mipmap bias per image: never shrink a texture below 64px on
     * its smallest axis — full bias turns small-but-important textures (the
     * 128px Basis planet) into unrecognizable mush. */
    auto bias_of = [this](BitmapItem const& img) -> u32 {
        u32  bias = params->mipmap_bias;
        auto sz   = img.image.mip->isize;
        i32  mind = std::min(sz.x, sz.y);
        while(bias > 0 &&
              ((mind >> bias) < 64 ||
               img.image.mip->mipmaps <= static_cast<i16>(bias)))
            bias--;
        return bias;
    };

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
        imsize >>= bias_of(bitm.second);
        //            auto&       surface =
        //            tex_buckets[bitm.second.image.bucket].surface;

        //            u32 mipmaps = surface->m_mipmaps;
        u32 pad = 0;
        //                = surface->m_format.pixfmt != pix_fmt::RGB565 ? 4
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
        u32 max_pad = 0; // surface->m_format.pixfmt != pix_fmt::RGB565 ? 4
                         // << mipmaps : 0;

        for(auto [id, fmt] : pool.images)
        {
            BitmapItem* img    = &m_cache.find(id)->second;
            auto        imsize = img->image.mip->isize;

            if(u32 bias = bias_of(*img); bias > 0)
            {
                imsize >>= bias;
                img->mipmaps.base = bias;
                img->mipmaps.last =
                    bias +
                    std::min<i32>(8, img->image.mip->mipmaps - bias);
            } else
            {
                img->mipmaps.base = 0;
                img->mipmaps.last = img->image.mip->mipmaps;
            }

            if(img->header->type == blam::bitm::bitmap_type_t::cube ||
                !supports_tex3d)
            {
                // Don't atlas cubemaps
                // Don't atlas when we don't support 2D array textures:
                // each bitmap becomes its own full-size layer texture, so
                // the UV remap is identity (offset 0, scale 1). The
                // post-commit /pool.max normalization is skipped for this
                // path (see below).
                img->image.layer  = layer++;
                img->image.offset = {0.f, 0.f};
                img->image.scale  = {1.f, 1.f};
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
    Profiler::PushContext("Committing textures to GPU memory");
    for(auto& bitm : m_cache)
        commit_bitmap<gfx::compat::texture_2da_t>(bitm.second);
    Profiler::PopContext();

    /* Normalize atlas offset/scale into [0,1] UV space. Skipped when not
     * atlasing (single texture per layer) — those already carry identity
     * offset 0 / scale 1. */
    if(supports_tex3d)
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

    auto shader_name = bitmap.to_name().to_string(magic);
    auto it          = index.template resource<blam::bitm::header_t>(bitmap);

    if(!it.has_value())
        return {};

    auto [bitm_tag, bitm_ptr, curr_magic] = *it;

    auto const& bitm = *bitm_ptr;

    BitmapItem out;
    out.header = &bitm;
    out.tag    = bitm_tag;

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

            fmt.comp = convert::to<pix_components>(fmt.c);
        } else
        {
            fmt.pixfmt                   = image.to_pixfmt();
            std::tie(fmt.bfmt, fmt.comp) = image.to_fmt();
        }

        img.bucket = create_hash(fmt, img.mip->type);
        img.fmt    = fmt;

        switch(im[0].type)
        {
#if GLEAM_MAX_VERSION >= 0x301 || GLEAM_MAX_VERSION_ES >= 0x300
        case blam::bitm::type_t::tex_3d: {
            if(!allocator->feature_info().texture.texture_3d)
                return {};
            auto& bucket = get_bucket<gfx::texture_3d_t>(fmt, img.mip->type);
            img.layer = bucket.ptr++;
            cDebug("3D texture: {}/{}",
                magic_enum::enum_name(fmt.pixfmt),
                magic_enum::enum_name(fmt.cmpflg));
            break;
        }
#endif
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
