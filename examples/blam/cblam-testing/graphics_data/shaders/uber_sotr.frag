#version 460 core

#extension GL_GOOGLE_include_directive : enable

/* shader_transparent only. Shared by the BSP and model pipelines, and by both
 * Halo versions: it reads neither the lightmap sampler (only shader_environment
 * does) nor the multipurpose map (only shader_model does, and that is the sole
 * user of MULTIPURPOSE_XBOX), so none of the variants those defines produce
 * differ here. */
#define USE_MODEL_SHADERS 0
#define USE_LIGHTMAPS 0
#define USE_SELF_ILLUMINATION 1
#define USE_NORMALMAP 1
#define USE_CHICAGO 0

#define USE_REFLECTIONS 1
#define USE_ARRAY_CUBEMAP 1

#define USE_BASE_MATERIALS 0
#define USE_TRANSPARENT 1

#include "fragments/shaders_common_uber.glsl"
