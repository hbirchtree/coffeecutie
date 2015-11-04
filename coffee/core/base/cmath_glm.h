#ifndef CMATH_GLM_H
#define CMATH_GLM_H

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#include "coffee/core/coffee_vector_types.h"

namespace Coffee{
namespace CMath{
using namespace glm;

/*!
 * \brief A redefinition of the vec2 structure from glm, with added interop for Coffee types
 */
template<typename T> struct tvec2 : public glm::tvec2<T>
{
public:
    using glm::tvec2<T>::tvec2;

    tvec2* operator=(_cbasic_vec2<T>* vec)
    {
        return (tvec2*)vec;
    }

    const tvec2* operator=(const _cbasic_vec2<T>* vec)
    {
        return (tvec2*)vec;
    }

    tvec2& operator=(const _cbasic_vec2<T>& vec)
    {
        this->x = vec.x;
        this->y = vec.y;
        return *this;
    }
};

/*!
 * \brief A redefinition of the vec3 structure from glm, with added interop for Coffee types
 */
template<typename T> struct tvec3 : public glm::tvec3<T>
{
public:
    using glm::tvec3<T>::tvec3;

    tvec3* operator=(_cbasic_vec3<T>* vec)
    {
        return (tvec3*)vec;
    }

    const tvec3* operator=(const _cbasic_vec3<T>* vec)
    {
        return (tvec3*)vec;
    }

    tvec3& operator=(_cbasic_vec3<T>& vec)
    {
        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;
        return *this;
    }
};

/*!
 * \brief A redefinition of the vec4 structure from glm, with added interop for Coffee types
 */
template<typename T> struct tvec4 : public glm::tvec4<T>
{
public:
    using glm::tvec4<T>::tvec4;

    tvec4* operator=(_cbasic_vec4<T>* vec)
    {
        return (tvec4*)vec;
    }

    const tvec4* operator=(const _cbasic_vec4<T>* vec)
    {
        return (tvec4*)vec;
    }

    tvec4& operator=(_cbasic_vec4<T>& vec)
    {
        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;
        this->w = vec.w;
        return *this;
    }
};

/*!
 * \brief A redefinition of the quat structure from glm, with added interop for Coffee types
 */
template<typename T> struct tquat : public glm::tquat<T>
{
public:
    using glm::tquat<T>::tquat;

    tquat* operator=(_cbasic_quat<T>* vec)
    {
        return (tquat*)vec;
    }

    const tquat* operator=(const _cbasic_quat<T>* vec)
    {
        return (tquat*)vec;
    }

    tquat& operator=(_cbasic_quat<T>& vec)
    {
        this->w = vec.w;
        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;
        return *this;
    }
};

typedef tvec2<scalar> vec2;
typedef tvec3<scalar> vec3;
typedef tvec4<scalar> vec4;
typedef tquat<scalar> quat;

}
}

#endif
