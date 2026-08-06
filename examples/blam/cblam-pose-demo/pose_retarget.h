#pragma once

#include <optional>
#include <string_view>

#include <peripherals/libc/types.h>
#include <peripherals/typing/vectors/glm_vector_types.h>

/* Maps a Kalidokit Pose.solve() bone name to a cyborg_mp mod2 bone name.
 * q_target_local = restDelta^-1 * q_source_local * restDelta */
struct BoneRetargetEntry
{
    std::string_view source_name;
    std::string_view blam_bone_name;
    Quatf            rest_delta{1, 0, 0, 0};
    /* bip01 mirrors left/right via Z-180 conjugation, not a sign flip:
     * (w,x,y,z) -> (w,-x,-y,z) on bind quats. */
    bool mirror_z180 = false;
    /* arm: unused (arms not retargeted, left defined for pose_apply.cpp's
     * switch). spine: live-tuned transform, see pose_apply.cpp. */
    enum class basis_t
    {
        none,
        arm,
        spine,
    } basis = basis_t::none;
};

/* Arms/hands not retargeted (never got the axis mapping right) — stay in
 * the pistol-idle animation pose spawn_static_biped selects. */
inline constexpr BoneRetargetEntry kRetargetTable[] = {
    /* Single aggregate torso rotation -> upper spine segment (parents
     * clavicles/neck). Midline, no mirroring. */
    {
        "Spine",
        "bip01 spine1",
        Quatf(1, 0, 0, 0),
        false,
        BoneRetargetEntry::basis_t::spine
    },
};

inline BoneRetargetEntry const* find_retarget_entry(
    std::string_view source_name)
{
    for(auto const& entry : kRetargetTable)
        if(entry.source_name == source_name)
            return &entry;
    return nullptr;
}

template<typename BoneSpan>
inline std::optional<libc_types::u32> find_bone_index(
    BoneSpan const& bones, std::string_view name)
{
    for(libc_types::u32 i = 0; i < bones.size(); ++i)
        if(bones[i].name.str() == name)
            return i;
    return std::nullopt;
}
