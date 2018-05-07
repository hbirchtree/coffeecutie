#ifndef COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_45_H
#define COFFEE_GRAPHICS_APIS_OPENGL_LEVELS_45_H

#include "../shared/arb_direct_state_access.h"
#include "../shared/textures/arb_bindless_texture.h"
#include "gl43.h"

namespace Coffee {
namespace CGL {
/*!
 * \brief OpenGL 4.5 compliance model
 */
template<typename ReqVer>
struct CGL45_Base : CGL43_Base<ReqVer>,
                    CGL_BindlessTexture,
                    CGL_DirectStateAccess<ReqVer>
{
    using Debug = typename CGL43::Debug;

    STATICINLINE GL_EXT_CHECK(DirectState, "GL_ARB_direct_state_access")

    STATICINLINE GL_EXT_CHECK(BindlessTexture, "GL_ARB_bindless_texture")

    STATICINLINE GL_EXT_CHECK(CullDistance, "GL_ARB_cull_distance")

    /*
     * Most functions here should use DSA
     *
     */
    GL_VERSION_REQ(GLVER_45)
    STATICINLINE void MemoryBarrierByRegion(GLbitfield f)
    {
        glMemoryBarrierByRegion(f);
    }

    /* DSA disabling */
    STATICINLINE void BufBind()
    {
    }
    STATICINLINE void FBBind()
    {
    }
    STATICINLINE void TexBind()
    {
    }
    STATICINLINE void SamplerBind()
    {
    }
    STATICINLINE void XFBind()
    {
    }
};

using CGL45 = CGL45_Base<GLVER_45>;
} // namespace CGL
} // namespace Coffee

#endif
