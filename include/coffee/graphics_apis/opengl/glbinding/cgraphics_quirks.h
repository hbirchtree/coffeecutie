#ifndef CGRAPHICS_QUIRKS
#define CGRAPHICS_QUIRKS

#include "coffee/core/CTypes"

namespace Coffee
{
namespace CGraphicsQuirks{

/*!
 * \brief Feature sets are used to present which extensions are possible on the runtime platform. Due to some extensions not being implemented with Intel, we want workarounds that will perform the task, but not optimally as with newer extensions. Case in point: Bindless textures allow us to use 64-bit handles to access textures without binding or texture units, Intel does not support it on older drivers, workaround uses old methods. Naming scheme for these flags should resemble the extension which they represent.
 */
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

/*!
 * \brief Queries if an extension is present in a feature set
 * \param features
 * \param requested
 * \return True if the extension is present
 */
extern bool coffee_quirks_query_extension(
        const CFeatureSet& features,
        cstring requested);
/*!
 * \brief Set the bool values to match what is presented by the string "extensions" within the structure
 * \param featureset Structure to modify
 */
extern void coffee_quirks_set(
        CFeatureSet* featureset);

/*!
 * \brief Initialize the global set of graphics-quirks
 */
extern void coffee_quirks_set_global(cstring extensions);

/*!
 * \brief Acquire global quirk set
 * \return A reference to the global set of quirks to the graphical system
 */
extern const CFeatureSet& coffee_quirks_get_global();

}
} //Coffee

#endif
