#include "pose_apply.h"
#include "pose_config.h"
#include "pose_demo_spawn.h"
#include "pose_retarget.h"

#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <utility>

#include <blam/volta/blam_antr.h>
#include <blam/volta/blam_mod2.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

Vecf3 g_pose_demo_root_offset{0.f};

std::optional<u32>                    g_pose_demo_oneshot_anim_idx;
std::chrono::steady_clock::time_point g_pose_demo_oneshot_start;
std::optional<u32>                    g_pose_demo_loop_anim_idx;

/* Shortest-arc rotation taking a onto b, both unit. Used by aim mode, where
 * the source supplies where a limb points rather than how it is turned. */
static Quatf rotation_between(Vecf3 const& a, Vecf3 const& b)
{
    f32 d = glm::dot(a, b);
    if(d > 0.99999f)
        return Quatf(1.f, 0.f, 0.f, 0.f);
    if(d < -0.99999f)
    {
        /* Opposite: any perpendicular axis will do, so take the one furthest
         * from a to keep the cross product well conditioned. */
        Vecf3 axis = glm::cross(
            a, std::abs(a.x) < 0.9f ? Vecf3(1.f, 0.f, 0.f) : Vecf3(0.f, 1.f, 0.f));
        return glm::angleAxis(glm::pi<f32>(), glm::normalize(axis));
    }
    Vecf3 axis = glm::cross(a, b);
    return glm::normalize(Quatf(1.f + d, axis.x, axis.y, axis.z));
}

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
            u32  effective_idx   = item.anim_idx;
            bool one_shot_active = false;
            f32  one_shot_time_s = 0.f;
            if(g_pose_demo_oneshot_anim_idx &&
               *g_pose_demo_oneshot_anim_idx < static_cast<u32>(anims.size()))
            {
                f32 elapsed = std::chrono::duration<f32>(
                                  std::chrono::steady_clock::now() -
                                  g_pose_demo_oneshot_start)
                                  .count();
                auto const& one_shot_anim =
                    anims[*g_pose_demo_oneshot_anim_idx];
                f32 duration_s =
                    one_shot_anim.frame_count / g_pose_config.animation_fps;
                if(elapsed < duration_s)
                {
                    effective_idx   = *g_pose_demo_oneshot_anim_idx;
                    one_shot_active = true;
                    one_shot_time_s = elapsed;
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
                    auto default_bytes_opt =
                        anim.default_data.data(cache.magic);
                    auto frame_bytes_opt = anim.frame_data.data(cache.magic);
                    if(default_bytes_opt.has_value() &&
                       frame_bytes_opt.has_value())
                    {
                        auto default_bytes = default_bytes_opt.value();
                        auto frame_bytes   = frame_bytes_opt.value();

                        static auto start_time =
                            std::chrono::steady_clock::now();
                        f32 time_s =
                            std::chrono::duration<f32>(
                                std::chrono::steady_clock::now() - start_time)
                                .count();
                        u32 frame_idx;
                        if(one_shot_active)
                            frame_idx =
                                anim.frame_count > 0
                                    ? std::min<u32>(
                                          static_cast<u32>(
                                              one_shot_time_s *
                                                  g_pose_config.animation_fps),
                                          static_cast<u32>(anim.frame_count) -
                                              1)
                                    : 0;
                        else
                            frame_idx =
                                anim.frame_count > 0
                                    ? static_cast<u32>(
                                          time_s * g_pose_config.animation_fps) %
                                          static_cast<u32>(anim.frame_count)
                                    : 0;

                        size_t d = 0;
                        size_t f =
                            static_cast<size_t>(frame_idx) * anim.frame_size;

                        auto read_quat = [](semantic::Span<const byte_t> buf,
                                            size_t                       off) {
                            return reinterpret_cast<
                                       blam::antr::compressed_quat_t const*>(
                                       buf.data() + off)
                                ->decompress();
                        };
                        auto read_vec3 = [](semantic::Span<const byte_t> buf,
                                            size_t                       off) {
                            return *reinterpret_cast<Vecf3 const*>(
                                buf.data() + off);
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
                                    translations[i] =
                                        read_vec3(default_bytes, d);
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

    /* Root motion, taken from whichever entry carries a position — only
     * Kalidokit's Hips does. Independent of the retarget table, so the source
     * bone needs no entry there. */
    if(auto const& root = g_pose_config.root_motion; root.enabled)
    {
        for(auto const& b : bones_json)
        {
            if(b.value("name", std::string{}) != root.source)
                continue;
            if(!b.contains("position") || b["position"].size() != 3)
                break;

            auto const& p = b["position"];
            f32 const   raw[3]{
                p[0].get<f32>(), p[1].get<f32>(), p[2].get<f32>()};

            Vecf3 target;
            for(int i = 0; i < 3; ++i)
                target[i] = root.axis_map[i].sign * raw[root.axis_map[i].source];

            target = target * root.scale + root.offset;

            for(int i = 0; i < 3; ++i)
            {
                if(std::abs(target[i]) < root.deadzone[i])
                    target[i] = 0.f;
                target[i] =
                    std::clamp(target[i], -root.clamp[i], root.clamp[i]);
            }

            g_pose_demo_root_offset =
                root.smoothing > 0.f
                    ? g_pose_demo_root_offset +
                          root.smoothing *
                              (target - g_pose_demo_root_offset)
                    : target;
            break;
        }
    }

    /* Bind-pose local rotations and the world orientation each accumulates to.
     * Bones are in DFS order, so a parent is always done before its children.
     *
     * A model-space source says where the limb is, not how far it has moved,
     * so it replaces the animation on its bone instead of composing onto it.
     * Composing would make the same source rotation mean something different
     * in every frame of the idle: "arm out to the side" applied on top of a
     * pistol-aiming pose swings the arm somewhere else entirely. */
    std::vector<Quatf> bind_local(n), bind_world(n);
    for(u32 i = 0; i < n; ++i)
    {
        bind_local[i] = glm::conjugate(bones[i].rotation);
        u16 parent    = bones[i].parent;
        bind_world[i] = parent != blam::mod2::bone::invalid_bone && parent < i
                            ? bind_world[parent] * bind_local[i]
                            : bind_local[i];
    }

    /* Resolved entries keyed by bone, so the walk below can go in bone order:
     * aim needs its parents already applied, and the shell sends bones in
     * whatever order the solver produced them. */
    std::vector<std::pair<BoneRetarget const*, nlohmann::json const*>> per_bone(
        n, {nullptr, nullptr});
    for(auto const& b : bones_json)
    {
        std::string name  = b.value("name", std::string{});
        auto const* entry = find_retarget_entry(name);
        if(!entry)
            continue;
        auto idx = find_bone_index(bones, entry->blam_bone_name);
        if(!idx)
        {
            cWarning(
                "apply_pose: bone '{}' not found in model",
                entry->blam_bone_name);
            continue;
        }
        per_bone[*idx] = {entry, &b};
    }

    /* World orientation as the walk sees it, kept current so that a bone's
     * ancestors have already contributed by the time it is reached. */
    std::vector<Quatf> cur_world(n);

    for(u32 i = 0; i < n; ++i)
    {
        u16 parent = bones[i].parent;
        auto accum = [&](u32 j) {
            u16 p = bones[j].parent;
            cur_world[j] =
                p != blam::mod2::bone::invalid_bone && p < j
                    ? cur_world[p] * rotations[j]
                    : rotations[j];
        };
        accum(i);

        auto const* entry = per_bone[i].first;
        if(!entry)
            continue;
        auto const& b   = *per_bone[i].second;
        auto const  idx = std::optional<u32>(i);

        /* aim takes a direction rather than a rotation, and turns the bone so
         * its own axis points along it. Everything downstream is shared. */
        if(entry->mode == BoneRetarget::mode_t::aim)
        {
            if(!b.contains("direction") || b["direction"].size() != 3)
                continue;
            auto const& d = b["direction"];
            Vecf3       raw(
                d[0].get<f32>(), d[1].get<f32>(), d[2].get<f32>());
            if(glm::length(raw) < 1e-6f)
                continue;

            /* Same signed permutation as a rotation gets, applied to the
             * vector directly. */
            Vecf3 target;
            for(int k = 0; k < 3; ++k)
                target[k] = entry->axis_map[k].sign * raw[entry->axis_map[k].source];
            target = glm::normalize(target);

            Vecf3 from = glm::normalize(
                cur_world[i] * glm::normalize(entry->aim_axis));
            Quatf q_aim = rotation_between(from, target);

            {
                static std::unordered_map<std::string, Quatf> aim_state;
                auto [it, inserted] =
                    aim_state.try_emplace(entry->source_name, q_aim);
                if(!inserted)
                    it->second = glm::slerp(
                        it->second, q_aim, g_pose_config.smoothing_alpha);
                q_aim = it->second;
            }

            /* Conjugating by the bone's live world orientation turns q_aim,
             * which is in model space, into the bone's own frame. */
            Quatf const& delta = cur_world[i];
            rotations[i] =
                rotations[i] * glm::conjugate(delta) * q_aim * delta;
            accum(i);
            continue;
        }

        if(!b.contains("rotation") || b["rotation"].size() != 4)
            continue;

        auto const& r = b["rotation"];
        /* JSON order is [x, y, z, w]; glm::quat's 4-arg ctor is (w, x, y, z).
         */
        Quatf q_src(
            r[3].get<f32>(), r[0].get<f32>(), r[1].get<f32>(), r[2].get<f32>());

        /* Stage 0: divide out the source's resting orientation. These are
         * absolute orientations, so the delta from rest is the world-frame
         * one, q_now * conj(q_rest), not the body-frame one. */
        q_src = q_src * glm::conjugate(entry->source_rest);

        /* Stage 1: signed axis permutation. Done on the components directly
         * because a reflection is not a rotation and cannot be conjugated in. */
        {
            f32 const imag[3]{q_src.x, q_src.y, q_src.z};
            auto const& map = entry->axis_map;
            q_src           = Quatf(
                q_src.w,
                map[0].sign * imag[map[0].source],
                map[1].sign * imag[map[1].source],
                map[2].sign * imag[map[2].source]);
        }

        /* Stage 2: change of basis. */
        q_src = entry->basis_rotation * q_src *
                glm::conjugate(entry->basis_rotation);

        /* Stage 3: optionally reduce to a single twist angle. */
        if(entry->mode == BoneRetarget::mode_t::twist)
        {
            Vecf3 axis      = glm::normalize(entry->twist_axis);
            f32   projected = glm::dot(Vecf3(q_src.x, q_src.y, q_src.z), axis);
            f32   theta     = 2.f * std::atan2(projected, q_src.w);
            f32   max_theta = glm::radians(entry->clamp_degrees);
            theta           = std::clamp(theta, -max_theta, max_theta);
            q_src           = glm::angleAxis(
                theta * entry->gain, glm::normalize(entry->output_axis));
        }

        {
            /* Exponential moving average via slerp: q = slerp(prev, q_src, alpha).
             * Keyed by source name rather than by entry pointer, since the
             * config's retarget vector owns the entries and reallocating it
             * would leave dangling keys. */
            static std::unordered_map<std::string, Quatf> smoothed_state;

            auto [it, inserted] =
                smoothed_state.try_emplace(entry->source_name, q_src);
            if(!inserted)
                it->second = glm::slerp(
                    it->second, q_src, g_pose_config.smoothing_alpha);
            q_src = it->second;
        }

        /* Model space: start from the bind pose and conjugate by its world
         * orientation, which leaves the bone sitting at exactly q_src in model
         * space. Bone space: compose onto whatever is already there, through
         * the constant the entry was tuned with. */
        if(entry->space == BoneRetarget::space_t::model)
        {
            Quatf const& delta = bind_world[*idx];
            rotations[*idx] =
                bind_local[*idx] * glm::conjugate(delta) * q_src * delta;
        } else
        {
            Quatf const& delta = entry->rest_delta;
            rotations[*idx] =
                rotations[*idx] * glm::conjugate(delta) * q_src * delta;
        }
        accum(i);
    }

    /* Microphone level, applied on top of the pose/animation result. */
    for(auto const& mic_bone : g_pose_config.microphone.bones)
    {
        auto idx = find_bone_index(bones, mic_bone.blam_bone_name);
        if(!idx)
        {
            cWarning(
                "apply_pose: microphone bone '{}' not found in model",
                mic_bone.blam_bone_name);
            continue;
        }
        f32 angle     = g_pose_demo_mic_volume * mic_bone.gain;
        f32 max_angle = glm::radians(mic_bone.clamp_degrees);
        angle         = std::clamp(angle, -max_angle, max_angle);
        rotations[*idx] =
            rotations[*idx] *
            glm::angleAxis(angle, glm::normalize(mic_bone.axis));
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
