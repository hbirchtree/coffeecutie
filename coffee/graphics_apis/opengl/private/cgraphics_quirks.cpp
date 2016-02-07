#include <coffee/graphics_apis/opengl/include/glbinding/cgraphics_quirks.h>

#include <cstring>

#include <coffee/core/base/cdebug.h>

namespace Coffee{
namespace CGraphicsQuirks{

static CFeatureSet _coffee_global_features;

bool _quirk_extension(cstring extension, cstring allexts)
{
    if(strstr(allexts,extension)){
        return true;
    }else{
        cDebug("Extension %s not available",extension);
        return false;
    }
}

void coffee_quirks_set(CFeatureSet *featureset)
{
    featureset->ext_multi_draw_indirect = _quirk_extension(
                "GL_ARB_multi_draw_indirect",
                featureset->extensions);

    featureset->ext_ssbo_support = _quirk_extension(
                "GL_ARB_shader_storage_buffer_object",
                featureset->extensions);

    featureset->ext_bindless_texture = _quirk_extension(
                "GL_ARB_bindless_texture",
                featureset->extensions);

    featureset->ext_separate_shader_objs = _quirk_extension(
                "GL_ARB_separate_shader_objects",
                featureset->extensions);

    featureset->ext_texture_storage = _quirk_extension(
                "GL_ARB_texture_storage",
                featureset->extensions);

    featureset->ext_direct_state_access = _quirk_extension(
                "GL_ARB_direct_state_access",
                featureset->extensions);

//    featureset->ext_bindless_texture = false;
    //    featureset->ext_ssbo_support = false;
}

const CFeatureSet &coffee_quirks_get_global()
{
    return _coffee_global_features;
}

void coffee_quirks_set_global(cstring extensions)
{
    _coffee_global_features.extensions = extensions;
    coffee_quirks_set(&_coffee_global_features);
}

}
}
