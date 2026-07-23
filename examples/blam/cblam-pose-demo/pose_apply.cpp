#include "pose_apply.h"
#include "pose_demo_spawn.h"
#include "pose_retarget.h"

#include <algorithm>
#include <chrono>
#include <unordered_map>

#include <blam/volta/blam_antr.h>
#include <blam/volta/blam_mod2.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

std::optional<u32>                    g_pose_demo_oneshot_anim_idx;
std::chrono::steady_clock::time_point g_pose_demo_oneshot_start;
std::optional<u32>                    g_pose_demo_loop_anim_idx;

std::optional<u32> find_animation_by_name(
    blam::antr::header const* antr_hdr,
    blam::map_ptr const&      magic,
    std::string_view          name)
{
    if(!antr_hdr)
        return std::nullopt;
    auto anims_opt = antr_hdr->animations.data(magic);
    if(!anims_opt.has_value())
        return std::nullopt;
    auto anims = anims_opt.value();
    for(u32 i = 0; i < static_cast<u32>(anims.size()); ++i)
        if(anims[i].name.str() == name)
            return i;
    return std::nullopt;
}

void apply_pose(
    ModelCache<halo_version>& cache,
    generation_idx_t          model_id,
    nlohmann::json const&     bones_json)
{
    auto& item = cache.get(model_id);
    if(item.inv_bind.empty() || !item.header)
        return;

    auto bones_opt = item.header->bones.data(cache.magic);
    if(!bones_opt.has_value())
        return;
    auto bones = bones_opt.value();

    u32 n = static_cast<u32>(item.inv_bind.size());
    if(static_cast<u32>(bones.size()) < n)
        return;

    std::vector<Quatf> rotations(n);
    std::vector<Vecf3> translations(n);

    bool seeded_from_anim = false;
    if(item.antr_hdr)
    {
        auto anims_opt = item.antr_hdr->animations.data(cache.magic);
        if(anims_opt.has_value())
        {
            auto anims = anims_opt.value();

            /* One-shot plays through once (clamped, not looped) from
             * g_pose_demo_oneshot_start; once its duration elapses, clear
             * it and fall back to the loop animation. */
            u32  effective_idx = item.anim_idx;
            bool one_shot_active = false;
            f32  one_shot_time_s = 0.f;
            if(g_pose_demo_oneshot_anim_idx &&
               *g_pose_demo_oneshot_anim_idx < static_cast<u32>(anims.size()))
            {
                f32 elapsed = std::chrono::duration<f32>(
                    std::chrono::steady_clock::now() - g_pose_demo_oneshot_start)
                                  .count();
                auto const& one_shot_anim = anims[*g_pose_demo_oneshot_anim_idx];
                f32         duration_s    = one_shot_anim.frame_count / 30.f;
                if(elapsed < duration_s)
                {
                    effective_idx    = *g_pose_demo_oneshot_anim_idx;
                    one_shot_active  = true;
                    one_shot_time_s  = elapsed;
                } else
                    g_pose_demo_oneshot_anim_idx.reset();
            }
            if(!one_shot_active && g_pose_demo_loop_anim_idx &&
               *g_pose_demo_loop_anim_idx < static_cast<u32>(anims.size()))
                effective_idx = *g_pose_demo_loop_anim_idx;

            if(effective_idx < static_cast<u32>(anims.size()))
            {
            auto const& anim = anims[effective_idx];
            if(!anim.is_compressed())
            {
                auto default_bytes_opt = anim.default_data.data(cache.magic);
                auto frame_bytes_opt   = anim.frame_data.data(cache.magic);
                if(default_bytes_opt.has_value() && frame_bytes_opt.has_value())
                {
                    auto default_bytes = default_bytes_opt.value();
                    auto frame_bytes   = frame_bytes_opt.value();

                    static auto start_time = std::chrono::steady_clock::now();
                    f32         time_s     = std::chrono::duration<f32>(
                        std::chrono::steady_clock::now() - start_time)
                                            .count();
                    u32 frame_idx;
                    if(one_shot_active)
                        frame_idx = anim.frame_count > 0
                                        ? std::min<u32>(
                                              static_cast<u32>(one_shot_time_s * 30.f),
                                              static_cast<u32>(anim.frame_count) - 1)
                                        : 0;
                    else
                        frame_idx = anim.frame_count > 0
                                        ? static_cast<u32>(time_s * 30.f) %
                                              static_cast<u32>(anim.frame_count)
                                        : 0;

                    size_t d = 0;
                    size_t f = static_cast<size_t>(frame_idx) * anim.frame_size;

                    auto read_quat = [](semantic::Span<const byte_t> buf, size_t off) {
                        return reinterpret_cast<blam::antr::compressed_quat_t const*>(
                                   buf.data() + off)
                            ->decompress();
                    };
                    auto read_vec3 = [](semantic::Span<const byte_t> buf, size_t off) {
                        return *reinterpret_cast<Vecf3 const*>(buf.data() + off);
                    };

                    rotations.assign(n, Quatf(1, 0, 0, 0));
                    translations.assign(n, Vecf3(0));
                    for(u32 i = 0; i < n; i++)
                    {
                        if(anim.has_rotation(i))
                        {
                            if(f + 8 <= frame_bytes.size())
                                rotations[i] = read_quat(frame_bytes, f);
                            f += 8;
                        } else
                        {
                            if(d + 8 <= default_bytes.size())
                                rotations[i] = read_quat(default_bytes, d);
                            d += 8;
                        }
                        if(anim.has_translation(i))
                        {
                            if(f + 12 <= frame_bytes.size())
                                translations[i] = read_vec3(frame_bytes, f);
                            f += 12;
                        } else
                        {
                            if(d + 12 <= default_bytes.size())
                                translations[i] = read_vec3(default_bytes, d);
                            d += 12;
                        }
                        if(anim.has_scale(i))
                            f += 4;
                        else
                            d += 4;
                    }
                    for(u32 i = 0; i < n; i++)
                        rotations[i] = glm::conjugate(rotations[i]);
                    seeded_from_anim = true;
                }
            }
            }
        }
    }
    if(!seeded_from_anim)
    {
        for(u32 i = 0; i < n; ++i)
        {
            rotations[i]    = glm::conjugate(bones[i].rotation);
            translations[i] = bones[i].translation;
        }
    }

    for(auto const& b : bones_json)
    {
        std::string name = b.value("name", std::string{});
        auto const* entry = find_retarget_entry(name);
        if(!entry)
            continue;
        auto idx = find_bone_index(bones, entry->blam_bone_name);
        if(!idx)
        {
            cWarning("apply_pose: bone '{}' not found in model", entry->blam_bone_name);
            continue;
        }
        // cDebug("apply_pose: retargeting '{}' -> '{}' (bone idx {})", name, entry->blam_bone_name, *idx);
        if(!b.contains("rotation") || b["rotation"].size() != 4)
            continue;

        auto const& r = b["rotation"];
        /* JSON order is [x, y, z, w]; glm::quat's 4-arg ctor is (w, x, y, z). */
        Quatf q_src(
            r[3].get<f32>(), r[0].get<f32>(), r[1].get<f32>(), r[2].get<f32>());

        static const Quatf kArmBasis = glm::quat_cast(Matf3(
            Vecf3{0.f, 0.f, 1.f}, Vecf3{1.f, 0.f, 0.f}, Vecf3{0.f, 1.f, 0.f}));
        switch(entry->basis)
        {
        case BoneRetargetEntry::basis_t::arm:
            q_src = Quatf(q_src.w, q_src.z, q_src.x, q_src.y);
            break;
        case BoneRetargetEntry::basis_t::spine:
        {
            f32 theta = 2.f * std::atan2(q_src.y, q_src.w);
            constexpr f32 kMaxTheta = 1.0472f; /* 60 degrees */
            theta = std::clamp(theta, -kMaxTheta, kMaxTheta);
            q_src = glm::angleAxis(theta * 2.f, Vecf3(1.f, 0.f, 0.f));
            break;
        }
        case BoneRetargetEntry::basis_t::none:
            break;
        }

        if(entry->mirror_z180)
        {
            static const Quatf kMirrorZ180(0.f, 0.f, 0.f, 1.f);
            q_src = kMirrorZ180 * q_src * glm::conjugate(kMirrorZ180);
        }

        {
            struct SmoothingState
            {
                std::vector<Quatf> history; /* oldest first, capped at kWindow */
            };
            static std::unordered_map<BoneRetargetEntry const*, SmoothingState>
                          smoothing_states;
            constexpr size_t kWindow = 3;

            auto& state = smoothing_states[entry];
            if(!state.history.empty() &&
               glm::dot(q_src, state.history.back()) < 0.f)
                q_src = -q_src;
            state.history.push_back(q_src);
            if(state.history.size() > kWindow)
                state.history.erase(state.history.begin());

            Quatf sum(0.f, 0.f, 0.f, 0.f);
            for(auto const& q : state.history)
                sum += q;
            q_src = glm::normalize(sum);
        }

        Quatf const& delta = entry->rest_delta;
        rotations[*idx]    = rotations[*idx] * glm::conjugate(delta) * q_src * delta;
    }

    if(auto spine_idx = find_bone_index(bones, "bip01 spine1"))
    {
        f32 bob_angle = -g_pose_demo_mic_volume * 2.f;
        rotations[*spine_idx] =
            rotations[*spine_idx] * glm::angleAxis(bob_angle, Vecf3(0.f, 0.f, 1.f));
    }

    std::vector<Matf4> world(n);
    for(u32 i = 0; i < n; ++i)
    {
        Matf4 local = glm::translate(Matf4(1), translations[i]) *
                      glm::mat4_cast(rotations[i]);
        u16 parent = bones[i].parent;
        if(parent != blam::mod2::bone::invalid_bone && parent < i)
            world[i] = world[parent] * local;
        else
            world[i] = local;
    }
    for(u32 i = 0; i < n; ++i)
        item.bone_matrices[i] = world[i] * item.inv_bind[i];

    item.anim_frame_count = 0;
}
