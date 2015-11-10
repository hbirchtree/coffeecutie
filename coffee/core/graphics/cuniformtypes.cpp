#include "cuniformtypes.h"

namespace Coffee{
namespace CGraphicsWrappers{

_cbasic_graphics_resource_desc::_cbasic_graphics_resource_desc():
    object_name(nullptr),
    index(0)
{
}

_cbasic_graphics_buffer_resource_desc::_cbasic_graphics_buffer_resource_desc():
    object_name(nullptr),
    buffer(nullptr),
    index(0)
{
}

}
}
