#ifndef COFFEE_CORE_VECTORS_H
#define COFFEE_CORE_VECTORS_H

#include "coffee/core/base/cmath.h"
#include "coffee/core/types/basetypes.h"

namespace Coffee{
namespace CVectors{

template<typename T, size_t Size> struct _cbasic_tvector
{
    _cbasic_tvector()
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] = T(0);
    }
    _cbasic_tvector(const T& c)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] = c;
    }
    _cbasic_tvector(const _cbasic_tvector<T,Size>& vector)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] = vector[i];
    }

    _cbasic_tvector(
            const _cbasic_tvector<T,Size-1>& v,
            const T& c)
    {
        static_assert((Size-1) > 1,"Invalid vector combination!");
        for(size_t i=0;i<(Size-1);i++)
            this[i] = v[i];
        this[Size-1] = c;
    }
    //This horrible crime against the laws of nature should not be put to use.
//    template<typename... I>
//    _cbasic_tvector(
//            typename std::enable_if<std::is_convertible<I,T>::value && Size == sizeof...(I),T>::type... args)
//    {
//        fprintf(stderr,"Sizes: %i, %i",sizeof...(args),Size);
//        for(size_t i=0;i<Size;i++)
//            this[i] = T(0);
//    }
    T& operator[](size_t i)
    {
        return data[i];
    }
    const T& operator[](size_t i) const
    {
        return data[i];
    }
    bool operator==(
            const _cbasic_tvector<T,Size>& v) const
    {
        for(size_t i=0;i<Size;i++)
            if(v[i]!=(*this)[i])
                return false;
        return true;
    }
    void operator=(
            const _cbasic_tvector<T,Size>& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] = v[i];
    }
    void operator+=(
            const _cbasic_tvector<T,Size>& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] += v[i];
    }
    void operator-=(
            const _cbasic_tvector<T,Size>& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] += v[i];
    }
    void operator*=(
            const _cbasic_tvector<T,Size>& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] *= v[i];
    }
    void operator/=(
            const _cbasic_tvector<T,Size>& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] /= v[i];
    }
    _cbasic_tvector<T,Size> operator+(
            const _cbasic_tvector<T,Size>& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]+v[i];
        return vnew;
    }
    _cbasic_tvector<T,Size> operator-(
            const _cbasic_tvector<T,Size>& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]-v[i];
        return vnew;
    }
    _cbasic_tvector<T,Size> operator*(
            const _cbasic_tvector<T,Size>& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]*v[i];
        return vnew;
    }
    _cbasic_tvector<T,Size> operator/(
            const _cbasic_tvector<T,Size>& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]/v[i];
        return vnew;
    }

    template<typename I>
    _cbasic_tvector<T,Size> operator*(const T& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]*v;
        return vnew;
    }
    template<typename I>
    _cbasic_tvector<T,Size> operator/(const T& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]/v;
        return vnew;
    }
    template<typename I>
    void operator*=(const T& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] *= v;
    }
    template<typename I>
    void operator/=(const T& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] /= v;
    }
private:
    T data[Size];
};

template<typename T,size_t Size> T length(
        const _cbasic_tvector<T,Size>& v)
{
    T sum = 0;
    for(int i=0;i<Size;i++)
        sum += pow(v[i],(T)2);
    return sqrt(sum);
}

template<typename T,size_t Size> T dot(
        const _cbasic_tvector<T,Size>& v1,
        const _cbasic_tvector<T,Size>& v2)
{
    T sum = 0;
    for(int i=0;i<Size;i++)
        sum += v1[i]*v2[i];
    return sum;
}

template<typename T,size_t Size> _cbasic_tvector<T,Size> normalize(
        const _cbasic_tvector<T,Size>& v)
{
    _cbasic_tvector<T,Size> vnew;
    T len = length<T,Size>(v);
    vnew = v/len;
    return vnew;
}

template<typename T> _cbasic_tvector<T,3> cross(
        const _cbasic_tvector<T,3>& v1,
        const _cbasic_tvector<T,3>& v2)
{
    _cbasic_tvector<T,3> vnew;
    vnew[0] = v1[1]*v2[2] - v1[2]*v2[1];
    vnew[1] = v1[2]*v2[0] - v1[0]*v2[2];
    vnew[2] = v1[0]*v2[1] - v1[1]*v2[0];
    return vnew;
}

template<typename T,size_t Size> T distance(
        const _cbasic_tvector<T,Size>& v1,
        const _cbasic_tvector<T,Size>& v2)
{
    return length(v1-v2);
}

}
}

#endif
