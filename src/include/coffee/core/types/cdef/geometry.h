#pragma once

#if !defined(SWIG_PARSER)
#include "../../coffee_mem_macros.h"
#include "../tdef/integertypes.h"
#include <coffee/core/CMath>
#include <coffee/core/types/vectors.h>
#endif

namespace Coffee{

struct _cbasic_size
{
    constexpr _cbasic_size(){}
    constexpr _cbasic_size(i32,i32){}
    constexpr _cbasic_size(i32,i32,i32){}
};

struct _cbasic_point
{
    _cbasic_point(){}
    _cbasic_point(i32,i32){}
    _cbasic_point(i32,i32,i32){}
};

/*!
 * \brief A template for sizes
 */
template<typename T,

         typename is_pod<T>::type* = nullptr

         >
struct _cbasic_size_2d : _cbasic_size
{
    template<typename U>
    operator _cbasic_size_2d<U>() const
    {
        return this->convert<U>();
    }

    _cbasic_size_2d(_cbasic_tvector<T, 2> const& src):
        w(src.x()),
        h(src.y())
    {
    }
    constexpr _cbasic_size_2d(T wd,T hg)
        : w(wd),h(hg)
    {
    }
    constexpr _cbasic_size_2d()
        : w(0), h(0)
    {
    }

    T w;
    T h;

    T area() const
    {
        return w*h;
    }

    template<typename U>
    inline _cbasic_size_2d<U> convert() const
    {
        return _cbasic_size_2d<U>(w,h);
    }

    FORCEDINLINE _cbasic_size_2d<T> operator/(const T &d)
    {
        return _cbasic_size_2d<T>(this->w/d,this->h/d);
    }
    FORCEDINLINE _cbasic_size_2d<T> operator*(const T &d)
    {
        return _cbasic_size_2d<T>(this->w*d,this->h*d);
    }


    FORCEDINLINE _cbasic_size_2d<T>& operator/=(const T &d)
    {
        (*this) = (*this)/d;
        return *this;
    }
    FORCEDINLINE _cbasic_size_2d<T>& operator*=(const T &d)
    {
        (*this) = (*this)*d;
        return *this;
    }

    inline T operator[](size_t i) const
    {
        switch(i)
        {
        case 0:
            return w;
        case 1:
            return h;
        default:
            return 0;
        }
    }

    inline T& operator[](size_t i)
    {
        switch(i)
        {
        case 0:
            return w;
        case 1:
            return h;
        }

        return w;
    }
};

template<typename T,

         typename is_pod<T>::type* = nullptr

         >
struct _cbasic_size_3d : _cbasic_size
{
    _cbasic_size_3d(_cbasic_tvector<T, 3> const& src):
        width(src.x()),
        height(src.y()),
        depth(src.z())
    {
    }
    _cbasic_size_3d(T w, T h, T d):
        width(w),height(h),depth(d)
    {
    }
    _cbasic_size_3d():
        width(0),height(0),depth(0)
    {
    }

    template<typename U>
    inline _cbasic_size_3d<U> convert() const
    {
        return _cbasic_size_3d<U>(width, height, depth);
    }

    T width,height,depth;

    T volume() const
    {
        return width*height*depth;
    }

    inline T operator[](size_t i) const
    {
        switch(i)
        {
        case 0:
            return width;
        case 1:
            return height;
        case 2:
            return depth;
        default:
            return 0;
        }
    }

    inline T& operator[](size_t i)
    {
        switch(i)
        {
        case 0:
            return width;
        case 1:
            return height;
        case 2:
            return depth;
        }

        return width;
    }
};

/*!
 * \brief A template for point types, x and y values
 */
template<typename T,

         typename is_pod<T>::type* = nullptr

         >
struct _cbasic_point_2d : _cbasic_point
{
    _cbasic_point_2d(_cbasic_tvector<T, 2> const& src):
        x(src.x()),
        y(src.y())
    {
    }
    _cbasic_point_2d(T x,T y):
        x(x), y(y)
    {
    }
    _cbasic_point_2d(){}

    T x,y;

    FORCEDINLINE T distance(const _cbasic_point_2d<T> &p)
    {
        return CMath::sqrt(
                    CMath::pow(this->x-p.x,2)
                    + CMath::pow(this->y-p.y,2)
                    );
    }

    template<typename T2>
    _cbasic_point_2d<T2> convert() const
    {
        _cbasic_point_2d<T2> out;
        out.x = x;
        out.y = y;
        return out;
    }

    template<typename T2>
    _cbasic_tvector<T2, 2> toVector() const
    {
        return {x, y};
    }
};

template<typename T,

         typename is_pod<T>::type* = nullptr

         >
struct _cbasic_point_3d : _cbasic_point
{
    _cbasic_point_3d(_cbasic_tvector<T, 3> const& src):
        x(src.x()),
        y(src.y()),
        z(src.z())
    {
    }
    _cbasic_point_3d(T x, T y, T z):
        x(x),y(y),z(z)
    {
    }
    _cbasic_point_3d():
        x(0),y(0),z(0)
    {
    }

    template<typename U>
    _cbasic_point_3d<U> convert() const
    {
        return _cbasic_point_3d<U>(x,y,z);
    }

    template<typename T2>
    _cbasic_tvector<T2, 3> toVector() const
    {
        return {x, y, z};
    }

    T x,y,z;
};

/*!
 * \brief Template for rectangle types, used for window rectangle positioning and orthographic view
 */
template<typename T,

         typename is_pod<T>::type* = nullptr

         >
struct _cbasic_rect
{
    _cbasic_rect(T x,T y,T w,T h){
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    _cbasic_rect():
        x(0),y(0),w(0),h(0){}

    _cbasic_rect(_cbasic_size_2d<T> const& s)
        : _cbasic_rect(0, 0, s.w, s.h)
    {}

    union{
        struct{
            T x;
            T y;
            T w;
            T h;
        };
        T data[4];
    };

    template<typename T2>
    _cbasic_rect<T2> convert() const
    {
        _cbasic_rect<T2> out;
        out.x = x;
        out.y = y;
        out.w = w;
        out.h = h;
        return out;
    }

    FORCEDINLINE T left() const
    {
        return x;
    }
    FORCEDINLINE T right() const
    {
        return x+w;
    }
    FORCEDINLINE T top() const
    {
        return y+h;
    }
    FORCEDINLINE T bottom() const
    {
        return y;
    }

    FORCEDINLINE _cbasic_point_2d<T> topleft() const
    {
        return _cbasic_point_2d<T>(left(),top());
    }
    FORCEDINLINE _cbasic_point_2d<T> topright() const
    {
        return _cbasic_point_2d<T>(right(),top());
    }
    FORCEDINLINE _cbasic_point_2d<T> bottomleft() const
    {
        return _cbasic_point_2d<T>(left(),bottom());
    }
    FORCEDINLINE _cbasic_point_2d<T> bottomright() const
    {
        return _cbasic_point_2d<T>(right(),bottom());
    }

    FORCEDINLINE T area() const
    {
        return w*h;
    }
    FORCEDINLINE _cbasic_rect<T> intersection(const _cbasic_rect<T>& rekt)

    {
        if(!( rekt.left() < this->right() &&
              rekt.right() > this->left() &&
              rekt.top() < this->bottom() &&
              rekt.bottom() > this->top() ))
            return _cbasic_rect<T>();

        return _cbasic_rect<T>(
                    CMath::max(rekt.x,this->x),
                    CMath::max(rekt.y,this->y),
                    CMath::min(rekt.w,this->w),
                    CMath::min(rekt.h,this->h));
    }
    FORCEDINLINE bool test(const _cbasic_point_2d<T>& point)
    {
        if(point.x>this->left()  && point.x<this->right() &&
                point.y>this->bottom() && point.y<this->top())
            return true;
        return true;
    }
    FORCEDINLINE _cbasic_rect<T> unite(const _cbasic_rect<T>& rekt)

    {
        return _cbasic_rect<T>(
                    CMath::min(rekt.x,this->x),
                    CMath::min(rekt.y,this->y),
                    CMath::max(rekt.w,this->w),
                    CMath::max(rekt.h,this->h));
    }
    FORCEDINLINE _cbasic_rect<T> offset(const _cbasic_point_2d<T>& point)
    {
        return _cbasic_rect<T>(this->x+point.x,
                               this->y+point.y,
                               this->w,this->h);
    }
    FORCEDINLINE _cbasic_size_2d<T> size() const
    {
        return _cbasic_size_2d<T>(w,h);
    }
};

template<typename T,

         typename is_pod<T>::type* = nullptr

         >
struct _cbasic_boundingbox
{
    _cbasic_boundingbox(T w,T h, T d, bool centered = false)
    {
        if(centered)
        {
            this->w = w/T(2);
            this->h = h/T(2);
            this->d = d/T(2);

            this->x = -w/T(2);
            this->y = -h/T(2);
            this->z = -d/T(2);
        }else{
            this->w = w;
            this->h = h;
            this->d = d;
        }
    }
    _cbasic_boundingbox(T x, T y, T z, T w, T h, T d):
        x(x), y(y), z(z),
        w(w), h(h), d(d)
    {
    }

    T x,y,z;
    T w,h,d;

    T x1()const{return x;}
    T x2()const{return x+w;}
    T y1()const{return y;}
    T y2()const{return y+h;}
    T z1()const{return z;}
    T z2()const{return z+d;}
};

/*!
 * \brief Describes the clipping distance of a GL scene
 */
template<typename T,

         typename is_pod<T>::type* = nullptr

         >
struct _cbasic_range
{
    _cbasic_range()
        : near_(0.1),
          far_(10)
    {
    }
    _cbasic_range(T vnear, T vfar)
        : near_(vnear),
          far_(vfar)
    {
    }

    //Why near_ and far_?
    //Because Windows is a piece of shit. :)

    T near_;
    T far_;
};

template<typename T,

         typename is_pod<T>::type* = nullptr

         >
struct _cbasic_fov
{
    _cbasic_fov():
        h(0), v(0)
    {
    }
    _cbasic_fov(T h, T v):
        h(h), v(v)
    {
    }

    T h, v;
};

using CSizeT = _cbasic_size;
using PointT = _cbasic_point;

/*!
 * \brief Typical size, uses integer, should be used for window size
 */
using CSize = _cbasic_size_2d<i32>;

using Size = CSize;

using CSize3 = _cbasic_size_3d<i32>;

using Size3 = CSize3;
/*!
 * \brief Heavy-duty size structure
 */
using CSize64 = _cbasic_size_2d<u64>;
/*!
 * \brief Size for inaccurate measurements
 */
using CSizeF = _cbasic_size_2d<scalar>;
/*!
 * \brief Size of accurate measurements
 */
using CSizeD = _cbasic_size_2d<bigscalar>;

/*!
 * \brief Used for window rectangles
 */
using CRect = _cbasic_rect<i32>;

using CRect64 = _cbasic_rect<i64>;
/*!
 * \brief Used for orthographic view rectangle
 */
using CRectF = _cbasic_rect<scalar>;
/*!
 * \brief Used for high-accuracy orthographic view rectangle
 */
using CRectD = _cbasic_rect<bigscalar>;

/*!
 * \brief 32-bit integer point
 */
using CPoint = _cbasic_point_2d<i32>;

using CPoint3 =  _cbasic_point_3d<i32>;
/*!
 * \brief Single-precision floating-point point
 */
using CPointF = _cbasic_point_2d<scalar>;
/*!
 * \brief Double-precision floating-point point
 */
using CPointD = _cbasic_point_2d<bigscalar>;

/*!
 * \brief Standard clipping type
 */
using ZField = _cbasic_range<scalar>;

using ZField64 = _cbasic_range<bigscalar>;

using FovDetail = _cbasic_fov<scalar>;

using BoundBox = _cbasic_boundingbox<scalar>;

using PtF = CPointF;
using PtD = CPointD;
using PtI = CPoint;

using Point = CPoint;
using Point3 = CPoint3;

}
