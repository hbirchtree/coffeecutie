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
        uint32 flags;
    };
    Vector<Pbo> buffers;
    uint32 idx = 0;
    Pbo& current()
    {
        return buffers.at(idx++);
    }
};

struct GLEAM_Instance_Data
{
    GLEAM_PboQueue pboQueue;
    struct {
	int32 NUM_PROGRAM_BINARY_FORMATS = -1;
    } GL_CACHED;
};

}
}
}
