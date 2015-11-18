#ifndef COFFEE_BASETYPES_H
#define COFFEE_BASETYPES_H

#define C_FLAGS(type,utype) \
    inline type operator&(type lhs, type rhs){return (type)(((utype)lhs)&((utype)rhs));} \
    inline type operator|(type lhs, type rhs){return (type)(((utype)lhs)|((utype)rhs));} \
    inline type operator~(type val){return (type)(~((utype)val));} \
    inline type operator&=(type lhs, type rhs){return (type)(((utype)lhs)&((utype)rhs));} \
    inline type operator|=(type lhs, type rhs){return (type)(((utype)lhs)|((utype)rhs));}

#undef far
#undef near

#include <string>
#include <stdexcept>

namespace Coffee{

template<typename T>
inline bool flag_eval(T flagcombo)
{
    return (int)flagcombo;
}

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
typedef int8                byte_t; /*!< System byte*/
typedef uint8               ubyte_t; /*!< System unsigned byte*/

typedef uint16              ushort;

typedef const byte_t*       cstring; /*!< Typical C-string*/
typedef byte_t*             cstring_w; /*!< Writable C-string*/
typedef const wchar_t*      cwstring; /*!< Wide C-string*/
typedef wchar_t*            cwstring_w; /*!< Wide, writable C-string*/

typedef size_t              szptr; /*!< Usable as offset into data*/

typedef std::string         CString; /*!< Typical string object */
typedef std::runtime_error  CStdFault; /*!< Exception to be thrown by default */

typedef const void* c_cptr;
typedef void*       c_ptr;

/*!
 * \brief A prototype for Coffee-managed main functions. Benefits to this is that Coffee will abstract away any platform-specific quirks to the main function, such as difference in arguments. (Eg. some platforms do not include the main executable while others do)
 */
typedef int32 (*CoffeeMainWithArgs)(int32,byte_t**);

}

#endif
