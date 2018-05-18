#pragma once

#include "gleam_types_rhi.h"

namespace Coffee{
namespace RHI{
namespace GLEAM{

struct GLEAM_Query
{
    friend struct GLEAM_API;

    GLEAM_Query(QueryT t) : m_handle(0),m_type(t){}
protected:
    void alloc();
    void dealloc();

    CGhnd m_handle;
    QueryT m_type;
};

struct GLEAM_OccludeQuery : GraphicsAPI::OccludeQuery<QueryT>, GLEAM_Query
{
    GLEAM_OccludeQuery(QueryT t) : GLEAM_Query(t){}

    void begin();
    void end();

    int64 resulti();
    uint64 resultu();
};

}
}
}
