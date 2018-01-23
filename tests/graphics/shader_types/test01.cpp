#include <coffee/core/CUnitTesting>
#include <coffee/core/types/edef/graphicsenum.h>

using namespace Coffee;
using namespace ShaderTypes;

template<typename sdt_struct>
bool test_sdt_flags(u32 flags)
{
    cDebug("Flags on {2}: {0} & {1}",
           sdt_struct::value,
           flags,
           Stacktracer::DemangleSymbol(typeid(sdt_struct).name()));

    return sdt_struct::value == flags;
}

bool shader_types_uniform_flags()
{
    if(!test_sdt_flags<sdt_uniff<S1>>(Uniform_v|S1|Scalar_t))
        return false;

    if(!test_sdt_flags<sdt_attr<Scalar_t, S1>>(Attrib_v|S1|Scalar_t))
        return false;

    if(!test_sdt_flags<sdt_sampf<S2>>(Sampler_v|S2|Scalar_t))
        return false;

    return true;
}

COFFEE_TEST_SUITE(1) = {
{shader_types_uniform_flags,"Uniform types"}
};

COFFEE_RUN_TESTS(_tests);
