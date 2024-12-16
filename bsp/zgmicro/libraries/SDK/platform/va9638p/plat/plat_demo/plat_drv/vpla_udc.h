/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2018, Zgmicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by ZGMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under ZGMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
ZGMICRO copyright notice and this paragraph in the transferred software.

Module:
        vpla udc driver

Description:
        wd Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_UDC_H_
#define _VPLA_UDC_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "vstdlib.h"
#include "vstddrv.h"

typedef enum
{
    VPLA_UOTGC_INT_ID_MC = 0,
    VPLA_UOTGC_INT_ID_DMA,
    VPLA_UOTGC_INT_ID_CHARGER,
    VPLA_UOTGC_INT_ID_DRVVBUS,
    VPLA_UOTGC_INT_ID_UNDRVVBUS,
    VPLA_UOTGC_INT_ID_BUSRESET,
    VPLA_UOTGC_INT_ID_PMU_PLUGIN,
    VPLA_UOTGC_INT_ID_VBUSVLD,
    VPLA_UOTGC_INT_ID_HSTDISCON,
    VPLA_UOTGC_INT_NUMBER
} VPLA_UOTGC_INT_ID;

typedef enum {
    VPLA_UOTGC_INT_MASK_MC       = BIT0,
    VPLA_UOTGC_INT_MASK_DMA      = BIT1,
    VPLA_UOTGC_INT_MASK_CHARGER  = BIT2,
    VPLA_UOTGC_INT_MASK_DRVVBUS  = BIT3,
    VPLA_UOTGC_INT_MASK_UNDRVVBUS = BIT4,
    VPLA_UOTGC_INT_MASK_BUSRESET = BIT5,
    VPLA_UOTGC_INT_MASK_ALL      = 0x3F
} VPLA_UOTGC_INT_MASK;

typedef enum {
    /* Device speeds */
    VPLA_USB_SPEED_UNKNOWN   = 0, /* Transfer rate not yet set */
    VPLA_USB_SPEED_LOW       = 1, /* USB 1.1 */
    VPLA_USB_SPEED_FULL      = 2, /* USB 1.1 */
    VPLA_USB_SPEED_HIGH      = 3, /* USB 2.0 */
    VPLA_USB_SPEED_WIRELESS  = 4, /* Wireless USB 2.5 */
    VPLA_USB_SPEED_SUPER     = 5, /* USB 3.0 */
    VPLA_USB_SPEED_SUPER_PLUS= 6, /* USB 3.1 */
} VPLA_USB_SPEED_ENUM;

typedef void ( *VPLA_UOTGC_Callback ) ( void );

void vusb_set_id_low(void);
void vusb_set_id_high(void);
void vusb_dma_disablerd(uint8_t ep);
void vusb_dma_disablewr(uint8_t ep);
void vusb_dma_start_rd(uint8_t ep, uint32_t Addr, uint32_t Len) ;
void vusb_dma_start_wr(uint8_t ep, uint32_t Addr, uint32_t Len)	;
void vusb_controller_reset(void);
void vusb_otgc_sim_mode(void);
void vusb_otgc_dmaif_ctrl(void);
uint8_t vusb_get_active_ep(void);
void vusb_set_active_ep(uint8_t ep_index);
void vusb_session(void);
void vusb_intrx_en(uint8_t ep ,uint8_t en);
void vusb_inttx_en(uint8_t ep ,uint8_t en);
uint32_t vusb_intrx_get_en(void);
uint32_t vusb_inttx_get_en(void);
uint32_t vusb_int_state(void);
uint32_t vusb_inttx_state(void);
uint32_t vusb_intrx_state(void);
uint32_t vusb_ep0_get_csr(void);
void vusb_ep0_set_csr(uint32_t val);
uint32_t vusb_ep_tx_get_csr(uint8_t ep);
uint32_t vusb_ep_rx_get_csr(uint8_t ep);
void vusb_ep_rdy_set(uint8_t ep, uint8_t istx, uint8_t end);
void vusb_ep_rdy_clr(uint8_t ep, uint8_t istx, uint8_t end);
int vusb_set_address(const uint8_t addr);
void vusb_ep_tx_set_csr(uint8_t ep, uint32_t val);
void vusb_ep_rx_set_csr(uint8_t ep, uint32_t val);
void vusb_ep_set_rxmaxp ( uint32_t val );
void vusb_ep_set_txmaxp ( uint32_t val );
uint32_t vusb_ep_count(const uint8_t ep);

void VOTGC_CfgPhySpdm ( BOOL setval );
void VOTGC_CfgConnect ( BOOL connect );
void VOTG_SoftReset ( void );
void VOTGC_CfgDect ( BOOL setval );
void VOTGC_CfgCG ( BOOL setval );
void VOTGC_CfgHighSpeed ( BOOL highspeed );
void _VOTG_RegisterLLevel2Isr(VPLA_UOTGC_INT_ID id, VPLA_UOTGC_Callback pcall);
void _VOTG_UnDriveBusIsr(void);
void _VOTG_DriveBusIsr(void);
void _VOTG_ClearSrcPending(UINT32 src);
void _VOTG_IntSetMask(UINT32 mask);

#if CONFIG_USBDEV
void vusbd_en_interrupts(void);
int vusbd_ep_stall(const uint8_t ep, uint8_t istx, uint8_t set);
int vusbd_ep_get_rdy(uint8_t ep, uint8_t istx);
#endif


#if CONFIG_USBHOST
void vusbh_ep0_setup(void);
void vusbh_ep_rx_reqpkt(void);
void vusbh_reset_ep(const uint8_t ep, uint8_t dir);
void vusbh_power_reset(uint8_t en);
void vusbh_power_hsenab(uint8_t en);
VPLA_USB_SPEED_ENUM vusbh_get_port_speed(const uint8_t port);
void vusbh_en_interrupts(void);
void vusbh_rxtype_set(uint32_t val);
void vusbh_txtype_set(uint32_t val);
void vusbh_rxintv_set(uint32_t val);
void vusbh_txintv_set(uint32_t val);
void vusbh_ep0_txcnt_set(uint32_t val);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _VPLA_UDC_H_ */
