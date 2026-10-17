/* Bits of the render_flags uniform, see RenderingParameters */
const int RENDER_FLAG_FOG                = 0x1;
const int RENDER_FLAG_LIGHTMAP           = 0x2;
const int RENDER_FLAG_REFLECTION         = 0x4;
// 0x8 taken by a vertex shader flag
const int RENDER_FLAG_ONLY_NORMALS       = 0x10;
const int RENDER_FLAG_ONLY_NORMALMAP     = 0x20;
const int RENDER_FLAG_ONLY_LIGHTMAP      = 0x40;
const int RENDER_FLAG_ONLY_REFLECTIONS   = 0x80;
const int RENDER_FLAG_ONLY_MULTIPURPOSE  = 0x100;
const int RENDER_FLAG_ONLY_MULTIPURPOSE2 = 0x200;
const int RENDER_FLAG_ONLY_DIFFUSE       = 0x400;
const int RENDER_FLAG_CUBE_YAW           = 0x800;
const int RENDER_FLAG_ONLY_DETAIL        = 0x1000;
const int RENDER_FLAG_ONLY_MICRO         = 0x2000;
const int RENDER_FLAG_ONLY_AUX           = 0x4000;
const int RENDER_FLAG_INTERIOR           = 0x8000;
const int RENDER_FLAG_WATER_MODULATE     = 0x10000;
