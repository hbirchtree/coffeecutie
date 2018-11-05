#pragma once

#include <peripherals/typing/enum/graphics/direction.h>
#include "vectors.h"

namespace typing {
namespace vectors {

template<typename T>
struct tquaternion : tvector<T,4>
{
    using tvector<T,4>::tvector;

    FORCEDINLINE
    tquaternion():
        tvector<T,4>()
    {
        (*this)[3] = T(1);
    }
    FORCEDINLINE
    tquaternion(T w,T x,T y,T z)
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
    tquaternion(const tvector<T,3>& euler)
    {
        tvector<T,3> c,s;
        for(size_t i=0;i<3;i++)
        {
            c[i] += cos(euler[i]*T(0.5));
            s[i] += sin(euler[i]*T(0.5));
        }
        (*this)[0] = c[0]*c[1]*c[2] + s[0]*s[1]*s[2];
        (*this)[1] = s[0]*c[1]*c[2] - c[0]*s[1]*s[2];
        (*this)[2] = c[0]*s[1]*c[2] + s[0]*c[1]*s[2];
        (*this)[3] = c[0]*c[1]*s[2] + s[0]*s[1]*c[2];
    }

    FORCEDINLINE
    tquaternion<T> operator*(
            const tquaternion<T>& v) const
    {
        tquaternion<T> vnew;

        vnew.w() = this->w()*v.w() - this->x()*v.x() - this->y()*v.y() - this->z()*v.z();
        vnew.x() = this->w()*v.x() + this->x()*v.w() + this->y()*v.z() - this->z()*v.y();
        vnew.y() = this->w()*v.y() - this->x()*v.z() + this->y()*v.w() + this->z()*v.w();
        vnew.z() = this->w()*v.z() + this->x()*v.y() - this->y()*v.x() + this->z()*v.x();

        return vnew;
    }

    STATICINLINE
    tquaternion<T> from_euler(
            const tvector<T, 3>& euler)
    {
        return tquaternion<T>(euler);
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
T dot(
        const tquaternion<T>& v1,
        const tquaternion<T>& v2)
{
    tvector<T,4> tmp;
    tmp[0] = v1.x()*v2.x();
    tmp[1] = v1.y()*v2.y();
    tmp[2] = v1.z()*v2.z();
    tmp[3] = v1.w()*v2.w();
    return (tmp[0]+tmp[1]) + (tmp[2]+tmp[3]);
}

template<typename T>
FORCEDINLINE
T length(
        const tquaternion<T>& v)
{
    return sqrt(dot(v,v));
}

template<typename T>
FORCEDINLINE
tquaternion<T> normalize_quat(
        const tquaternion<T>& v)
{
    T len = length(v);
    if(len <= T(0))
        return tquaternion<T>();
    len = T(1)/len;
    return tquaternion<T>(v.w()*len,v.x()*len,v.y()*len,v.z()*len);
}

template<CameraDirection Direction, typename T>
FORCEDINLINE
tvector<T, 3> quaternion_to_direction(
        tquaternion<T> const& q)
{
    const auto imm_mat = matrixify_mat3(q);
    auto comp = tvector<T, 3>();
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
tquaternion<T> lock_axis(
        tquaternion<T> const& v, tvector<T, 3> const& axes)
{
    return v;
}

}
}
