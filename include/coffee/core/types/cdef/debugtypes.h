#pragma once

#include "../../coffee_mem_macros.h"
#include "../tdef/integertypes.h"

namespace Coffee{

struct bigscalar_dissect
{
    union
    {
        struct{
            uint8 sign:1;
            uint16 exponent:11;
            uint64 mantissa:52;
        };
        bigscalar v;
    };
};
struct scalar_dissect
{
    union
    {
        PACKEDSTRUCT {
            uint8 sign:1;
            uint8 exponent:8;
            uint32 mantissa1:23;
        };
        scalar v;
    };
};
struct half_dissect
{
    union
    {
        struct{
            uint8 sign:1;
            uint8 exponent:5;
            uint16 mantissa:10;
        };
        uint16 v;
    };
};

/* The below floating-point formats are used in OpenGL */
//struct C_FORCE_PACKING scalar11
//{
//    uint8 exponent:6;
//    uint8 mantissa:5;
//};
//struct C_FORCE_PACKING scalar10
//{
//    uint8 exponent:5;
//    uint8 mantissa:5;
//};
/**/

}