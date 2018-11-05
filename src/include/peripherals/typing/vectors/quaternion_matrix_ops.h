#pragma once

#include "matrices.h"
#include "quaternion.h"

namespace typing {
namespace vectors {

template<typename T>
FORCEDINLINE tmatrix<T, 4> matrixify(const tquaternion<T>& q)
{
    tmatrix<T, 3> m3 = matrixify_mat3(q);

    tmatrix<T, 4> res;

    for(size_t i = 0; i < 3; i++)
        for(size_t j = 0; j < 3; j++)
            res[i][j] = m3[i][j];
    res[3][3] = 1.f;

    return res;
}

template<typename T>
FORCEDINLINE
tmatrix<T,3> matrixify_mat3(
        const tquaternion<T>& q)
{
    tmatrix<T,3> Result;

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

} // namespace Coffee
}
