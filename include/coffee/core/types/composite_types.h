#ifndef COFFEE_COMPOSITE_TYPES_H
#define COFFEE_COMPOSITE_TYPES_H

#include <cmath>

#include "../coffee_macros.h"
#include "basetypes.h"
#include "map.h"
#include "translatable_string.h"

#undef near
#undef far

namespace Coffee{

PACKEDSTRUCT uint24
{
    uint32 d:24;
};

PACKEDSTRUCT bigscalar_dissect
{
    union
    {
        struct{
            uint8 sign:1;
            uint16 exponent:11;
            uint64 mantissa:52;
        };
        bigscalar v;
    };
};
PACKEDSTRUCT scalar_dissect
{
    union
    {
        struct{
            uint8 sign:1;
            uint8 exponent:8;
            uint32 mantissa:24;
        };
        scalar v;
    };
};
PACKEDSTRUCT half_dissect
{
    union
    {
        struct{
            uint8 sign:1;
            uint8 exponent:5;
            uint16 mantissa:10;
        };
        uint16 v;
    };
};

template<typename T>
/*!
 * \brief For storing arrays
 */
struct chunk_mem
{
    szptr size = 0;
    T* d = nullptr;
};

template<typename T>
struct _cbasic_data_chunk
{
    /*!
     * \brief Size in bytes
     */
    szptr size;
    /*!
     * \brief Pointer to data
     */
    T* data;
    /*!
     * \brief Number of elements, if applicable
     */
    szptr elements;
};

/*!
 * \brief A template for version structures
 */
template<typename T> struct _cbasic_version
{
    T major;
    T minor;
    T revision;

    inline bool operator>=(const _cbasic_version<T>& v)
    {
        if(this->major>v.major)
            return true;
        if(this->major==v.major)
        {
            if(this->minor>v.minor)
                return true;
            if(this->minor==v.minor)
            {
                if(this->revision>=v.revision)
                    return true;
                else
                    return false;
            }else
                return false;
        }else
            return false;
    }
    inline bool operator<(const _cbasic_version<T>& v)
    {
        return !((*this)>=v);
    }
    inline bool operator<=(const _cbasic_version<T>& v)
    {
        if(this->major<v.major)
            return true;
        if(this->major==v.major)
        {
            if(this->minor<v.minor)
                return true;
            if(this->minor==v.minor)
            {
                if(this->revision<=v.revision)
                    return true;
                else
                    return false;
            }else
                return false;
        }else
            return false;
    }
    inline bool operator>(const _cbasic_version<T>& v)
    {
        return !((*this)<=v);
    }
    inline bool operator==(const _cbasic_version<T>& v)
    {
        return this->major==v.major
                &&this->minor==v.minor
                &&this->revision==v.revision;
    }
};

/*!
 * \brief Expands an array contained in chunk_mem<T> types
 * \param mem
 * \param size
 */
template<typename T> static void coffee_mem_expand_array(
        chunk_mem<T> *mem, szptr size)
{
    szptr osize = mem->size;
    mem->size += size;
    mem->d = (T*)CRealloc(mem->d,sizeof(T)*mem->size);
    CMemClear(&mem->d[osize],sizeof(T)*(mem->size-osize));
}

typedef _cbasic_data_chunk<void> CVoidData;
typedef _cbasic_data_chunk<byte_t> CByteData;

/* The below floating-point formats are used in OpenGL */
//struct C_FORCE_PACKING scalar11
//{
//    uint8 exponent:6;
//    uint8 mantissa:5;
//};
//struct C_FORCE_PACKING scalar10
//{
//    uint8 exponent:5;
//    uint8 mantissa:5;
//};
/**/

using Timestamp = uint64;

using DateTime = tm;

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

struct CRGB
{
    union{
        uint24 i;
        struct
        {
            uint8 r,g,b;
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

    T area() const
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
                    std::max(rekt.x,this->x),
                    std::max(rekt.y,this->y),
                    std::min(rekt.w,this->w),
                    std::min(rekt.h,this->h));
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
                    std::min(rekt.x,this->x),
                    std::min(rekt.y,this->y),
                    std::max(rekt.w,this->w),
                    std::max(rekt.h,this->h));
    }
    FORCEDINLINE _cbasic_rect<T> offset(const _cbasic_point<T>& point)
    {
        return _cbasic_rect<T>(this->x+point.x,
                               this->y+point.y,
                               this->w,this->h);
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
    FORCEDINLINE _cbasic_raii_container():
        m_data(new T)
    {
    }
    FORCEDINLINE _cbasic_raii_container(T* d):
        m_data(d)
    {
    }
    virtual ~_cbasic_raii_container()
    {
    }

    FORCEDINLINE T *data()
    {
        return m_data;
    }
    FORCEDINLINE T& data_ref()
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
    FORCEDINLINE _cbasic_cookie_container(std::mutex& acc, T& cookie):
        _access(acc),
        _cookie(cookie)
    {
        _access.lock();
    }
    FORCEDINLINE ~_cbasic_cookie_container()
    {
        _access.unlock();
    }
    FORCEDINLINE T& object()
    {
        return _cookie;
    }
};

template<typename PixelType,typename DimT>
struct _cbasic_bitmap_base
{
    FORCEDINLINE _cbasic_bitmap_base(DimT w, DimT h):
        size(w,h)
    {
        m_pixels = (PixelType*)CCalloc(sizeof(PixelType),w*h);
        m_internal_data = true;
    }
    FORCEDINLINE _cbasic_bitmap_base(DimT w, DimT h, PixelType* data):
        size(w,h)
    {
        m_pixels = data;
        m_internal_data = false;
    }

    FORCEDINLINE ~_cbasic_bitmap_base()
    {
        if(m_internal_data)
            CFree(m_pixels);
    }

    const _cbasic_size_2d<DimT> size;

    FORCEDINLINE PixelType* data()
    {
        return m_pixels;
    }
    FORCEDINLINE const PixelType* data() const
    {
        return m_pixels;
    }
private:
    PixelType* m_pixels;
    bool m_internal_data;
};

using CBitmap = _cbasic_bitmap_base<CRGBA,int32>;

template<class ClassName,typename FReturnType,typename... FArgumentTypes>
struct CFunctionSlot
{
    using ClassType = ClassName;
    using FunctionReturn = FReturnType;
    using Function = FunctionReturn(ClassType::*)(FArgumentTypes...);
    using FunctionConst = FunctionReturn(ClassType::*)(FArgumentTypes...) const;

    FORCEDINLINE CFunctionSlot(ClassType* c, Function f):
        instance(c),n(f)
    {
    }
    FORCEDINLINE CFunctionSlot(ClassType* c, FunctionConst f):
        instance(c),c(f)
    {
    }

    FORCEDINLINE FunctionReturn call(FArgumentTypes... arg)
    {
        return (instance->*n)(arg...);
    }

    FORCEDINLINE FunctionReturn call(FArgumentTypes... arg) const
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

    virtual FunctionReturn call(
            CFunctionSlot<ClassType,FReturnType,FArgumentTypes...> f,
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

    FORCEDINLINE CStaticFunctionBinding(Function f):
        function(f)
    {
    }

    FORCEDINLINE FunctionReturn call(FArgumentTypes... arg)
    {
        return function(arg...);
    }

    Function function;
};

struct CMimeData
{
    FORCEDINLINE CMimeData(cstring id, void* data,
                           const szptr& size,
                           bool doClean = false):
        b_doClean(doClean),
        m_data(data),
        m_size(size),
        m_id(id)
    {
    }
    FORCEDINLINE ~CMimeData()
    {
        if(b_doClean)
            CFree(m_data);
    }
    FORCEDINLINE const CString& id(){return m_id;}
    FORCEDINLINE const void* data(){return m_data;}
    FORCEDINLINE const szptr& dataSize(){return m_size;}
private:
    bool b_doClean;
    void* m_data;
    szptr m_size;
    CString m_id;
};

class _cbasic_threadrunner_command
{
public:
    STATICINLINE void perform(_cbasic_threadrunner_command*){}
    STATICINLINE void await(_cbasic_threadrunner_command*,uint64){}
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
    STATICINLINE void perform(CThreadCommand* c)
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
    FORCEDINLINE void insertCmd(CThreadCommand* c)
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

/*!
 * \brief Describes details of a hardware device, only informative
 */
struct HWDeviceInfo
{
    FORCEDINLINE HWDeviceInfo(CString model,
                              CString firmware):
        model(model),
        firmware(firmware)
    {}
    FORCEDINLINE HWDeviceInfo(CString manufacturer,
                              CString model,
                              CString firmware):
        manufacturer(manufacturer),
        model(model),
        firmware(firmware)
    {}
    const CString manufacturer;
    const CString model;
    const CString firmware;
};

template<typename PT>
/*!
 * \brief Wrapper around std::queue to make it easier to recycle packets in a fast loop
 */
class PacketPool
{
public:
    ~PacketPool()
    {

    }

    FORCEDINLINE void expand(size_t n)
    {
        /* Place lock on data */
        Lock l(m_lock);

        for(size_t i=0;i<n;i++)
        {
            PT* p = new PT;
            initPacket(p);
            m_free.push(p);
        }
    }
    FORCEDINLINE PT* grab()
    {
        autoRecycle();

        m_lock.lock();
        size_t numfree = m_free.size();
        m_lock.unlock();

        if(numfree<1)
            expand(1);

        /* Place lock on data */
        Lock l(m_lock);

        PT* p = m_free.front();
        m_free.pop();
        return p;
    }
    FORCEDINLINE void recycle(PT* p)
    {
        /* Place lock on data */
        Lock l(m_lock);
        C_UNUSED(l);

        cleanPacket(p);
        m_occupied.remove(p);
        m_free.push(p);
    }
    FORCEDINLINE void autoRecycle()
    {
        /* Place lock on data */
        Lock l(m_lock);

        for(PT* p : m_occupied)
            if(isAvailable(p))
                m_tmp_store.push_back(p);

        for(PT* p : m_tmp_store)
            recycle(p);
        m_tmp_store.clear();
    }
protected:
    virtual void initPacket(PT*)
    {
    }
    virtual bool isAvailable(PT*)
    {
        return false;
    }
    virtual void cleanPacket(PT*)
    {
    }
    virtual void freePacket(PT*)
    {
    }

    Mutex m_lock;
    std::list<PT*> m_occupied;
    std::queue<PT*> m_free;

    std::vector<PT*> m_tmp_store;
};

template<typename PT>
class PacketConsumer
{
public:
    FORCEDINLINE PacketConsumer()
    {
    }
    virtual void processPacket(PT* packet) = 0;
};

template<typename PT>
class PacketProducer
{
public:
    FORCEDINLINE PacketProducer(PacketPool<PT>& pool):
        m_pool(&pool)
    {
    }

    FORCEDINLINE void registerConsumer(PacketConsumer<PT>* c)
    {
        Lock l(m_consumer_mutex);
        m_consumers.push_back(c);
    }
    FORCEDINLINE void removeConsumer(PacketConsumer<PT>* c)
    {
        Lock l(m_consumer_mutex);
        m_consumers.remove(c);
    }

    FORCEDINLINE PT* getPacket()
    {
        return m_pool->grab();
    }
    FORCEDINLINE void usePacket(PT* p)
    {
        Lock l(m_consumer_mutex);
        for(PacketConsumer<PT>* pc : m_consumers)
            pc->processPacket(p);
        m_pool->recycle(p);
    }

protected:
    PacketPool<PT>* m_pool;

    Mutex m_consumer_mutex;
    std::list<PacketConsumer<PT>*> m_consumers;
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
typedef _cbasic_zfield<scalar> CZField;

typedef _cbasic_zfield<scalar> CZField64;
}

#endif
