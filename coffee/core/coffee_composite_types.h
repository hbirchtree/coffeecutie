#ifndef COFFEE_COMPOSITE_TYPES_H
#define COFFEE_COMPOSITE_TYPES_H

#include <cmath>
#include "coffee_basetypes.h"

namespace Coffee{

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
        } c;
    };
};

/*!
 * \brief Describes the clipping distance of a GL scene
 */
template<typename T>
struct _cbasic_zfield
{
    _cbasic_zfield()
        : near(0.1),
          far(10)
    {
    }

    T near;
    T far;
};

/*!
 * \brief A template for sizes
 */
template<typename T> struct _cbasic_size
{
    _cbasic_size(T wd,T hg) : w(wd),h(hg){}
    _cbasic_size(){}

    T w = 0;
    T h = 0;

    T area()
    {
        return w*h;
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

    T left()
    {
        return x;
    }
    T right()
    {
        return x+w;
    }
    T bottom()
    {
        return y;
    }
    T top()
    {
        return y+h;
    }

    T area()
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

/*!
 * \brief Typical size, uses integer, should be used for window size
 */
struct CSize : public  _cbasic_size<int32>
{
    using _cbasic_size::_cbasic_size;
};
/*!
 * \brief Size for inaccurate measurements
 */
typedef _cbasic_size<scalar> CSizeF;
/*!
 * \brief Size of accurate measurements
 */
typedef _cbasic_size<bigscalar> CSizeD;

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
}

#endif
