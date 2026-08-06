#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <peripherals/libc/types.h>
#include <peripherals/stl/json.h>
#include <peripherals/typing/vectors/glm_vector_types.h>

using libc_types::f32;
using typing::vector_types::Matf4;
using typing::vector_types::Quatf;
using typing::vector_types::Vecf3;

/* Runtime configuration for BlamPoseDemo, read from pose_config.json.
 *
 * The shell fetches the same file (overridable with ?config=<url>) and writes
 * it into MEMFS at /pose_config.json from the module's preRun hook, so it is
 * on disk before main() runs and can be read synchronously here — the map
 * name lives in it, so it has to be available before the map fetch starts.
 * The shell consumes only the "pose" section; everything else is ours.
 *
 * Every field is optional. A missing file, malformed JSON, or an absent key
 * falls back to the value that was hardcoded before this existed, so the
 * demo still runs with no config present at all. */

/* One output component of a signed axis permutation: which source component it
 * takes, and its sign. Written in JSON as "x" / "-z" / "+y". Shared by the
 * per-bone rotation basis and by root motion, which both need to reconcile
 * Kalidokit's axes with the model's. */
struct AxisPick
{
    /* 0=x, 1=y, 2=z of the source. */
    int source{0};
    f32 sign{1.f};
};

/* Kalidokit's Hips is the only solved output carrying a position, so it drives
 * the biped's root transform rather than a bone. The value is a normalized,
 * roughly hip-width-relative estimate, not metres, so scale is doing real work
 * here.
 *
 * Conditioning order:
 *   remap -> scale + offset -> deadzone -> clamp -> EMA
 * with the EMA held across pose events, which is where smoothing lives. */
struct RootMotionConfig
{
    bool enabled{false};

    /* Solved bone supplying the position. Only Hips has one today. */
    std::string source{"Hips"};

    /* Reconciles Kalidokit's axes with the model's. */
    AxisPick axis_map[3]{{0, 1.f}, {1, 1.f}, {2, 1.f}};

    /* Per-axis direction and amplitude, then a constant bias. */
    Vecf3 scale{1.f, 1.f, 1.f};
    Vecf3 offset{0.f, 0.f, 0.f};

    /* Per-axis: magnitudes below deadzone read as zero, above clamp are
     * capped. Axes mean different things, so both are per-axis rather than a
     * single magnitude. */
    Vecf3 deadzone{0.f, 0.f, 0.f};
    Vecf3 clamp{1.f, 1.f, 1.f};

    /* EMA factor: offset += smoothing * (target - offset). 0 disables it and
     * the raw value passes through. */
    f32 smoothing{0.f};
};

/* How a solved source rotation is carried into the biped's bone space.
 *
 * The source quaternion goes through three stages, in order:
 *
 *   1. axis_map      — signed permutation of the imaginary part. Expresses
 *                      axis swaps and handedness flips, which a rotation
 *                      cannot (a reflection has determinant -1).
 *   2. basis_rotation— change of basis by conjugation, q' = B * q * conj(B).
 *                      Any true rotation of the frame, at whatever angle.
 *   3. mode          — direct passes the result through; twist projects it
 *                      onto one axis and re-emits that angle about another.
 *
 * The old none/arm/spine presets are all expressible here and still accepted
 * as "basis" for existing configs:
 *   none  -> axis_map ["x","y","z"]
 *   arm   -> axis_map ["z","x","y"]
 *   spine -> mode "twist", twist_axis [0,1,0], output_axis [1,0,0]
 * and "mirror_z180": true composes a 180-degree Z rotation into
 * basis_rotation, which is exactly what it always did. */
struct BoneRetarget
{
    /* Kalidokit bone name, e.g. "Spine", "Head", "LeftUpperArm". The shell
     * sends every bone Kalidokit solves; entries missing here are dropped. */
    std::string source_name;
    /* mod2 bone name on the spawned biped, e.g. "bip01 spine1". */
    std::string blam_bone_name;

    /* q_target_local = restDelta^-1 * q_source_local * restDelta */
    Quatf rest_delta{1.f, 0.f, 0.f, 0.f};

    /* Stage 1. Each output component names the input component it takes,
     * with a sign. Identity is x<-x, y<-y, z<-z. */
    AxisPick axis_map[3]{{0, 1.f}, {1, 1.f}, {2, 1.f}};

    /* Stage 2. Identity leaves the frame alone. */
    Quatf basis_rotation{1.f, 0.f, 0.f, 0.f};

    /* Stage 3. */
    enum class mode_t
    {
        direct,
        twist,
    } mode{mode_t::direct};

    /* twist only: axis the angle is read off, and the axis it is re-applied
     * about. Normalized on use. */
    Vecf3 twist_axis{0.f, 1.f, 0.f};
    Vecf3 output_axis{1.f, 0.f, 0.f};
    /* twist only: extracted angle is clamped to +/- this, then scaled. */
    f32 clamp_degrees{60.f};
    f32 gain{2.f};
};

struct AttachmentConfig
{
    /* Object tag to spawn, e.g. "weapons\\pistol\\pistol". */
    std::string tag;
    /* Marker on the biped to parent it to, e.g. "left hand". */
    std::string marker;
};

/* One button in the shell's animation strip. */
struct AnimationButton
{
    /* Button face — an emoji or short label. */
    std::string label;
    /* Blam animation name, looked up in the biped's antr by
     * find_animation_by_name(). */
    std::string name;
    /* Tooltip; falls back to name when empty. */
    std::string title;
    /* loop_animation (replaces the base loop) vs play_animation (plays once,
     * then falls back to the loop). */
    bool loop{false};
};

/* Animation names are per-biped — cyborg_mp's "stand pistol idle" does not
 * exist on the monitor — so sets are selected by biped tag. First set whose
 * biped matches the resolved biped_tag wins; "*" matches anything, so put it
 * last. */
struct AnimationSet
{
    std::string                  biped{"*"};
    f32                          fps{30.f};
    std::vector<AnimationButton> buttons;
};

/* One bone driven by the microphone level, applied on top of whatever the
 * pose and animation stages already produced. */
struct MicBoneMapping
{
    /* mod2 bone name, e.g. "bip01 spine1". */
    std::string blam_bone_name;
    /* Axis the level rotates about, in the bone's local space. Normalized on
     * use. */
    Vecf3 axis{0.f, 0.f, 1.f};
    /* Radians at a level of 1.0. Negative leans the other way — the original
     * hardcoded head bob was -2.0 about Z. */
    f32 gain{-2.f};
    /* Ceiling on the resulting angle, so a shout cannot fold the biped. */
    f32 clamp_degrees{90.f};
};

/* Moves the whole biped instead of bending a joint. The vector carries both
 * direction and amplitude: offset = vector * level, in world space, with its
 * magnitude capped at clamp. Attachments derive from the biped's transform, so
 * they ride along. */
struct MicTranslation
{
    bool  enabled{false};
    Vecf3 vector{0.f, 0.f, 0.f};
    f32   clamp{1.f};
};

/* The shell measures RMS over the time-domain waveform and conditions it
 * (noise_floor, gain, smoothing) before sending mic_volume; the per-bone gain
 * below is the second, per-joint stage. Splitting it this way means one
 * sensitivity knob for the room and one response knob per bone. */
struct MicrophoneConfig
{
    bool enabled{true};

    /* Shell-side capture. */
    f32                 interval_ms{33.f};
    libc_types::u32     fft_size{2048};
    /* level = clamp((rms - noise_floor) * gain, 0, 1), then an optional EMA:
     * level = prev + smoothing * (level - prev). smoothing 0 disables it. */
    f32                 noise_floor{0.f};
    f32                 gain{1.f};
    f32                 smoothing{0.f};

    /* C++ side. Defaults to the bone the hardcoded bob used. */
    std::vector<MicBoneMapping> bones;

    /* Independent of bones — both can run at once. Off by default. */
    MicTranslation model_translation;
};

struct CameraConfig
{
    Vecf3           position{0.4f, -0.03f, 0.6f};
    f32 field_of_view{70.f};
    f32 aspect{1.6f};
    f32 z_near{0.001f};
    f32 z_far{100.f};
    bool            keyboard{true};
};

struct PoseConfig
{
    /* Map to load. bitmaps defaults to "bitmaps.map" beside it. */
    std::string map_file{"bloodgulch.map"};
    std::string bitmaps_file{};

    std::string biped_tag{"characters\\cyborg_mp\\cyborg_mp"};
    std::string start_animation{"stand pistol idle"};

    std::vector<AttachmentConfig> attachments{
        {"weapons\\pistol\\pistol", "left hand"},
    };

    CameraConfig     camera;
    MicrophoneConfig microphone;
    RootMotionConfig root_motion;

    /* Defaults to the single spine entry that used to live in
     * pose_retarget.h's kRetargetTable. */
    std::vector<BoneRetarget> retarget;

    /* Exponential moving average factor for the slerp in apply_pose. */
    f32 smoothing_alpha{0.1f};

    /* Every set, as written. The shell reads the same array to build its
     * button strip; C++ only needs the frame rate off the matching one. */
    std::vector<AnimationSet> animation_sets;

    /* fps of the set matching biped_tag, resolved once at load. Drives
     * apply_pose's frame indexing and one-shot duration, which were a
     * hardcoded 30. */
    f32 animation_fps{30.f};

    static PoseConfig defaults();
    static PoseConfig from_json(nlohmann::json const& doc);
};

/* Parsed once during app load. Reads /pose_config.json under emscripten (staged by --pre-js), or
 * config_path otherwise; returns defaults() when unreadable. */
PoseConfig load_pose_config(std::string_view config_path);

extern PoseConfig g_pose_config;

/* Replaces pose_retarget.h's compile-time table lookup. Returns nullptr for
 * bones with no mapping, which apply_pose skips. */
BoneRetarget const* find_retarget_entry(std::string_view source_name);
