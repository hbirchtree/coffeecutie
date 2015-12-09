#ifndef COFFEE_COMPOSITE_TYPES_H
#define COFFEE_COMPOSITE_TYPES_H

#include <cmath>
#include "basetypes.h"
#include "map.h"
#include "translatable_string.h"

namespace Coffee{

template<typename T> class _cbasic_timer
{
public:
    virtual ~_cbasic_timer(){}
    virtual void start() = 0;
    virtual T elapsed() = 0;
};

/*!
 * \brief Used for storing typical 8-bit color with alpha
 */
struct CRGBA
{
    union{
        uint32 i = 0;
        struct
        {
            uint8 r,g,b,a;
        };
    };
};

/*!
 * \brief Describes the clipping distance of a GL scene
 */
template<typename T> struct _cbasic_zfield
{
    _cbasic_zfield()
        : near(0.1),
          far(10)
    {
    }
    _cbasic_zfield(T vnear, T vfar)
        : near(vnear),
          far(vfar)
    {
    }

    //Why fnear and ffar?
    //Because Windows is a piece of shit. :)

    T near;
    T far;
};

/*!
 * \brief A template for sizes
 */
template<typename T> struct _cbasic_size_2d
{
    _cbasic_size_2d(T wd,T hg) : w(wd),h(hg){}
    _cbasic_size_2d(): w(0),h(0)
    {}

    T w;
    T h;

    T area()
    {
        return w*h;
    }
};

template<typename T> struct _cbasic_size_3d
{
    _cbasic_size_3d(T w, T h, T d):
        w(w),h(h),d(d)
    {
    }
    _cbasic_size_3d():
        w(0),h(0),d(0)
    {
    }

    T w,h,d;

    T volume()
    {
        return w*h*d;
    }
};

/*!
 * \brief A template for point types, x and y values
 */
template<typename T> struct _cbasic_point
{
    _cbasic_point(T x,T y){
        this->x = x;
        this->y = y;
    }
    _cbasic_point(){}

    T x = 0;
    T y = 0;

    T distance(const _cbasic_point& p)
    {
        return sqrt(pow(this->x-p.x,2)+pow(this->y-p.y,2));
    }
};

/*!
 * \brief Allows for buffering of any type of object, texture, buffer, etc. according to conditions.
 */
template<typename T, size_t BufferSize>
struct _cbasic_nbuffer
{
    _cbasic_nbuffer()
    {
        ptr = 0;
    }

    static constexpr size_t size = BufferSize;
    T data[BufferSize];
    size_t ptr;

    virtual void advance()
    {
        ptr = next_idx();
    }
    T& current()
    {
        return data[ptr];
    }
    T& next()
    {
        return data[next_idx()];
    }


    virtual size_t next_idx()
    {
        return (ptr+1)%BufferSize;
    }
};

/*!
 * \brief Template for rectangle types, used for window rectangle positioning and orthographic view
 */
template<typename T> struct _cbasic_rect
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

    T left() const
    {
        return x;
    }
    T right() const
    {
        return x+w;
    }
    T bottom() const
    {
        return y;
    }
    T top() const
    {
        return y+h;
    }

    T area() const
    {
        return w*h;
    }
    _cbasic_rect<T>& intersection(const _cbasic_rect<T>& rekt)
    {
        if(!( rekt.left() < this->right() &&
              rekt.right() > this->left() &&
              rekt.top() < this->bottom() &&
              rekt.bottom() > this->top() ))
            return _cbasic_rect<T>();

        return _cbasic_rect<T>(
                    std::max(rekt.x,this->x),
                    std::max(rekt.y,this->y),
                    std::min(rekt.w,this->w),
                    std::min(rekt.h,this->h));
    }
};

template<typename T>
/*!
 * \brief Allows for RAII-consistent objects. The destructor should be specific to each specialization such that it behaves properly.
 */
class _cbasic_raii_container
{
protected:
    T* m_data;
public:
    _cbasic_raii_container(T* d):
        m_data(d)
    {
    }
    virtual ~_cbasic_raii_container()
    {
    }

    T *data()
    {
        return m_data;
    }
};

/*!
 * \brief Typical size, uses integer, should be used for window size
 */
struct CSize : public  _cbasic_size_2d<int32>
{
    using _cbasic_size_2d::_cbasic_size_2d;
};
/*!
 * \brief Size for inaccurate measurements
 */
typedef _cbasic_size_2d<scalar> CSizeF;
/*!
 * \brief Size of accurate measurements
 */
typedef _cbasic_size_2d<bigscalar> CSizeD;

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
 * \brief 32-bit integer point
 */
typedef _cbasic_point<int32> CPoint;
/*!
 * \brief Single-precision floating-point point
 */
typedef _cbasic_point<scalar> CPointF;
/*!
 * \brief Double-precision floating-point point
 */
typedef _cbasic_point<bigscalar> CPointD;

/*!
 * \brief Standard clipping type
 */
typedef _cbasic_zfield<scalar> CZField;

typedef _cbasic_timer<uint64> CElapsedTimer;

typedef _cbasic_timer<uint64> CElapsedTimerMicro;

typedef _cbasic_timer<bigscalar> CElapsedTimerD;
}

#endif
