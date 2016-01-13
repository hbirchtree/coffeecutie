#include <plat/platform_detect.h>

#ifdef COFFEE_UNIXPLAT

#ifndef UNIX_CORE
#define UNIX_CORE

/*
 * Here we specify UNIX-specific code
 * This includes Linux for a couple of reasons
 *
 */

#include <coffee/core/CTypes>
#include <sys/time.h>

namespace Coffee{

namespace CFunctional{

template<typename T>
class _cbasic_posix_timer : public _cbasic_timer<T>
{
public:
    virtual ~_cbasic_posix_timer(){}
    void start(){
        _start = _curr_time();
    }
    T elapsed(){
        return _curr_time()-_start;
    }
protected:
    timeval _tv(){
        struct timeval tv;
        gettimeofday(&tv,0);
        return tv;
    }
    virtual T _curr_time() = 0;
private:
    T _start = 0;
};

class _plat_timer_ll_milli : public _cbasic_posix_timer<uint64>
{
private:
    uint64 _curr_time(){
        timeval tv = _tv();
        return tv.tv_sec*1000+tv.tv_usec/1000;
    }
};
class _plat_timer_ll_micro : public _cbasic_posix_timer<uint64>
{
private:
    uint64 _curr_time(){
        timeval tv = _tv();
        return tv.tv_sec*1000000+tv.tv_usec;
    }
};
class _plat_timer_scalar_micro : public _cbasic_posix_timer<bigscalar>
{
private:
    bigscalar _curr_time(){
        timeval tv = _tv();
        return (bigscalar)tv.tv_sec+(bigscalar)tv.tv_usec/1000000.0;
    }
};

} //CFunctional
} //Coffee

#endif // UNIX_CORE
#endif // __unix__
