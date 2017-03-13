#pragma once

#include <coffee/core/coffee_mem_macros.h>

#ifdef COFFEE_GLEAM_DESKTOP

#include <glad/glad.h>
#include <glad/KHR/khrplatform.h>

#else

#if defined(COFFEE_MAEMO)
#include <GLES2/gl2.h>
#else
#include <glad_es/glad.h>
#include <glad_es/KHR/khrplatform.h>
#endif

#endif
