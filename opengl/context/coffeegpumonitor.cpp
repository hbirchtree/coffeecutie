#include "coffeegpumonitor.h"

void CoffeeGPUMonitor::getGpuMemoryUsage(qint32 *current, qint32 *total) const
{
    if(this->vmem_free==0&&vmem_total==0)
        return;
    if(!current||!total)
        return;
    *current = vmem_free;
    *total = vmem_total;
}

void CoffeeGPUMonitor::requestGpuMemoryCheck()
{
    m_gpumemcheck = true;
}

void CoffeeGPUMonitor::checkGpuMemoryStatistics()
{
//    if(m_gpumemcheck){

        if(gpumemcheck_nvidia){
            glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,&vmem_total);
            glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,&vmem_free);

            if(vmem_total==0||vmem_free==0){ //If check fails, stop checking.
                gpumemcheck_nvidia = false;
            }
        }
        //We will see if we can add Intel and AMD support later.
        //So far, it seems like AMD only support Windows with that.
        //Core functionality should work across all platforms.

//        m_gpumemcheck = false;
//    }
}
