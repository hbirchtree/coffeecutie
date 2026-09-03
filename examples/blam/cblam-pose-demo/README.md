# Test URLs

[https://localhost:8443/BlamPoseDemo.bundle/BlamPoseDemo.html?dummy_plug&map=/pc/bloodgulch.map](Chief)

[https://localhost:8443/BlamPoseDemo.bundle/BlamPoseDemo.html?dummy_plug&map=/pc/c10.map&biped=characters\monitor\monitor&height=0.01](Monitor)

[https://localhost:8443/BlamPoseDemo.bundle/BlamPoseDemo.html?dummy_plug&map=/pc/b30.map&biped=characters\elite\elite&start_anim=stand%20pistol%20idle%0&marker=right%20hand%20elite](Elite)

[https://localhost:8443/BlamPoseDemo.bundle/BlamPoseDemo.html?dummy_plug&map=/pc/b30.map&biped=characters\marine_armored\marine_armored&start_anim=stand%20pistol%20idle&marker=right%20hand&height=0.55](Marine)

# Configuration

Runtime settings live in `pose_config.json`, copied next to the page at build
time. It is fetched at startup and staged into MEMFS by `--pre-js`
(`pose_config_preload.js`) before `main()` runs, because the map name is in it
and the map fetch starts during app load.

Edit the copy in the bundle directory and reload — no rebuild needed. Point
elsewhere with `?config=<url>`.

Everything is optional; a missing file, bad JSON or an absent key falls back to
the value that was hardcoded before the config existed.

```jsonc
{
  "map":     { "file": "bloodgulch.map", "bitmaps": "" },
  "biped":   { "tag": "characters\\cyborg_mp\\cyborg_mp",
               "start_animation": "stand pistol idle" },

  // Each entry spawns an object and parents it to a marker on the biped.
  // A marker that does not exist on the model is warned about and skipped.
  "attachments": [
    { "tag": "weapons\\pistol\\pistol", "marker": "left hand" }
  ],

  "camera": { "position": [0.4, -0.03, 0.6], "field_of_view": 70.0,
              "aspect": 1.6, "z_near": 0.001, "z_far": 100.0,
              "keyboard": true },

  // Kalidokit bone name -> mod2 bone. The shell sends every bone the solvers
  // produce; anything without an entry here is dropped. See "Rotation basis".
  "retarget": [
    { "source": "Spine", "bone": "bip01 spine1",
      "mode": "twist", "twist_axis": [0.0, 1.0, 0.0],
      "output_axis": [1.0, 0.0, 0.0],
      "axis_map": ["x", "y", "z"], "basis_rotation": [0.0, 0.0, 0.0, 1.0],
      "rest_delta": [0.0, 0.0, 0.0, 1.0],
      "clamp_degrees": 60.0, "gain": 2.0 }
  ],

  "smoothing": { "alpha": 0.1 },

  // Mic level -> bone rotation, applied on top of the pose/animation result.
  // See "Microphone".
  "microphone": {
    "enabled": true, "interval_ms": 33, "fft_size": 2048,
    "noise_floor": 0.0, "gain": 1.0, "smoothing": 0.0,
    "bones": [
      { "bone": "bip01 spine1", "axis": [0.0, 0.0, 1.0],
        "gain": -2.0, "clamp_degrees": 90.0 }
    ],
    // Moves the whole biped instead of bending a joint. Off by default.
    "model_translation": { "enabled": false,
                           "vector": [0.0, 0.0, 0.2], "clamp": 0.5 }
  },

  // Read by the shell, not by C++. Set "face_model": null to skip the face
  // solver entirely.
  "pose": {
    "pose_model": "vendor/tasks-vision/pose_landmarker_full.task",
    "face_model": "vendor/tasks-vision/face_landmarker.task",
    "delegate": "GPU",
    "min_detection_confidence": 0.5,
    "min_tracking_confidence": 0.5,
    "min_presence_confidence": 0.5
  }
}
```

## Rotation basis

Each `retarget` entry carries the source rotation into bone space in four
stages, applied in order:

| field | what it does |
|---|---|
| `source_rest` | The source's orientation when the subject is at rest, divided out as `q * conj(source_rest)`, as `[x, y, z, w]`. Kalidokit does not solve to zero at rest — the upper arms sit at `z = ±1.25` rad — so without this a still subject holds the model 71° off its animation. |
| `axis_map` | Signed permutation of the quaternion's imaginary part, e.g. `["-z","x","y"]`. Each slot names the *source* component it takes. This is the stage that expresses axis swaps and handedness flips — a reflection is not a rotation, so conjugation cannot do it. |
| `basis_rotation` | Change of basis by conjugation, `q' = B * q * conj(B)`, as `[x, y, z, w]`. Any true rotation of the frame, at any angle. |
| `mode` | `direct` (default) passes the result through. `twist` projects it onto `twist_axis`, clamps the angle to `±clamp_degrees`, scales by `gain`, and re-emits it about `output_axis`. `aim` ignores the rotation entirely and turns the bone so its `aim_axis` points along a `direction` supplied with the bone — see [Arms](#arms). |

The result is conjugated into the bone's frame last, in `apply_pose`, as
`q_target = delta^-1 * q_src * delta`. What `delta` is depends on `space`:

| `"space"` | what happens |
|---|---|
| `bone` (default) | `delta` is `rest_delta`, a constant tuned per bone, and the result composes onto whatever the animation left on that bone. |
| `model` | `delta` is the bone's bind world orientation, and the bone is reset to its bind pose first, so it ends up sitting at exactly `q_src` off the bind pose. `rest_delta` goes unused. |

`model` is the one to reach for when the source says where a limb *is* rather
than how far it has moved — Kalidokit's arms and legs do. It deliberately
replaces the animation on that bone: composed instead, the same "arm out to the
side" would land somewhere different in every frame of the idle, because it
would be measured from wherever the animation had just put the arm. Bones with
no entry keep animating as usual, so a config that maps only the arms leaves the
legs and torso alone.

Tuning is usually `axis_map` first — it is discrete, 48 combinations, and one
of them is almost always right — then `basis_rotation` for the leftover angle.

The old presets still parse and expand into the above, so existing configs keep
working:

| `"basis"` | expands to |
|---|---|
| `none` | `axis_map: ["x","y","z"]` |
| `arm` | `axis_map: ["z","x","y"]` |
| `spine` | `mode: "twist"`, `twist_axis: [0,1,0]`, `output_axis: [1,0,0]` |

`"mirror_z180": true` still works too — it composes a 180° Z rotation into
`basis_rotation`, which is exactly what it always did. Explicit fields override
whatever the preset set, so you can start from `"basis": "arm"` and adjust.

### Arms

The arms do not use Kalidokit's rotations at all. They are aimed straight at
MediaPipe's world landmarks:

```jsonc
{ "source": "LeftUpperArm", "bone": "bip01 l upperarm",
  "mode": "aim", "axis_map": ["-z", "-x", "-y"] }
```

**Why not the rotations.** Kalidokit's arm solve is measurably not good enough
here, for two independent reasons. Its flexion scale is 2.14 where the exact
figure is π, so a 100° elbow comes out as 54° — `normalizeRadians` returns
units of π, not radians. And a lowered arm puts its XYZ Euler on gimbal lock
(`y ≈ ±π/2`), where the triple flips between frames. Driven from those, the
upper arms landed 16–21° out and the forearms 106–126° out, pointing at the
floor. Aimed at the landmarks instead, the same clip gives 2–4° on the upper
arms and 2–13° on the forearms.

**How aim works.** The shell attaches a unit `direction` to each arm bone,
taken from the landmark pair spanning it, and `apply_pose` rotates the bone so
its `aim_axis` — local X, which is down the bone on a bip01 skeleton — points
along it. Because it sets a direction rather than composing a rotation, it is
independent of whatever the animation is doing and of the bind pose, which is
what makes it immune to both problems above. `space`, `rest_delta` and
`source_rest` are unused in this mode.

`axis_map` here converts *MediaPipe world space* (X image-right, Y down, Z away
from camera) into blam's model space (X forward, Y left, Z up). Note this is a
different map from the one a `direct` entry needs, because Kalidokit's rotations
are in VRM axes rather than MediaPipe's.

The lateral sign is the subtle one. Kalidokit's `Left` is solved from landmarks
12/14/16 — the subject's anatomical *right*, since it mirrors the camera itself
— and that drives `bip01 l`, which sits at **+Y**. The subject's right arm
points image-left, at **-x**. So blam's +Y comes from MediaPipe's -x, and the
map is `["-z", "-x", "-y"]`. Getting that sign backwards points each arm across
the body: a T-pose comes out crossed, and crossed arms come out T-posed.

That error is invisible in any clip where the arms stay near the body's
midline, which is most seated footage — `arms_down`, `arms_up`, `arms_forward`
and `elbow_bend` all pass with the sign inverted. The `t_pose` and
`arms_crossed` cases in `pose_demo_arms.mjs` exist to catch it, and both fail
if it is flipped back.

**What it does not fix.** Aim matches bone *directions*, not wrist positions.
The cyborg's shoulders are much wider than a person's, so hands clasped
together on the subject still come out apart on the model. Closing that needs
IK onto the wrist landmark, not a direction.

`blam_tag_inspect --dump-bones` prints a model's bind pose, including each
bone's local axes in model space; that is where `aim_axis` comes from. For
`bip01 l upperarm` the bone's local X runs down the arm.

## Microphone

Two stages, deliberately split: the shell conditions the room signal once, and
each bone decides its own response.

**Capture and conditioning (shell).** RMS over the time-domain waveform, then

```
level = clamp((rms - noise_floor) * gain, 0, 1)
level = prev + smoothing * (level - prev)      // skipped when smoothing is 0
```

| field | meaning |
|---|---|
| `enabled` | `false` skips `getUserMedia` entirely — no mic permission prompt |
| `device_id` | pins a specific input; omit for the browser default |
| `interval_ms` | send rate, was a hardcoded 33 |
| `fft_size` | analyser window, was a hardcoded 2048 |
| `noise_floor` | subtracted before scaling; raise it to ignore room tone |
| `gain` | sensitivity, applied after the floor |
| `smoothing` | EMA factor, `0` = raw, higher = laggier but steadier |

**Mapping (C++).** Each `bones` entry rotates one bone by `level * gain`
radians about `axis` in that bone's local space, clamped to `±clamp_degrees`,
composed on top of whatever the pose and animation stages produced. Several
bones can be driven from the same level with different axes and gains.

The default reproduces the bob that used to be hardcoded in `apply_pose`:
`bip01 spine1`, local Z, gain `-2.0`. Bones named here are looked up
independently of the `retarget` table — a bone can appear in both.

**Translation mode (`model_translation`).** Moves the whole biped rather than
bending a joint:

```
offset = vector * level          // world space
if |offset| > clamp: scaled back to clamp
transform = translate(offset) * spawn_transform
```

`vector` carries both direction and amplitude — it *is* the displacement at a
level of `1.0`, so `[0, 0, 0.2]` is a 0.2-unit hop straight up and
`[0.1, 0, 0]` is a sideways nudge. `clamp` caps the magnitude regardless.

Rebuilt from the spawn transform every frame, so it cannot accumulate.
Attachments derive from the biped's transform, so they ride along. Independent
of `bones` — run both at once if you want a bob *and* a lean. Off unless the
block is present; writing the block without `enabled` turns it on.

## Root motion

Kalidokit's `Hips` is the only solved output carrying a position, so it drives
the biped's **root transform** rather than a bone. Off by default.

```jsonc
"root_motion": {
  "enabled": false,
  "source": "Hips",
  "axis_map": ["x", "y", "z"],     // same syntax as the rotation basis
  "scale":    [1.0, 1.0, 1.0],     // per-axis direction and amplitude
  "offset":   [0.0, 0.0, 0.0],     // constant bias
  "deadzone": [0.02, 0.02, 0.02],  // below this reads as zero
  "clamp":    [0.5, 0.5, 0.5],     // per-axis cap
  "smoothing": 0.15                // EMA, 0 = raw
}
```

Conditioning order, applied in `apply_pose`:

```
remap -> scale + offset -> deadzone -> clamp -> EMA
```

`scale` is doing real work: the hips position is a normalized, roughly
hip-width-relative estimate, **not** metres, so the raw numbers are small and
unitless. `axis_map` reconciles Kalidokit's axes with the model's.

`smoothing` is an EMA held across pose events (~30 Hz), not frames, which is
where the jitter actually arrives. Per-axis `deadzone` and `clamp` because the
axes mean different things — a vertical bob wants different limits to a
sideways drift.

Pressing 🎯 (calibrate) captures the current hips position as the new origin,
so root motion is relative to where you were standing. Until the first
calibrate, the raw solved position is used.

Root motion and `microphone.model_translation` **sum** into one offset, both
rebuilt from the spawn transform each frame, so neither accumulates and neither
overwrites the other. Attachments derive from the biped transform and ride
along.

## Query parameters

Query params override the config file, so the URLs above keep working:
`map`, `biped`, `weapon`, `marker`, `start_anim`, `height`, plus `config`.

`weapon` and `marker` address the first attachment only — use the config file
for more than one. `height` sets the camera's Z.

Natively, the map is still positional and `--config <path>` selects the file.

# Pose pipeline

`@mediapipe/tasks-vision` `PoseLandmarker` + `FaceLandmarker` in `VIDEO` mode
feed Kalidokit's `Pose.solve` / `Face.solve` (both with `runtime: 'mediapipe'`,
which is a coordinate convention, not a library binding — Kalidokit has no
MediaPipe dependency). Solved bones go to `coffee_dummy_plug_event` as a
`pose_apply` event, and `apply_pose` filters them against the retarget table.

`vendor/pose/` holds the superseded `@mediapipe/pose` Solutions API, frozen
upstream since 2023-02 and no longer loaded. It is kept only for comparison and
can be deleted.

# Tests

Two harnesses in `.github/tests/web/`, answering different questions.

`pose_demo_kalidokit_smoke.mjs` runs the real pipeline: Chrome's fake webcam
plays a video of a person, and the test passes if MediaPipe and Kalidokit
produce pose payloads at all. It says the chain is live; it says nothing about
whether the retargeting is right, because the input is a video and the output
is a picture.

`run_pose_demo_arms.sh` (driving `pose_demo_arms.mjs`) asks the other question.
It skips the webcam entirely, injects exact quaternions as `pose_apply` events,
and reads the resulting bone positions back out of the engine with the
`dump_pose` event, so each case is a known input checked against a measured
output in metres. A mirrored axis fails as a sign rather than as an impression
of a screenshot. It also checks that moving one arm leaves the other exactly
where the bind pose put it, which is what catches a basis change leaking across
the body.

```
.github/tests/web/run_pose_demo_arms.sh [BUNDLE_DIR] [OUT_DIR]
```

It freezes the idle animation (via a near-zero `fps`) and turns off smoothing,
root motion and the microphone, so a bone only moves when a pose moves it. Both
run under Xvfb against a real GL backend — SwiftShader loses the WebGL context
on this bundle, and everything after that is fallout rather than a result.

`blam_tag_inspect --dump-bones <map>` prints the bind pose a mapping has to be
written against, which is the offline half of the same question:

```
blam_tag_inspect -c mod2 -n cyborg --dump-bones /path/to/bloodgulch.map
```
