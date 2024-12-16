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
    OTG device

Description:
    OTG device Driver

Revision History:
    Date                  Author                   Description
    2014.05.04            yinxingjie                   initial version
------------------------------------------------------------------------------*/

#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef __VUDC_DEVICE_H__
#define __VUDC_DEVICE_H__

#include "vstdlib.h"

#define VRET_DRV_USB_ERROR_BASE						0xa00
#define	VRET_DRV_USB_UNKCMD 				 (VRET_DRV_USB_ERROR_BASE + 1)
#define VRET_DRV_USB_ERRDATA			     (VRET_DRV_USB_ERROR_BASE + 2)
#define VRET_DRV_USB_UNVALID				 (VRET_DRV_USB_ERROR_BASE + 3)
#define VRET_DRV_USB_UNMEANINGFUL		 	 (VRET_DRV_USB_ERROR_BASE + 4)
#define VRET_DRV_USB_ERRPHASE			     (VRET_DRV_USB_ERROR_BASE + 5)
#define VRET_DRV_USB_CMDFAILED			     (VRET_DRV_USB_ERROR_BASE + 6)
#define VRET_DRV_USB_HOST_PARA_ERR		     (VRET_DRV_USB_ERROR_BASE + 7)
#define VRET_DRV_USB_DMA_TIMEOUT_ERR	     (VRET_DRV_USB_ERROR_BASE + 8)
#define VRET_DRV_USB_SMC_RESPONSE_TIMEOUT    (VRET_DRV_USB_ERROR_BASE + 9)
#define VRET_DRV_USB_SMC_FS_UNCOMPATIBLE     (VRET_DRV_USB_ERROR_BASE + 10)
#define VRET_DRV_USB_SMC_NOINSERT		     (VRET_DRV_USB_ERROR_BASE + 11)
#define VRET_DRV_USB_SMC_NOTREADY		     (VRET_DRV_USB_ERROR_BASE + 12)

//Endpoint config
#define VUDC_EP0_MAX_TX_SIZE 64
#define VUDC_EP1_MAX_TX_SIZE 512
#define VUDC_EP2_MAX_TX_SIZE 512

#define VUDC_EP0_MAX_RX_SIZE 64
#define VUDC_EP1_MAX_RX_SIZE 512
#define VUDC_EP2_MAX_RX_SIZE 512

/* All standard descriptors have these 2 fields in common */
typedef struct {
    UINT8 bLength;
    const UINT8 *pDesc;
} VUDC_Descriptor;

typedef enum enum_VIM_DEV_EP0_STATES {
    VUDC_EP0_ST_IDLE  = 0,
    VUDC_EP0_ST_RX,
    VUDC_EP0_ST_TX,
    VUDC_EP0_ST_ERR
} DEV_EP0_STATES;

typedef struct {
    UINT32 reqTxLen;
    UINT32 actTxLen;
    UINT32 reqRxLen;
    UINT32 actRxLen;
    UINT8 *cacheAddr;
    DEV_EP0_STATES st;
} VUDC_EP0;

typedef struct {
    void    ( *audio_data_in ) ( void );
} DEVICE_AUDIO_HDL;

typedef struct {
    //usb interrupt
    void    ( *usb_int_data_in ) ( void );
    void    ( *usb_int_data_out ) ( void );
    //dma interrupt
    void    ( *dma_int_data_in ) ( void );
    void    ( *dma_int_data_out ) ( void );
} DEVICE_MSD_HDL;

typedef struct {
    void    ( *serial_data_in ) ( void );
    void    ( *serial_data_out ) ( void );
} DEVICE_SERIAL_HDL;

typedef struct {
    void    ( *vsol_sendmsg ) ( UINT32 u32MsgId, UINT32 Param );
} DEVICE_SOL ;

//usb device  data in/out handler func;
typedef struct {
    //usb device solution
    DEVICE_SOL *device_sol;
    //usb device class
    DEVICE_AUDIO_HDL *device_audio;
    DEVICE_MSD_HDL  *device_msd;
    DEVICE_SERIAL_HDL *device_serial;
} USB_DEVICE_HDL;

#define EP0_TSTALL 0x01
#define EP0_RSTALL 0x02
#define EP1_TSTALL 0x04
#define EP1_RSTALL 0x08
#define EP2_TSTALL 0x10
#define EP2_RSTALL 0x20
#define USBD_ADDRED 0x40
#define USBD_CFGED  0x80
#define VSOL_SUPPORT                    0

//exported functions
DEV_EP0_STATES VOTG_DEV_EP0_RxParser ( UINT8 *dmabuf );
UINT32 VUDC_GetMinLen ( UINT32 lenA, UINT32 lenB );
void VOTG_Dev_Isr ( void );
VRET_VALUE VOTG_Dev_MSGHandler ( UINT32 Msg_Id, void *p_Msg );
void VUDC_TX_EPx ( UINT8 ep, UINT32 len, UINT32 addr, UINT32 offset );
void VUDC_RX_EPx ( UINT8 ep, UINT32 len, UINT32 addr, UINT32 offset );
void VUDC_EPxTxSTALL ( UINT8 ep );
void VUDC_EPxRxSTALL ( UINT8 ep );
void VUDC_CfgDesc ( void );
void VUDC_CfgDescSerialFixCom ( void );
void VUDC_EPx_CfgISORx ( UINT8 ep );
DEV_EP0_STATES VUDC_EP0_Isr ( void );

void VUDC_WaitEtiPktReady(UINT8 ep_id);

#endif  /* multiple inclusion protection */
