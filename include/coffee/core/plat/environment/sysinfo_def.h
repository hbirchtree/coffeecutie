#ifndef COFFEE_CORE_PLAT_ENV_SYSINFO_DEF_H
#define COFFEE_CORE_PLAT_ENV_SYSINFO_DEF_H

#include "../plat_primary_identify.h"
#include "../../coffee_mem_macros.h"
#include "../../base/math/cmath.h"
#include "../../types/cdef/infotypes.h"

#include "../memory/stlstring_ops.h"
#include "../memory/string_ops.h"
#include "../../types/tdef/stlfunctypes.h"

namespace Coffee{

struct SysInfoDef
{
    enum NetStatusFlags
    {
        NetStatDisconnected = 0x0,
        NetStatConnected = 0x1,

        NetStatLocalOnly = 0x2,
    };

    STATICINLINE
    /*!
     * \brief Whether this build is 32-bit, 64-bit, 128-bit, blah.
     * \return
     */
    uint32 BitNess()
    {
        return C_SYSTEM_BITNESS;
    }

    STATICINLINE
    /*!
     * \brief Version of the system we run on
     * \return
     */
    CString GetSystemVersion()
    {
        return "?";
    }

    static
    /*!
     * \brief Get string representing system architecture (eg. win64, lin64, mac64, ios64, and32)
     * \return
     */
    CString GetSystemString();

    static
    /*!
     * \brief Only for use in safe environments, such as internal servers.
     * \return
     */
    CString HostName();

    STATICINLINE
    /*!
     * \brief CPU count in total
     * \return
     */
    CoreCnt CpuCount()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief CPU core count in total, all CPUs
     * \return
     */
    CoreCnt CoreCount()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief CPU thread count in total, all CPUs
     * \return
     */
    ThrdCnt ThreadCount()
    {
        return Thread::hardware_concurrency();
    }

    STATICINLINE
    /*!
     * \brief Get amount of parallel tasks that can be launched
     * \return
     */
    ThrdCnt Parallelism()
    {
        return ThreadCount()*64;
    }

    static
    /*!
     * \brief Calculate a 'smart' amount of tasks to launch based upon amount of tasks
     * \param worksize Amount of tasks
     * \param weight Weight given to each tasks. Should only be modified if each task is significant in size.
     * \return An estimated value for what would be a suitable amount of tasks
     */
    ThrdCnt SmartParallelism(uint64 worksize, uint64 weight = 1);

    STATICINLINE
    /*!
     * \brief Whether system supports virtual memory
     * \return
     */
    bool MemVirtualAvailable()
    {
        return true;
    }

    STATICINLINE
    /*!
     * \brief Total amount of memory in system
     * \return
     */
    MemUnit MemTotal()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief Available free memory that can be used
     * \return
     */
    MemUnit MemAvailable()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief Total amount of swap in system
     * \return
     */
    MemUnit SwapTotal()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief Available free swap space in system, not very useful, system-managed
     * \return
     */
    MemUnit SwapAvailable()
    {
        return 0;
    }

    static
    /*!
     * \brief Info about the system's processor
     * \return
     */
    HWDeviceInfo Processor();

    STATICINLINE
    /*!
     * \brief Current processor frequency in GHz, allowed to be inaccurate if hardware clocks particular cores differently
     * \return
     */
    bigscalar ProcessorFrequency()
    {
        return 1.0;
    }

    STATICINLINE
    /*!
     * \brief Whether the system has an FPU, has a great impact on floating-point calculations
     * \return
     */
    bool HasFPU()
    {
        return false;
    }

    STATICINLINE
    bool HasFPUExceptions()
    {
        return false;
    }

    STATICINLINE
    /*!
     * \brief Cache size can be used to optimize tight loops, or just to make screens look tacky
     * \return
     */
    MemUnit ProcessorCacheSize()
    {
        return 0;
    }

    STATICINLINE
    /*!
     * \brief Useful in relation to core count
     * \return
     */
    bool HasHyperThreading()
    {
        return false;
    }

    STATICINLINE
    /*!
     * \brief Whether the 32-bit system supports PAE, doesn't really apply to 64-bit
     * \return
     */
    bool HasPAE()
    {
        return true;
    }

    STATICINLINE
    /*!
     * \brief Reports networking status
     * \return
     */
    NetStatusFlags NetStatus()
    {
        return NetStatLocalOnly;
    }

    static
    HWDeviceInfo DeviceName();

    static
    HWDeviceInfo Motherboard();

    static
    HWDeviceInfo Chassis();
};


struct PowerInfoDef
{
    struct Temp
    {
        scalar current;
        scalar trip_point;
    };

    STATICINLINE
    /*!
     * \brief If device is connected to wall power, often same as charging status,
     *   but primarily refers to having little limitation to power usage.
     * \return
     */
    bool IsPowered()
    {
        return true;
    }
    STATICINLINE
    /*!
     * \brief If device is charging
     * \return
     */
    bool IsCharging()
    {
        return false;
    }
    STATICINLINE
    /*!
     * \brief If device has battery at all
     * \return
     */
    bool HasBattery()
    {
        return false;
    }
    STATICINLINE
    /*!
     * \brief Battery percentage
     * \return
     */
    uint16 BatteryPercentage()
    {
        return 0;
    }
    STATICINLINE
    /*!
     * \brief Remaining minutes of battery
     * \return
     */
    uint32 BatteryLifetime()
    {
        return 0;
    }
    STATICINLINE
    /*!
     * \brief Basic system temperature in Celsius
     * \return
     */
    Temp CpuTemperature()
    {
        return {0.f,0.f};
    }
};

struct DeviceInfoDef
{
    static CString Manufacturer(){return {};}
    static CString Model(){return {};}
    static CString Board(){return {};}

    struct Screen
    {
        enum TouchType
        {
            None,
            Resistive,
            Capacitive,
        };

        scalar size;
        u32 density;
        u32 res_x;
        u32 res_y;

        u32 color_space;

        TouchType touch_type;
    };

    static Screen Screen(){return {};}

    static scalar Weight(){return {};}
};

}

#endif

