# Halo 3 (elegy) notes

Findings from bringing up `blam/elegy` and ElegyInfo against the retail
Xbox 360 maps in `/mnt/blam/halo3xbox` (build `11855.07.08.20.2317.halo3_ship`,
version 11). Assembly's `Formats/Halo3` layouts and `Plugins/Halo3` were the
starting point; anything about model, BSP or texture data was derived from
the map bytes, since Assembly has no layouts for it.

`/mnt/blam/halo3beta` is version 9 with a different header layout and is not
supported.

## Cache file

- Everything is big-endian. `elegy::be<T>` swaps on read, per component for
  vectors, and is free on big-endian hosts. It converts implicitly to `T`;
  use `.value()` where a template or `auto` would see the wrapper.
- Header is 0x3000 bytes. Pointers belong to one of four sections
  (debug, resource, tag, locale): `sections[i]` gives a virtual base and
  `offset_masks[i]` the value adding which yields a file offset.
  - tag pointer -> `sections[tag].va + masks[tag] + (ptr - virtual_base)`
  - debug pointer (tag names, string_ids) -> `ptr + masks[debug]`
  - raw pages are relative to `sections[resource].va + masks[resource]`
- `shared.map`, `campaign.map` have no tag index (index address 0); check
  before parsing, a count read from them is garbage.
- string_id is 16:8:8 (index, namespace, length); namespace start offsets
  are in Assembly's `H3_StringIDs.xml`.

## Resources

- `zone` holds the resource table (0x40 entries) and the info buffer
  resource definitions are sliced from; `play` holds external cache files,
  pages, sizes and segments.
- Page codec 0 is raw deflate. Pages may live in `mainmenu.map`,
  `shared.map` or `campaign.map`, matched by file name.
- A resource's fixups write addresses into its definition; the pointer
  fields themselves are 0 in the file. High nibble of the address:
  2 = definition offset, 4 = primary page, 6 = secondary page.
- Some resource slots are empty in shipped maps (stripped): every sbsp's own
  geometry resource, and a handful of bitmaps per map.

## Geometry

- render_geometry is embedded in `mode`, `sbsp` and `sLdT`. Its resource
  root has four blocks; [2] vertex buffers (0x1C defs), [3] index buffers
  (0x18 defs).
- Vertex formats: 1 world (32 B, float position/uv), 2 rigid (24 B),
  3 skinned (rigid + u8x4 nodes + u8x4 weights), 24 decorator (not done).
- Rigid/skinned positions and texcoords are **unsigned** 16-bit normalized
  (0..65535 -> compression bounds). Reading them as signed swaps the halves
  of every model.
- Normals/tangents/binormals are DHEN3N: 10:11:11 signed, x in the low bits.
- Index buffers: type 5 strips (models), type 3 lists (BSP).
- BSP render geometry is not in the sbsp: the lightmapper moves it to
  `sLdT` (`lightmap_bsp[i]` + 0x130). Clusters name a mesh at 0x9C;
  instances name a definition in the sbsp's tag resources (0xB8 each,
  mesh at 0xB0, compression at 0xB2).
- Sanity check used throughout: signed cosine between stored normals and
  face normals is about 0.92-0.99 when decoding is right.

## Textures

- Bitmap images (0x30) carry format, type and Xbox flags; pixels are in the
  matching `bitmap_texture_interop_resource`: a primary data_ref, a
  secondary data_ref, then the GPU texture header
  (width, height, depth, mips, type, high-res, D3D format word).
- Xbox flags: 0x04 byte order, 0x08 tiled, 0x20 top mip in the secondary
  page, 0x40 interleaved.
- Tiled surfaces use `XGAddress2DTiledOffset` with 32x32-block macro tiles;
  stored data stops at the last block in use. Untiled rows are 32 blocks
  apart.
- Byte order swaps per element: 8-in-16 for DXT/DXN/CTX1 and 16-bit
  formats, 8-in-32 for 32-bit formats, none for 8-bit.
- DXN is BC5 (x block, then y block). CTX1 is 360-only: 8 bytes per block,
  two (x, y) u8 endpoints and shared 2-bit indices; z is reconstructed.
  Normal maps are stored biased (unsigned).
- The D3D format word mostly repeats the image entry; its extra content is
  channel swizzle and sign, which the format names already imply.
- Not done: lower mips and the packed mip tail, cube/array faces, PNG
  export (other inspectors dump PNG; `-e` currently writes DDS).

## Shaders

All shader groups derive from `rm  ` and share `render_method_t`: the
option picked per category of the `rmdf` definition, authored parameters,
and the compiled postprocess block (template, textures, constants).

- `rmdf` options name the HLSL functions of Bungie's shader source, e.g.
  albedo/default -> `calc_albedo_default_ps`,
  bump_mapping/standard -> `calc_bumpmap_default_ps`.
- `rmt2` lists name the postprocess textures and constants by index
  (`base_map`, `detail_map`, `albedo_color`, ...). A texture's `transform`
  indexes the float constant holding its UV scale (xy) and offset (zw).
- The compiled `vtsh`/`pixl` are Xenos microcode; translating those is out
  of scope.

### Survey over all retail maps

Tag counts summed per map (shared tags count once per map):
rmsh 16113, rmhg 821, rmd 591, rmtr 178, rmct 68, rmw 30, rmfl 28, rmcs 9.

`shaders\shader` (704 distinct templates), option usage:

| Category | Options in use |
|---|---|
| albedo | default 12472, constant_color 1302, two_detail 922, two_change_color 909, detail_blend 258; rare: two_detail_overlay, color_mask, four_change_color, three_detail_blend, two_detail_black_point |
| bump_mapping | detail 7079, off 4525, standard 4509 |
| material_model | two_lobe_phong 5330, cook_torrance 4826, diffuse_only 4299; rare: organism, single_lobe_phong, glass, none, foliage |
| environment_mapping | none 7132, dynamic 6730, per_pixel 2198, from_flat_texture 53 |
| self_illumination | off 10395, simple 3765, plasma 555, illum_detail 512, 3_channel 300, meter 221, others under 150 |
| alpha_test | none 14368, simple 1745 |
| blend_mode | opaque 12425, additive 2031, alpha_blend 1584, others under 50 |
| parallax | off 15456, simple 462, interpolated 192 |
| specular_mask | from_diffuse 10403, none 5136, from_texture 574 |

Other families:

- decal: diffuse_only + alpha_blend nearly everywhere; bump leave/standard.
- halogram: additive; 8 self-illumination variants (plasma, illum_detail,
  multilayer_additive, scope_blur, ...); overlay none/additive_detail.
- terrain: morph blend of up to 4 materials, each diffuse or
  diffuse+specular; env map none/per_pixel/dynamic.
- foliage, cortana, water: one or two variants each.

### Plan

Hand-written shaders, not generated ones, so they stay few and optimisable
(as for Halo 1 and 2):

- one `shader` program covering the common options above as uniform or
  `#define` branches; rare options fall back to the nearest path;
- decal, terrain, foliage and water as small dedicated programs;
- halogram and cortana later.

Bindings come from the `rmt2` name lists, so the program needs no knowledge
of per-template register layouts.
