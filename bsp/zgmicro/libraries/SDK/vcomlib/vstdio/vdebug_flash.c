#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vdrv_spi.h"
#include "vdrv_spiflash.h"
//#include "vsys_flashlog.h"

#if (VDBG_IO_RETARGET==IO_FLASH)

#include <stdio.h>

extern VSPI_FLASH_INFO g_VSPI_FLASH_Info;

void VDBG_DrvInit(void) {
	if(!(g_VSPI_FLASH_Info.ready&FLOG_DEST_FLASHDEV))
	{
	  VSPI_SetEnable(TRUE);
	  //VSPI_FLASH_RstCrm(FLOG_DEST_FLASHDEV);
		VSPI_FLASH_Init(VSPI_FLASH_INPACKAGE);
	}
	FLASH_LOG_Init();
	VDWT_Start();
}

void vputs(char *str) {
	FLASH_LOG_Write((UINT8*)str,VCOMMON_StrLen((SINT8*)str));
}

int fputc(int ch, FILE *f) 
{
  return 0;
}

int fgetc(FILE *f) 
{
  return 0;
}

void _ttywrch(int ch) 
{
;
}

#endif

