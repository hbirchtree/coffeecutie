#include <coffee/image/cnoise-gen.h>

//#define STB_PERLIN_IMPLEMENTATION
//#include <stb_perlin.h>

namespace Coffee{
namespace NoiseGen{

scalar Perlin(Vecf3 const& pos, Veci3 const& wrap)
{
    return 0.f;
//    return stb_perlin_noise3(pos.x(),pos.y(),pos.z(),wrap.x(),wrap.y(),wrap.z());
}

}
}
