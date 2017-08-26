#pragma once

#define SWIG

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/tdef/stltypes.h>

#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/interfaces/cgraphics_api.h>
#include <coffee/windowing/renderer/renderer.h>

#include <coffee/graphics/common/gltypes.h>

#include <coffee/core/CObject>

using SDL2Renderer = Coffee::Display::CSDL2Renderer;
using GLContext = Coffee::CGL::CGL_Context;
using Props = Coffee::Display::CDProperties;

#include "coffeeswig.inl"
