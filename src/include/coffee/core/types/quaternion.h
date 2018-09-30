#pragma once

#include <coffee/core/types/edef/logicenum.h>
#include "vectors.h"
#include "matrices.h"

namespace Coffee{

template<typename T>
struct _cbasic_tquaternion : _cbasic_tvector<T,4>
{
    using _cbasic_tvector<T,4>::_cbasic_tvector;

    FORCEDINLINE
    _cbasic_tquaternion():
        _cbasic_tvector<T,4>()
    {
        (*this)[3] = T(1);
    }
    FORCEDINLINE
    _cbasic_tquaternion(T w,T x,T y,T z)
    {
        (*this)[0] = x;
        (*this)[1] = y;
        (*this)[2] = z;
        (*this)[3] = w;
    }
    /*!
     * \brief Converts from euler angles
     * \param euler
     */
    FORCEDINLINE
    _cbasic_tquaternion(const _cbasic_tvector<T,3>& euler)
    {
        _cbasic_tvector<T,3> c,s;
        for(size_t i=0;i<3;i++)
        {
            c[i] += CMath::cos(euler[i]*T(0.5));
            s[i] += CMath::sin(euler[i]*T(0.5));
        }
        (*this)[0] = c[0]*c[1]*c[2] + s[0]*s[1]*s[2];
        (*this)[1] = s[0]*c[1]*c[2] - c[0]*s[1]*s[2];
        (*this)[2] = c[0]*s[1]*c[2] + s[0]*c[1]*s[2];
        (*this)[3] = c[0]*c[1]*s[2] + s[0]*s[1]*c[2];
    }

    FORCEDINLINE
    _cbasic_tquaternion<T> operator*(
            const _cbasic_tquaternion<T>& v) const
    {
        _cbasic_tquaternion<T> vnew;

        vnew.w() = this->w()*v.w() - this->x()*v.x() - this->y()*v.y() - this->z()*v.z();
        vnew.x() = this->w()*v.x() + this->x()*v.w() + this->y()*v.z() - this->z()*v.y();
        vnew.y() = this->w()*v.y() - this->x()*v.z() + this->y()*v.w() + this->z()*v.w();
        vnew.z() = this->w()*v.z() + this->x()*v.y() - this->y()*v.x() + this->z()*v.x();

        return vnew;
    }

    STATICINLINE
    _cbasic_tquaternion<T> from_euler(
            const _cbasic_tvector<T, 3>& euler)
    {
        return _cbasic_tquaternion<T>(euler);
    }

    FORCEDINLINE
    T& w()
    {
        return (*this)[3];
    }
    FORCEDINLINE
    T& x()
    {
        return (*this)[0];
    }
    FORCEDINLINE
    T& y()
    {
        return (*this)[1];
    }
    FORCEDINLINE
    T& z()
    {
        return (*this)[2];
    }

    FORCEDINLINE
    const T& w() const
    {
        return (*this)[3];
    }
    FORCEDINLINE
    const T& x() const
    {
        return (*this)[0];
    }
    FORCEDINLINE
    const T& y() const
    {
        return (*this)[1];
    }
    FORCEDINLINE
    const T& z() const
    {
        return (*this)[2];
    }
};

template<typename T>
FORCEDINLINE
_cbasic_tmatrix<T,3> matrixify_mat3(
        const _cbasic_tquaternion<T>& q)
{
    _cbasic_tmatrix<T,3> Result;

    T qxx(q.x() * q.x());
    T qyy(q.y() * q.y());
    T qzz(q.z() * q.z());
    T qxz(q.x() * q.z());
    T qxy(q.x() * q.y());
    T qyz(q.y() * q.z());
    T qwx(q.w() * q.x());
    T qwy(q.w() * q.y());
    T qwz(q.w() * q.z());

    Result[0][0] = 1 - 2 * (qyy +  qzz);
    Result[0][1] = 2 * (qxy + qwz);
    Result[0][2] = 2 * (qxz - qwy);

    Result[1][0] = 2 * (qxy - qwz);
    Result[1][1] = 1 - 2 * (qxx +  qzz);
    Result[1][2] = 2 * (qyz + qwx);

    Result[2][0] = 2 * (qxz + qwy);
    Result[2][1] = 2 * (qyz - qwx);
    Result[2][2] = 1 - 2 * (qxx +  qyy);

    return Result;
}

template<typename T>
FORCEDINLINE
_cbasic_tmatrix<T,4> matrixify(
        const _cbasic_tquaternion<T>& q)
{
    _cbasic_tmatrix<T,3> m3 = matrixify_mat3(q);

    _cbasic_tmatrix<T,4> res;

    for(size_t i=0;i<3;i++)
        for(size_t j=0;j<3;j++)
            res[i][j] = m3[i][j];
    res[3][3] = 1.f;

    return res;
}

template<typename T>
FORCEDINLINE
T dot(
        const _cbasic_tquaternion<T>& v1,
        const _cbasic_tquaternion<T>& v2)
{
    _cbasic_tvector<T,4> tmp;
    tmp[0] = v1.x()*v2.x();
    tmp[1] = v1.y()*v2.y();
    tmp[2] = v1.z()*v2.z();
    tmp[3] = v1.w()*v2.w();
    return (tmp[0]+tmp[1]) + (tmp[2]+tmp[3]);
}

template<typename T>
FORCEDINLINE
T length(
        const _cbasic_tquaternion<T>& v)
{
    return sqrt(dot(v,v));
}

template<typename T>
FORCEDINLINE
_cbasic_tquaternion<T> normalize_quat(
        const _cbasic_tquaternion<T>& v)
{
    T len = length(v);
    if(len <= T(0))
        return _cbasic_tquaternion<T>();
    len = T(1)/len;
    return _cbasic_tquaternion<T>(v.w()*len,v.x()*len,v.y()*len,v.z()*len);
}

template<CameraDirection Direction, typename T>
FORCEDINLINE
_cbasic_tvector<T, 3> quaternion_to_direction(
        _cbasic_tquaternion<T> const& q)
{
    const auto imm_mat = matrixify_mat3(q);
    auto comp = _cbasic_tvector<T, 3>();
    switch(Direction)
    {
    case CameraDirection::Forward:
        comp.z() = -1.f;
        break;
    case CameraDirection::Up:
        comp.y() = 1.f;
        break;
    case CameraDirection::Right:
        comp.x() = -1.f;
        break;
    }
    auto dir = imm_mat * comp;

    return dir;
}

template<typename T>
_cbasic_tquaternion<T> lock_axis(
        _cbasic_tquaternion<T> const& v, _cbasic_tvector<T, 3> const& axes)
{
    return v;
}

}
