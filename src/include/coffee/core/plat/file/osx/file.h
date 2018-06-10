#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE)

#include "../unix/file.h"

namespace Coffee{
namespace CResources{
namespace Mac{

struct MacFileFun : Posix::PosixFileFun
{
};

struct MacDirFun : Posix::PosixDirFun
{

};

}

using FileFun = Mac::MacFileFun;
using DirFun = Mac::MacDirFun;

}
}

#endif
