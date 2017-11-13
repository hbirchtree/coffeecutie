#include <coffee/core/types/cdef/infotypes.h>

namespace Coffee{

void _cbasic_arg_container::rebuildArgs()
{
    for(auto i : Range<>(m_storage.size()))
        m_ptrStorage.push_back(&m_storage[i][0]);
}

_cbasic_arg_container _cbasic_arg_container::Clone(int32 argc, cstring_w *argv)
{
    _cbasic_arg_container arg;

    arg.m_storage.reserve(C_FCAST<u32>(argc));

    for(auto i : Range<>(C_FCAST<u32>(argc)))
        arg.m_storage.push_back(argv[i]);

    arg.rebuildArgs();

    return arg;
}

const Vector<cstring_w> &_cbasic_arg_container::arguments() const
{
    return m_ptrStorage;
}

}
