#pragma once

#include "../../coffee_mem_macros.h"
#include "../tdef/integertypes.h"
#include <coffee/core/CMath>

namespace Coffee{

/*!
 * \brief A template for sizes
 */
template<typename T>
struct _cbasic_size_2d
{
    FORCEDINLINE _cbasic_size_2d(T wd,T hg) : w(wd),h(hg){}
    FORCEDINLINE _cbasic_size_2d(): w(0),h(0)
    {}

    T w;
    T h;

    FORCEDINLINE T area() const
    {
        return w*h;
    }

    FORCEDINLINE _cbasic_size_2d<T> operator/(const T& d)
    {
        return _cbasic_size_2d<T>(this->w/d,this->h/d);
    }
    FORCEDINLINE _cbasic_size_2d<T> operator*(const T& d)
    {
        return _cbasic_size_2d<T>(this->w*d,this->h*d);
    }

    FORCEDINLINE _cbasic_size_2d<T>& operator/=(const T& d)
    {
        (*this) = (*this)/d;
        return *this;
    }
    FORCEDINLINE _cbasic_size_2d<T>& operator*=(const T& d)
    {
        (*this) = (*this)*d;
        return *this;
    }
};

template<typename T>
struct _cbasic_size_3d
{
    FORCEDINLINE _cbasic_size_3d(T w, T h, T d):
        width(w),height(h),depth(d)
    {
    }
    FORCEDINLINE _cbasic_size_3d():
        width(0),height(0),depth(0)
    {
    }

    T width,height,depth;

    FORCEDINLINE T volume()
    {
        return width*height*depth;
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
        return CMath::sqrt(CMath::pow(this->x-p.x,2)+CMath::pow(this->y-p.y,2));
    }
};

/*!
 * \brief Template for rectangle types, used for window rectangle positioning and orthographic view
 */
template<typename T> struct _cbasic_rect
{
    FORCEDINLINE _cbasic_rect(T x,T y,T w,T h){
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    FORCEDINLINE _cbasic_rect(){}

    T x = 0;
    T y = 0;
    T w = 0;
    T h = 0;

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

    FORCEDINLINE _cbasic_point<T> topleft() const
    {
	return _cbasic_point<T>(left(),top());
    }
    FORCEDINLINE _cbasic_point<T> topright() const
    {
	return _cbasic_point<T>(right(),top());
    }
    FORCEDINLINE _cbasic_point<T> bottomleft() const
    {
	return _cbasic_point<T>(left(),bottom());
    }
    FORCEDINLINE _cbasic_point<T> bottomright() const
    {
	return _cbasic_point<T>(right(),bottom());
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
    FORCEDINLINE bool test(const _cbasic_point<T>& point)
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
    FORCEDINLINE _cbasic_rect<T> offset(const _cbasic_point<T>& point)
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

template<typename T>
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
	x(x),
	y(y),
	z(z),
	w(w),
	h(h),
	d(d)
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
template<typename T>
struct _cbasic_zfield
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

template<typename T>
struct _cbasic_fov
{
    _cbasic_fov():
        h(0),
        v(0)
    {
    }
    _cbasic_fov(T h, T v):
        h(h),
        v(v)
    {
    }

    T h;
    T v;
};

/*!
 * \brief Typical size, uses integer, should be used for window size
 */
using CSize = _cbasic_size_2d<int32>;
/*!
 * \brief Heavy-duty size structure
 */
using CSize64 = _cbasic_size_2d<uint64>;
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

typedef _cbasic_rect<int64> CRect64;
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
typedef _cbasic_zfield<scalar> ZField;

typedef _cbasic_zfield<bigscalar> ZField64;

typedef _cbasic_fov<scalar> FovDetail;

typedef _cbasic_boundingbox<scalar> BoundBox;

}
