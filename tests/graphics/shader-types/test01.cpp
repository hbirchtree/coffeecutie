#include <coffee/strings/libc_types.h>
#include <peripherals/typing/enum/graphics/shader_types.h>
#include <platforms/stacktrace.h>

#include <coffee/core/CUnitTesting>

using namespace Coffee;

template<typename sdt_struct>
bool test_sdt_flags(u32 flags)
{
    cDebug(
        "Flags on {2}: {0} & {1}",
        sdt_struct::value,
        flags,
        platform::env::Stacktracer::DemangleSymbol(typeid(sdt_struct).name()));

    return sdt_struct::value == flags;
}

bool shader_types_uniform_flags()
{
    using namespace typing::graphics::shader_type;

    if(!test_sdt_flags<sdt_uniff<S1>>(Uniform_v | S1 | Scalar_t))
        return false;

    if(!test_sdt_flags<sdt_attr<Scalar_t, S1>>(Attrib_v | S1 | Scalar_t))
        return false;

    if(!test_sdt_flags<sdt_sampf<S2>>(Sampler_v | S2 | Scalar_t))
        return false;

    return true;
}

COFFEE_TESTS_BEGIN(1)

    {shader_types_uniform_flags, "Uniform types"}

COFFEE_TESTS_END()
