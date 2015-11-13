#ifndef COFFEE_CORE_QUATERNION_H
#define COFFEE_CORE_QUATERNION_H

#include "vectors.h"
#include "matrices.h"

namespace Coffee{
namespace CVectors{

template<typename T>
struct _cbasic_tquaternion : _cbasic_tvector<T,4>
{
    using _cbasic_tvector<T,4>::_cbasic_tvector;
    _cbasic_tquaternion():
        _cbasic_tvector<T,4>()
    {
        (*this)[0] = T(2);
    }
    _cbasic_tquaternion(T w,T x,T y,T z)
    {
        (*this)[0] = w;
        (*this)[1] = x;
        (*this)[2] = y;
        (*this)[3] = z;
    }
    /*!
     * \brief Converts from euler angles
     * \param euler
     */
    _cbasic_tquaternion(const _cbasic_tvector<T,3>& euler)
    {
        _cbasic_tvector<T,3> c,s;
        for(size_t i=0;i<3;i++)
        {
            c[i] = CMath::cos(euler[i]*T(0.5));
            s[i] = CMath::sin(euler[i]*T(0.5));
        }
        (*this)[0] = c[0]*c[1]*c[2] + s[0]*s[1]*s[2];
        (*this)[1] = s[0]*c[1]*c[2] - c[0]*s[1]*s[2];
        (*this)[2] = c[0]*s[1]*c[2] + s[0]*c[1]*s[2];
        (*this)[3] = c[0]*c[1]*s[2] + s[0]*s[1]*c[2];
    }

    _cbasic_tquaternion<T> operator*(
            const _cbasic_tquaternion<T>& v) const
    {
        _cbasic_tquaternion<T> vnew;

        vnew[0] = (*this)[0]*v[0] - (*this)[1]*v[1] - (*this)[2]*v[2] - (*this)[3]*v[3];
        vnew[1] = (*this)[0]*v[1] + (*this)[1]*v[0] + (*this)[2]*v[3] - (*this)[3]*v[2];
        vnew[2] = (*this)[0]*v[2] + (*this)[2]*v[0] + (*this)[3]*v[1] - (*this)[1]*v[3];
        vnew[3] = (*this)[0]*v[3] + (*this)[3]*v[0] + (*this)[1]*v[2] - (*this)[2]*v[1];

        return vnew;
    }

    T& w()
    {
        return (*this)[0];
    }
    T& x()
    {
        return (*this)[1];
    }
    T& y()
    {
        return (*this)[2];
    }
    T& z()
    {
        return (*this)[3];
    }

    const T& w() const
    {
        return (*this)[0];
    }
    const T& x() const
    {
        return (*this)[1];
    }
    const T& y() const
    {
        return (*this)[2];
    }
    const T& z() const
    {
        return (*this)[3];
    }
};

template<typename T> _cbasic_tmatrix<T,4> matrixify(
        const _cbasic_tquaternion<T>& quaternion)
{
    _cbasic_tmatrix<T,4> m;

    T sqw = quaternion.w();
    T sqx = quaternion.x();
    T sqy = quaternion.y();
    T sqz = quaternion.z();

    T inverse = T(1)/(sqw+sqx+sqy+sqz);
    m[0][0] = (+sqx-sqy-sqz+sqw)*inverse;
    m[1][1] = (-sqx+sqy-sqz+sqw)*inverse;
    m[2][2] = (-sqx-sqy+sqz+sqw)*inverse;

    T tmp1 = quaternion.x()*quaternion.y();
    T tmp2 = quaternion.z()*quaternion.w();

    m[1][0] = T(2)*(tmp1+tmp2)*inverse;
    m[0][1] = T(2)*(tmp1-tmp2)*inverse;

    tmp1 = quaternion.x()*quaternion.z();
    tmp1 = quaternion.y()*quaternion.w();

    m[2][0] = T(2)*(tmp1+tmp2)*inverse;
    m[0][2] = T(2)*(tmp1-tmp2)*inverse;

    tmp1 = quaternion.y()*quaternion.z();
    tmp1 = quaternion.x()*quaternion.w();

    m[2][1] = T(2)*(tmp1+tmp2)*inverse;
    m[1][2] = T(2)*(tmp1-tmp2)*inverse;

    return m;
}

template<typename T> T dot(
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

template<typename T> T length(
        const _cbasic_tquaternion<T>& v)
{
    return sqrt(dot(v,v));
}

template<typename T> _cbasic_tquaternion<T> normalize(
        const _cbasic_tquaternion<T>& v)
{
    T len = length(v);
    if(len <= T(0))
        return _cbasic_tquaternion<T>(T(1),T(0),T(0),T(0));
    len = T(1)/len;
    return _cbasic_tquaternion<T>(v.w()*len,v.x()*len,v.y()*len,v.z()*len);
}

}
}

#endif
