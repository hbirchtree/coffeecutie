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

Each `retarget` entry carries the source rotation into bone space in three
stages, applied in order:

| field | what it does |
|---|---|
| `axis_map` | Signed permutation of the quaternion's imaginary part, e.g. `["-z","x","y"]`. Each slot names the *source* component it takes. This is the stage that expresses axis swaps and handedness flips — a reflection is not a rotation, so conjugation cannot do it. |
| `basis_rotation` | Change of basis by conjugation, `q' = B * q * conj(B)`, as `[x, y, z, w]`. Any true rotation of the frame, at any angle. |
| `mode` | `direct` (default) passes the result through. `twist` projects it onto `twist_axis`, clamps the angle to `±clamp_degrees`, scales by `gain`, and re-emits it about `output_axis`. |

`rest_delta` is applied last, in `apply_pose`, as
`q_target = restDelta^-1 * q_src * restDelta`.

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

The arms have no entry by default; the axis mapping was never worked out. This
is the knob for that — `LeftUpperArm`, `RightUpperArm`, `LeftLowerArm`,
`RightLowerArm` are all being sent already.

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
