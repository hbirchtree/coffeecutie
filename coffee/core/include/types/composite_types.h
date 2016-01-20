#ifndef COFFEE_COMPOSITE_TYPES_H
#define COFFEE_COMPOSITE_TYPES_H

#include <coffee_macros.h>
#include <cmath>
#include "basetypes.h"
#include "map.h"
#include "translatable_string.h"

namespace Coffee{

struct C_FORCE_PACKING uint24
{
    uint32 d:24;
};

struct C_FORCE_PACKING bigscalar_dissect
{
    uint8 sign:1;
    uint16 exponent:11;
    uint64 mantissa:52;
};
struct C_FORCE_PACKING scalar_dissect
{
    uint8 sign:1;
    uint8 exponent:8;
    uint32 mantissa:24;
};
struct C_FORCE_PACKING half_dissect
{
    uint8 sign:1;
    uint8 exponent:5;
    uint16 mantissa:10;
};

struct C_FORCE_PACKING scalar11
{
    uint8 exponent:6;
    uint8 mantissa:5;
};
struct C_FORCE_PACKING scalar10
{
    uint8 exponent:5;
    uint8 mantissa:5;
};

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
    uint32 rgba() const
    {
#if defined(COFFEE_LIL_ENDIAN)
        CRGBA t;
        t.r = b;
        t.g = g;
        t.b = r;
        t.a = a;
        return t.i;
#elif defined(COFFEE_BIG_ENDIAN)
        return i;
#endif
    }
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

    _cbasic_size_2d<T> operator/(const T& d)
    {
        return _cbasic_size_2d<T>(this->w/d,this->h/d);
    }
    _cbasic_size_2d<T> operator*(const T& d)
    {
        return _cbasic_size_2d<T>(this->w*d,this->h*d);
    }

    _cbasic_size_2d<T>& operator/=(const T& d)
    {
        (*this) = (*this)/d;
    }
    _cbasic_size_2d<T>& operator*=(const T& d)
    {
        (*this) = (*this)*d;
    }
};

template<typename T> struct _cbasic_size_3d
{
    _cbasic_size_3d(T w, T h, T d):
        width(w),height(h),depth(d)
    {
    }
    _cbasic_size_3d():
        width(0),height(0),depth(0)
    {
    }

    T width,height,depth;

    T volume()
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
        return data[current_idx()];
    }
    T& next()
    {
        return data[next_idx()];
    }

    size_t next_idx()
    {
        return (ptr+1)%BufferSize;
    }
    size_t current_idx()
    {
        return ptr;
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
    T top() const
    {
        return y+h;
    }
    T bottom() const
    {
        return y;
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
    bool test(const _cbasic_point<T>& point)
    {
        if(point.x>this->left()  && point.x<this->right() &&
                point.y>this->bottom() && point.y<this->top())
            return true;
        return true;
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
    _cbasic_raii_container():
        m_data(new T)
    {
    }
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
    T& data_ref()
    {
        return *m_data;
    }
};

template<typename T>
/*!
 * \brief Locks access to a resource, unlocks on destruction.
 */
class _cbasic_cookie_container
{
    std::mutex& _access;
    T& _cookie;
public:
    _cbasic_cookie_container(std::mutex& acc, T& cookie):
        _access(acc),
        _cookie(cookie)
    {
        _access.lock();
    }
    ~_cbasic_cookie_container()
    {
        _access.unlock();
    }
    T& object()
    {
        return _cookie;
    }
};

template<typename PixelType,typename DimT>
struct _cbasic_bitmap_base
{
    _cbasic_bitmap_base(DimT w, DimT h):
        size(w,h)
    {
        m_pixels = (PixelType*)CCalloc(sizeof(PixelType),w*h);
    }
    ~_cbasic_bitmap_base()
    {
        CFree(m_pixels);
    }

    const _cbasic_size_2d<DimT> size;

    PixelType* data()
    {
        return m_pixels;
    }
    const PixelType* data() const
    {
        return m_pixels;
    }
private:
    PixelType* m_pixels;
};

using CBitmap = _cbasic_bitmap_base<CRGBA,int32>;

template<class ClassName,typename FReturnType,typename... FArgumentTypes>
struct CFunctionSlot
{
    using ClassType = ClassName;
    using FunctionReturn = FReturnType;
    using Function = FunctionReturn(ClassType::*)(FArgumentTypes...);
    using FunctionConst = FunctionReturn(ClassType::*)(FArgumentTypes...) const;

    CFunctionSlot(ClassType* c, Function f):
        instance(c),n(f)
    {
    }
    CFunctionSlot(ClassType* c, FunctionConst f):
        instance(c),c(f)
    {
    }

    FunctionReturn call(FArgumentTypes... arg)
    {
        return (instance->*n)(arg...);
    }

    FunctionReturn call(FArgumentTypes... arg) const
    {
        return (instance->*c)(arg...);
    }

    ClassName* instance;
    union{
        Function n;
        FunctionConst c;
    };
};

template<class ClassName, typename FReturnType, typename... FArgumentTypes>
/*!
 * \brief Can be extended for indirect calls to functions, either depending on thread or whatever.
 */
struct CFunctionSignal
{
    using ClassType = ClassName;
    using FunctionReturn = FReturnType;
    using Function = FunctionReturn(ClassType::*)(FArgumentTypes...);

    virtual FunctionReturn call(CFunctionSlot<ClassType,FReturnType,FArgumentTypes...> f,
                        FArgumentTypes... args)
    {
        return f.call(args...);
    }
};

template<typename FReturnType,typename... FArgumentTypes>
struct CStaticFunctionBinding
{
    using FunctionReturn = FReturnType;
    using Function = FunctionReturn(*)(FArgumentTypes...);

    CStaticFunctionBinding(Function f):
        function(f)
    {
    }

    FunctionReturn call(FArgumentTypes... arg)
    {
        return function(arg...);
    }

    Function function;
};

struct CMimeData
{
    CMimeData(cstring id, void* data, const szptr& size, bool doClean = false):
        b_doClean(doClean),
        m_data(data),
        m_size(size),
        m_id(id)
    {
    }
    ~CMimeData()
    {
        if(b_doClean)
            CFree(m_data);
    }
    const CString& id(){return m_id;}
    const void* data(){return m_data;}
    const szptr& dataSize(){return m_size;}
private:
    bool b_doClean;
    void* m_data;
    szptr m_size;
    CString m_id;
};

class _cbasic_threadrunner_command
{
public:
    static inline C_FORCE_INLINE void perform(_cbasic_threadrunner_command*){}
    static inline C_FORCE_INLINE void await(_cbasic_threadrunner_command*,uint64){}
};

class _cbasic_threadrunner_queue
{
public:
    virtual void processEvents() = 0;
    void insertCmd(_cbasic_threadrunner_command*){}
};

class CThreadCommand : public _cbasic_threadrunner_command
{
public:
    CThreadCommand(std::function<void()> f):m_cmd(f){}
    static inline C_FORCE_INLINE void perform(CThreadCommand* c)
    {
        c->m_cmd();
    }
    virtual bool await(uint64 = 0)
    {
        return m_stat.load();
    }
protected:
    std::function<void()> m_cmd;
    std::atomic_bool m_stat;
};

/*!
 * \brief Event loop for inserting commands from multiple threads, reimplement for
 */
class CEventLoop
{
public:
    virtual void processEvents()
    {
        m_cmdlistaccess.lock();
        while(!m_cmds.empty())
        {
            CThreadCommand::perform(m_cmds.front());
            m_cmds.pop();
        }
        m_cmdlistaccess.unlock();
    }
    void insertCmd(CThreadCommand* c)
    {
        m_cmdlistaccess.lock();
        m_cmds.push(c);
        m_cmdlistaccess.unlock();
    }
protected:
    std::mutex m_cmdlistaccess;
    std::queue<CThreadCommand*> m_cmds;
};

struct CColorMask
{
    bool r:1;
    bool g:1;
    bool b:1;
    bool a:1;
    uint8 padding:4;
};

struct HWDeviceInfo
{
    HWDeviceInfo(CString model, CString firmware):
        model(model),
        firmware(firmware)
    {}
    const CString model;
    const CString firmware;
};

/*!
 * \brief Typical size, uses integer, should be used for window size
 */
using CSize = _cbasic_size_2d<int32>;
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
typedef _cbasic_zfield<scalar> CZField;

typedef _cbasic_zfield<scalar> CZField64;

typedef _cbasic_timer<uint64> CElapsedTimer;

typedef _cbasic_timer<uint64> CElapsedTimerMicro;

typedef _cbasic_timer<bigscalar> CElapsedTimerD;
}

#endif
