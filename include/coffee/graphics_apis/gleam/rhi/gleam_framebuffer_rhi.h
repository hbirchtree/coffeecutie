#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_RenderDummy : GraphicsAPI::RenderDummy
{
};

struct GLEAM_RenderTarget : GraphicsAPI::RenderTarget
{
    void resize(uint32 i,CRect64 const& view);
    void clear(uint32 i, Vecf4 const& color);
    void clear(bigscalar depth);
    void clear(uint32 i, Vecf4 const& color, bigscalar depth);
    void clear(bigscalar depth, int32 stencil);
};



}
}
}

