/*                                        COPYRIGHT    (C)    2009,    Vimicro    Corporation
                                                            ALL    RIGHTS RESERVED

This source    code has been    made available to    you    by VIMICRO on    an AS-IS basis.
Anyone receiving this    source code    is licensed    under    VIMICRO    copyrights to    use
it in    any    way    he or    she    deems    fit, including copying it, modifying it,
compiling    it,    and    redistributing it    either with    or without modifications.    Any
person who transfers this    source code    or any derivative    work must    include    the
VIMICRO    copyright    notice and this    paragraph    in the transferred software.

Module:
    AES

Description:
    AES Driver

Revision History:
    Date                     Author                                     Description
    2015.11.03        lijianduo            initial    version, ported    from VC9638
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdcore.h"
#include "vreg_def.h"
#include "vdrv_aes.h"
#include "vdrv_ckm.h"
#include "vdrv_dma.h"
#include "vdrv_gpio.h"
#include "vdrv_uart.h"

//typedef unsigned int   OS_CPU_SR;
//OS_CPU_SR OS_CPU_SR_Save(void){return 0;}
//void OS_CPU_SR_Restore(OS_CPU_SR cpu_sr){;}
    
Aes_param  Aesparam;

BOOL    _VAES_DMA_TransferDone    =    FALSE;
BOOL    _VDMA_AES_TransferDone    =    FALSE;    


//static VDMA_ISR_CALLBACK g_VDMA_ISR_Callback[VDMA_DEV_TOTAL];

UINT32 x[132];
UINT32 y[66];
UINT32 test_buf[66];
/*************************************************
Function:                    VDMA_AES_ReadEnable(BOOL RdEn)
Description:             dma read enable 

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/
void VDMA_AES_ReadEnable(BOOL RdEn)
{
    UINT32 reg;

    reg = READ_REG(REG_DMA_AES_RD_CTRL);
    WRITE_REG(REG_DMA_AES_RD_CTRL, (RdEn ? (reg | 0x00000001) : (reg & (~0x00000001))));
}
/*************************************************
Function:                    VDMA_AES_WriteEnable(BOOL WrEn)
Description:             dma write enable 

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/
void VDMA_AES_WriteEnable(BOOL WrEn)
{
    UINT32 reg;

    reg = READ_REG(REG_DMA_AES_WR_CTRL);
    WRITE_REG(REG_DMA_AES_WR_CTRL, (WrEn ? (reg | 0x00000001) : (reg & (~0x00000001))));
}
    
/*************************************************
Function:                    VDMA_AES_IntCallBack
Description:             the CB of dma2aes when interrupt 

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/

void    VDMA_AES_IntCallBack()    
{    

    VCPU_SR_ALLOC(); 
    VCPU_CRITICAL_ENTER();
    _VDMA_AES_TransferDone    = TRUE;
	VDMA_ClearInt(VDMA_INT_MC_AES_SRAM_DONE_MSK | VDMA_INT_MC_SRAM_AES_DONE_MSK);
    VCPU_CRITICAL_EXIT();        

}
/*************************************************
Function:                    VAES_DMA_IntCallBack
Description:             the CB of aes2dma when interrupt 

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/

void    VAES_DMA_IntCallBack()    
{    

    VCPU_SR_ALLOC(); 
    VCPU_CRITICAL_ENTER();
    _VAES_DMA_TransferDone    = TRUE;
	VDMA_ClearInt(VDMA_INT_MC_AES_SRAM_DONE_MSK | VDMA_INT_MC_SRAM_AES_DONE_MSK);
    VCPU_CRITICAL_EXIT();        

    }

/*************************************************
Function:                    VAES_Init
Description:     aes initialization

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/
void AES_init(Aes_param *aesp)
{
    //VDMA_MaskAllInt();
    //WRITE_REG(REG_DMA_INTR_MASK, VDMA_INT_MC_ALL_MSK);
    //WRITE_REG(REG_DMA_AES_WR_CTRL,0x0);
    //WRITE_REG(REG_DMA_AES_RD_CTRL,0x0);
    //VDMA_DEV_AES
    //VDMA_RegisterISRCallback(VDMA_DEV_AES,FALSE, VDMA_AES_IntCallBack);
    //VDMA_RegisterISRCallback(VDMA_DEV_AES, TRUE,VAES_DMA_IntCallBack);
    //VDMA_RegisterISRCallback(VDMA_DEV_SRAM2AES,TRUE, VDMA_AES_IntCallBack);
    //VDMA_RegisterISRCallback(VDMA_DEV_AES2SRAM, FALSE,VAES_DMA_IntCallBack);
    //VDMA_UnmaskInt(VMDA_INT_MC_AES_SRAM_DONE_MSK | VMDA_INT_MC_SRAM_AES_DONE_MSK);
	VDMA_MaskInt(VDMA_INT_MC_AES_SRAM_DONE_MSK | VDMA_INT_MC_SRAM_AES_DONE_MSK);
    WRITE_REG(REG_DMA_AES_WR_ADD,(UINT32)(aesp->wr_addr));
    WRITE_REG(REG_DMA_AES_RD_ADD,(UINT32)(aesp->rd_addr));
      WRITE_REG(REG_DMA_AES_WR_LENGTH,aesp->wr_length);
    WRITE_REG(REG_DMA_AES_RD_LENGTH,aesp->rd_length);
    WRITE_REG(REG_AES_BB_AES_SKD_0, aesp->sdk.skd_0);
    WRITE_REG(REG_AES_BB_AES_SKD_1, aesp->sdk.skd_1);
    WRITE_REG(REG_AES_BB_AES_SKD_2, aesp->sdk.skd_2);
    WRITE_REG(REG_AES_BB_AES_SKD_3, aesp->sdk.skd_3);
    WRITE_REG(REG_AES_BB_AES_LTK_0, aesp->ltk.ltk_0);
    WRITE_REG(REG_AES_BB_AES_LTK_1, aesp->ltk.ltk_1);
    WRITE_REG(REG_AES_BB_AES_LTK_2, aesp->ltk.ltk_2);
    WRITE_REG(REG_AES_BB_AES_LTK_3, aesp->ltk.ltk_3);
    WRITE_REG(REG_AES_BB_AES_NONCE_0,aesp->nonce.nonce_0);
    WRITE_REG(REG_AES_BB_AES_NONCE_1,aesp->nonce.nonce_1);
    WRITE_REG(REG_AES_BB_AES_NONCE_2,aesp->nonce.nonce_2);
    WRITE_REG(REG_AES_BB_AES_NONCE_3,aesp->nonce.nonce_3);
    WRITE_REG(REG_AES_BB_LENGTH,aesp->pkt_length);
    WRITE_REG(REG_AES_B1_BYTE,aesp->BBbyte);
    WRITE_REG(REG_AES_BB_RX_PDU_MIC,0x0);
}


/*************************************************
Function:                    VAES_KEY_Generation
Description:             aes key generation 

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/
void    VAES_KEY_Generation(void)    
{    
            WRITE_REG(REG_AES_BB_AES_CTRL,0x8);
            VCOMMON_Delay(1);
            WRITE_REG(REG_AES_BB_AES_CTRL,0x0);
}
/*************************************************
Function:                    VAES_ENC_Write
Description:             sram write to aes by dma when encryption

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/
void    VAES_ENC_Write(void)    
{    
            WRITE_REG(REG_AES_BB_AES_CTRL,0x3);
            WRITE_REG(REG_DMA_AES_WR_CTRL,0x4);
            WRITE_REG(REG_DMA_AES_RD_CTRL,0x4);
            VDMA_AES_ReadEnable(TRUE);
            VDMA_AES_WriteEnable(TRUE);
}
/*************************************************
Function:                    VAES_ENC_Read
Description:             aes write to sram by dma when encryption

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/
void    VAES_ENC_Read(void)    
{    
        VDMA_AES_ReadEnable(TRUE);
}
/*************************************************
Function:                    VAES_DEC_Write_first
Description:             sram write to aes by dma when decryption

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/
void    VAES_DEC_Write_first(Aes_param *aesp)    
{
        //VDMA_UnmaskInt(VMDA_INT_MC_AES_SRAM_DONE_MSK | VMDA_INT_MC_SRAM_AES_DONE_MSK);
        //aesp->wr_addr = (UINT32)(&y[0]);
        //aesp->rd_addr = (UINT32)(&x[0]);
        WRITE_REG(REG_DMA_AES_WR_CTRL,0x4);
        WRITE_REG(REG_DMA_AES_RD_CTRL,0x4);
        WRITE_REG(REG_AES_BB_AES_CTRL,0x4);
        WRITE_REG(REG_AES_BB_AES_CTRL,0x5);
        VDMA_AES_WriteEnable(TRUE);    
}
/*************************************************
Function:                    VAES_DEC_Read_first
Description:             aes write to sram by dma when decryption

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/
void    VAES_DEC_Read_first(void)    
{    
        VDMA_AES_ReadEnable(TRUE);
}
/*************************************************
Function:                    VAES_DEC_Write_first
Description:             sram write to aes by dma when decryption

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/
void    VAES_DEC_Write_second(Aes_param *aesp)    
{
        aesp->wr_addr = (UINT32)(&(y[0]));
        aesp->rd_addr = (UINT32)(&(test_buf[0]));
        WRITE_REG(REG_DMA_AES_WR_ADD,(UINT32)(aesp->wr_addr));
        WRITE_REG(REG_DMA_AES_RD_ADD,(UINT32)(aesp->rd_addr));
        aesp->wr_length = 1;
        aesp->rd_length = 1;
        WRITE_REG(REG_DMA_AES_WR_LENGTH,(UINT32)aesp->wr_length);
        WRITE_REG(REG_DMA_AES_RD_LENGTH,(UINT32)aesp->rd_length);
        //WRITE_REG(REG_DMA_AES_WR_CTRL,0x4);
        //WRITE_REG(REG_DMA_AES_WR_CTRL,0x5);
        WRITE_REG(REG_DMA_AES_RD_CTRL,0x0);
        VDMA_AES_WriteEnable(TRUE);    
}
/*************************************************
Function:                    VAES_DMA_Disable
Description:             disbale aes_dma

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/
void    VAES_DMA_Disable(void)    
{
	VDMA_MaskInt(VDMA_INT_MC_AES_SRAM_DONE_MSK | VDMA_INT_MC_SRAM_AES_DONE_MSK );
    VDMA_AES_WriteEnable(FALSE);
    VDMA_AES_ReadEnable(FALSE);
}
/*************************************************
Function:                VAES_ENC_Write_Disable(void)
Description:             aes encryption function

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/
void VAES_ENC_Write_Disable(void)
{
    WRITE_REG(REG_DMA_AES_WR_CTRL,0x0);    
}
/*************************************************
Function:                VAES_ENC_Read_Disable(void)
Description:             aes encryption function

Input: 

Output:                 
Return:                    
Note:    

History:                        
*************************************************/
void VAES_ENC_Read_Disable(void)
{
    WRITE_REG(REG_DMA_AES_RD_CTRL,0x0);    
}
/*************************************************
Function:				VAES_Uart_Init(void)
Description:	 		aes Uart function

Input: 

Output:				 
Return:					
Note:	

History:						
*************************************************/
void VAES_Uart_Init(void)
{
	VGPIO_SetMuxFunction(GPIO_MUX_FUNC_UART_TRX, GPIO_MUX_FUNC_UART_TRX);	
	//output
	WRITE_REG(REG_GPIO_GDOEN, READ_REG(REG_GPIO_GDOEN) & ~(GPIO_MUX_FUNC_LED_ALL));
	WRITE_REG(REG_GPIO_GDDRIVE, READ_REG(REG_GPIO_GDDRIVE) | GPIO_MUX_FUNC_LED_ALL);	
	//high
	//REG_GPIO_GDOUT
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
		WRITE_REG(REG_GPIO_GDOUT, READ_REG(REG_GPIO_GDOUT) | GPIO_MUX_FUNC_LED_ALL);
#else
		BB_GPIO_GDOUT_SET(GPIO_MUX_FUNC_LED_ALL);
#endif
	//low
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
		WRITE_REG(REG_GPIO_GDOUT, READ_REG(REG_GPIO_GDOUT) & ~(GPIO_MUX_FUNC_LED_ALL));	
#else
		BB_GPIO_GDOUT_CLR(GPIO_MUX_FUNC_LED_ALL);
#endif	
#if (GPIO_BIT_MODE == GPIO_OUT_ADDR_OPERATION)
		WRITE_REG(REG_GPIO_GDOUT, READ_REG(REG_GPIO_GDOUT) | GPIO_MUX_FUNC_LED_ALL);
#else
		BB_GPIO_GDOUT_SET(GPIO_MUX_FUNC_LED_ALL);
#endif
//#if (VDBG_IO_RETARGET == IO_UART)
	VUART_SetBaudRate(115200, VCKM_GetUartClockFreq());
	VUART_Config(UART_CONF_PARITY_NONE, UART_CONF_STOP_BIT1, UART_CONF_CHAR_BIT8);
	VUART_Enable();
//#endif	

}

void VAES_Enc(Aes_param *param)
{
	//int i;
	//int mic ;
	//UINT32 mask_reg;
	
	//VDMA_EnableInt(0,0);
	//sram write to aes
	VAES_ENC_Write();
	//VCOMMON_Delay(1);
#if 0
	while(1)
	{
		if((param->mode) == 0)
		{
			UINT32 mask_reg = READ_REG(REG_DMA_INTR_MASK);
			UINT32 error_reg = READ_REG(REG_DMA_INTR_ERR_STAT);
			//UINT32 fnfh_reg = READ_REG(REG_DMA_INTR_FNSH_STAT);
			//NOTICE_PRINTF("mask_reg = %x\r\n",mask_reg);
			//if(((READ_REG(REG_DMA_INTR_ERR_STAT) & 0x0 )== 0)&&((READ_REG(REG_DMA_INTR_ERR_STAT) & 0x80 )== 0x80)) //needn`t dmac interrupt callback,only read FNSH_STAT
			if(((READ_REG(REG_DMA_INTR_ERR_STAT) & 0x80 )== 0x80)) //needn`t dmac interrupt callback,only read FNSH_STAT
			{
				NOTICE_PRINTF("1\r\n");
				break;
			}
			else 
				continue;
		}
		else //need dmac interrupt callback
		{
			//mask_reg = READ_REG(REG_DMA_INTR_MASK);
			//NOTICE_PRINTF("mask_reg = %x\r\n",mask_reg);
			if(_VDMA_AES_TransferDone == 0x1 )
			{
				break;
			}
		}
	}
	_VDMA_AES_TransferDone = FALSE;
#endif 
	//VAES_ENC_Read();
	VCOMMON_Delay(10);
	//mask_reg = READ_REG(REG_DMA_INTR_MASK);
#if 0	
	while(1)
	{
		if(param->mode == 0)
		{
			UINT32 error_reg = READ_REG(REG_DMA_INTR_ERR_STAT);
			//UINT32 fnfh_reg = READ_REG(REG_DMA_INTR_FNSH_STAT);		
			if(((READ_REG(REG_DMA_INTR_ERR_STAT))&0x40)==0x40) //needn`t dmac interrupt callback,only read FNSH_STAT
			{
			VAES_ENC_Read_Disable();
			break;
			}
		}	
		else 
		{
			if(_VAES_DMA_TransferDone == 0x1 )
			{
				break;
			}	
		}
	if ((param->micneed) == 1)
	{
		mic = y[param->pkt_length-4];
		mic = ((mic>>24)&0xff)|((mic>>8)&0xff00)|((mic<<8)&0xff0000)|((mic<<24)&0xff000000);	
		WRITE_REG(BB_AES_RX_PDU_MIC,mic);	
	}	
		WRITE_REG(BB_AES_CTRL,0x0);
		VAES_ENC_Write_Disable();
		VAES_ENC_Read_Disable();
		_VAES_DMA_TransferDone = FALSE;
	}
	#endif 
	//WRITE_REG(BB_AES_RX_PDU_MIC,READ_REG(BB_AES_RX_PDU_MIC_EPT));
	VAES_ENC_Write_Disable();
	VAES_ENC_Read_Disable();
	WRITE_REG(REG_AES_BB_AES_CTRL,0x0);
}

void VAES_Dec(Aes_param *param)
{
    //UINT32 mask_reg;
    
    //VDMA_EnableInt(0,0);    
    VAES_DEC_Write_first(param);

    //mask_reg = READ_REG(REG_DMA_INTR_MASK);    
#if 0    
    while(1)
    {
        if((param->mode)==0)
        {
            UINT32 error_reg = READ_REG(REG_DMA_INTR_ERR_STAT);
            //UINT32 fnfh_reg = READ_REG(REG_DMA_INTR_FNSH_STAT);
            //if(((READ_REG(REG_DMA_INTR_ERR_STAT) & 0x0 )== 0)&&((READ_REG(REG_DMA_INTR_ERR_STAT) & 0x80 )== 0x80)) //needn`t dmac interrupt callback,only read FNFH_STAT
            if((READ_REG(REG_DMA_INTR_ERR_STAT) & 0x80 )== 0x80) //needn`t dmac interrupt callback,only read FNFH_STAT
            {
                break;
            }
            else 
                {
                    continue;
                }    
        }
        else //need dmac interrupt callback
            {
                if(_VDMA_AES_TransferDone == 0x1 )
                {
                    break;
                }    
            }
        }
#endif
    //delay(1);
    VCOMMON_Delay(1);
    VAES_DEC_Read_first();
#if 0
    while(1)
    {
        if((param->mode) ==0)
        {
            UINT32 error_reg = READ_REG(REG_DMA_INTR_ERR_STAT);
            //UINT32 fnfh_reg = READ_REG(REG_DMA_INTR_FNSH_STAT);
            if(((READ_REG(REG_DMA_INTR_ERR_STAT))&0x40)==0x40) //needn`t dmac interrupt callback,only read FNFH_STAT
            {
                break;
            }
        }
        else 
            {
                if(_VAES_DMA_TransferDone == 0x1 )
                {
                    break;
                }
            }
    }
#endif
    //VCOMMON_Delay(1);
    #if 0
    VAES_DEC_Write_second(param);    
    while(1)
    {
        if((param->mode) ==0)
        {
            UINT32 error_reg = READ_REG(REG_DMA_INTR_ERR_STAT);
            //UINT32 fnfh_reg = READ_REG(REG_DMA_INTR_FNSH_STAT);
            if((READ_REG(REG_DMA_INTR_ERR_STAT) & 0x80 )== 0x80) //needn`t dmac interrupt callback,only read FNFH_STAT
            {
                break;
            }
            else 
                {
                    continue;
                }    
            }
        else
        {
            if(_VDMA_AES_TransferDone == 0x1 )
            {
                break;
            }    
        }
    _VDMA_AES_TransferDone = FALSE;
     mic = READ_REG(BB_AES_RX_PDU_MIC);
    //    VDMA_DisableInt()    ;
        }
    #endif
    VAES_DEC_Write_second(param);
    VCOMMON_Delay(1);
    WRITE_REG(REG_AES_BB_AES_INT_CLR,0xf);
	WRITE_REG(REG_DMA_INTR_CLR,0x800000);
    VAES_ENC_Write_Disable();
    VAES_ENC_Read_Disable();
    WRITE_REG(REG_AES_BB_AES_CTRL,0x0);
}

    

