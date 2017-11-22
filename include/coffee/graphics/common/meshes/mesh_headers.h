#pragma once

#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace SMSH{

const constexpr cstring magic_word = "CSMSH";

typedef struct
{
    szptr content_size;
    int64 content_offset;
    uint16 id;

    uint16 pad_2;
    uint32 pad_3;
} attr_grouping_t;

typedef PACKEDSTRUCT
{
    char magic[8];
    char build_ver[32];
    uint32 num_groups;
} smsh_header_t;

typedef struct
{
    attr_grouping_t groups[];
} smsh_groups_t;

}
}
