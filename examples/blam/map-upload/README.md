# Blam Map Upload

A browser page (`BlamMapUpload.html`, web builds only) that stores Halo CE
maps in the browser so the web builds of BlamGraphics can load them without
downloading anything. Drop `.map` files or a whole `maps` folder onto the page.

## Where files go

Files are written to the IndexedDB store that Emscripten's Fetch API uses as
its cache: database `emscripten_filesystem`, object store `FILES`, value an
`ArrayBuffer`, key the URL BlamGraphics requests:

| Detected            | Stored as                                     |
|---------------------|-----------------------------------------------|
| Halo PC map         | `/pc/<name>.map`                              |
| Custom Edition map  | `/custom/<name>.map`                          |
| Xbox map            | `/xbox/<name>.map` (decompressed when needed) |
| MCC map             | `/mcc/<name>.map`                             |
| Resource maps       | `/<prefix>/bitmaps.map`, `sounds.map`, `loc.map` |

File names are lowercased. Resource maps get their canonical name from their
contents, whatever the file was called. Halo PC Trial maps and anything else
are skipped.

Load one with `BlamGraphics.html?map=/pc/bloodgulch.map`. The page links each
stored map to the BlamGraphics build for its version when it is served from
the GitHub Pages layout (`bin/`, `custom/bin/`, `xbox/bin/`, `mcc/bin/`).

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

## BlamGraphics side

`examples/blam/cblam-testing/offline_maps.*` marks `/pc/`, `/custom/`,
`/xbox/` and `/mcc/` as offline-only, so `emscripten_fetch()` is called with
`EMSCRIPTEN_FETCH_NO_DOWNLOAD` for them: a map missing from IndexedDB fails
instead of being requested from the server. `offline_maps_preload.js` reads
the stored keys before `main()`, which lets the map list show uploaded maps,
and BlamGraphics boots `/<its version>/ui.map` when there is one and no
`?map=` was given. The list is a snapshot taken at page load, so reload
BlamGraphics after uploading.
