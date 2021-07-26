#pragma once

#include <peripherals/semantic/enum/data_types.h>

#include "rhi_program.h"
#include "rhi_query.h"
#include "rhi_versioning.h"
#include "rhi_vertex.h"

namespace gleam {

struct draw_command
{
    using vertex_type =
#if GLEAM_MAX_VERSION_ES != 0x200
        vertex_array_t;
#else
        vertex_array_legacy_t;
#endif
    using query_type =
#if GLEAM_MAX_VERSION_ES != 0x200
        query_t;
#else
        null_query_t;
#endif

    stl_types::WkPtr<program_t>   program;
    stl_types::WkPtr<vertex_type> vertices;
    struct
    {
        bool indexed{false};
        bool instanced{false};

        drawing::primitive mode{drawing::primitive::triangle};
    } call;
    struct
    {
        struct
        {
            u32 count{0}, offset{0};
        } arrays;
        struct
        {
            u32 count{0};
            u64 offset{0};

            semantic::TypeEnum type{semantic::TypeEnum::UInt};
        } elements;
        struct
        {
            u32 count{0}, offset{0};
        } instances;
    } data;

    stl_types::WkPtr<query_type> conditional_query;

    int uniforms;
    int buffers;
    int samplers;
};

} // namespace gleam
