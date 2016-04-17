#pragma once

#include "../libraries.h"

namespace Coffee{
namespace Library{
namespace Mac{

struct MacFunctionLoader : FunctionLoad_def
{

};

struct MacObjectLoader : ObjectLoader_def<MacFunctionLoader>
{

};

}

using FunctionLoader = Mac::MacFunctionLoader;
using ObjectLoader = Mac::MacObjectLoader;

}
}
