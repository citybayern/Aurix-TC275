#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

extern IfxCpu_syncEvent g_cpuSyncEvent;

int core1_main(void)
{
    IfxCpu_enableInterrupts();
    
    /* !!WATCHDOG1 IS DISABLED HERE!!
     * Enable the watchdog and service it periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    
    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);
    
    while(1)
    {
    }
    return (1);
}
