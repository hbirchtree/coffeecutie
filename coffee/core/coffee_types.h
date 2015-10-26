#ifndef COFFEE_TYPES
#define COFFEE_TYPES

#include <stdint.h>
#include <malloc.h>
#include <cstring>
#include <string>
#include <stdexcept>
#include <map>
#include <vector>

namespace Coffee{

//Core typedefs

//Integer types
typedef char                int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

typedef float               scalar; /*!< Low-precision float value*/
typedef double              bigscalar; /*!< High-precision float value*/
typedef int8                byte; /*!< System byte*/
typedef uint8               ubyte; /*!< System unsigned byte*/

typedef uint16              ushort;

typedef const byte*         cstring; /*!< Typical C-string*/
typedef byte*               cstring_w; /*!< Writable C-string*/
typedef const int16*        cwstring; /*!< Wide C-string*/
typedef int16*              cwstring_w; /*!< Wide, writable C-string*/

typedef uint64              szptr; /*!< Usable as offset into data*/

typedef uint32              csize_t; /*!< Typical size type*/

typedef std::string         CString; /*!< Typical string object */
typedef std::runtime_error  CStdFault; /*!< Exception to be thrown by default */

/*!
 * \brief Primarily created to compare tag classes, it compares part of memory
 * \param target Typically a non-null-terminated string
 * \param cmp Typically what is compared to
 * \param len The length of the comparison
 * \return
 */
inline static bool coffee_cmp_memarea(cstring target, cstring cmp, szptr len)
{
    return memcmp(target,cmp,len)==0;
}

//Different data types we implement
namespace CPrimitiveDataTypes{

/*!
 * \brief Convenience function for copying strings
 * \param str
 * \return
 */
inline static cstring coffee_cpy_string(cstring str){
    cstring_w buf = reinterpret_cast<cstring_w>(malloc(strlen(str)+1));
    strcpy(buf,str);
    return buf;
}

struct CRGBA
{
    union{
        uint32 i = 0;
        struct
        {
            uint8 r,g,b,a;
        } c;
    };
};

/*!
 * \brief Convenience function for clearing memory
 * \param start
 * \param size
 */
inline static void coffee_mem_clear(void* start, szptr size){
    memset(start,0,size);
}

template<typename T>
/*!
 * \brief For storing arrays
 */
struct chunk_mem
{
    szptr   size    = 0;
    T*      d   = nullptr;
};

template<typename T>
/*!
 * \brief Expands an array contained in chunk_mem<T> types
 * \param mem
 * \param size
 */
static void coffee_mem_expand_array(chunk_mem<T> *mem, szptr size){
    szptr osize = mem->size;
    mem->size += size;
    mem->d = (T*)realloc(mem->d,sizeof(T)*mem->size);
    coffee_mem_clear(&mem->d[osize],sizeof(T)*(mem->size-osize));
}

template<typename T>
/*!
 * \brief A template for version structures
 */
struct _cbasic_version
{
    T major     = 0;
    T minor     = 0;
    T revision  = 0;
};

template<typename T>
/*!
 * \brief A template for sizes
 */
struct _cbasic_size
{
    _cbasic_size(){}
    _cbasic_size(T wd,T hg) : w(wd),h(hg){}
    T w = 0;
    T h = 0;
};

/*!
 * \brief Typical size, uses integer, should be used for window size
 */
struct CSize : public  _cbasic_size<int32>
{
    CSize(){}
    CSize(int32 wid,int32 hgh) : _cbasic_size<int32>(wid,hgh){}
};
/*!
 * \brief Size for inaccurate measurements
 */
typedef _cbasic_size<scalar> CSizeF;
/*!
 * \brief Size of accurate measurements
 */
typedef _cbasic_size<bigscalar> CSizeD;

template<typename T>
/*!
 * \brief A template for point types, x and y values
 */
struct _cbasic_point
{
    _cbasic_point(T x,T y){
        this->x = x;
        this->y = y;
    }
    _cbasic_point(){}
    T x = 0;
    T y = 0;
};

typedef _cbasic_point<int32> CPoint;
typedef _cbasic_point<scalar> CPointF;
typedef _cbasic_point<bigscalar> CPointD;

template<typename T>
/*!
 * \brief Template for rectangle types, used for window rectangle positioning and orthographic view
 */
struct _cbasic_rect
{
    _cbasic_rect(T x,T y,T w,T h){
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    _cbasic_rect(){}
    T x = 0;
    T y = 0;
    T w = 0;
    T h = 0;
};

/*!
 * \brief Used for window rectangles
 */
typedef _cbasic_rect<int32> CRect;
/*!
 * \brief Used for orthographic view rectangle
 */
typedef _cbasic_rect<scalar> CRectF;
/*!
 * \brief Used for high-accuracy orthographic view rectangle
 */
typedef _cbasic_rect<bigscalar> CRectD;

/*!
 * \brief A vector2 type used for storage of texture coordinates and screen positions. Compatible with glm::vec2. Has XY, ST and UV members.
 */
struct CVec2{
    CVec2(scalar x,scalar y){
        this->x = x;
        this->y = y;
    }
    CVec2(){}
    union{
        scalar x;
        scalar s;
        scalar u;
    };
    union{
        scalar y;
        scalar t;
        scalar v;
    };
};
/*!
 * \brief A vector2 type used for storage of positions, normals and tangents. Compatible with glm::vec3. Has XYZ, RGB and UVW members
 */
struct CVec3{
    CVec3(scalar x,scalar y,scalar z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
    CVec3(){}
    union{
        scalar x;
        scalar r;
        scalar s;
        scalar u;
    };
    union{
        scalar y;
        scalar g;
        scalar t;
        scalar v;
    };
    union{
        scalar z;
        scalar b;
        scalar p;
        scalar w;
    };
};
/*!
 * \brief A vector4 type used for storage of colors and positions. Compatible with glm::vec4. Has RGBA and XYZW members.
 */
struct CVec4{
    CVec4(scalar x,scalar y,scalar z,scalar w){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    CVec4(){}
    union{
        scalar x = 0.f;
        scalar r;
    };
    union{
        scalar y = 0.f;
        scalar g;
    };
    union{
        scalar z = 0.f;
        scalar b;
    };
    union{
        scalar w = 0.f;
        scalar a;
    };
};
/*!
 * \brief A quaternion type used for storage of orientation. Compatible with glm::quat.
 */
struct CQuat{
    CQuat(scalar w,scalar x,scalar y,scalar z){
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
    CQuat(){}
    scalar x = 0.f;
    scalar y = 0.f;
    scalar z = 0.f;
    scalar w = 0.f;
};

/*!
 * \brief A matrix3 type used for storage of orientation, rotation and similar. Compatible with glm::mat3.
 */
struct CMat3{
    scalar m[3][3];
};
/*!
 * \brief A matrix4 type used for storage of transform. Compatible with glm::mat4.
 */
struct CMat4{
    scalar m[4][4];
};

}
}

using namespace Coffee::CPrimitiveDataTypes;

#endif
