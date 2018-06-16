#pragma once

#include "gleam_types_rhi.h"

namespace Coffee {
namespace RHI {
namespace GLEAM {

struct GLEAM_Query
{
    friend struct GLEAM_API;

    GLEAM_Query(query::flags t) : m_handle(0), m_type(t)
    {
    }

  protected:
    void alloc();
    void dealloc();

    u32          m_handle;
    query::flags m_type;
};

struct GLEAM_OccludeQuery : GraphicsAPI::OccludeQuery<query::flags>, GLEAM_Query
{
    GLEAM_OccludeQuery(query::flags t) : GLEAM_Query(t)
    {
    }

    void begin();
    void end();

    int64  resulti();
    uint64 resultu();
};

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
