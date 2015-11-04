#ifndef COFFEE_VECTOR_TYPES_H
#define COFFEE_VECTOR_TYPES_H

#include "coffee_basetypes.h"

namespace Coffee{

/*!
 * \brief A vector2 type used for storage of texture coordinates and screen positions. Compatible with glm::vec2. Has XY, ST and UV members.
 */
template<typename T> struct _cbasic_vec2{
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
template<typename T> struct _cbasic_vec3{
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
template<typename T> struct _cbasic_vec4{
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
 * \brief A base for all NxN matrix types
 */
template<typename T, int ns> struct _cbasic_matrix_n{
    _cbasic_matrix_n()
    {
        for(int x=0;x<ns;x++)
            for(int y=0;y<ns;y++)
                m[x][y] = 1;
    }
    _cbasic_matrix_n(T v)
    {
        for(int x=0;x<ns;x++)
            for(int y=0;y<ns;y++)
                m[x][y] = v;
    }

    T m[ns][ns];
};

/*!
 * \brief A base for all MxN matrix types
 */
template<typename T, int ms, int ns> struct _cbasic_matrix_nm{
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

typedef _cbasic_vec2<scalar> CVec2;
typedef _cbasic_vec3<scalar> CVec3;
typedef _cbasic_vec4<scalar> CVec4;

typedef _cbasic_quat<scalar> CQuat;

typedef _cbasic_matrix_n<scalar,3> CMat3;
typedef _cbasic_matrix_n<scalar,4> CMat4;

}

#endif
