#pragma once

#include <coffee/core/CMath>

namespace Coffee {

template<typename T, size_t Size>
struct _cbasic_tvector
{
  private:
    T data[Size];

  public:
    static constexpr size_t size = Size;

    FORCEDINLINE _cbasic_tvector()
    {
        for(size_t i = 0; i < Size; i++)
            (*this)[i] = T(0);
    }
    FORCEDINLINE _cbasic_tvector(const T& c)
    {
        for(size_t i = 0; i < Size; i++)
            (*this)[i] = c;
    }
    FORCEDINLINE _cbasic_tvector(const _cbasic_tvector<T, Size>& vector)
    {
        for(size_t i = 0; i < Size; i++)
            (*this)[i] = vector[i];
    }
    template<class = typename std::enable_if<Size == 2>>
    FORCEDINLINE _cbasic_tvector(const T& x, const T& y)
    {
        this->x() = x;
        this->y() = y;
    }
    template<class = typename std::enable_if<Size == 3>>
    FORCEDINLINE _cbasic_tvector(const T& x, const T& y, const T& z)
    {
        this->x() = x;
        this->y() = y;
        this->z() = z;
    }
    template<class = typename std::enable_if<Size == 4>>
    FORCEDINLINE _cbasic_tvector(const T& x, const T& y, const T& z, const T& w)
    {
        this->x() = x;
        this->y() = y;
        this->z() = z;
        this->w() = w;
    }

    FORCEDINLINE _cbasic_tvector(
        const _cbasic_tvector<T, Size - 1>& v, const T& c)
    {
        static_assert((Size - 1) > 1, "Invalid vector combination!");
        for(size_t i = 0; i < Size - 1; i++)
            (*this)[i] = v[i];
        (*this)[Size - 1] = c;
    }

    FORCEDINLINE _cbasic_tvector(const _cbasic_tvector<T, Size + 1>& v)
    {
        static_assert((Size + 1) <= 4, "Invalid vector combination!");
        for(size_t i = 0; i < Size; i++)
            (*this)[i] = v[i];
    }

    FORCEDINLINE T& x()
    {
        return (*this)[0];
    }
    template<class = typename std::enable_if<Size >= 2>>
    FORCEDINLINE T& y()
    {
        return (*this)[1];
    }
    template<class = typename std::enable_if<Size >= 3>>
    FORCEDINLINE T& z()
    {
        return (*this)[2];
    }
    template<class = typename std::enable_if<Size >= 4>>
    FORCEDINLINE T& w()
    {
        return (*this)[3];
    }

    FORCEDINLINE const T& x() const
    {
        return (*this)[0];
    }
    template<class = typename std::enable_if<Size >= 2>>
    FORCEDINLINE const T& y() const
    {
        return (*this)[1];
    }
    template<class = typename std::enable_if<Size >= 3>>
    FORCEDINLINE const T& z() const
    {
        return (*this)[2];
    }
    template<class = typename std::enable_if<Size >= 4>>
    FORCEDINLINE const T& w() const
    {
        return (*this)[3];
    }

    FORCEDINLINE T& r()
    {
        return (*this)[0];
    }
    template<class = typename std::enable_if<Size >= 2>>
    FORCEDINLINE T& g()
    {
        return (*this)[1];
    }
    template<class = typename std::enable_if<Size >= 3>>
    FORCEDINLINE T& b()
    {
        return (*this)[2];
    }
    template<class = typename std::enable_if<Size >= 4>>
    FORCEDINLINE T& a()
    {
        return (*this)[3];
    }

    FORCEDINLINE const T& r() const
    {
        return (*this)[0];
    }
    template<class = typename std::enable_if<Size >= 2>>
    FORCEDINLINE const T& g() const
    {
        return (*this)[1];
    }
    template<class = typename std::enable_if<Size >= 3>>
    FORCEDINLINE const T& b() const
    {
        return (*this)[2];
    }
    template<class = typename std::enable_if<Size >= 4>>
    FORCEDINLINE const T& a() const
    {
        return (*this)[3];
    }

    template<class = typename std::enable_if<Size == 2>>
    FORCEDINLINE T& u()
    {
        return (*this)[0];
    }
    template<class = typename std::enable_if<Size == 2>>
    FORCEDINLINE T& v()
    {
        return (*this)[1];
    }

    template<class = typename std::enable_if<Size == 2>>
    FORCEDINLINE const T& u() const
    {
        return (*this)[0];
    }
    template<class = typename std::enable_if<Size == 2>>
    FORCEDINLINE const T& v() const
    {
        return (*this)[1];
    }

    /*Swizzling operators!*/

    template<class = typename std::enable_if<Size == 4>>
    FORCEDINLINE _cbasic_tvector<T, Size - 1> xyz() const
    {
        return _cbasic_tvector<T, Size - 1>(*this);
    }
    template<class = typename std::enable_if<Size == 3>>
    FORCEDINLINE _cbasic_tvector<T, Size - 1> xy() const
    {
        return _cbasic_tvector<T, Size - 1>(*this);
    }

    /*No more swizzles*/

    FORCEDINLINE T& operator[](size_t i)
    {
        return data[i];
    }
    FORCEDINLINE const T& operator[](size_t i) const
    {
        return data[i];
    }
    FORCEDINLINE bool operator==(const _cbasic_tvector<T, Size>& v) const
    {
        for(size_t i = 0; i < Size; i++)
            if(v[i] != (*this)[i])
                return false;
        return true;
    }
    FORCEDINLINE void operator=(const _cbasic_tvector<T, Size>& v)
    {
        for(size_t i = 0; i < Size; i++)
            (*this)[i] = v[i];
    }
    inline void operator+=(const _cbasic_tvector<T, Size>& v)
    {
        for(size_t i = 0; i < Size; i++)
            (*this)[i] += v[i];
    }
    FORCEDINLINE void operator-=(const _cbasic_tvector<T, Size>& v)
    {
        for(size_t i = 0; i < Size; i++)
            (*this)[i] -= v[i];
    }
    FORCEDINLINE void operator*=(const _cbasic_tvector<T, Size>& v)
    {
        for(size_t i = 0; i < Size; i++)
            (*this)[i] *= v[i];
    }
    FORCEDINLINE void operator/=(const _cbasic_tvector<T, Size>& v)
    {
        for(size_t i = 0; i < Size; i++)
            (*this)[i] /= v[i];
    }
    FORCEDINLINE _cbasic_tvector<T, Size> operator+(
        const _cbasic_tvector<T, Size>& v) const
    {
        _cbasic_tvector<T, Size> vnew;
        for(size_t i = 0; i < Size; i++)
            vnew[i] = (*this)[i] + v[i];
        return vnew;
    }
    FORCEDINLINE _cbasic_tvector<T, Size> operator-(
        const _cbasic_tvector<T, Size>& v) const
    {
        _cbasic_tvector<T, Size> vnew;
        for(size_t i = 0; i < Size; i++)
            vnew[i] = (*this)[i] - v[i];
        return vnew;
    }
    FORCEDINLINE _cbasic_tvector<T, Size> operator*(
        const _cbasic_tvector<T, Size>& v) const
    {
        _cbasic_tvector<T, Size> vnew;
        for(size_t i = 0; i < Size; i++)
            vnew[i] = (*this)[i] * v[i];
        return vnew;
    }
    FORCEDINLINE _cbasic_tvector<T, Size> operator/(
        const _cbasic_tvector<T, Size>& v) const
    {
        _cbasic_tvector<T, Size> vnew;
        for(size_t i = 0; i < Size; i++)
            vnew[i] = (*this)[i] / v[i];
        return vnew;
    }

    FORCEDINLINE _cbasic_tvector<T, Size> operator*(const T& v) const
    {
        _cbasic_tvector<T, Size> vnew;
        for(size_t i = 0; i < Size; i++)
            vnew[i] = (*this)[i] * v;
        return vnew;
    }
    FORCEDINLINE _cbasic_tvector<T, Size> operator/(const T& v) const
    {
        _cbasic_tvector<T, Size> vnew;
        for(size_t i = 0; i < Size; i++)
            vnew[i] = (*this)[i] / v;
        return vnew;
    }
    FORCEDINLINE void operator*=(const T& v)
    {
        for(size_t i = 0; i < Size; i++)
            (*this)[i] *= v;
    }
    FORCEDINLINE void operator/=(const T& v)
    {
        for(size_t i = 0; i < Size; i++)
            (*this)[i] /= v;
    }
};

template<typename T, size_t Size>
FORCEDINLINE _cbasic_tvector<T, Size> operator+(
    T const& v, _cbasic_tvector<T, Size> const& vec)
{
    _cbasic_tvector<T, Size> out;

    for(size_t i=0; i<Size; i++)
        out[i] = vec[i] + v;

    return out;
}

template<typename T, size_t Size>
FORCEDINLINE _cbasic_tvector<T, Size> operator-(
    T const& v, _cbasic_tvector<T, Size> const& vec)
{
    _cbasic_tvector<T, Size> out;

    for(size_t i=0; i<Size; i++)
        out[i] = vec[i] - v;

    return out;
}

template<typename T, size_t Size>
FORCEDINLINE _cbasic_tvector<T, Size> operator*(
    T const& v, _cbasic_tvector<T, Size> const& vec)
{
    _cbasic_tvector<T, Size> out;

    for(size_t i=0; i<Size; i++)
        out[i] = vec[i] * v;

    return out;
}

template<typename T, size_t Size>
FORCEDINLINE _cbasic_tvector<T, Size> operator/(
    T const& v, _cbasic_tvector<T, Size> const& vec)
{
    _cbasic_tvector<T, Size> out;

    for(size_t i=0; i<Size; i++)
        out[i] = vec[i] / v;

    return out;
}

/*
 * length() is overloaded with use of hypotx(), a C++11 function.
 * hypot() does not support integers, and falls back to the
 *  traditional loop in that case.
 */

template<
    typename T,
    size_t Size,
    typename std::enable_if<Size >= 4 || std::is_integral<T>::value>::type* =
        nullptr>
FORCEDINLINE T length(const _cbasic_tvector<T, Size>& v)
{
    T sum = 0;
    for(size_t i = 0; i < Size; i++)
        sum += pow(v[i], (T)2);
    return sqrt(sum);
}

template<
    typename T,
    size_t Size,
    typename std::enable_if<Size == 2>::type*                      = nullptr,
    typename std::enable_if<std::is_same<T, double>::value>::type* = nullptr>
FORCEDINLINE T length(const _cbasic_tvector<T, Size>& v)
{
    return hypot(v[0], v[1]);
}

template<
    typename T,
    size_t Size,
    typename std::enable_if<Size == 2>::type*                     = nullptr,
    typename std::enable_if<std::is_same<T, float>::value>::type* = nullptr>
FORCEDINLINE T length(const _cbasic_tvector<T, Size>& v)
{
    return hypotf(v[0], v[1]);
}

template<
    typename T,
    size_t Size,
    typename std::enable_if<Size == 2>::type* = nullptr,
    typename std::enable_if<std::is_same<T, long double>::value>::type* =
        nullptr>
FORCEDINLINE T length(const _cbasic_tvector<T, Size>& v)
{
    return hypotl(v[0], v[1]);
}

template<
    typename T,
    size_t Size,
    typename std::enable_if<Size == 3>::type*                      = nullptr,
    typename std::enable_if<std::is_same<T, double>::value>::type* = nullptr>
FORCEDINLINE T length(const _cbasic_tvector<T, Size>& v)
{
    return CMath::sqrt(
        CMath::pow(v[0], 2.) + CMath::pow(v[1], 2.) + CMath::pow(v[2], 2.));
}

template<
    typename T,
    size_t Size,
    typename std::enable_if<Size == 3>::type*                     = nullptr,
    typename std::enable_if<std::is_same<T, float>::value>::type* = nullptr>
FORCEDINLINE T length(const _cbasic_tvector<T, Size>& v)
{
    return CMath::sqrt(
        CMath::pow(v[0], 2.f) + CMath::pow(v[1], 2.f) + CMath::pow(v[2], 2.f));
}

template<
    typename T,
    size_t Size,
    typename std::enable_if<Size == 3>::type* = nullptr,
    typename std::enable_if<std::is_same<T, long double>::value>::type* =
        nullptr>
FORCEDINLINE T length(const _cbasic_tvector<T, Size>& v)
{
    return CMath::sqrt(
        CMath::pow(v[0], 2.) + CMath::pow(v[1], 2.) + CMath::pow(v[2], 2.));
}

template<typename T, size_t Size>
FORCEDINLINE T
             dot(const _cbasic_tvector<T, Size>& v1, const _cbasic_tvector<T, Size>& v2)
{
    T sum = 0;
    for(size_t i = 0; i < Size; i++)
        sum += v1[i] * v2[i];
    return sum;
}

template<typename T, size_t Size>
FORCEDINLINE _cbasic_tvector<T, Size> normalize(
    const _cbasic_tvector<T, Size>& v)
{
    _cbasic_tvector<T, Size> vnew;
    T                        len = length<T, Size>(v);
    vnew                         = v / len;
    return vnew;
}

template<typename T>
FORCEDINLINE _cbasic_tvector<T, 3> cross(
    const _cbasic_tvector<T, 3>& v1, const _cbasic_tvector<T, 3>& v2)
{
    _cbasic_tvector<T, 3> vnew;
    vnew[0] = v1[1] * v2[2] - v1[2] * v2[1];
    vnew[1] = v1[2] * v2[0] - v1[0] * v2[2];
    vnew[2] = v1[0] * v2[1] - v1[1] * v2[0];
    return vnew;
}

template<typename T, size_t Size>
FORCEDINLINE T
             distance(const _cbasic_tvector<T, Size>& v1, const _cbasic_tvector<T, Size>& v2)
{
    return length(v1 - v2);
}

template<typename T, size_t Size>
FORCEDINLINE T
             reflect(const _cbasic_tvector<T, Size>& v, const _cbasic_tvector<T, Size>& n)
{
    return v - ((n * T(2)) * v) * n;
}

template<typename T, size_t Size>
FORCEDINLINE T refract(
    const _cbasic_tvector<T, Size>& v,
    const _cbasic_tvector<T, Size>& n,
    const T&                        c)
{
    T k = T(1) - CMath::pow(c, 2) * (T(1) - CMath::pow(n * v, 2));

    if(k < T(0))
        return T(0);
    else
        return v * c - (c * (n * v) + CMath::sqrt(k)) * n;
}

template<typename T, size_t Size>
FORCEDINLINE _cbasic_tvector<T, Size> mix(
    const _cbasic_tvector<T, Size>& v1,
    const _cbasic_tvector<T, Size>& v2,
    const T&                        f)
{
    return _cbasic_tvector<T, Size>(v1 * (T(1) - f) + v2 * f);
}

} // namespace Coffee
