#include <coffee/core/base/strings/vector_print_extensions.h>
#include <peripherals/stl/string_casting.h>

namespace Coffee{
namespace Strings{

template<typename T, size_t num, char v,

         typename is_pod<T>::type* = nullptr

         >
static inline CString vector_to_string(
        _cbasic_tvector<T, num> const& value)
{
    CString out;

    out += v;
    out += '(';

    for(size_t i=0;i<num;i++)
    {
        out += cast_pod(value[i]);
        if(i < num-1)
            out += ',';
    }

    out += ')';

    return out;
}

template<typename T, size_t num, char v,

         typename std::is_pod<T>::type* = nullptr

         >
static inline CString matrix_to_string(
        _cbasic_tmatrix<T, num> const& value)
{
    CString out;

    out += v;
    out += '(';

    for(size_t i=0;i<num;i++)
    {
        out += vector_to_string<T, num, 'v'>(value[i]);
    }

    out += ')';

    return out;
}

CString to_string(Quatf const& v)
{
    return vector_to_string<scalar, 4, 'q'>(v);
}

CString to_string(Vecf4 const& v)
{
    return vector_to_string<scalar, 4, 'v'>(v);
}

CString to_string(Vecf3 const& v)
{
    return vector_to_string<scalar, 3, 'v'>(v);
}

CString to_string(Vecf2 const& v)
{
    return vector_to_string<scalar, 2, 'v'>(v);
}

CString to_string(Matf4 const& v)
{
    return matrix_to_string<scalar, 4, 'm'>(v);
}

CString to_string(Matf3 const& v)
{
    return matrix_to_string<scalar, 3, 'm'>(v);
}

}
}
