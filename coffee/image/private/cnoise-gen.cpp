#include <coffee/image/cnoise-gen.h>

#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

namespace Coffee{
namespace NoiseGen{

scalar Perlin(const CVec3 &pos, const _cbasic_vec3<int32> &wrap)
{
    return stb_perlin_noise3(pos.x(),pos.y(),pos.z(),wrap.x(),wrap.y(),wrap.z());
}

}
}
