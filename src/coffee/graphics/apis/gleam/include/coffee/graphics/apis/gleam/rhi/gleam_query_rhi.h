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

struct GLEAM_TimeQuery : GraphicsAPI::TimeQuery<query::flags>, GLEAM_Query
{
    GLEAM_TimeQuery() : GLEAM_Query(query::timing::value)
    {
    }
    ~GLEAM_TimeQuery()
    {
        dealloc();
    }

    void begin();
    void end();

    bool available();
    u64  result();
};

struct GLEAM_OccludeQuery : GraphicsAPI::OccludeQuery<query::flags>, GLEAM_Query
{
    GLEAM_OccludeQuery(query::flags t) : GLEAM_Query(t)
    {
    }

    void begin();
    void end();

    i64 resulti();
    u64 resultu();
};

} // namespace GLEAM
} // namespace RHI
} // namespace Coffee
