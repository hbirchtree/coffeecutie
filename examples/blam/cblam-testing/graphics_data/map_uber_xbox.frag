#version 460 core

#extension GL_GOOGLE_include_directive : enable

#define USE_BSP_SHADERS 1
#define USE_MODEL_SHADERS 0
#define USE_LIGHTMAPS 0
#define USE_REFLECTIONS 1
#define USE_ARRAY_CUBEMAP 1
#define USE_NORMALMAP 0 // Normals are broken with Xbox :(

#include "fragments/shaders_common_uber.glsl"
