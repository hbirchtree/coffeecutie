#pragma once

#include <coffee/graphics_apis/gleam/rhi/gleam_types_rhi.h>
#include <coffee/graphics_apis/gleam/gleam.h>

namespace Coffee{
namespace RHI{
namespace GLEAM{

#ifndef COFFEE_GLEAM_DESKTOP
using CGL33 = CGLES30;
using CGL43 = CGLES32;
#endif

using namespace CGL;

struct GLEAM_PboQueue
{
    struct Pbo
    {
        CGhnd buf;
    };
    Vector<Pbo> buffers;
    Pbo& current()
    {
        return buffers.at(0);
    }
};

struct GLEAM_Instance_Data
{
    GLEAM_PboQueue pboQueue;
};

}
}
}
