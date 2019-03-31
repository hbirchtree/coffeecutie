#pragma once

namespace typing {
namespace graphics {

enum class VertexFace
{
    None     = 0x0,
    Front    = 0x1,
    Back     = 0x2,
    Both     = 0x3,
    FaceMask = Both,
};

}
}
