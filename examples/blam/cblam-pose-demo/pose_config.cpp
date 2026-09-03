#include "pose_config.h"

#include <fstream>
#include <sstream>

#include <coffee/core/CDebug>
#include <peripherals/stl/magic_enum.hpp>
#include <platforms/sysinfo.h>

using namespace Coffee;

PoseConfig g_pose_config = PoseConfig::defaults();

namespace {

/* Reads [x, y, z]; leaves fallback untouched if absent or the wrong shape. */
Vecf3 read_vec3(nlohmann::json const& parent, char const* key, Vecf3 fallback)
{
    if(!parent.contains(key))
        return fallback;
    auto const& node = parent[key];
    if(!node.is_array() || node.size() != 3)
    {
        cWarning("pose_config: '{}' must be [x, y, z], ignoring", key);
        return fallback;
    }
    return Vecf3(
        node[0].get<f32>(), node[1].get<f32>(), node[2].get<f32>());
}

/* Reads [x, y, z, w] — same order as the bone rotations on the wire — and
 * builds a glm quat, whose 4-arg ctor is (w, x, y, z). */
Quatf read_quat(nlohmann::json const& parent, char const* key, Quatf fallback)
{
    if(!parent.contains(key))
        return fallback;
    auto const& node = parent[key];
    if(!node.is_array() || node.size() != 4)
    {
        cWarning("pose_config: '{}' must be [x, y, z, w], ignoring", key);
        return fallback;
    }
    return Quatf(
        node[3].get<f32>(),
        node[0].get<f32>(),
        node[1].get<f32>(),
        node[2].get<f32>());
}

/* "x" / "-x" / "+y" / "z" ... -> which source component, and its sign. */
bool parse_axis_pick(std::string_view text, AxisPick& out)
{
    if(text.empty())
        return false;
    f32 sign = 1.f;
    if(text.front() == '-')
    {
        sign = -1.f;
        text.remove_prefix(1);
    } else if(text.front() == '+')
        text.remove_prefix(1);

    if(text.size() != 1)
        return false;
    switch(text.front())
    {
    case 'x':
    case 'X':
        out = {0, sign};
        return true;
    case 'y':
    case 'Y':
        out = {1, sign};
        return true;
    case 'z':
    case 'Z':
        out = {2, sign};
        return true;
    default:
        return false;
    }
}

/* Reads an "axis_map": ["z","-x","y"] into out, leaving it alone on any
 * problem. Shared by the per-bone basis and root motion. */
void read_axis_map(nlohmann::json const& parent, AxisPick (&out)[3])
{
    auto it = parent.find("axis_map");
    if(it == parent.end())
        return;
    if(!it->is_array() || it->size() != 3)
    {
        cWarning(
            "pose_config: axis_map must be 3 entries like [\"z\",\"x\",\"y\"], "
            "ignoring");
        return;
    }
    AxisPick picks[3];
    for(int i = 0; i < 3; ++i)
        if(!(*it)[i].is_string() ||
           !parse_axis_pick((*it)[i].get<std::string>(), picks[i]))
        {
            cWarning(
                "pose_config: axis_map entries must be x/y/z with an optional "
                "sign, ignoring");
            return;
        }
    for(int i = 0; i < 3; ++i)
        out[i] = picks[i];
}

/* Expands the legacy none/arm/spine presets, then lets the explicit fields
 * override whatever they set. */
void read_basis(nlohmann::json const& entry, BoneRetarget& out)
{
    auto preset = entry.value("basis", std::string("none"));
    if(preset == "arm")
        out.axis_map[0] = {2, 1.f}, out.axis_map[1] = {0, 1.f},
        out.axis_map[2] = {1, 1.f};
    else if(preset == "spine")
    {
        out.mode        = BoneRetarget::mode_t::twist;
        out.twist_axis  = Vecf3(0.f, 1.f, 0.f);
        out.output_axis = Vecf3(1.f, 0.f, 0.f);
    } else if(preset != "none")
        cWarning(
            "pose_config: unknown basis preset '{}' (expected none|arm|spine), "
            "ignoring",
            preset);

    read_axis_map(entry, out.axis_map);

    out.basis_rotation =
        read_quat(entry, "basis_rotation", out.basis_rotation);

    /* Kept as shorthand: bip01 mirrors left/right by a 180-degree Z turn,
     * which is a change of basis like any other. */
    if(entry.value("mirror_z180", false))
        out.basis_rotation =
            Quatf(0.f, 0.f, 0.f, 1.f) * out.basis_rotation;

    if(auto it = entry.find("mode"); it != entry.end() && it->is_string())
    {
        auto mode = it->get<std::string>();
        if(auto parsed = magic_enum::enum_cast<BoneRetarget::mode_t>(mode);
           parsed.has_value())
            out.mode = parsed.value();
        else
            cWarning(
                "pose_config: unknown mode '{}' (expected direct|twist|aim), "
                "keeping {}",
                mode,
                magic_enum::enum_name(out.mode));
    }

    if(auto it = entry.find("space"); it != entry.end() && it->is_string())
    {
        auto space = it->get<std::string>();
        if(auto parsed = magic_enum::enum_cast<BoneRetarget::space_t>(space);
           parsed.has_value())
            out.space = parsed.value();
        else
            cWarning(
                "pose_config: unknown space '{}' (expected bone|model), "
                "keeping {}",
                space,
                magic_enum::enum_name(out.space));
    }

    out.source_rest = read_quat(entry, "source_rest", out.source_rest);

    out.twist_axis  = read_vec3(entry, "twist_axis", out.twist_axis);
    out.output_axis = read_vec3(entry, "output_axis", out.output_axis);
    out.aim_axis    = read_vec3(entry, "aim_axis", out.aim_axis);
}

/* URL query params win over the config file, so the README's test URLs keep
 * working and a single config can be re-pointed without editing it. The names
 * are the ones that existed before the config file: map, biped, weapon,
 * marker, start_anim, height. */
void apply_query_overrides([[maybe_unused]] PoseConfig& config)
{
#if defined(COFFEE_EMSCRIPTEN)
    auto params = emscripten::args::query_params();

    auto param = [&params](char const* key) -> std::optional<std::string> {
        auto it = params.find(key);
        if(it == params.end() || it->second.empty())
            return std::nullopt;
        return it->second;
    };

    if(auto v = param("map"))
        config.map_file = *v;
    if(auto v = param("biped"))
        config.biped_tag = *v;
    if(auto v = param("start_anim"))
        config.start_animation = *v;

    /* weapon/marker address the first attachment only — the multi-attachment
     * case is what the config file is for. */
    auto weapon = param("weapon");
    auto marker = param("marker");
    if(weapon || marker)
    {
        if(config.attachments.empty())
            config.attachments.push_back(AttachmentConfig{});
        if(weapon)
            config.attachments.front().tag = *weapon;
        if(marker)
            config.attachments.front().marker = *marker;
    }

    if(auto v = param("height"))
    {
        try
        {
            config.camera.position.z = std::stof(*v);
        } catch(std::exception const&)
        {
            cWarning("pose_config: height='{}' is not a number, ignoring", *v);
        }
    }
#endif
}

} // namespace

PoseConfig PoseConfig::defaults()
{
    PoseConfig out;
    /* Matches the old kRetargetTable: one aggregate torso rotation onto the
     * upper spine segment, midline, no mirroring. */
    BoneRetarget spine{
        .source_name    = "Spine",
        .blam_bone_name = "bip01 spine1",
        .rest_delta     = Quatf(1.f, 0.f, 0.f, 0.f),
    };
    spine.mode          = BoneRetarget::mode_t::twist;
    spine.twist_axis    = Vecf3(0.f, 1.f, 0.f);
    spine.output_axis   = Vecf3(1.f, 0.f, 0.f);
    spine.clamp_degrees = 60.f;
    spine.gain          = 2.f;
    out.retarget        = {std::move(spine)};

    out.microphone.bones = {};
    return out;
}

PoseConfig PoseConfig::from_json(nlohmann::json const& doc)
{
    PoseConfig out = defaults();

    if(auto it = doc.find("map"); it != doc.end() && it->is_object())
    {
        out.map_file     = it->value("file", out.map_file);
        out.bitmaps_file = it->value("bitmaps", out.bitmaps_file);
    }

    if(auto it = doc.find("biped"); it != doc.end() && it->is_object())
    {
        out.biped_tag       = it->value("tag", out.biped_tag);
        out.start_animation = it->value("start_animation", out.start_animation);
    }

    if(auto it = doc.find("attachments"); it != doc.end() && it->is_array())
    {
        out.attachments.clear();
        for(auto const& entry : *it)
        {
            if(!entry.is_object() || !entry.contains("tag"))
            {
                cWarning("pose_config: attachment without 'tag', skipping");
                continue;
            }
            out.attachments.push_back(AttachmentConfig{
                .tag    = entry.value("tag", std::string{}),
                .marker = entry.value("marker", std::string{}),
            });
        }
    }

    if(auto it = doc.find("camera"); it != doc.end() && it->is_object())
    {
        auto& cam        = out.camera;
        cam.position     = read_vec3(*it, "position", cam.position);
        cam.field_of_view = it->value("field_of_view", cam.field_of_view);
        cam.aspect       = it->value("aspect", cam.aspect);
        cam.z_near       = it->value("z_near", cam.z_near);
        cam.z_far        = it->value("z_far", cam.z_far);
        cam.keyboard     = it->value("keyboard", cam.keyboard);
    }

    if(auto it = doc.find("retarget"); it != doc.end() && it->is_array())
    {
        out.retarget.clear();
        for(auto const& entry : *it)
        {
            if(!entry.is_object() || !entry.contains("source") ||
               !entry.contains("bone"))
            {
                cWarning(
                    "pose_config: retarget entry needs 'source' and 'bone', "
                    "skipping");
                continue;
            }
            BoneRetarget bone{
                .source_name    = entry.value("source", std::string{}),
                .blam_bone_name = entry.value("bone", std::string{}),
                .rest_delta =
                    read_quat(entry, "rest_delta", Quatf(1.f, 0.f, 0.f, 0.f)),
            };
            read_basis(entry, bone);
            bone.clamp_degrees = entry.value("clamp_degrees", 60.f);
            bone.gain          = entry.value("gain", 2.f);
            out.retarget.push_back(std::move(bone));
        }
    }

    if(auto it = doc.find("smoothing"); it != doc.end() && it->is_object())
        out.smoothing_alpha = it->value("alpha", out.smoothing_alpha);

    if(auto it = doc.find("microphone"); it != doc.end() && it->is_object())
    {
        auto& mic       = out.microphone;
        mic.enabled     = it->value("enabled", mic.enabled);
        mic.interval_ms = it->value("interval_ms", mic.interval_ms);
        mic.fft_size    = it->value("fft_size", mic.fft_size);
        mic.noise_floor = it->value("noise_floor", mic.noise_floor);
        mic.gain        = it->value("gain", mic.gain);
        mic.smoothing   = it->value("smoothing", mic.smoothing);

        if(auto translation = it->find("model_translation");
           translation != it->end() && translation->is_object())
        {
            auto& out_translation = mic.model_translation;
            out_translation.vector =
                read_vec3(*translation, "vector", out_translation.vector);
            out_translation.clamp =
                translation->value("clamp", out_translation.clamp);
            /* Defaults on when the block is present, since writing it at all
             * is the intent. */
            out_translation.enabled = translation->value("enabled", true);
        }

        if(auto bones = it->find("bones");
           bones != it->end() && bones->is_array())
        {
            mic.bones.clear();
            for(auto const& bone : *bones)
            {
                if(!bone.is_object() || !bone.contains("bone"))
                {
                    cWarning(
                        "pose_config: microphone bone without 'bone', "
                        "skipping");
                    continue;
                }
                mic.bones.push_back(MicBoneMapping{
                    .blam_bone_name = bone.value("bone", std::string{}),
                    .axis = read_vec3(bone, "axis", Vecf3(0.f, 0.f, 1.f)),
                    .gain = bone.value("gain", -2.f),
                    .clamp_degrees = bone.value("clamp_degrees", 90.f),
                });
            }
        }
    }

    if(auto it = doc.find("root_motion"); it != doc.end() && it->is_object())
    {
        auto& root    = out.root_motion;
        root.enabled  = it->value("enabled", true);
        root.source   = it->value("source", root.source);
        read_axis_map(*it, root.axis_map);
        root.scale     = read_vec3(*it, "scale", root.scale);
        root.offset    = read_vec3(*it, "offset", root.offset);
        root.deadzone  = read_vec3(*it, "deadzone", root.deadzone);
        root.clamp     = read_vec3(*it, "clamp", root.clamp);
        root.smoothing = it->value("smoothing", root.smoothing);
    }

    if(auto it = doc.find("animations"); it != doc.end() && it->is_array())
    {
        out.animation_sets.clear();
        for(auto const& set_node : *it)
        {
            if(!set_node.is_object())
                continue;
            AnimationSet set;
            set.biped = set_node.value("biped", std::string("*"));
            set.fps   = set_node.value("fps", 30.f);
            if(set.fps <= 0.f)
            {
                cWarning(
                    "pose_config: animation set '{}' has fps={}, using 30",
                    set.biped,
                    set.fps);
                set.fps = 30.f;
            }
            if(auto buttons = set_node.find("buttons");
               buttons != set_node.end() && buttons->is_array())
            {
                for(auto const& button : *buttons)
                {
                    if(!button.is_object() || !button.contains("name"))
                    {
                        cWarning(
                            "pose_config: animation button without 'name', "
                            "skipping");
                        continue;
                    }
                    set.buttons.push_back(AnimationButton{
                        .label = button.value("label", std::string{}),
                        .name  = button.value("name", std::string{}),
                        .title = button.value("title", std::string{}),
                        .loop  = button.value("loop", false),
                    });
                }
            }
            out.animation_sets.push_back(std::move(set));
        }
    }

    return out;
}

PoseConfig load_pose_config(std::string_view config_path)
{
    std::string path(config_path);
    PoseConfig  out = PoseConfig::defaults();

    /* Every failure below is non-fatal: fall through to defaults, then let the
     * query params override, so the demo still runs with no config present. */
    if(std::ifstream in(path); !in.is_open())
        cWarning(
            "pose_config: '{}' not readable, using built-in defaults", path);
    else
    {
        std::stringstream buffer;
        buffer << in.rdbuf();
        try
        {
            auto doc = nlohmann::json::parse(buffer.str());
            if(doc.is_object())
                out = PoseConfig::from_json(doc);
            else
                cWarning(
                    "pose_config: '{}' must contain a JSON object, using "
                    "built-in defaults",
                    path);
        } catch(std::exception const& e)
        {
            cWarning(
                "pose_config: '{}' is not valid JSON ({}), using built-in "
                "defaults",
                path,
                e.what());
        }
    }

    apply_query_overrides(out);

    /* Resolved after the overrides, since ?biped= can change which set wins. */
    for(auto const& set : out.animation_sets)
        if(set.biped == "*" || set.biped == out.biped_tag)
        {
            out.animation_fps = set.fps;
            break;
        }

    cDebug(
        "pose_config: map='{}' biped='{}' {} attachment(s), {} retarget "
        "entr(ies)",
        out.map_file,
        out.biped_tag,
        out.attachments.size(),
        out.retarget.size());
    return out;
}

BoneRetarget const* find_retarget_entry(std::string_view source_name)
{
    for(auto const& entry : g_pose_config.retarget)
        if(entry.source_name == source_name)
            return &entry;
    return nullptr;
}
