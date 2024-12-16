/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2009, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
    AES

Description:
     AES Driver

Revision History:
     Date            Author                   Description
     2015.11.03        lijianduo            initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VAES_H_
#define _VAES_H_

typedef unsigned int    u_int32;
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */

extern BOOL    _VAES_DMA_TransferDone    ;
extern BOOL    _VDMA_AES_TransferDone    ;

typedef struct sdk
{
    u_int32 skd_0;
    u_int32 skd_1;
    u_int32 skd_2;
    u_int32 skd_3;

}SDK;
typedef struct ltk
{
    u_int32 ltk_0;
    u_int32 ltk_1;
    u_int32 ltk_2;
    u_int32 ltk_3;
}LTK;
typedef struct nonce
{
    u_int32 nonce_0;
    u_int32 nonce_1;
    u_int32 nonce_2;
    u_int32 nonce_3;
}NONCE;
typedef u_int32 aes_state;
typedef enum
{
    AES_INT_DPT_STATUS  =  0x1,
    AES_INT_EPT_STATUS  =  0x2,
    AES_INT_INIT_STATUS =  0x4,
    AES_INT_DPT_ERR_STATUS =0x8
}a_flag;
typedef enum
{
    bb2sram_rd_done_msk     = 0x1,        //1
    sram2bb_wr_done_msk     = 0x2,        //2
    aes2sram_rd_done_msk     = 0x4,        //3
    sram2aes_wr_done_msk    = 0x8,        //4
    usb2sram_rd_done_msk0    =    0x10,        //5
    usb2sram_rd_done_msk1    =    0x20,        //6
    usb2sram_rd_done_msk2    =    0x40,        //7
    sram2usb_wr_done_msk0    =    0x80,        //8
    sram2usb_wr_done_msk1    =    0x100,    //9
    sram2usb_wr_done_msk2    =    0x200,    //10
    sd2sram_rd_done_msk        =    0x400,    //11
    sd2dspdm_rd_done_msk    =    0x800,    //12
    sram2sd_wr_done_msk        =    0x1000,    //13
    spi2sram_rd_done_msk    =    0x2000,    //14
    spi2dsppm_rd_done_msk    =    0x4000,    //15
    sram2spi_wr_done_msk    =    0x8000,    //16
    sram2lcd_wr_done_msk    =    0x10000,//17
    dspdm2lcd_wr_done_msk    =    0x20000,//18
    aif2sram_rd_done_msk  = 0x40000,//19
    aif2dspdm_rd_done_msk    =    0x80000,//20
    i2s2dspdm_rd_done_msk    =    0x100000,//21
    sram2aif_wr_done_msk    =    0x200000,//22
    dspdm2aif_wr_done_msk    =    0x400000,//23
    dspdm2sram_done_msk        =    0x800000,//24
    sram2dspdm_done_msk        =    0x1000000,//25
    sram2dsppm_done_msk        =    0x2000000,//26
    dspdm2i2s_wr_done_msk    =    0x4000000,//27
    sram2i2s_wr_done_msk    =    0x8000000    //28
    
}intr_FNSH;
typedef struct aes_param 
{    
    SDK sdk;
    
    LTK ltk;

    NONCE nonce;
    
    u_int32 pkt_length;

    u_int32 BBbyte;
    u_int32 wr_addr;
        u_int32 rd_addr;
    
    u_int32 wr_length;
    u_int32 rd_length;
    
    aes_state state;
    
    a_flag flag;
        
        intr_FNSH fnsh;
        
        u_int32 mode;
        
        u_int32 micneed;

    } Aes_param;    


 /******************************************************************************

                 AES Function Prototypes

*******************************************************************************/
void    VDMA_AES_IntCallBack(void);
void    VAES_DMA_IntCallBack(void);
void    VAES_KEY_Generation(void)    ;
void VDMA_AES_ReadEnable(BOOL RdEn);
void VDMA_AES_WriteEnable(BOOL WrEn);
void    VAES_ENC_Write(void);
void    VAES_ENC_Read(void)    ;
void     VAES_DEC_Write_first(Aes_param *aesp);
void    VAES_DEC_Read_first(void)    ;
void    VAES_DEC_Write_second(Aes_param *aesp);
void    VAES_DMA_Disable(void);
void AES_init(Aes_param *aesp);
void VAES_Encryption(Aes_param *aesp);
void VAES_Decryption(Aes_param *aesp);
void VAES_ENC_Write_Disable(void);
void VAES_ENC_Read_Disable(void);
void VAES_Uart_Init(void);
void VAES_Enc(Aes_param *param);
void VAES_Dec(Aes_param *param);
//void VDMA_ClearErrInt(UINT32 BitMasks);
//void VDMA_ClearInt(UINT32 BitMasks);
#endif
