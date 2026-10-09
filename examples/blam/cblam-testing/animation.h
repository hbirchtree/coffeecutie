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
 *   base        - sets the whole pose; base layers average by weight
 *   replacement - overwrites only the nodes it animates
 *   overlay     - adds its frame (a delta from the neutral pose) on top
 */
struct AnimationLayer
{
    blam::antr::header const* graph{nullptr};
    libc_types::u32           animation{0};

    libc_types::f32 time{0.f};   /* seconds since the layer started */
    libc_types::f32 rate{1.f};   /* playback speed */
    libc_types::f32 weight{1.f}; /* 0 silences a layer without clearing it */

    /* Weight the layer fades toward, per second at `fade`; negative = none.
     * A layer that has faded out to 0 is cleared. */
    libc_types::f32 target{-1.f};
    libc_types::f32 fade{0.f};

    /* Aim/look overlays are a yaw x pitch grid of poses, picked by `cell`
     * (fractional column, row) instead of by time. 0 = plays by time. */
    libc_types::u16 grid_columns{0};
    typing::vector_types::Vecf2 cell{};

    bool loop{true};
    bool paused{false};
    bool finished{false}; /* a non-looping layer that ran past its last frame */
    bool sync{false};     /* keeps phase with the heaviest synced layer */

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

    /* Fixed roles, in composition order */
    enum slot_t : libc_types::u32
    {
        base_slot     = 0, /* base_slot .. base_slot+base_slots-1 */
        base_slots    = 4,
        action_slot   = 4, /* replacements: melee, reload, custom */
        aim_slot      = 5,
        aim_move_slot = 6,
        overlay_slot  = 7, /* fire, gestures */
        max_layers    = 8,
    };

    std::array<AnimationLayer, max_layers> layers{};

    /* The entity's own graph, which names in play requests refer to */
    blam::antr::header const* graph{nullptr};

    /* Feet that came down during the last advance (the antr foot frames) */
    enum footstep_t : libc_types::u8
    {
        no_feet    = 0x0,
        left_foot  = 0x1,
        right_foot = 0x2,
    };
    libc_types::u8 footsteps{no_feet};

    /* Sound keys passed during the last advance: graph sound_refs indices */
    struct cue_t
    {
        blam::antr::header const* graph{nullptr};
        libc_types::i16           sound{-1};
    };
    std::array<cue_t, 4> cues{};
    libc_types::u8       cue_count{0};

    /* Where this instance's bones landed in its bucket's window, negative if
     * unposed. Scratch owned by DrawListBuilder. */
    libc_types::i32 bone_base{-1};

    /* Last frame's skinning matrices in model space, for bodies whose
     * collision follows the animation; filled only where asked for */
    std::vector<typing::vector_types::Matf4> pose;

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

    /* Fades a base animation toward `weight` over `seconds`, starting it in
     * a free base slot if it is not playing. Several base animations at once
     * blend by weight, e.g. move-front and move-left for a diagonal. */
    AnimationLayer* blend(
        blam::antr::header const* graph,
        libc_types::u32           animation,
        libc_types::f32           weight,
        libc_types::f32           seconds,
        bool                      loop = true)
    {
        AnimationLayer* layer = find_base(graph, animation);
        if(!layer && weight <= 0.f)
            return nullptr;
        if(!layer)
        {
            layer = free_base();
            bool const first = !any_base();
            *layer           = AnimationLayer{
                          .graph     = graph,
                          .animation = animation,
                          .weight    = first || seconds <= 0.f ? weight : 0.f,
                          .loop      = loop,
            };
        }
        layer->target = weight;
        layer->fade   = seconds > 0.f ? 1.f / seconds : 0.f;
        return layer;
    }

    /* Fades every other base animation out while `animation` fades in */
    AnimationLayer* crossfade(
        blam::antr::header const* graph,
        libc_types::u32           animation,
        libc_types::f32           seconds,
        bool                      loop = true)
    {
        fade_out_base(seconds, graph, animation);
        return blend(graph, animation, 1.f, seconds, loop);
    }

    void fade_out_base(
        libc_types::f32           seconds,
        blam::antr::header const* keep_graph = nullptr,
        libc_types::u32           keep       = 0)
    {
        for(libc_types::u32 i = base_slot; i < base_slot + base_slots; i++)
        {
            auto& layer = layers[i];
            if(!layer.graph ||
               (layer.graph == keep_graph && layer.animation == keep))
                continue;
            layer.target = 0.f;
            layer.fade   = seconds > 0.f ? 1.f / seconds : 0.f;
        }
    }

    void stop(libc_types::u32 slot)
    {
        if(slot < max_layers)
            layers[slot] = AnimationLayer{};
    }

    void stop_all()
    {
        layers.fill(AnimationLayer{});
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

  private:
    AnimationLayer* find_base(
        blam::antr::header const* graph, libc_types::u32 animation)
    {
        for(libc_types::u32 i = base_slot; i < base_slot + base_slots; i++)
            if(layers[i].graph == graph && layers[i].animation == animation)
                return &layers[i];
        return nullptr;
    }

    bool any_base() const
    {
        for(libc_types::u32 i = base_slot; i < base_slot + base_slots; i++)
            if(layers[i].active())
                return true;
        return false;
    }

    /* An empty slot, else the faintest one */
    AnimationLayer* free_base()
    {
        AnimationLayer* best = &layers[base_slot];
        for(libc_types::u32 i = base_slot; i < base_slot + base_slots; i++)
        {
            if(!layers[i].graph)
                return &layers[i];
            if(layers[i].weight < best->weight)
                best = &layers[i];
        }
        return best;
    }
};
