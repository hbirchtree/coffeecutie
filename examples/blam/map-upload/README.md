# Blam Map Upload

A browser page (`BlamMapUpload.html`, web builds only) that stores Halo CE
and Halo 2 maps in the browser, so the web builds of BlamGraphics can load
them without downloading anything. Drop `.map` files or a whole `maps` folder
onto the page.

## Where files go

Files are written to the origin's OPFS at their path: `/pc/bloodgulch.map`
is `pc/bloodgulch.map`.

| Detected            | Stored as                                     |
|---------------------|-----------------------------------------------|
| Halo PC map         | `/pc/<name>.map`                              |
| Custom Edition map  | `/custom/<name>.map`                          |
| Xbox map            | `/xbox/<name>.map` (decompressed when needed) |
| MCC map             | `/mcc/<name>.map`                             |
| Resource maps       | `/<prefix>/bitmaps.map`, `sounds.map`, `loc.map` |
| Halo 2 Xbox map     | `/h2xbox/<name>.map`                          |

File names are lowercased. Resource maps get their canonical name from their
contents, whatever the file was called. Halo PC Trial maps and anything else
are skipped.

Load one with `BlamGraphics.html?map=/pc/bloodgulch.map`. The page links each
stored map to the BlamGraphics build for its version when it is served from
the GitHub Pages layout (`bin/`, `custom/bin/`, `xbox/bin/`, `mcc/bin/`).

## Texture transcoding

**Optimize for performance** (on by default) stores Xbox textures
unswizzled. The Xbox keeps its uncompressed textures (lightmaps, many cube
maps) Morton-swizzled, and BlamGraphics otherwise deswizzles every one while
loading. The image's `swizzled` flag is cleared as it is rewritten, so the
runtime never does it twice. Volume textures are left swizzled. Unchecked,
and with no transcode target, maps are stored as they were before.

Halo PC, Custom Edition and Xbox textures are S3TC (DXT). Browsers on GPUs without
`WEBGL_compressed_texture_s3tc`, which is most phones, make BlamGraphics
decode every one of them in software. The page checks WebGL 2 for S3TC and
ETC2 (`WEBGL_compressed_texture_etc`) and, when only ETC2 is there, picks
ETC2 as the transcode target. It can be changed in the drop-down.

Transcoding uses the same kernels as `MapTranscode` (the `BlamTranscode`
library, `src/coffee/blam/transcode`), on a thread per core. Only 2D
textures are encoded; cube maps and volume textures are kept as they are.

Xbox maps carry their own textures and are transcoded one by one, after
they are decompressed. On PC and Custom Edition most textures live in `bitmaps.map`
and are shared between level maps, so a level map is only transcoded when
`bitmaps.map` is dropped with it. The original `bitmaps.map` is held in wasm
while each level map patches its images into a copy, which is stored last.
Drop the whole `maps` folder at once.

Which stored files were transcoded together is kept in `localStorage`. A
level map stored without the `bitmaps.map` it belongs with is marked
"Textures out of sync", and one dropped without `bitmaps.map` while the
stored one is transcoded is skipped.

## How files are identified

Only the first 2 KiB of a file is read for this (`blam_upload_identify()` in
`main.cpp`):

- **Maps** have a 2048-byte header (`blam::file_header_t`) starting with
  `head` and ending with `foot`. The `u32` after `head` is the game version:
  5 Xbox, 7 PC, 609 Custom Edition, 13 MCC. A map is compressed when the
  header's decompressed length differs from the file size. Only then is the
  whole file passed to wasm, where `blam::map_container::from_bytes()`
  inflates it. The output keeps the original header, whose decompressed
  length now matches the file size.
- **Halo 2 maps** have the same `head`/`foot` header with version 8, but
  Xbox and Vista lay the rest of it out differently, and nothing in the
  header itself tells them apart. The page also reads the 32-byte tag index
  at the header's `meta_offset`. As in `blam::dimeter::map_container`, its
  group table pointer is a virtual address on Xbox and a small offset on
  Vista. Only Xbox maps are stored, as they are; Vista maps are skipped, and
  so is a map with no tag index there (a compressed map, say).
- **Halo 2 `mainmenu.map`, `shared.map` and `single_player_shared.map`**
  exist for both Xbox and Vista, and every Halo 2 map reads raw data from
  them by those names in its own directory (see `dimeter-info`). They are
  recognised by the header's `map_type` (`blam::dimeter::cache_type_t` 2, 3
  and 4) and stored under those names, whatever the files were called.
  `shared.map` may carry too little tag data for the Xbox/Vista check. In
  that case `map_type` is read at both layouts' offsets (0x140 Xbox, 0x14C
  Vista) and trusted only when exactly one of them reads as one of these
  files. Either way it is stored under `/h2xbox/`.
- **Trial maps** have the scrambled `Ehed`/`Gfot` header
  (`blam::file_header_trial_t`).
- **`bitmaps.map`, `sounds.map` and `loc.map`** have no map header. They
  are resource maps (`blam::tag_atlas_t`): a `u32` type (1 bitmaps, 2 sounds,
  3 localization), the offset of the path strings, then the offset and count
  of 12-byte locators. A map's first `u32` is `head`, so it never reads as
  1-3, and the offsets have to fall inside the file. These are stored byte
  for byte without being passed to wasm.

Resource maps are the same format on PC, Custom Edition and MCC, so nothing
in them says which game they came from. They go under the prefix of the
regular maps dropped with them. If there are none, or several versions were
dropped together, choose a target in the drop-down and drop them again.

## Styling

The page follows the blue scheme of the forerunner theme used by the speeny
dev blog (`hbirchtree/kafei-py`, `blog/themes/forerunner`). `assets/` holds
the Fira Code font (SIL Open Font License, see `assets/fonts/OFL.txt`) and
the theme's `noise.png`, which drives the animated "plasma" hover effect.
CMake copies them into the bundle.

## BlamGraphics side

`examples/blam/cblam-testing/offline_maps.*` turns `?map=/pc/...` into
`/maps/pc/...` and marks `/maps/` as offline-only. `comp_app::FileMapper`
mounts the OPFS root as `/maps` (its store) and reads those URLs from there
without asking the server, so a map missing there fails instead of being
downloaded. `offline_maps_preload.js` lists the OPFS directories before
`main()`, which lets the map list show uploaded maps,
and BlamGraphics boots `/<its version>/ui.map` when there is one and no
`?map=` was given. The list is a snapshot taken at page load, so reload
BlamGraphics after uploading.
