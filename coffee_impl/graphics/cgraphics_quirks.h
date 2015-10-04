#ifndef CGRAPHICS_QUIRKS
#define CGRAPHICS_QUIRKS

#include "coffee_types.h"

namespace Coffee
{
namespace CGraphicsQuirks{

//With the feature set of the platform, we can determine if a certain rendering method can be performed and do with a workaround
struct CFeatureSet
{
    //Is a pointer to a string, not an array
    cstring extensions  = nullptr;

    bool    render_multidraw		= false;
    bool    render_ssbo_support		= false;
    bool    render_bindless_texture     = false;
};

static bool coffee_quirks_query_extension(
        const CFeatureSet& features,
        cstring requested)
{
    return strstr(features.extensions,requested);
}

static bool _quirk_extension(cstring extension, cstring allexts)
{
    if(strstr(allexts,extension)){
//        cDebug("Enabling code for %s",extension);
        return true;
    }else{
        cDebug("Extension %s not available",extension);
        return false;
    }
}

static void coffee_quirks_set(CFeatureSet* featureset)
{
    featureset->render_multidraw = _quirk_extension(
                "GL_ARB_multi_draw_indirect",
                featureset->extensions);

    featureset->render_ssbo_support = _quirk_extension(
                "GL_ARB_shader_storage_buffer_object",
                featureset->extensions);

    featureset->render_bindless_texture = _quirk_extension(
                "GL_ARB_bindless_texture",
                featureset->extensions);
}

}

} //Coffee

#endif
