#ifndef COFFEEGPUMONITOR_H
#define COFFEEGPUMONITOR_H

#include "general/common.h"

class CoffeeGPUMonitor
{
public:
    void getGpuMemoryUsage(qint32 *current, qint32 *total) const;

protected:
    void requestGpuMemoryCheck();
    void checkGpuMemoryStatistics();

private:
    bool m_gpumemcheck = false;

    //Data sources, different methods for acquiring the data
    bool gpumemcheck_nvidia = true;
    bool gpumemcheck_ati = true;

    qint32 vmem_free = -1;
    qint32 vmem_total = 0;
};

#endif // COFFEEGPUMONITOR_H
