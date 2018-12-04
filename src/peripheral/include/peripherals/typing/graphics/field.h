#pragma once

namespace typing {
namespace graphics {

template<typename T>
struct field
{
    T near_, far_;

    T range() const
    {
        return far_ - near_;
    }
};

} // namespace graphics
} // namespace typing
