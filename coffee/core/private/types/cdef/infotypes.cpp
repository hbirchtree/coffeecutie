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

    if(argc > 0)
        arg.m_programName = argv[0];

    argc--;
    argv++;

    if(argc < 0)
        argc = 0;

    arg.m_storage.reserve(C_FCAST<u32>(argc));

    for(auto i : Range<>(C_FCAST<u32>(argc)))
        arg.m_storage.push_back(argv[i]);

    arg.rebuildArgs();

    return arg;
}

cstring_w _cbasic_arg_container::programName() const
{
    return m_programName;
}

const Vector<cstring_w> &_cbasic_arg_container::arguments() const
{
    return m_ptrStorage;
}

const Vector<CString> &_cbasic_arg_container::originalArguments() const
{
    return m_storage;
}

}
