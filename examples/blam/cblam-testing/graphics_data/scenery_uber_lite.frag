#version 460 core

#extension GL_GOOGLE_include_directive : enable

#define USE_MODEL_SHADERS 1
#define USE_LIGHTMAPS 0
#define USE_REFLECTIONS 0
#define USE_SELF_ILLUMINATION 0
#define USE_ARRAY_CUBEMAP 0
#define USE_NORMALMAP 1
#define USE_CHICAGO 1

#include "fragments/shaders_common_uber.glsl"
