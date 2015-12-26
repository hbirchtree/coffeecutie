#ifndef COFFEE_VECTOR_TYPES_H
#define COFFEE_VECTOR_TYPES_H

#include <type_traits>
#include <cmath>
#include "vectors.h"
#include "matrices.h"
#include "quaternion.h"

namespace Coffee{

template<typename T>
struct _cbasic_vec2 : _cbasic_tvector<T,2>
{
    _cbasic_vec2():
        _cbasic_tvector<T,2>::_cbasic_tvector()
    {
    }

    using _cbasic_tvector<T,2>::_cbasic_tvector;
    _cbasic_vec2(T x,T y)
    {
        this->x() = x;
        this->y() = y;
    }

    T& x()
    {
        return (*this)[0];
    }
    T& y()
    {
        return (*this)[1];
    }

    T& u()
    {
        return (*this)[0];
    }
    T& v()
    {
        return (*this)[1];
    }
};

template<typename T>
struct _cbasic_vec3 : _cbasic_tvector<T,3>
{
    _cbasic_vec3():
        _cbasic_tvector<T,3>::_cbasic_tvector()
    {
    }
    using _cbasic_tvector<T,3>::_cbasic_tvector;
    _cbasic_vec3(T x,T y,T z)
    {
        this->x() = x;
        this->y() = y;
        this->z() = z;
    }

    T& x()
    {
        return (*this)[0];
    }
    T& y()
    {
        return (*this)[1];
    }
    T& z()
    {
        return (*this)[2];
    }

    const T& x()const
    {
        return (*this)[0];
    }
    const T& y()const
    {
        return (*this)[1];
    }
    const T& z()const
    {
        return (*this)[2];
    }

    T& r()
    {
        return (*this)[0];
    }
    T& g()
    {
        return (*this)[1];
    }
    T& b()
    {
        return (*this)[2];
    }
};

template<typename T>
struct _cbasic_vec4 : _cbasic_tvector<T,4>
{
    _cbasic_vec4():
        _cbasic_tvector<T,4>::_cbasic_tvector()
    {
    }
    using _cbasic_tvector<T,4>::_cbasic_tvector;
    _cbasic_vec4(T x,T y,T z,T w)
    {
        this->x() = x;
        this->y() = y;
        this->z() = z;
        this->w() = w;
    }

    T& x()
    {
        return (*this)[0];
    }
    T& y()
    {
        return (*this)[1];
    }
    T& z()
    {
        return (*this)[2];
    }
    T& w()
    {
        return (*this)[3];
    }

    T& r()
    {
        return (*this)[0];
    }
    T& g()
    {
        return (*this)[1];
    }
    T& b()
    {
        return (*this)[2];
    }
    T& a()
    {
        return (*this)[3];
    }

    const T& r() const
    {
        return (*this)[0];
    }
    const T& g() const
    {
        return (*this)[1];
    }
    const T& b() const
    {
        return (*this)[2];
    }
    const T& a() const
    {
        return (*this)[3];
    }
};

template<typename T> using _cbasic_quat = _cbasic_tquaternion<T>;

template<typename T,size_t Size> using _cbasic_matrix_n = _cbasic_tmatrix<T,Size>;

typedef _cbasic_vec2<scalar> CVec2;
typedef _cbasic_vec3<scalar> CVec3;
typedef _cbasic_vec4<scalar> CVec4;

typedef _cbasic_quat<scalar> CQuat;

typedef _cbasic_matrix_n<scalar,3> CMat3;
typedef _cbasic_matrix_n<scalar,4> CMat4;

}

#endif
