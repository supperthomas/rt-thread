
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VHD_CPORT_H_
#define _VHD_CPORT_H_

#define VRET_DRV_HDS_ERROR_BASE                         0xB00
#define	VRET_DRV_HDS_TIMEOUT                            (VRET_DRV_HDS_ERROR_BASE + 0)
#define	VRET_DRV_HDS_PARAM_ERROR                        (VRET_DRV_HDS_ERROR_BASE + 1)
#define	VRET_DRV_HDS_STATUS_ERROR                       (VRET_DRV_HDS_ERROR_BASE + 2)
#define	VRET_DRV_HDS_CMD_ERROR                          (VRET_DRV_HDS_ERROR_BASE + 3)

#define VHDS_DONE_RETRY     (3000)

typedef enum {
    VHD_DMA_SIZE_24BIT = 0,
    VHD_DMA_SIZE_16BIT = 1  //2 valid 16b data
} VHD_DMA_SIZE;

typedef enum {
    VHD_CM = 0,
    VHD_DSP = 1
} VHD_MEM_SEL;

void VHDS_Initialize(UINT8 div);
void VHDS_HW_Bclk_Enable(void);
void VHDS_HW_Bclk_Disable(void);
void VHDS_Finit(void);
void VHDS_SetRdAudio(void);
void VHDS_DisRdAudio(void);
void VHDS_DisWrAudio(void);
void VHDS_DisDMAMode(void);
void VHDS_EnClock(void);
void VHDS_DisClock(void);
void VHDS_EnCtrl(void);
void VHDS_DisCtrl(void);
void VHDS_SetClock(UINT8 div);
UINT8 VHDS_GetClock(void);
void VHDS_DataSizeConfig(VHD_DMA_SIZE dma_size);
UINT32 VHDS_GetCtrlConfig(void);
VRET_VALUE VHDS_WriteCmd(UINT8 addr, UINT8 val);
VRET_VALUE VHDS_ReadCmd(UINT8 addr, UINT8 *p_val);
void VHDS_WriteAudioData(VHD_DMA_SIZE size);
void VHDS_ReadAudioData(VHD_DMA_SIZE size);


#define ADC_CLK_MODE        (PDM_6M)
#define PDM_1M              (0)
#define PDM_2M              (1)
#define PDM_4M              (2)
#define PDM_6M              (3)

/* HD Ctrl bit definition */
#define RBIT_HD_CTRL_DMA_MODE_EN   (10)
#define RBIT_HD_CTRL_DATA_SIZE     (9)
#define RBIT_HD_CTRL_RDFIFO_RST    (8)
#define RBIT_HD_CTRL_WRFIFO_RST    (7)
#define RBIT_HD_CTRL_CLK_EN        (6)
#define RBIT_HD_CTRL_HD_I2C_SEL    (5)
#define RBIT_HD_CTRL_TX_EN         (4)
#define RBIT_HD_CTRL_RX_EN         (3)
#define RBIT_HD_CTRL_CMD_WR        (2)
#define RBIT_HD_CTRL_CMD_RD        (1)
#define RBIT_HD_CTRL_EN            (0)
/* HD Status bit definition */
#define RBIT_HD_STATUS_CMDERR      (1)
#define RBIT_HD_STATUS_DONE        (0)

#if 0
/* DMA Interface */
/* HDS channel enable */
#define VDMA_HDS_BOTH_CHANNEL_DISABLE		0x00
#define VDMA_HDS_LEFT_CHANNEL_ENABLE		0x01
#define VDMA_HDS_RIGHT_CHANNEL_ENABLE		0x02
#define VDMA_HDS_BOTH_CHANNEL_ENABLE		0x03

void VDMA_HDS_ConfigReadPath(VDMA_BUF Buf, UINT32 RightAddr, UINT32 LeftAddr, UINT32 Len, BOOL Stop);
void VDMA_HDS_ConfigWritePath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop);
void VDMA_HDS_ReadEnable(UINT8 RdEn);
void VDMA_HDS_WriteEnable(UINT8 WrEn);
void VDMA_HDS_ReadConfig(BOOL RdSrc, BOOL RdDst, BOOL mode);
void VDMA_HDS_WriteConfig(BOOL WrSrc, BOOL WrDst, BOOL mode);
void VDMA_HDS_ReadAction(VDMA_ACTION Action);
void VDMA_HDS_WriteAction(VDMA_ACTION Action);
UINT32 VDMA_HDS_GetReadCount(void);
UINT32 VDMA_HDS_GetWritenCount(void);
VDMA_BUF VDMA_HDS_GetReadActiveBuffer(void);
VDMA_BUF VDMA_HDS_GetWriteActiveBuffer(void);
BOOL VDMA_HDS_IsReadStopped(void);
BOOL VDMA_HDS_IsWriteStopped(void);
VDMA_BUF VDMA_HDS_GetReadIntSource(void);
VDMA_BUF VDMA_HDS_GetWriteIntSource(void);
UINT32 VDMA_HDS_GetReadBufferLength(VDMA_BUF Buf);
UINT32 VDMA_HDS_GetWriteBufferLength(VDMA_BUF Buf);
void VDMA_HDS_GetWriteBufAddress(UINT32* p_Buf0, UINT32* p_Buf1);
void VDMA_HDS_GetReadBufAddress(UINT32* p_RBuf0, UINT32* p_RBuf1, UINT32* p_LBuf0, UINT32* p_LBuf1);
#endif

VRET_VALUE VHDS_register_read(UINT8 addr, UINT8 *p_val);
VRET_VALUE VHDS_register_write(UINT8 addr, UINT8 val);

#endif
