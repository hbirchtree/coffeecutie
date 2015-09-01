#include "coffeegpumonitor.h"

void CoffeeGPUMonitor::getGpuMemoryUsage(qint32 *current, qint32 *total) const
{
    if(this->vmem_free<0&&vmem_total==0)
        return;
    if(!current||!total)
        return;
    *current = (vmem_free>0) ? vmem_free : INT_MAX;
    *total = (vmem_total>0) ? vmem_total : INT_MAX;
}

void CoffeeGPUMonitor::requestGpuMemoryCheck()
{
    m_gpumemcheck = true;
}

void CoffeeGPUMonitor::checkGpuMemoryStatistics()
{
    if(gpumemcheck_nvidia){
        glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,&vmem_total);
        glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,&vmem_free);
        if(vmem_total==0||vmem_free<0){ //If check fails, stop checking.
            gpumemcheck_nvidia = false;
            qDebug() << "Disabling NVIDIA video memory check";
        }
    }else if(gpumemcheck_ati){
        glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI,&vmem_free);
        //Do not know about total memory yet
        if(vmem_total==0||vmem_free<0){
            gpumemcheck_ati = false;
            qDebug() << "Disabling AMD/ATI video memory check";
        }
    }
}
