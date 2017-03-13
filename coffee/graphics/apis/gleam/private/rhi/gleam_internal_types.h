#pragma once

#include <coffee/graphics/apis/gleam/rhi/gleam_types_rhi.h>
#include <coffee/graphics/apis/gleam/rhi/gleam_data.h>
#include <coffee/graphics/apis/gleam/gleam.h>

namespace Coffee{
namespace RHI{
namespace GLEAM{

#ifndef COFFEE_GLEAM_DESKTOP
#if !defined(COFFEE_ONLY_GLES20)
using CGL33 = CGLES30;
using CGL43 = CGLES32;
#else
using CGL33 = CGLES20;
using CGL43 = CGLES20;
#endif
#endif

using namespace CGL;

extern thread_local GLEAM_DataStore* m_store;

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
        Pbo& ref = buffers.at(idx++);
        idx = idx % buffers.size();
        return ref;
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

#define GL_CURR_API m_store->CURR_API
#define GL_DEBUG_MODE m_store->DEBUG_MODE
#define GLEAM_API_INSTANCE_DATA m_store->inst_data
