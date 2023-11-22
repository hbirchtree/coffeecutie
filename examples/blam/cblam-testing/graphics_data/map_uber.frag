#version 460 core

#extension GL_GOOGLE_include_directive : enable

#define USE_MODEL_SHADERS 0
#define USE_LIGHTMAPS 1
#define USE_SELF_ILLUMINATION 1
#define USE_NORMALMAP 1
#define USE_CHICAGO 1

#define USE_REFLECTIONS 1
#define USE_ARRAY_CUBEMAP 1

#include "fragments/shaders_common_uber.glsl"
