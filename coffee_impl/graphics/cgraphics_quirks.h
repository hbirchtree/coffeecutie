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

    bool    ext_multi_draw_indirect     = false;

    bool    ext_ssbo_support            = false;

    bool    ext_bindless_texture        = false;
    bool    ext_texture_storage         = false;

    bool    ext_separate_shader_objs    = false;

    bool    ext_direct_state_access     = false;
    bool    ext_invalidate_subdata      = false;
};

extern bool coffee_quirks_query_extension(
        const CFeatureSet& features,
        cstring requested);
extern bool _quirk_extension(
        cstring extension, cstring allexts);
extern void coffee_quirks_set(
        CFeatureSet* featureset);

}

} //Coffee

#endif
