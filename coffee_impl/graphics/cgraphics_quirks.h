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
    cstring const* extensions = nullptr;

    bool    render_multidraw		= false;
    bool    render_ssbo_support		= false;
};

static bool coffee_quirks_query_extension(
	const CFeatureSet& features,
	cstring requested)
{
    return strstr(*features.extensions,requested);
}

}

} //Coffee

#endif
