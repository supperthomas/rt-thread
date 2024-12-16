
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "SEGGER_SYSVIEW_Wrap.h"
#if VSYS_SYSVIEW_SUPPORTED
#if (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_UCOSIII)
#include ".\Sample\uCOS-III\SEGGER_SYSVIEW_uCOSIII.c"
#include ".\Sample\uCOS-III\Config\Cortex-M\SEGGER_SYSVIEW_Config_uCOSIII.c"
#elif (VOS_PLATFORM_SELECT_TYPE == VOS_PLATFORM_FREERTOS)
#include ".\Sample\FreeRTOSV10.4\SEGGER_SYSVIEW_FreeRTOS.c"
#include ".\Sample\FreeRTOSV10.4\Config\Cortex-M\SEGGER_SYSVIEW_Config_FreeRTOS.c"
#else
void SEGGER_SYSVIEW_Conf(void) 
{
}
#endif
#endif
