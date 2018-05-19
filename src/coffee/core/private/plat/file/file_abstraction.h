#pragma once

#include <coffee/core/plat/memory/string_ops.h>
#include <coffee/core/coffee_mem_macros.h>
#include <coffee/core/types/tdef/integertypes.h>

namespace Coffee{
namespace CResources{

struct AssetApi
{
    static const constexpr cstring AssetPrefix = ":/";

    STATICINLINE cstring GetAsset(cstring fn)
    {
        if(!fn)
            return nullptr;
        if(Mem::Search::StrFind(fn,AssetPrefix)==fn)
            return &fn[Mem::StrLen(AssetPrefix)];
        else
            return nullptr;
    }
};

}
}
