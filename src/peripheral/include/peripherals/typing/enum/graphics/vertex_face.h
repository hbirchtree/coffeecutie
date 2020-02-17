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

enum class CubeFace
{
    x_positive = 0,
    x_negative = 1,

    y_positive = 2,
    y_negative = 3,

    z_positive = 4,
    z_negative = 5,
};

} // namespace graphics
} // namespace typing
