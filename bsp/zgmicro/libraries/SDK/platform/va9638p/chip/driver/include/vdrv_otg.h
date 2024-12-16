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
    OTG

Description:
    OTG Driver

Revision History:
    Date                  Author                   Description
    2014.05.04            yinxingjie                   initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif


#ifndef __VOTG_H__
#define __VOTG_H__

#include "vdrv_dma.h"


/***********************中断的处理*************************/
#define __UOTGC_CheckSrcPending()                        READ_REG(REG_UOTG_UOTGC_SRC_PND)
#define __UOTGC_ClearSrcPending(x)                        WRITE_REG(REG_UOTG_UOTGC_SRC_PND, (x))
#define __UOTGC_IntGetMask()                             READ_REG(REG_UOTG_UOTGC_INTMASK)
#define __UOTGC_IntSetMask(x)                             WRITE_REG(REG_UOTG_UOTGC_SET_MASK, (x))
#define __UOTGC_IntUnMask(x)                              WRITE_REG(REG_UOTG_UOTGC_UNSET_MASK, (x))

// Set Feaure fields
#define B_HNP_ENABLE            3

//////////////register bits///////////////////
// Power register bits
#define PWR_NULL                0x00
#define PWR_SUSE                0x01        // Enable SuspendM
#define PWR_SUSM                0x02        // Suspend Mode
#define PWR_RSM                 0x04        // Resume
#define PWR_RST                 0x08        // Reset
#define PWR_HSMOD               0x10        // HS Mode
#define PWR_HSEN                0x20        // HS Enab
#define PWR_SFTCON              0x40        // Soft Conn
#define PWR_ISOUP               0x80        // ISO Update

// DEVCTL bits
#define UOTGC_DVC_NULL                0x00
#define UOTGC_DVC_SESS                0x01        // Session
#define UOTGC_DVC_HREQ                0x02        // Host Req
#define UOTGC_DVC_HMODE               0x04        // Host Mode
#define UOTGC_DVC_VBUS                0x18        // VBus[1:0]
#define UOTGC_DVC_LSDEV               0x20        // LSDev
#define UOTGC_DVC_FSDEV               0x40        // FSDev
#define UOTGC_DVC_CID                 0x80        // B-Device

// CSR0 bits: in peripheral mode
#define UOTGC_D_CSR0_NULL               0x0000
#define UOTGC_D_CSR0_OPR                0x0001      // RxPktRdy  
#define UOTGC_D_CSR0_IPR                0x0002      // TxPktRdy  
#define UOTGC_D_CSR0_SNTS               0x0004      // SentStall 
#define UOTGC_D_CSR0_DE                 0x0008      // DataEnd   
#define UOTGC_D_CSR0_SE                 0x0010      // SetupEnd  
#define UOTGC_D_CSR0_SS                 0x0020      // SendStall 
#define UOTGC_D_CSR0_SVDOPR             0x0040      // Serviced RxPktRdy
#define UOTGC_D_CSR0_SVDSE              0x0080      // Serviced SetupEnd
//            in host mode
#define UOTGC_H_CSR0_RPR                0x0001      // RxPktRdy
#define UOTGC_H_CSR0_TPR                0x0002      // TxPktRdy
#define UOTGC_H_CSR0_RSTALL             0x0004      // RxStall
#define UOTGC_H_CSR0_SETUP              0x0008      // SetupPkt
#define UOTGC_H_CSR0_ERR                0x0010      // Error
#define UOTGC_H_CSR0_REQP               0x0020      // ReqPkt
#define UOTGC_H_CSR0_STATP              0x0040      // StatusPkt
#define UOTGC_H_CSR0_NAKL               0x0080      // NAK Timeout
#define UOTGC_H_CSR0_FF                 0x0100      // FlushFIFO

// TXCSR bits
#define UOTGC_TXCSR_NULL              0x0000
#define UOTGC_TXCSR_TPR               0x0001      // TxPktRdy
#define UOTGC_TXCSR_IPR               0x0001      // TxPktRdy
#define UOTGC_TXCSR_FNE               0x0002      // FIFO NotEmpty
#define UOTGC_TXCSR_UR                0x0004      // UnderRun
#define UOTGC_TXCSR_ERR               0x0004      // Error
#define UOTGC_TXCSR_FF                0x0008      // FlushFIFO
#define UOTGC_TXCSR_SDS               0x0010      // SendStall
#define UOTGC_TXCSR_STS               0x0020      // SentStall
#define UOTGC_TXCSR_RSTL              0x0020      // RxStall
#define UOTGC_TXCSR_CDT               0x0040      // ClrDataTog
#define UOTGC_TXCSR_ITX               0x0080      // IncompTx
#define UOTGC_TXCSR_NAKL              0x0080      // NAKTimeout
#define UOTGC_TXCSR_DMAMODE           0x0400      // DMAReqMode
#define UOTGC_TXCSR_FDT               0x0800      // FrcDataTog
#define UOTGC_TXCSR_DMA               0x1000      // DMAReqEnab
#define UOTGC_TXCSR_MODE              0x2000      // Mode
#define UOTGC_TXCSR_ISO               0x4000      // ISO
#define UOTGC_TXCSR_AUTO              0x8000      // AutoSet

// RXCSR bits
#define UOTGC_RXCSR_NULL              0x0000
#define UOTGC_RXCSR_RPR               0x0001      // RxPktRdy
#define UOTGC_RXCSR_OPR               0x0001      // RxPktRdy
#define UOTGC_RXCSR_FULL              0x0002      // FIFOFull
#define UOTGC_RXCSR_OR                0x0004      // OverRun
#define UOTGC_RXCSR_ERR               0x0004      // Error
#define UOTGC_RXCSR_DE                0x0008      // DataError
#define UOTGC_RXCSR_NAKL              0x0008      // NAK Timeout
#define UOTGC_RXCSR_FF                0x0010      // FlushFIFO
#define UOTGC_RXCSR_SDS               0x0020      // SendStall
#define UOTGC_RXCSR_REQP              0x0020      // ReqPkt
#define UOTGC_RXCSR_STS               0x0040      // SentStall
#define UOTGC_RXCSR_RSTL              0x0040      // RxStall
#define UOTGC_RXCSR_CDT               0x0080      // ClrDataTog
#define UOTGC_RXCSR_IRX               0x0100      // IncompRx
#define UOTGC_RXCSR_DMAMODE           0x0800      // DMAReqMode
#define UOTGC_RXCSR_DISNYET           0x1000      // DisNyet
#define UOTGC_RXCSR_DMA               0x2000      // DMAReqEnab
#define UOTGC_RXCSR_ISO               0x4000      // ISO
#define UOTGC_RXCSR_AREQ              0x4000      // AutoReq
#define UOTGC_RXCSR_AUTO              0x8000      // AutoClear

// UOTGC GATE bits
#define      UOTGC_GATE_UOTGEN      1
#define      UOTGC_GATE_PHY_BISTEN  (1<<1)
#define      UOTGC_GATE_PHY_RST     (1<<2)
#define      UOTGC_GATE_PHY_BISTRO  (1<<3)
#define      UOTGC_GATE_PHY_DETEN   (1<<4)

// UOTGC PHY STRAP
#define      UOTGC_PHYSTRAP_SUSPEM    (1<<13)
#define      UOTGC_PHYSTRAP_IDPULLUP  (1<<14)

//UOTGC SIM MODE bits
#define UOTGC_SIMMODE_SIMUL 1
#define UOTGC_SIMMODE_NEG_SEL   (1<<1)


//USB transfer type for RXTYPE
#define UOTGC_TFTYPE_ISOC               0x10
#define UOTGC_TFTYPE_BULK               0x20
#define UOTGC_TFTYPE_INTR               0x30

//endpoint ID define
#define VOTG_EP0 0
#define VOTG_EP1 1
#define VOTG_EP2 2

//define device and host
#define HOST_TYPE 0
#define DEVICE_TYPE 1


////////////////for setup command ///////////////////
// Device Request Type Field
//req type
#define USB_CMD_TYPEMASK            0x60
#define USB_CMD_STDREQ              0x00
#define USB_CMD_CLASSREQ            0x20
#define USB_CMD_VENDREQ             0x40
//req dir
#define USB_CMD_DIRMASK             0x80
#define USB_CMD_D2H_IN              0x80
#define USB_CMD_H2D_OUT             0x00
//req recip
#define USB_CMD_RECIPMASK           0x1F
#define USB_CMD_RECIP_DEV            0x0
#define USB_CMD_RECIP_IF             0x1
#define USB_CMD_RECIP_EP             0x2
#define USB_CMD_RECIP_OTHER          0x03

// Standard Request Codes
#define USB_REQ_GET_STATUS              0x00
#define USB_REQ_CLEAR_FEATURE           0x01
#define USB_REQ_SET_FEATURE             0x03
#define USB_REQ_SET_ADDRESS             0x05
#define USB_REQ_GET_DESCRIPTOR          0x06
#define USB_REQ_SET_DESCRIPTOR          0x07
#define USB_REQ_GET_CONFIGURATION       0x08
#define USB_REQ_SET_CONFIGURATION       0x09
#define USB_REQ_GET_INTERFACE           0x0A
#define USB_REQ_SET_INTERFACE           0x0B
#define USB_REQ_SYNCH_FRAME             0x0C
#define USB_REQ_GET_LUN           0xFE
#define USB_REQ_BOT_RESET             0xFF

/* USB GET_STATUS Bit Values */
#define USB_GETSTATUS_SELF_POWERED             0x01
#define USB_GETSTATUS_REMOTE_WAKEUP            0x02
#define USB_GETSTATUS_ENDPOINT_STALL           0x01

/* USB Standard Feature selectors */
#define USB_FEATURE_ENDPOINT_STALL             0
#define USB_FEATURE_REMOTE_WAKEUP              1
#define USB_FEATURE_TESTMODE                   2

/*
* Endpoints
*/
#define USB_ENDPOINT_NUMBER_MASK    0x0f    /* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK       0x80

/*
* USB directions
*/
#define USB_DIR_OUT         0
#define USB_DIR_IN          0x80


////////////////for  common descriptors//////////////////
#undef WBVAL
#define WBVAL(x) ((x) & 0xFF),(((x) >> 8) & 0xFF)
#define B3VAL(x) ((x) & 0xFF),(((x) >> 8) & 0xFF),(((x) >> 16) & 0xFF)

#define USB_CONFIGUARTION_DESC_SIZE       9
#define USB_DEVICE_DESC_SIZE              18
#define USB_INTERFACE_DESC_SIZE           9
#define USB_ENDPOINT_DESC_SIZE            7
#define USB_DEVQUAL_DESC_SIZE            10
#define USB_OTHER_SPEED_CONFIGUARTION_DESC_SIZE       9


/* USB Descriptor Types */
#define USB_DEVICE_DESCRIPTOR_TYPE             1
#define USB_CONFIGURATION_DESCRIPTOR_TYPE      2
#define USB_STRING_DESCRIPTOR_TYPE             3
#define USB_INTERFACE_DESCRIPTOR_TYPE          4
#define USB_ENDPOINT_DESCRIPTOR_TYPE           5
#define USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE   6
#define USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE 7
#define USB_INTERFACE_POWER_DESCRIPTOR_TYPE    8
#define USB_OTG_DESCRIPTOR_TYPE                9
#define USB_HID_DESCRIPTOR_TYPE                33
#define USB_RPT_DESCRIPTOR_TYPE                34

/* USB Device Classes */
#define USB_DEVICE_CLASS_RESERVED              0x00
#define USB_DEVICE_CLASS_AUDIO                 0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS        0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE       0x03
#define USB_DEVICE_CLASS_MONITOR               0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE    0x05
#define USB_DEVICE_CLASS_POWER                 0x06
#define USB_DEVICE_CLASS_PRINTER               0x07
#define USB_DEVICE_CLASS_STORAGE               0x08
#define USB_DEVICE_CLASS_HUB                   0x09
#define USB_DEVICE_CLASS_DATA                  0X0A
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC       0xFF

/* bmAttributes in Endpoint Descriptor */
#define USB_ENDPOINT_TYPE_MASK                 0x03
#define USB_ENDPOINT_TYPE_CONTROL              0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS          0x01
#define USB_ENDPOINT_TYPE_BULK                 0x02
#define USB_ENDPOINT_TYPE_INTERRUPT            0x03

#define USB_ENDPOINT_SYNC_MASK                 0x0C
#define USB_ENDPOINT_SYNC_NO_SYNCHRONIZATION   0x00
#define USB_ENDPOINT_SYNC_ASYNCHRONOUS         0x04
#define USB_ENDPOINT_SYNC_ADAPTIVE             0x08
#define USB_ENDPOINT_SYNC_SYNCHRONOUS          0x0C

#define USB_ENDPOINT_USAGE_MASK                0x30
#define USB_ENDPOINT_USAGE_DATA                0x00
#define USB_ENDPOINT_USAGE_FEEDBACK            0x10
#define USB_ENDPOINT_USAGE_IMPLICIT_FEEDBACK   0x20
#define USB_ENDPOINT_USAGE_RESERVED            0x30

/* bmAttributes in Configuration Descriptor */
#define USB_CONFIG_POWERED_MASK                0xC0
#define USB_CONFIG_BUS_POWERED                 0x80
#define USB_CONFIG_SELF_POWERED                0x40
#define USB_CONFIG_REMOTE_WAKEUP               0x20

/* bMaxPower in Configuration Descriptor */
#define USB_CONFIG_POWER_MA(mA)                ((mA)/2)

/* bEndpointAddress in Endpoint Descriptor */
#define USB_ENDPOINT_DIRECTION_MASK            0x80
#define USB_ENDPOINT_OUT(addr)                 ((addr) | USB_DIR_OUT)
#define USB_ENDPOINT_IN(addr)                  ((addr) | USB_DIR_IN)

/* Test modes */
#define VUDC_TESTMODE_TEST_J        0x01
#define VUDC_TESTMODE_TEST_K        0x02
#define VUDC_TESTMODE_TEST_SE0_NAK  0x03
#define VUDC_TESTMODE_TEST_PACKET   0x04
#define VUDC_TESTMODE_TEST_FORCE_ENABLE 0x05


/* OTG requests */
#define USB_OTG_SRP         0x01    /* bit 0 of bmAttributes */
#define USB_OTG_HNP         0x02    /* bit 1 of bmAttributes */

/* OTG with Vui */

#define OTG_HARD_WAIT_TIME      1000000//500000

//////////////OTG IP///////////////////
typedef void ( *UOTGC_Callback ) ( void );

typedef enum enum_UOTGC_MODE_ID {
    UOTGC_MODE_HOST = 0,
    UOTGC_MODE_DEVICE_STORE,
    UOTGC_MODE_DEVICE_AUDIO,
    UOTGC_MODE_DEVICE_SERIAL,
    UOTGC_MODE_DEVICE_SERIAL_COM_FIX,
    UOTGC_MODE_NONE,
} UOTGC_MODE_ID;

typedef enum enum_VIM_UOTGC_INT_ID {
    UOTGC_INT_ID_MC = 0,
    UOTGC_INT_ID_DMA,
    UOTGC_INT_ID_CHARGER,
    UOTGC_INT_ID_DRVVBUS,
    UOTGC_INT_ID_UNDRVVBUS,
    UOTGC_INT_ID_BUSRESET,
    UOTGC_INT_ID_PMU_PLUGIN,
    UOTGC_INT_ID_VBUSVLD,
    UOTGC_INT_ID_HSTDISCON,
    UOTGC_INT_NUMBER
} UOTGC_INT_ID;


typedef enum enum_UOTGC_INT_MASK {
    UOTGC_INT_MASK_MC       = BIT0,
    UOTGC_INT_MASK_DMA      = BIT1,
    UOTGC_INT_MASK_CHARGER  = BIT2,
    UOTGC_INT_MASK_DRVVBUS  = BIT3,
    UOTGC_INT_MASK_UNDRVVBUS = BIT4,
    UOTGC_INT_MASK_BUSRESET = BIT5,
    UOTGC_INT_MASK_ALL      = 0x3F
} UOTGC_INT_MASK;

typedef enum enum_VIM_UOTGC_USBINT_ID {
    UOTGC_USBINTR_SUS  = 0,                 // Suspend
    UOTGC_USBINTR_RES,                      // Resume
    UOTGC_USBINTR_RST,                      // Reset/Babble
    UOTGC_USBINTR_SOF,                      // SOF
    UOTGC_USBINTR_CONN,                     // Conn
    UOTGC_USBINTR_DISCON,                   // Discon
    UOTGC_USBINTR_SREQ,                     // Sess Req
    UOTGC_USBINTR_VBERR                    // VBus Error
} UOTGC_USBINT_ID;

typedef enum enum_VIM_UOTGC_USBINT_MASK {
    UOTGC_USBINTR_MASK_SUS  = 0,                 // Suspend
    UOTGC_USBINTR_MASK_RES,                      // Resume
    UOTGC_USBINTR_MASK_RST,                      // Reset/Babble
    UOTGC_USBINTR_MASK_SOF,                      // SOF
    UOTGC_USBINTR_MASK_CONN,                     // Conn
    UOTGC_USBINTR_MASK_DISCON,                   // Discon
    UOTGC_USBINTR_MASK_SREQ,                     // Sess Req
    UOTGC_USBINTR_MASK_VBERR                    // VBus Error
} UOTGC_USBINT_MASK;

typedef enum enum_VIM_UOTGC_EPINT_ID {
    UOTGC_EPINTR_0  = 0,
    UOTGC_EPINTR_1,
    UOTGC_EPINTR_2
} UOTGC_EPINT_ID;


typedef enum enum_VIM_UOTGC_EPINT_MASK {
    UOTGC_EPINTR_MASK_0  = 0,
    UOTGC_EPINTR_MASK_1,
    UOTGC_EPINTR_MASK_2
} UOTGC_EPINT_MASK;

typedef enum enum_VIM_UOTGC_CTRL_XFER_STATES {
    UOTGC_CTRL_ST_SETUP  = 0,
    UOTGC_CTRL_ST_DATA_OUT,
    UOTGC_CTRL_ST_DATA_IN,
    UOTGC_CTRL_ST_STATUS
} UOTGC_CTRL_XFER_STATES;

typedef struct {
    UINT8 bmRequestType;
    UINT8 bRequest;
    UINT16 wValue;
    UINT16 wIndex;
    UINT16 wLength;
} UOTGC_DEV_REQ;
typedef struct {

    UINT32 dCBWSignature;
    UINT32 dCBWTag;
    UINT32 dCBWDataTransferLength;
    UINT8 bmCBWFlags;
    UINT8 bCBWLUN;
    UINT8 bCBWCBLength;
    UINT8 CBWCB[16];
} UOTGC_HOST_BULK_CBW_REQ;


typedef struct {
    UINT32 dCSWSignature;
    UINT32 dCSWTag;
    UINT32 dCSWDataResidue;
    UINT8 bCSWStatus;
} UOTGC_DEV_BULK_CSW;


/////////////////for message////////////////

typedef struct _VOTG_MSG {
    void *p_msg;
    UINT32 idx;
    UINT32 len;
    UINT32 offset;
    BOOL dend;
} VOTG_MSG;

typedef struct _VUSB_MSG {
    void *p_msg;
} VUSB_MSG;

typedef struct _VUDC_MSG {
    void    *p_msg;
    UINT32  dataAddr;
    UINT32  dataSize;
    UINT32  sectorAddr;
    UINT32  sectorNum;
} VUDC_MSG;

typedef struct _VOTG_MSG_INFO {
    VOTG_MSG   msgPkt;
    UINT32     msgId;
    UINT8      MsgInd;
} VOTG_MSG_INFO;

#define VSYS_MSG_OTG_BASE            0
#define VOTG_MSG_BASE               (VSYS_MSG_OTG_BASE + 0x000)

#define VOTG_MSG_DEV_BASE           (VSYS_MSG_OTG_BASE + 0x100)
#define VOTG_MSG_DEV_EP0_NODATA             VOTG_MSG_DEV_BASE+1
#define VOTG_MSG_DEV_EP0_TX             VOTG_MSG_DEV_BASE+2
#define VOTG_MSG_DEV_EP0_RX             VOTG_MSG_DEV_BASE+3
#define VOTG_MSG_DEV_EP1_TX             VOTG_MSG_DEV_BASE+4
#define VOTG_MSG_DEV_EP1_RX             VOTG_MSG_DEV_BASE+5
#define VOTG_MSG_DEV_EP2_TX             VOTG_MSG_DEV_BASE+6
#define VOTG_MSG_DEV_EP2_RX             VOTG_MSG_DEV_BASE+7
#define VOTG_MSG_DEV_EP1_TSTALL         VOTG_MSG_DEV_BASE+8
#define VOTG_MSG_DEV_EP1_RSTALL         VOTG_MSG_DEV_BASE+9
#define VOTG_MSG_DEV_EP2_TSTALL         VOTG_MSG_DEV_BASE+10
#define VOTG_MSG_DEV_EP2_RSTALL         VOTG_MSG_DEV_BASE+11
#define VOTG_MSG_DEV_EP0_STALL          VOTG_MSG_DEV_BASE+12
#define VOTG_MSG_DEV_READ_MEM           VOTG_MSG_DEV_BASE+13
#define VOTG_MSG_DEV_WRITE_MEM          VOTG_MSG_DEV_BASE+14
#define VOTG_MSG_DEV_SUSPEND            VOTG_MSG_DEV_BASE+15
#define VOTG_MSG_DEV_START_UAC          VOTG_MSG_DEV_BASE+16
#define VOTG_MSG_DEV_STOP_UAC           VOTG_MSG_DEV_BASE+17
#define VOTG_MSG_DEV_DSP_DBG            VOTG_MSG_DEV_BASE+20

#define VOTG_MSG_HOST_BASE              (VSYS_MSG_OTG_BASE + 0x200)
#define VOTG_MSG_HOST_RST               VOTG_MSG_HOST_BASE + 1
#define VOTG_MSG_HOST_SEND              VOTG_MSG_HOST_BASE + 2

#define VOTG_MSG_HOST_EP0_TX_DATA       VOTG_MSG_HOST_BASE + 3
#define VOTG_MSG_HOST_EP0_RX_DATA       VOTG_MSG_HOST_BASE + 4
#define VOTG_MSG_HOST_EP0_TX_STATUS     VOTG_MSG_HOST_BASE + 5
#define VOTG_MSG_HOST_EP0_RX_STATUS     VOTG_MSG_HOST_BASE + 6


#define VOTG_MSG_HOST_EP0_MASS_ENUM     VOTG_MSG_HOST_BASE +7
#define VOTG_MSG_HOST_EP0_GET_DEV       VOTG_MSG_HOST_BASE +8
#define VOTG_MSG_HOST_EP0_SET_ADDR      VOTG_MSG_HOST_BASE +9
#define VOTG_MSG_HOST_EP0_GET_CONF         VOTG_MSG_HOST_BASE +10
#define VOTG_MSG_HOST_EP0_SET_CONF      VOTG_MSG_HOST_BASE +11
#define VOTG_MSG_HOST_EP0_SET_IFAC      VOTG_MSG_HOST_BASE +12
#define VOTG_MSG_HOST_EP0_GET_LUN       VOTG_MSG_HOST_BASE +13
#define VOTG_MSG_HOST_EP0_GET_CONF_VAL  VOTG_MSG_HOST_BASE +14
#define VOTG_MSG_HOST_EP0_GET_IFAC_VAL  VOTG_MSG_HOST_BASE +15
//#define VOTG_MSG_HOST_EP0_DEVICE_CHECK    VOTG_MSG_HOST_BASE +15
#define VOTG_MSG_HOST_EP0_CLEAR_HALT    VOTG_MSG_HOST_BASE +16
#define VOTG_MSG_HOST_INQUIRY_DISK_PARA VOTG_MSG_HOST_BASE +17
#define VOTG_MSG_HOST_READ_FORMAT_CAPA VOTG_MSG_HOST_BASE +18
#define VOTG_MSG_HOST_READ_CAPA         VOTG_MSG_HOST_BASE +19
#define VOTG_MSG_HOST_REQUEST_SENSE     VOTG_MSG_HOST_BASE +20
#define VOTG_MSG_HOST_TEST_UNIT_READY   VOTG_MSG_HOST_BASE +21
#define VOTG_MSG_HOST_READ_SMC          VOTG_MSG_HOST_BASE +22
#define VOTG_MSG_HOST_WRITE_SMC         VOTG_MSG_HOST_BASE +23
#define VOTG_MSG_HOST_READ_SMC_ERR      VOTG_MSG_HOST_BASE +24
#define VOTG_MSG_HOST_WRITE_SMC_ERR     VOTG_MSG_HOST_BASE +25


#define  VOTG_MSG_HOST_EP1_TX_CBW          VOTG_MSG_HOST_BASE +26
#define  VOTG_MSG_HOST_EP1_TX_DATA         VOTG_MSG_HOST_BASE +27
#define  VOTG_MSG_HOST_EP1_RX                  VOTG_MSG_HOST_BASE +28
#define  VOTG_MSG_HOST_EP2_TX                  VOTG_MSG_HOST_BASE +29
#define  VOTG_MSG_HOST_EP2_RX_DATA      VOTG_MSG_HOST_BASE +30
#define  VOTG_MSG_HOST_EP2_RX_CSW       VOTG_MSG_HOST_BASE +31
#define  VOTG_MSG_HOST_EP1_CLEAR_HALT   VOTG_MSG_HOST_BASE +32
#define  VOTG_MSG_HOST_EP2_CLEAR_HALT   VOTG_MSG_HOST_BASE +33
#define  VOTG_MSG_HOST_SMC_READY            VOTG_MSG_HOST_BASE +34
#define  VOTG_MSG_HOST_SMC_NOTREADY         VOTG_MSG_HOST_BASE +35
#define  VOTG_MSG_HOST_IDLE                VOTG_MSG_HOST_BASE +36

//define buffers for endpoints
#define VOTG_EP0_TXBUF_SIZE 64
#define VOTG_EP0_RXBUF_SIZE 128
#define VOTG_EP1_TXBUF_SIZE 64  //60 //VUSB_PRINT_BUFFER_MAX_SIZE//1024
#define VOTG_EP1_RXBUF_SIZE 200
#define VOTG_EP2_TXBUF_SIZE  4
#define VOTG_EP2_RXBUF_SIZE 1024


void    VOTG_DMA_StartWrite ( VDMA_USB_PATH chanel, UINT32    Addr,   UINT32 Len );
void    VOTG_DMA_StartRead ( VDMA_USB_PATH chanel, UINT32 Addr, UINT32 Len );

void VOTG_Interrupt_Routine_Service ( void );
void VOTG_Task ( void );
void VOTG_Print_Task ( void *p_Arg );
UINT32 VOTG_GetEp0RxByteCnt ( void );
UINT32 VOTG_GetEpxRxByteCnt ( UINT32 ep );
VRET_VALUE VOTG_MSG_Send ( UINT32 MsgID, void *p_Msg );
void VOTG_DMA_StartRead ( VDMA_USB_PATH channel, UINT32 Addr, UINT32 Len );
void VOTG_DMA_StartWrite ( VDMA_USB_PATH channel, UINT32 Addr, UINT32 Len );
void VOTGC_EnControllerInt ( void );
void VOTGC_DisISOInt ( void );
void VOTGC_EnISOInt ( void );
void VOTGC_CfgEpxMaxPktSize ( void );
BOOL VOTG_Function_Set ( BOOL funsel );
void VOTG_Function_Test ( UINT32 mode );
void VOTG_SetUsbMode ( UINT8 mode );
UINT8 VOTG_GetUsbMode ( void );
void VOTGC_Init ( void );
void VOTGC_CfgPhySpdm ( BOOL setval );
void VOTGC_CfgConnect ( BOOL connect );
void VOTG_SoftReset ( void );
void VOTGC_CfgDect ( BOOL setval );
void VOTGC_CfgCG ( BOOL setval );

UINT16 VUDC_GetDataFromHost ( UINT8 *buffer, UINT16 size );
UINT16 VUDC_SetDataToHost ( UINT8 *buffer, UINT16 size );
UINT8 VUDC_GetReadyState ( void );
void VUDC_SetReadyState ( void );
void VOTGC_CfgDect ( BOOL setval );

#define ALIGN_UP(size, align)           (((size) + (align) - 1) & ~((align) - 1))
#define DCACHE_OP_PER_SIZE              (64) // dcache flush discard??,?????
//buffers
extern UINT8 __attribute__((aligned(64))) dmabuf[ALIGN_UP(VOTG_EP0_RXBUF_SIZE, DCACHE_OP_PER_SIZE)];        // EP0 RX   枚举
extern UINT8 __attribute__((aligned(64))) dmatxbuf[ALIGN_UP(VOTG_EP0_TXBUF_SIZE, DCACHE_OP_PER_SIZE)];      // EP0 TX   枚举
extern UINT8 __attribute__((aligned(64))) dmabuf1[ALIGN_UP(VOTG_EP1_RXBUF_SIZE, DCACHE_OP_PER_SIZE)];       // EP1 RX
extern UINT8 __attribute__((aligned(64))) dmatxbuf1[ALIGN_UP(VOTG_EP1_TXBUF_SIZE, DCACHE_OP_PER_SIZE)];     // EP1 TX   buckin
extern UINT8 __attribute__((aligned(64))) dmabuf2[ALIGN_UP(VOTG_EP2_RXBUF_SIZE, DCACHE_OP_PER_SIZE)];       // EP2 RX   buckout
extern UINT8 __attribute__((aligned(64))) dmatxbuf2[ALIGN_UP(VOTG_EP2_TXBUF_SIZE, DCACHE_OP_PER_SIZE)];     // EP2 TX

extern BOOL USB_audioDev;

#endif  /* __VOTG_H__ */


