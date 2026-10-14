# Halo 2 renderer

- Fix fog planes: the planar fog (sbsp fog planes + `fog ` tag) is a
  per-pixel ray integral under the plane, but with the tag's constants it
  saturates into a flat slab; needs the real density curve.
- Shell: sprite sequences, list skins with player blocks (profile entries),
  dialog screens, shape groups, sounds, blur behind the shell.
- Player starting location `facing_degrees` may be radians like the
  placements.
