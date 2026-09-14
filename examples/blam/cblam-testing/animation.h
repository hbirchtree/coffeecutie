#pragma once

#include <array>
#include <vector>

#include <blam/volta/blam_antr.h>
#include <peripherals/typing/vectors/vector_types.h>

#include "types.h"

/* One animation playing on a model instance.
 *
 * Layers compose in array order. The antr tag carries the composition rule on
 * the animation itself (blam::antr::anim_type), so a layer never picks it:
 *   base        - sets the whole pose
 *   replacement - overwrites only the nodes it animates
 *   overlay     - adds its offset from its own frame 0 to what is already there
 */
struct AnimationLayer
{
    blam::antr::header const* graph{nullptr};
    libc_types::u32           animation{0};

    libc_types::f32 time{0.f};   /* seconds since the layer started */
    libc_types::f32 rate{1.f};   /* playback speed */
    libc_types::f32 weight{1.f}; /* 0 silences a layer without clearing it */

    bool loop{true};
    bool paused{false};
    bool finished{false}; /* a non-looping layer that ran past its last frame */

    bool active() const
    {
        return graph != nullptr && weight > 0.f;
    }
};

/* Per-instance animation state, present only on entities that animate.
 * Without it a model poses nothing, costs no bone slots, and renders from the
 * bind pose its vertices already hold. */
struct AnimationPlayback
{
    using value_type = AnimationPlayback;
    using type       = compo::alloc::VectorContainer<value_type>;

    /* Halo's own stacking: base pose plus aim/look, damage, one spare. */
    static constexpr libc_types::u32 max_layers = 4;

    std::array<AnimationLayer, max_layers> layers{};

    /* Where this instance's bones landed in its bucket's window, negative if
     * unposed. Scratch owned by DrawListBuilder. */
    libc_types::i32 bone_base{-1};

    /* Skinning matrices from outside — motion capture, a ragdoll, anything
     * the layers cannot express. Wins over them when long enough. */
    std::vector<typing::vector_types::Matf4> external_pose;

    /* Starts `animation` from `graph` on `slot`, from the beginning. */
    void play(
        libc_types::u32           slot,
        blam::antr::header const* graph,
        libc_types::u32           animation,
        bool                      loop   = true,
        libc_types::f32           weight = 1.f)
    {
        if(slot >= max_layers)
            return;
        layers[slot] = AnimationLayer{
            .graph     = graph,
            .animation = animation,
            .weight    = weight,
            .loop      = loop,
        };
    }

    void stop(libc_types::u32 slot)
    {
        if(slot < max_layers)
            layers[slot] = AnimationLayer{};
    }

    bool animating() const
    {
        if(!external_pose.empty())
            return true;
        for(auto const& layer : layers)
            if(layer.active())
                return true;
        return false;
    }
};
