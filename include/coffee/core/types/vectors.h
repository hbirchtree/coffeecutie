#ifndef COFFEE_CORE_VECTORS_H
#define COFFEE_CORE_VECTORS_H

#include "../coffee_macros.h"
#include "../base/cmath.h"
#include "basetypes.h"

namespace Coffee{

template<typename T, size_t Size> struct _cbasic_tvector
{
    static constexpr size_t size = Size;

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
    template<class = typename std::enable_if<Size==2>>
    _cbasic_tvector(const T& x, const T& y)
    {
        this->x() = x;
        this->y() = y;
    }
    template<class = typename std::enable_if<Size==3>>
    _cbasic_tvector(const T& x, const T& y,const T& z)
    {
        this->x() = x;
        this->y() = y;
        this->z() = z;
    }
    template<class = typename std::enable_if<Size==4>>
    _cbasic_tvector(const T& x, const T& y,const T& z,const T& w)
    {
        this->x() = x;
        this->y() = y;
        this->z() = z;
        this->w() = w;
    }

    _cbasic_tvector(
            const _cbasic_tvector<T,Size-1>& v,
            const T& c)
    {
        static_assert((Size-1) > 1,"Invalid vector combination!");
        for(size_t i=0;i<Size-1;i++)
            this[i] = v[i];
        this[Size-1] = c;
    }

    T& x()
    {
        return (*this)[0];
    }
    template<class = typename std::enable_if<Size>=2>>
    T& y()
    {
        return (*this)[1];
    }
    template<class = typename std::enable_if<Size>=3>>
    T& z()
    {
        return (*this)[2];
    }
    template<class = typename std::enable_if<Size>=4>>
    T& w()
    {
        return (*this)[3];
    }

    const T& x() const
    {
        return (*this)[0];
    }
    template<class = typename std::enable_if<Size>=2>>
    const T& y() const
    {
        return (*this)[1];
    }
    template<class = typename std::enable_if<Size>=3>>
    const T& z() const
    {
        return (*this)[2];
    }
    template<class = typename std::enable_if<Size>=4>>
    const T& w() const
    {
        return (*this)[3];
    }

    T& r()
    {
        return (*this)[0];
    }
    template<class = typename std::enable_if<Size>=2>>
    T& g()
    {
        return (*this)[1];
    }
    template<class = typename std::enable_if<Size>=3>>
    T& b()
    {
        return (*this)[2];
    }
    template<class = typename std::enable_if<Size>=4>>
    T& a()
    {
        return (*this)[3];
    }

    const T& r() const
    {
        return (*this)[0];
    }
    template<class = typename std::enable_if<Size>=2>>
    const T& g() const
    {
        return (*this)[1];
    }
    template<class = typename std::enable_if<Size>=3>>
    const T& b() const
    {
        return (*this)[2];
    }
    template<class = typename std::enable_if<Size>=4>>
    const T& a() const
    {
        return (*this)[3];
    }

    template<class = typename std::enable_if<Size==2>>
    T& u()
    {
        return (*this)[0];
    }
    template<class = typename std::enable_if<Size==2>>
    T& v()
    {
        return (*this)[1];
    }

    template<class = typename std::enable_if<Size==2>>
    const T& u() const
    {
        return (*this)[0];
    }
    template<class = typename std::enable_if<Size==2>>
    const T& v() const
    {
        return (*this)[1];
    }

    /*Swizzling operators!*/

    template<class = typename std::enable_if<Size==4>>
    _cbasic_tvector<T,Size-1> xyz() const
    {
        return _cbasic_tvector<T,Size-1>(*this);
    }
    template<class = typename std::enable_if<Size==3>>
    _cbasic_tvector<T,Size-1> xy() const
    {
        return _cbasic_tvector<T,Size-1>(*this);
    }

    /*No more swizzles*/

    inline C_FORCE_INLINE T& operator[](size_t i)
    {
        return data[i];
    }
    inline C_FORCE_INLINE const T& operator[](size_t i) const
    {
        return data[i];
    }
    inline C_FORCE_INLINE bool operator==(
            const _cbasic_tvector<T,Size>& v) const
    {
        for(size_t i=0;i<Size;i++)
            if(v[i]!=(*this)[i])
                return false;
        return true;
    }
    inline C_FORCE_INLINE void operator=(
            const _cbasic_tvector<T,Size>& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] = v[i];
    }
    inline void operator+=(
            const _cbasic_tvector<T,Size>& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] += v[i];
    }
    inline C_FORCE_INLINE void operator-=(
            const _cbasic_tvector<T,Size>& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] += v[i];
    }
    inline C_FORCE_INLINE void operator*=(
            const _cbasic_tvector<T,Size>& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] *= v[i];
    }
    inline C_FORCE_INLINE void operator/=(
            const _cbasic_tvector<T,Size>& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] /= v[i];
    }
    inline C_FORCE_INLINE _cbasic_tvector<T,Size> operator+(
            const _cbasic_tvector<T,Size>& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]+v[i];
        return vnew;
    }
    inline C_FORCE_INLINE _cbasic_tvector<T,Size> operator-(
            const _cbasic_tvector<T,Size>& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]-v[i];
        return vnew;
    }
    inline C_FORCE_INLINE _cbasic_tvector<T,Size> operator*(
            const _cbasic_tvector<T,Size>& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]*v[i];
        return vnew;
    }
    inline C_FORCE_INLINE _cbasic_tvector<T,Size> operator/(
            const _cbasic_tvector<T,Size>& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]/v[i];
        return vnew;
    }

    inline C_FORCE_INLINE _cbasic_tvector<T,Size> operator*(const T& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]*v;
        return vnew;
    }
    inline C_FORCE_INLINE _cbasic_tvector<T,Size> operator/(const T& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]/v;
        return vnew;
    }
    inline C_FORCE_INLINE void operator*=(const T& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] *= v;
    }
    inline C_FORCE_INLINE void operator/=(const T& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] /= v;
    }
private:
    T data[Size];
};

template<typename T,size_t Size>
inline C_FORCE_INLINE T length(
        const _cbasic_tvector<T,Size>& v)
{
    T sum = 0;
    for(size_t i=0;i<Size;i++)
        sum += pow(v[i],(T)2);
    return sqrt(sum);
}

template<typename T,size_t Size>
inline C_FORCE_INLINE T dot(
        const _cbasic_tvector<T,Size>& v1,
        const _cbasic_tvector<T,Size>& v2)
{
    T sum = 0;
    for(size_t i=0;i<Size;i++)
        sum += v1[i]*v2[i];
    return sum;
}

template<typename T,size_t Size>
inline C_FORCE_INLINE _cbasic_tvector<T,Size> normalize(
        const _cbasic_tvector<T,Size>& v)
{
    _cbasic_tvector<T,Size> vnew;
    T len = length<T,Size>(v);
    vnew = v/len;
    return vnew;
}

template<typename T>
inline C_FORCE_INLINE _cbasic_tvector<T,3> cross(
        const _cbasic_tvector<T,3>& v1,
        const _cbasic_tvector<T,3>& v2)
{
    _cbasic_tvector<T,3> vnew;
    vnew[0] = v1[1]*v2[2] - v1[2]*v2[1];
    vnew[1] = v1[2]*v2[0] - v1[0]*v2[2];
    vnew[2] = v1[0]*v2[1] - v1[1]*v2[0];
    return vnew;
}

template<typename T,size_t Size>
inline C_FORCE_INLINE T distance(
        const _cbasic_tvector<T,Size>& v1,
        const _cbasic_tvector<T,Size>& v2)
{
    return length(v1-v2);
}

template<typename T,size_t Size>
inline C_FORCE_INLINE T reflect(
        const _cbasic_tvector<T,Size>& v,
        const _cbasic_tvector<T,Size>& n)
{
    return v-((n*T(2))*v)*n;
}

template<typename T,size_t Size>
inline C_FORCE_INLINE T refract(
        const _cbasic_tvector<T,Size>& v,
        const _cbasic_tvector<T,Size>& n,
        const T& c)
{
    T k = T(1)-CMath::pow(c,2)*(T(1)-CMath::pow(n*v,2));

    if(k < T(0))
        return T(0);
    else
        return v*c-(c*(n*v)+CMath::sqrt(k))*n;
}

template<typename T,size_t Size>
inline C_FORCE_INLINE _cbasic_tvector<T,Size> mix(
        const _cbasic_tvector<T,Size>& v1,
        const _cbasic_tvector<T,Size>& v2,
        const T& f)
{
    return _cbasic_tvector<T,Size>(v1*(T(1)-f)+v2*f);
}

}

#endif
