#pragma once

#include <optional>
#include <string_view>

#include <peripherals/libc/types.h>

/* The landmark -> bone mapping that used to live here as a constexpr
 * kRetargetTable is now data: see BoneRetarget and find_retarget_entry() in
 * pose_config.h, populated from pose_config.json's "retarget" array. Only the
 * mod2 bone lookup remains here, since it is generic over the bone span type. */

template<typename BoneSpan>
inline std::optional<libc_types::u32> find_bone_index(
    BoneSpan const& bones, std::string_view name)
{
    for(libc_types::u32 i = 0; i < bones.size(); ++i)
        if(bones[i].name.str() == name)
            return i;
    return std::nullopt;
}
