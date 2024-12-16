
#ifndef _VDRV_9900_H_
#define _VDRV_9900_H_

enum{
    INIT_9900_OPEN_TX_RX   = 0,
    INIT_9900_CLOSE_TX_RX
};
extern UINT8 g_is_9900_exist;
#define HW_RADIO_9900_REG_TOTAL_NUM 0x0E

/* 9900 reginster table */
#define VA9900_BASE                 (0x00000000)
#define REG_FEM_LNA_BIAS            (VA9900_BASE + 0x0)
#define REG_FEM_LNA_GC              (VA9900_BASE + 0x1)
#define REG_FEM_LNA_CAP             (VA9900_BASE + 0x2)
#define REG_FEM_PA_GM_BIAS          (VA9900_BASE + 0x3)
#define REG_FEM_PA_GC               (VA9900_BASE + 0x4)
#define REG_FEM_PA_CAS_BIAS         (VA9900_BASE + 0x5)
#define REG_FEM_DET_SEL             (VA9900_BASE + 0x6)
#define REG_FEM_LNA_BYP             (VA9900_BASE + 0x7)
#define REG_FEM_RXON                (VA9900_BASE + 0x8)
#define REG_FEM_TXON                (VA9900_BASE + 0x9)
#define REG_FEM_DET_EN              (VA9900_BASE + 0xA)
#define REG_FEM_INT_CTR_EN          (VA9900_BASE + 0xB)
#define REG_FEM_VDD1P2_SEL          (VA9900_BASE + 0xC)
#define REG_FEM_RESERVE1            (VA9900_BASE + 0xD)

typedef struct HW_RADIO_9900_REG_INIT_TAG {
    UINT8 regs_valid[ ( HW_RADIO_9900_REG_TOTAL_NUM + 8 ) >> 3];
    UINT8 regs_init[HW_RADIO_9900_REG_TOTAL_NUM];
    UINT8 external_data_valid;
} HW_RADIO_9900_REG_INIT;

void Enable_VA9900 ( void );
void Disable_VA9900 ( void );

VRET_VALUE DRV_9900_Init( UINT8 isClose9900 );
VRET_VALUE DRV_9900_WriteReg ( UINT8 Reg, UINT8 Value );
VRET_VALUE DRV_9900_ReadReg ( UINT8 Reg, UINT8 *p_Value );
UINT8 Is_VA9900_VersionChip ( void );

#endif /* _VDRV_9900_H_ */

