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
        wd

Description:
        wd Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vpla_udc.h"
#include "vreg_def.h"
#include "vdebug_errno.h"
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vdrv_dma.h"
#include "vdrv_otg.h"

//*****************************************************************************
//
// The following are defines for the bit fields in the USB_O_DEVCTL register.
//
//*****************************************************************************
#define USB_DEVCTL_DEV          0x00000080  // Device Mode (OTG only)
#define USB_DEVCTL_FSDEV        0x00000040  // Full-Speed Device Detected
#define USB_DEVCTL_LSDEV        0x00000020  // Low-Speed Device Detected
#define USB_DEVCTL_VBUS_M       0x00000018  // VBUS Level (OTG only)
#define USB_DEVCTL_VBUS_NONE    0x00000000  // Below SessionEnd
#define USB_DEVCTL_VBUS_SEND    0x00000008  // Above SessionEnd, below AValid
#define USB_DEVCTL_VBUS_AVALID  0x00000010  // Above AValid, below VBUSValid
#define USB_DEVCTL_VBUS_VALID   0x00000018  // Above VBUSValid
#define USB_DEVCTL_HOST         0x00000004  // Host Mode
#define USB_DEVCTL_HOSTREQ      0x00000002  // Host Request (OTG only)
#define USB_DEVCTL_SESSION      0x00000001  // Session Start/End (OTG only)

//*****************************************************************************
//
// The following are defines for the bit fields in the USB_O_IE register.
//
//*****************************************************************************
#define USB_IE_VBUSERR          0x00000080  // Enable VBUS Error Interrupt (OTG
                                            // only)
#define USB_IE_SESREQ           0x00000040  // Enable Session Request (OTG
                                            // only)
#define USB_IE_DISCON           0x00000020  // Enable Disconnect Interrupt
#define USB_IE_CONN             0x00000010  // Enable Connect Interrupt
#define USB_IE_SOF              0x00000008  // Enable Start-of-Frame Interrupt
#define USB_IE_BABBLE           0x00000004  // Enable Babble Interrupt
#define USB_IE_RESET            0x00000004  // Enable RESET Interrupt
#define USB_IE_RESUME           0x00000002  // Enable RESUME Interrupt
#define USB_IE_SUSPND           0x00000001  // Enable SUSPEND Interrupt

//*****************************************************************************
//
// The following are defines for the bit fields in the USB_O_TXIE register.
//
//*****************************************************************************
#define USB_TXIE_EP7            0x00000080  // TX Endpoint 7 Interrupt Enable
#define USB_TXIE_EP6            0x00000040  // TX Endpoint 6 Interrupt Enable
#define USB_TXIE_EP5            0x00000020  // TX Endpoint 5 Interrupt Enable
#define USB_TXIE_EP4            0x00000010  // TX Endpoint 4 Interrupt Enable
#define USB_TXIE_EP3            0x00000008  // TX Endpoint 3 Interrupt Enable
#define USB_TXIE_EP2            0x00000004  // TX Endpoint 2 Interrupt Enable
#define USB_TXIE_EP1            0x00000002  // TX Endpoint 1 Interrupt Enable
#define USB_TXIE_EP0            0x00000001  // TX and RX Endpoint 0 Interrupt
                                            // Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the USB_O_RXIE register.
//
//*****************************************************************************
#define USB_RXIE_EP7            0x00000080  // RX Endpoint 7 Interrupt Enable
#define USB_RXIE_EP6            0x00000040  // RX Endpoint 6 Interrupt Enable
#define USB_RXIE_EP5            0x00000020  // RX Endpoint 5 Interrupt Enable
#define USB_RXIE_EP4            0x00000010  // RX Endpoint 4 Interrupt Enable
#define USB_RXIE_EP3            0x00000008  // RX Endpoint 3 Interrupt Enable
#define USB_RXIE_EP2            0x00000004  // RX Endpoint 2 Interrupt Enable
#define USB_RXIE_EP1            0x00000002  // RX Endpoint 1 Interrupt Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the USB_O_CSRL0 register.
//
//*****************************************************************************
#define USB_CSRL0_NAKTO         0x00000080  // NAK Timeout
#define USB_CSRL0_SETENDC       0x00000080  // Setup End Clear
#define USB_CSRL0_STATUS        0x00000040  // STATUS Packet
#define USB_CSRL0_RXRDYC        0x00000040  // RXRDY Clear
#define USB_CSRL0_REQPKT        0x00000020  // Request Packet
#define USB_CSRL0_STALL         0x00000020  // Send Stall
#define USB_CSRL0_SETEND        0x00000010  // Setup End
#define USB_CSRL0_ERROR         0x00000010  // Error
#define USB_CSRL0_DATAEND       0x00000008  // Data End
#define USB_CSRL0_SETUP         0x00000008  // Setup Packet
#define USB_CSRL0_STALLED       0x00000004  // Endpoint Stalled
#define USB_CSRL0_TXRDY         0x00000002  // Transmit Packet Ready
#define USB_CSRL0_RXRDY         0x00000001  // Receive Packet Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the USB_O_RXCSRL1 register.
//
//*****************************************************************************
#define USB_RXCSRL1_CLRDT       0x00000080  // Clear Data Toggle
#define USB_RXCSRL1_STALLED     0x00000040  // Endpoint Stalled
#define USB_RXCSRL1_STALL       0x00000020  // Send STALL
#define USB_RXCSRL1_REQPKT      0x00000020  // Request Packet
#define USB_RXCSRL1_FLUSH       0x00000010  // Flush FIFO
#define USB_RXCSRL1_DATAERR     0x00000008  // Data Error
#define USB_RXCSRL1_NAKTO       0x00000008  // NAK Timeout
#define USB_RXCSRL1_OVER        0x00000004  // Overrun
#define USB_RXCSRL1_ERROR       0x00000004  // Error
#define USB_RXCSRL1_FULL        0x00000002  // FIFO Full
#define USB_RXCSRL1_RXRDY       0x00000001  // Receive Packet Ready

//*****************************************************************************
//
// The following are defines for the bit fields in the USB_O_TXCSRL1 register.
//
//*****************************************************************************
#define USB_TXCSRL1_NAKTO       0x00000080  // NAK Timeout
#define USB_TXCSRL1_CLRDT       0x00000040  // Clear Data Toggle
#define USB_TXCSRL1_STALLED     0x00000020  // Endpoint Stalled
#define USB_TXCSRL1_STALL       0x00000010  // Send STALL
#define USB_TXCSRL1_SETUP       0x00000010  // Setup Packet
#define USB_TXCSRL1_FLUSH       0x00000008  // Flush FIFO
#define USB_TXCSRL1_ERROR       0x00000004  // Error
#define USB_TXCSRL1_UNDRN       0x00000004  // Underrun
#define USB_TXCSRL1_FIFONE      0x00000002  // FIFO Not Empty
#define USB_TXCSRL1_TXRDY       0x00000001  // Transmit Packet Ready


//*****************************************************************************
//
// The following are defines for the bit fields in the USB_O_POWER register.
//
//*****************************************************************************
#define USB_POWER_ISOUP         0x00000080  // Isochronous Update
#define USB_POWER_SOFTCONN      0x00000040  // Soft Connect/Disconnect
#define USB_POWER_HSENAB        0x00000020  // High Speed Enable
#define USB_POWER_HSMODE        0x00000010  // High Speed Enable
#define USB_POWER_RESET         0x00000008  // RESET Signaling
#define USB_POWER_RESUME        0x00000004  // RESUME Signaling
#define USB_POWER_SUSPEND       0x00000002  // SUSPEND Mode
#define USB_POWER_PWRDNPHY      0x00000001  // Power Down PHY


#undef USB_LOG_INFO
#define USB_LOG_INFO(...)  DEBUG_PRINTF("[VPLA_UDC]", __VA_ARGS__)

#undef USB_LOG_DBG
#define USB_LOG_DBG(...)  DEBUG_PRINTF("[VPLA_UDC]", __VA_ARGS__)


/* 静态变量定义 */
static VPLA_UOTGC_Callback usb_isr_callback[VPLA_UOTGC_INT_NUMBER];

/* dma channel table define */
VDMA_USB_PATH epx_dma_channel_tb[VDMA_USB_PATH_LIMIT] = 
{
	VDMA_USB_SRAM0,
	VDMA_USB_SRAM1,
	VDMA_USB_SRAM2,
	VDMA_USB_SRAM3,
	VDMA_USB_SRAM4,
};

__WEAK void vusb_set_id_low(void)
{
    // VIO_TIMING_Set(3, 0);
}

__WEAK void vusb_set_id_high(void)
{
    // VIO_TIMING_Set(3, 1);
}

/* DMA和软件fifo相关操作 */
void vusb_dma_disablerd(uint8_t ep)
{
	VDMA_USB_PATH chanel = epx_dma_channel_tb[ep];
    if(chanel <= VDMA_USB_SRAM2){
		VDMA_MaskInt(VDMA_INT_MC_USB_SRAM_DONE_MSK0 << chanel);
		VDMA_USB_ReadAction(chanel, VDMA_ACTION_ABORT);
		VDMA_USB_ReadEnable(chanel, FALSE);
	}
    else{
		uint8_t shift = (chanel - VDMA_USB_SRAM3);
		VDMA_MaskInt_1(VDMA_INT_MC_USB_SRAM_DONE_MSK3 << shift );
		VDMA_USB_ReadAction(chanel, VDMA_ACTION_ABORT);
		VDMA_USB_ReadEnable(chanel, FALSE);              
	}
}

void vusb_dma_disablewr(uint8_t ep)
{
	VDMA_USB_PATH chanel = epx_dma_channel_tb[ep];
    if(chanel <= VDMA_USB_SRAM2){
		VDMA_MaskInt(VDMA_INT_MC_SRAM_USB_DONE_MSK0 << chanel);
		VDMA_USB_WriteAction(chanel, VDMA_ACTION_ABORT);
		VDMA_USB_WriteEnable(chanel, FALSE);
	}
    else{
		uint8_t shift = (chanel - VDMA_USB_SRAM3);
		VDMA_MaskInt_1(VDMA_INT_MC_SRAM_USB_DONE_MSK3 << shift);
        VDMA_USB_WriteAction(chanel, VDMA_ACTION_ABORT);
        VDMA_USB_WriteEnable(chanel, FALSE);
	}
}

void vusb_dma_start_rd(uint8_t ep, uint32_t Addr, uint32_t Len) 
{
	VDMA_USB_PATH chanel = epx_dma_channel_tb[ep];
	vusb_dma_disablerd(ep); //	Disable	both read	and	write	channels before	a	new	start.
	VDMA_USB_ReadEnable(chanel, TRUE);
	VDMA_USB_ConfigReadPath(chanel, Addr, Len); 
	VDMA_USB_ReadAction(chanel, VDMA_ACTION_START);
}

void vusb_dma_start_wr(uint8_t ep, uint32_t Addr, uint32_t Len)	
{
	VDMA_USB_PATH chanel = epx_dma_channel_tb[ep];
	vusb_dma_disablewr(ep); //	Disable	both read	and	write	channels before	a	new	start.
	VDMA_USB_WriteEnable(chanel, TRUE);
	VDMA_USB_ConfigWritePath(chanel, Addr,	Len);
	VDMA_USB_WriteAction(chanel, VDMA_ACTION_START);
	if (0 == ep) {
        WRITE_REG ( REG_UOTG_EP0_TXCNT, Len );
    } else {
        WRITE_REG ( REG_UOTG_EP_TXBYTECNT, Len );
    }
}

void _VOTG_RegisterLLevel2Isr(VPLA_UOTGC_INT_ID id, VPLA_UOTGC_Callback pcall)
{
    UINT32 mask = 0;

    usb_isr_callback[id] = pcall;

    mask = 1 << id;
    __UOTGC_ClearSrcPending(mask);
    __UOTGC_IntUnMask(mask);
}

void _VOTG_UnDriveBusIsr(void)
{
    UINT32 reg = READ_REG(REG_UOTG_DEVCTL);
    USB_LOG_INFO("There is a UOTG UnDriveBus Interupt! DEVCTL %x \r\n", reg);
}

void _VOTG_DriveBusIsr(void)
{
    UINT32 reg = READ_REG(REG_UOTG_DEVCTL);
    USB_LOG_INFO("There is a UOTG DriveBus Interupt!,DEVCTL %X\r\n", reg);
}

static void _VOTG_ClearInt(UINT32 BitMasks)
{
    UINT32 reg;
    reg	= __UOTGC_CheckSrcPending();
    __UOTGC_ClearSrcPending(reg | (BitMasks & UOTGC_INT_MASK_ALL));
}


void _VOTG_ClearSrcPending(UINT32 src)
{
    __UOTGC_ClearSrcPending(src);
}

void _VOTG_IntSetMask(UINT32 mask)
{
    __UOTGC_IntSetMask(mask);
}



void VOTG_Interrupt_Routine_Service(void)
{
    UINT32 i;
    UINT32 irq_id;
    UINT32 fnfh_reg	= __UOTGC_CheckSrcPending();

    for (i = UOTGC_INT_NUMBER; i > 0; i--) {
        irq_id = i - 1;
        if(fnfh_reg	& (1 << irq_id)) {
            _VOTG_ClearInt(1 << irq_id);
            if(usb_isr_callback[irq_id] != NULL)
                (usb_isr_callback[irq_id])();
        }
    }
}

void vusb_controller_reset(void)
{
	WRITE_REG(REG_UOTG_UOTGC_RSTN, 0);
    vdelayus(100);
	WRITE_REG(REG_UOTG_UOTGC_RSTN, 1);
    vdelayus(100);
}

void vusb_otgc_sim_mode(void)
{
	WRITE_REG ( REG_UOTG_UOTGC_SIM_MODE, UOTGC_SIMMODE_NEG_SEL );
}

void vusb_otgc_dmaif_ctrl(void)
{
	 WRITE_REG(REG_UOTG_DMAIF_CTRL, 0x2);
}

/* get current active ep */
uint8_t vusb_get_active_ep(void)
{
    return REG(REG_UOTG_INDEX);
}

/* set the active ep */
void vusb_set_active_ep(uint8_t ep_index)
{
    REG(REG_UOTG_INDEX) = ep_index;
}

void vusb_session(void)
{
	 REG(REG_UOTG_DEVCTL) |= USB_DEVCTL_SESSION;
}

int vusb_set_address(const uint8_t addr)
{
    REG(REG_UOTG_FADDR) = addr;

    return 0;
}

void vusb_intrx_en(uint8_t ep ,uint8_t en)
{
	if (en)
	{
		REG(REG_UOTG_INTRRXE) |= (1 << ep);
	}
	else
	{
		REG(REG_UOTG_INTRRXE) &= ~(1 << ep);
	}
}

void vusb_inttx_en(uint8_t ep ,uint8_t en)
{
	if (en)
	{
		REG(REG_UOTG_INTRTXE) |= (1 << ep);
	}
	else
	{
		REG(REG_UOTG_INTRTXE) &= ~(1 << ep);
	}
}

uint32_t vusb_intrx_get_en(void)
{
	return READ_REG(REG_UOTG_INTRRXE);
}

uint32_t vusb_inttx_get_en(void)
{
	return READ_REG(REG_UOTG_INTRTXE);
}

uint32_t vusb_int_state(void)
{
	return READ_REG(REG_UOTG_INTRUSB );
}

uint32_t vusb_inttx_state(void)
{
	return READ_REG(REG_UOTG_INTRTX);
}

uint32_t vusb_intrx_state(void)
{
	return READ_REG(REG_UOTG_INTRRX);
}

uint32_t vusb_ep0_get_csr(void)
{
	return READ_REG(REG_UOTG_EP0_CSR);
}

void vusb_ep0_set_csr(uint32_t val)
{
	WRITE_REG(REG_UOTG_EP0_CSR, val);
}

uint32_t vusb_ep_tx_get_csr(uint8_t ep)
{
	if (ep == 0x00) {
		return vusb_ep0_get_csr();
	}
	else{
		return READ_REG(REG_UOTG_EP_TXCSR);
	}
}

uint32_t vusb_ep_rx_get_csr(uint8_t ep)
{
	if (ep == 0x00) {
		return vusb_ep0_get_csr();
	}
	else{
		return READ_REG(REG_UOTG_EP_RXCSR);
	}
}

void vusb_ep_rdy_set(uint8_t ep, uint8_t istx, uint8_t end)
{
	if (istx) {
		if (ep == 0x00) {
			if (end) {
				REG(REG_UOTG_EP0_CSR) = (USB_CSRL0_TXRDY | USB_CSRL0_DATAEND);
			} else {
				REG(REG_UOTG_EP0_CSR) = USB_CSRL0_TXRDY;
			}
		} else {
			REG(REG_UOTG_EP_TXCSR) |= USB_TXCSRL1_TXRDY;
		}
	}
	else{
		if (ep == 0x00) {
			if (end) {
				REG(REG_UOTG_EP0_CSR) = (USB_CSRL0_RXRDYC | USB_CSRL0_DATAEND);
			} else {
				REG(REG_UOTG_EP0_CSR) = USB_CSRL0_RXRDYC;
			}
		} else {
			REG(REG_UOTG_EP_RXCSR) |= USB_RXCSRL1_RXRDY;
		}
	}
}


void vusb_ep_rdy_clr(uint8_t ep, uint8_t istx, uint8_t end)
{
	if (istx) {
	}else {
		if (ep == 0x00) {
			if (end) {
			REG(REG_UOTG_EP0_CSR) &= ~(USB_CSRL0_RXRDYC | USB_CSRL0_DATAEND);
			} else {
			REG(REG_UOTG_EP0_CSR) &= ~USB_RXCSRL1_RXRDY;
			}
		} else {
			REG(REG_UOTG_EP_RXCSR) &= ~USB_RXCSRL1_RXRDY;
		}
	}
}

void vusb_ep_tx_set_csr(uint8_t ep, uint32_t val)
{
	if (ep == 0x00) {
		vusb_ep0_set_csr(val);
	}
	else{
		WRITE_REG(REG_UOTG_EP_TXCSR, val);
	}
}

void vusb_ep_rx_set_csr(uint8_t ep, uint32_t val)
{
	if (ep == 0x00) {
		vusb_ep0_set_csr(val);
	}
	else{
		WRITE_REG(REG_UOTG_EP_RXCSR, val);
	}
}

void vusb_ep_set_txmaxp ( uint32_t val )
{
    WRITE_REG ( REG_UOTG_EP_TXMAXP, val );
}

void vusb_ep_set_rxmaxp ( uint32_t val )
{
    WRITE_REG ( REG_UOTG_EP_RXMAXP, val );
	/* zgmicro特有寄存器, 设置每次接受的数据为1包, 不使用多包模式 */
	WRITE_REG ( REG_UOTG_EP_PKTNUM, 1 );
}

uint32_t vusb_ep_count(const uint8_t ep)
{
	if (ep == 0)
		return READ_REG ( REG_UOTG_EP0_COUNT );
	else
		return READ_REG ( REG_UOTG_EP_RXCOUNT );
}

#if CONFIG_USBDEV

void vusbd_en_interrupts(void)
{
	 /* Enable USB interrupts */
    REG(REG_UOTG_INTRUSBE) = USB_IE_RESET | USB_IE_SUSPND;
    REG(REG_UOTG_INTRTXE) = USB_TXIE_EP0;
    REG(REG_UOTG_INTRRXE) = 0;
}

int vusbd_ep_stall(const uint8_t ep, uint8_t istx, uint8_t set)
{
	if (set)
	{
		if (istx) {
			if (ep == 0x00) {
				REG(REG_UOTG_EP0_CSR) |= (USB_CSRL0_STALL | USB_CSRL0_RXRDYC);
			} else {
				REG(REG_UOTG_EP_TXCSR) |= USB_TXCSRL1_STALL;
			}
		}
		else {
			if (ep == 0x00) {
				REG(REG_UOTG_EP0_CSR) |= (USB_CSRL0_STALL | USB_CSRL0_RXRDYC);
			} else {
				REG(REG_UOTG_EP_RXCSR) |= USB_RXCSRL1_STALL;
			}
		}
	}
	else
	{
		if (istx) {
			if (ep == 0x00) {
				REG(REG_UOTG_EP0_CSR) &= ~USB_CSRL0_STALLED;
			} else {
				// Clear the stall on an IN endpoint.
				REG(REG_UOTG_EP_TXCSR) &= ~(USB_TXCSRL1_STALL | USB_TXCSRL1_STALLED);
				// Reset the data toggle.
				REG(REG_UOTG_EP_TXCSR) |= USB_TXCSRL1_CLRDT;
			}
		}
		else {
			if (ep == 0x00) {
				REG(REG_UOTG_EP0_CSR) &= ~USB_CSRL0_STALLED;
			} else {
				// Clear the stall on an OUT endpoint.
				REG(REG_UOTG_EP_RXCSR) &= ~(USB_RXCSRL1_STALL | USB_RXCSRL1_STALLED);
				// Reset the data toggle.
				REG(REG_UOTG_EP_RXCSR) |= USB_RXCSRL1_CLRDT;
			}
		}
	}
    return 0;
}

int vusbd_ep_get_rdy(uint8_t ep, uint8_t istx)
{
	if (istx) {
		if (ep == 0x00) {
			return REG(REG_UOTG_EP0_CSR) & USB_CSRL0_TXRDY;
		} else {
			return REG(REG_UOTG_EP_TXCSR) & USB_TXCSRL1_TXRDY;
		}
	}
	return 0;
}

#endif /* CONFIG_USBDEV */

#if CONFIG_USBHOST

void vusbh_ep0_setup(void)
{
	WRITE_REG ( REG_UOTG_EP0_CSR, USB_CSRL0_TXRDY | USB_CSRL0_SETUP );
}

void vusbh_ep_rx_reqpkt(void)
{
	REG ( REG_UOTG_EP_RXCSR) |= USB_RXCSRL1_REQPKT;
}

void vusbh_reset_ep(const uint8_t ep, uint8_t dir)
{
	/* 复位USB寄存器 */
	if (0 == ep) {
		WRITE_REG(REG_UOTG_EP0_CSR, 0);
		WRITE_REG(REG_UOTG_EP0_TXCNT, 0);
	} else {
		if (dir&0x02){
			/* RX寄存器 */
			WRITE_REG(REG_UOTG_EP_PKTNUM, 0);
			WRITE_REG(REG_UOTG_EP_RXMAXP, 0);
			WRITE_REG(REG_UOTG_EP_RXTYPE, 0);
			WRITE_REG(REG_UOTG_EP_RXINTV, 0);
			WRITE_REG(REG_UOTG_EP_TXCSR, 0);
			WRITE_REG(REG_UOTG_EP_RXCSR, 0);
			vusb_intrx_en(ep,FALSE);
		}
		if (dir&0x01){
			/* TX寄存器 */
			WRITE_REG(REG_UOTG_EP_TXMAXP, 0);
			WRITE_REG(REG_UOTG_EP_TXTYPE, 0);
			WRITE_REG(REG_UOTG_EP_TXINTV, 0);
			WRITE_REG(REG_UOTG_EP_TXCSR, 0);
			vusb_inttx_en(ep,FALSE);
		}
	}
}

void vusbh_power_reset(uint8_t en)
{
	if (en) {
		REG ( REG_UOTG_POWER) |= USB_POWER_RESET;
	} else {
		REG ( REG_UOTG_POWER) &= ~(USB_POWER_RESET);
	}
}

void vusbh_power_hsenab(uint8_t en)
{
	if (en) {
		REG ( REG_UOTG_POWER) |= USB_POWER_HSENAB;
	} else {
		REG ( REG_UOTG_POWER) &= ~(USB_POWER_HSENAB);
	}
}

VPLA_USB_SPEED_ENUM vusbh_get_port_speed(const uint8_t port)
{
    VPLA_USB_SPEED_ENUM speed = VPLA_USB_SPEED_UNKNOWN;

    if (READ_REG ( REG_UOTG_POWER) & USB_POWER_HSMODE)
        speed = VPLA_USB_SPEED_HIGH;
    else if (READ_REG(REG_UOTG_DEVCTL) & USB_DEVCTL_FSDEV)
        speed = VPLA_USB_SPEED_FULL;
    else if (READ_REG(REG_UOTG_DEVCTL) & USB_DEVCTL_LSDEV)
        speed = VPLA_USB_SPEED_LOW;

    return speed;
}

void vusbh_en_interrupts(void)
{
	uint32_t regval = 	USB_IE_CONN   | USB_IE_DISCON |
						USB_IE_RESUME | USB_IE_SUSPND |
						USB_IE_SESREQ | USB_IE_VBUSERR|
						USB_IE_BABBLE;
	 /* Enable USB interrupts */
    REG(REG_UOTG_INTRUSBE) = regval;
    REG(REG_UOTG_INTRTXE) = USB_TXIE_EP0;
    REG(REG_UOTG_INTRRXE) = 0;
}

void vusbh_rxtype_set(uint32_t val)
{
    WRITE_REG(REG_UOTG_EP_RXTYPE, val);
}

void vusbh_txtype_set(uint32_t val)
{
    WRITE_REG(REG_UOTG_EP_TXTYPE, val);
}


void vusbh_rxintv_set(uint32_t val)
{
    WRITE_REG(REG_UOTG_EP_RXINTV, val);
}

void vusbh_txintv_set(uint32_t val)
{
    WRITE_REG(REG_UOTG_EP_TXINTV, val);
}

void vusbh_ep0_txcnt_set(uint32_t val)
{
	WRITE_REG ( REG_UOTG_EP0_TXCNT, val );
}
#endif /* CONFIG_USBHOST */
