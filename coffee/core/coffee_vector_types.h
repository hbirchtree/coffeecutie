#ifndef COFFEE_VECTOR_TYPES_H
#define COFFEE_VECTOR_TYPES_H

#include <type_traits>
#include <cmath>
#include "coffee_basetypes.h"

namespace Coffee{

namespace CVectors{

template<typename T, size_t Size> struct _cbasic_tvector
{
    _cbasic_tvector()
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] = 0;
    }
    _cbasic_tvector(const T& c)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] = c;
    }
    _cbasic_tvector(
            const _cbasic_tvector<T,Size-1>& v,
            T c)
    {
        static_assert((Size-1) > 1,"Invalid vector combination!");
        for(size_t i=0;i<(Size-1);i++)
            this[i] = v[i];
        this[Size-1] = c;
    }
    //This is absolutely disgusting. And yet it works.
    template<typename... I>
    _cbasic_tvector(
            typename std::enable_if<std::is_convertible<I,T>::value && Size == sizeof...(I),T>::type... args)
    {
        fprintf(stderr,"Sizes: %i, %i",sizeof...(args),Size);
        for(size_t i=0;i<Size;i++)
            this[i] = 0;
    }
    T& operator[](size_t i)
    {
        return d[i];
    }
    const T& operator[](size_t i) const
    {
        return d[i];
    }
    bool operator==(
            const _cbasic_tvector<T,Size>& v)
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
    _cbasic_tvector<T,Size> operator*(
            typename std::enable_if<std::is_convertible<I,T>::value>::type& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]*v;
        return vnew;
    }
    template<typename I>
    _cbasic_tvector<T,Size> operator/(
            typename std::enable_if<std::is_convertible<I,T>::value>::type& v) const
    {
        _cbasic_tvector<T,Size> vnew;
        for(size_t i=0;i<Size;i++)
            vnew[i] = (*this)[i]/v;
        return vnew;
    }
    template<typename I>
    void operator*(
            typename std::enable_if<std::is_convertible<I,T>::value>::type& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] *= v;
    }
    template<typename I>
    void operator/(
            typename std::enable_if<std::is_convertible<I,T>::value>::type& v)
    {
        for(size_t i=0;i<Size;i++)
            (*this)[i] /= v;
    }

    T d[Size];
};

template<typename T,size_t Size> T length(
        const _cbasic_tvector<T,Size>& v)
{
    T sum = 0;
    for(int i=0;i<Size;i++)
        sum += pow(v.d[i],(T)2);
    return sqrt(sum);
}

template<typename T,size_t Size> T dot(
        const _cbasic_tvector<T,Size>& v1,
        const _cbasic_tvector<T,Size>& v2)
{
    T sum = 0;
    for(int i=0;i<Size;i++)
        sum += v1.d[i]*v2.d[i];
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

/*!
 * \brief A vector2 type used for storage of texture coordinates and screen positions. Compatible with glm::vec2. Has XY, ST and UV members.
 */
template<typename T> struct _cbasic_vec2
{
    _cbasic_vec2(){}
    _cbasic_vec2(T x,T y){
        this->x = x;
        this->y = y;
    }
    union{
        T x;
        T s;
        T u;
    };
    union{
        T y;
        T t;
        T v;
    };
};

/*!
 * \brief A vector2 type used for storage of positions, normals and tangents. Compatible with glm::vec3. Has XYZ, RGB and UVW members
 */
template<typename T> struct _cbasic_vec3
{
    _cbasic_vec3(T x,T y,T z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
    _cbasic_vec3(){}
    union{
        T x;
        T r;
        T s;
        T u;
    };
    union{
        T y;
        T g;
        T t;
        T v;
    };
    union{
        T z;
        T b;
        T p;
        T w;
    };
};
/*!
 * \brief A vector4 type used for storage of colors and positions. Compatible with glm::vec4. Has RGBA and XYZW members.
 */
template<typename T> struct _cbasic_vec4
{
    _cbasic_vec4(T x,T y,T z,T w){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    _cbasic_vec4(){}
    union{
        T x;
        T r;
    };
    union{
        T y;
        T g;
    };
    union{
        T z;
        T b;
    };
    union{
        T w;
        T a;
    };
};
/*!
 * \brief A quaternion type used for storage of orientation. Compatible with glm::quat.
 */
template<typename T> struct _cbasic_quat{
    _cbasic_quat(T w,T x,T y,T z){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    _cbasic_quat(){}
    T x;
    T y;
    T z;
    T w;
};

/*!
 * \brief A base for all MxN matrix types
 */
template<typename T, size_t ms, size_t ns> struct _cbasic_matrix_nm{
    _cbasic_matrix_nm()
    {
        for(int x=0;x<ms;x++)
            for(int y=0;y<ns;y++)
                m[x][y] = 1;
    }
    _cbasic_matrix_nm(T v)
    {
        for(int x=0;x<ms;x++)
            for(int y=0;y<ns;y++)
                m[x][y] = v;
    }

    T m[ms][ns];
};

/*!
 * \brief A base for all NxN matrix types
 */
template<typename T, size_t ms> using _cbasic_matrix_n = _cbasic_matrix_nm<T,ms,ms>;

typedef _cbasic_vec2<scalar> CVec2;
typedef _cbasic_vec3<scalar> CVec3;
typedef _cbasic_vec4<scalar> CVec4;

typedef _cbasic_quat<scalar> CQuat;

typedef _cbasic_matrix_n<scalar,3> CMat3;
typedef _cbasic_matrix_n<scalar,4> CMat4;

}

#endif
