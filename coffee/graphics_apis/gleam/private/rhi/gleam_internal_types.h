#pragma once

#include <coffee/graphics_apis/gleam/rhi/gleam_types_rhi.h>

namespace Coffee{
namespace RHI{
namespace GLEAM{

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

struct GLEAM_API::GLEAM_Instance_Data
{
    GLEAM_PboQueue pboQueue;
};

}
}
}
