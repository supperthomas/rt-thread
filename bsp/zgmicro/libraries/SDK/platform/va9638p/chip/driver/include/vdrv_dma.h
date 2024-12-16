#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VDMA_H_
#define _VDMA_H_
	
#include "vstdlib.h"
#define VDMA_AIF_ADC_BUG_FIX			1

#define VRET_DRV_DMA_READ_AIF_CNT_FAIL					0x440


/* priority among device*/
#define VDMA_DEV_PRIO_SPI_BB_AIF			0
#define VDMA_DEV_PRIO_SPI_AIF_BB			1
#define VDMA_DEV_PRIO_BB_SPI_AIF			2
#define VDMA_DEV_PRIO_AIF_BB_SPI			3

#define VDMA_DEV_PRIO_USB_SD_LCD_AIF			0
#define VDMA_DEV_PRIO_AIF_USB_SD_LCD			1
#define VDMA_DEV_PRIO_AIF_SD_USB_LCD			2


/* priority of writing and reading */
#define VDMA_DEV_RW_PRIO_WR_RD				0
#define VDMA_DEV_RW_PRIO_RD_WR				1

/* AIF channel enable */
#define VDMA_AIF_BOTH_CHANNEL_DISABLE		0x00
#define VDMA_AIF_LEFT_CHANNEL_ENABLE		0x01
#define VDMA_AIF_RIGHT_CHANNEL_ENABLE		0x02
#define VDMA_AIF_BOTH_CHANNEL_ENABLE		0x03

#define VDMA_AIF_WR_SRC_CM3								0
#define VDMA_AIF_WR_SRC_DSP								1

#define VDMA_AIF_DMAC_ADC_BUF							0	//the DMAC adc buffer most empty==0, by xiaoyi.
#define VDMA_AIF_DMAC_DAC_BUF							16  //the DMAC dac buffer most full==4*word, by xiaoyi.
#define VDMA_AIF_DIGIT_ADC_SAMPL						9	//the aif adc digit delay sample 9
#define VDMA_AIF_DIGIT_DAC_SAMPL						23  //the aif dac digit delay sample 23

#define CODEC_DSP_BITMODE_24BIT									0
#define CODEC_DSP_BITMODE_16BIT									1
#define CODEC_DSP_BITMODE_LOW24BIT								2

#define CODEC_DSP_XMEM												0
#define CODEC_DSP_YMEM												1
#define CODEC_DSP_PMEM												2

#define CODEC_SRAM2DSPDM_PEND            0
#define CODEC_SRAM2DSPDM_POST            1
#define CODEC_DSPDM2SRAM_PEND            2
#define CODEC_DSPDM2SRAM_POST            3
#define CODEC_DMA_BSP_NUM                    4

/****************************************************/
/*					Register Bit Field					*/
/****************************************************/
/*ERR interrupt mask and clear bit field */
#define VDMA_INT_MC_ALL_MSK					0xFFFFFFFF
//BB
#define VDMA_INT_MC_BB_SRAM_OVER_SHFT		0
#define VDMA_INT_MC_BB_SRAM_OVER_MSK		0x00000001
#define VDMA_INT_MC_SRAM_BB_UNDER_SHFT		1
#define VDMA_INT_MC_SRAM_BB_UNDER_MSK		0x00000002
//AES
#define VDMA_INT_MC_AES_SRAM_OVER_SHFT      2
#define VDMA_INT_MC_AES_SRAM_OVER_MSK       0x00000004
#define VDMA_INT_MC_SRAM_AES_UNDER_SHFT     3
#define VDMA_INT_MC_SRAM_AES_UNDER_MSK      0x00000008
//USB
#define VDMA_INT_MC_USB_SRAM_OVER_SHFT0		4           
#define VDMA_INT_MC_USB_SRAM_OVER_MSK0		0x00000010  
#define VDMA_INT_MC_USB_SRAM_OVER_SHFT1		5           
#define VDMA_INT_MC_USB_SRAM_OVER_MSK1		0x00000020  
#define VDMA_INT_MC_USB_SRAM_OVER_SHFT2		6           
#define VDMA_INT_MC_USB_SRAM_OVER_MSK2		0x00000040

#define VDMA_INT_MC_SRAM_USB_OVER_SHFT0		7           
#define VDMA_INT_MC_SRAM_USB_OVER_MSK0		0x00000080  
#define VDMA_INT_MC_SRAM_USB_OVER_SHFT1		8           
#define VDMA_INT_MC_SRAM_USB_OVER_MSK1		0x00000100  
#define VDMA_INT_MC_SRAM_USB_OVER_SHFT2		9           
#define VDMA_INT_MC_SRAM_USB_OVER_MSK2		0x00000200

//
#define VDMA_INT_MC_SD_SRAM_OVER_SHFT		10          
#define VDMA_INT_MC_SD_SRAM_OVER_MSK		0x00000400  
//SD unconnect to DSPRAM
//#define VDMA_INT_MC_SD_DSPDM_OVER_SHFT		11
//#define VDMA_INT_MC_SD_DSPDM_OVER_MSK		0x00000800
#define VDMA_INT_MC_SRAM_SD_UNDER_SHFT		12          
#define VDMA_INT_MC_SRAM_SD_UNDER_MSK		0x00001000

 //SPI
#define VDMA_INT_MC_SPI_SRAM_OVER_SHFT		13          
#define VDMA_INT_MC_SPI_SRAM_OVER_MSK		0x00002000  
//SPI unconnect to DSPRAM
//#define VDMA_INT_MC_SPI_DSPPM_OVER_SHFT		14
//#define VDMA_INT_MC_SPI_DSPPM_OVER_MSK		0x00004000
#define VDMA_INT_MC_SRAM_SPI_UNDER_SHFT		15          
#define VDMA_INT_MC_SRAM_SPI_UNDER_MSK		0x00008000  

//LCD
#define VDMA_INT_MC_SRAM_LCD_UNDER_SHFT		16          
#define VDMA_INT_MC_SRAM_LCD_UNDER_MSK		0x00010000  
//#define VDMA_INT_MC_DSPDM_LCD_UNDER_SHFT	17
//#define VDMA_INT_MC_DSPDM_LCD_UNDER_MSK		0x00020000

//AIF
#define VDMA_INT_MC_AIF_SRAM_OVER_SHFT		18//read
#define VDMA_INT_MC_AIF_SRAM_OVER_MSK		0x00040000  

#define VDMA_INT_MC_AIF_DSPDM_OVER_SHFT		19          
#define VDMA_INT_MC_AIF_DSPDM_OVER_MSK		0x00080000  

//I2S
#define VDMA_INT_MC_I2S_DSPDM_OVER_SHFT		20          
#define VDMA_INT_MC_I2S_DSPDM_OVER_MSK		0x00100000

#define VDMA_INT_MC_I2S_SRAM_OVER_SHFT		21
#define VDMA_INT_MC_I2S_SRAM_OVER_MSK		0x00200000

//22
//23 unuse

//AIF
#define VDMA_INT_MC_SRAM_AIF_UNDER_SHFT		24  //write
#define VDMA_INT_MC_SRAM_AIF_UNDER_MSK		0x01000000

#define VDMA_INT_MC_DSPDM_AIF_UNDER_SHFT		25
#define VDMA_INT_MC_DSPDM_AIF_UNDER_MSK		0x02000000

#define VDMA_INT_MC_DSPDM_I2S_UNDER_SHFT     26
#define VDMA_INT_MC_DSPDM_I2S_UNDER_MSK      0x04000000

#define VDMA_INT_MC_SRAM_I2S_UNDER_SHFT      27
#define VDMA_INT_MC_SRAM_I2S_UNDER_MSK       0x08000000


#define VDMA_INT_MC_HDS_SRAM_OVER_SHFT	28
#define VDMA_INT_MC_HDS_SRAM_OVER_MSK		0x10000000

#define VDMA_INT_MC_HDS_DSPDM_OVER_SHFT	29
#define VDMA_INT_MC_HDS_DSPDM_OVER_MSK		0x20000000

#define VDMA_INT_MC_SRAM_HDS_UNDER_SHFT	30
#define VDMA_INT_MC_SRAM_HDS_UNDER_MSK		0x40000000

#define VDMA_INT_MC_DSPDM_HDS_UNDER_SHFT	31
#define VDMA_INT_MC_DSPDM_HDS_UNDER_MSK		0x80000000

/*FINISH interrupt mask and clear bit field */
//BB
#define VDMA_INT_MC_BB_SRAM_DONE_SHFT		0
#define VDMA_INT_MC_BB_SRAM_DONE_MSK		0x00000001
#define VDMA_INT_MC_SRAM_BB_DONE_SHFT		1
#define VDMA_INT_MC_SRAM_BB_DONE_MSK		0x00000002
//AES
#define VDMA_INT_MC_AES_SRAM_DONE_SHFT      2
#define VDMA_INT_MC_AES_SRAM_DONE_MSK       0x00000004
#define VDMA_INT_MC_SRAM_AES_DONE_SHFT      3
#define VDMA_INT_MC_SRAM_AES_DONE_MSK       0x00000008
//USB
#define VDMA_INT_MC_USB_SRAM_DONE_SHFT0		4           
#define VDMA_INT_MC_USB_SRAM_DONE_MSK0		0x00000010  
#define VDMA_INT_MC_USB_SRAM_DONE_SHFT1		5           
#define VDMA_INT_MC_USB_SRAM_DONE_MSK1		0x00000020  
#define VDMA_INT_MC_USB_SRAM_DONE_SHFT2		6           
#define VDMA_INT_MC_USB_SRAM_DONE_MSK2		0x00000040  

#define VDMA_INT_MC_SRAM_USB_DONE_SHFT0		7           
#define VDMA_INT_MC_SRAM_USB_DONE_MSK0		0x00000080  
#define VDMA_INT_MC_SRAM_USB_DONE_SHFT1		8           
#define VDMA_INT_MC_SRAM_USB_DONE_MSK1		0x00000100  
#define VDMA_INT_MC_SRAM_USB_DONE_SHFT2		9           
#define VDMA_INT_MC_SRAM_USB_DONE_MSK2		0x00000200

//
#define VDMA_INT_MC_SD_SRAM_DONE_SHFT		10          
#define VDMA_INT_MC_SD_SRAM_DONE_MSK		0x00000400
//SD unconnect to DSPRAM

#define VDMA_INT_MC_SRAM_DSPPM_DONE_SHFT		11
#define VDMA_INT_MC_SRAM_DSPPM_DONE_MSK		0x00000800

#define VDMA_INT_MC_SRAM_SD_DONE_SHFT		12          
#define VDMA_INT_MC_SRAM_SD_DONE_MSK		0x00001000  

//SPI
#define VDMA_INT_MC_SPI_SRAM_DONE_SHFT		13          
#define VDMA_INT_MC_SPI_SRAM_DONE_MSK		0x00002000  
//SPI unconnect to DSPRAM
#define VDMA_INT_MC_SPI_DSPPM_DONE_SHFT		14
#define VDMA_INT_MC_SPI_DSPPM_DONE_MSK		0x00004000//
#define VDMA_INT_MC_SRAM_SPI_DONE_SHFT		15          
#define VDMA_INT_MC_SRAM_SPI_DONE_MSK		0x00008000  

//LCD
#define VDMA_INT_MC_SRAM_LCD_DONE_SHFT		16          
#define VDMA_INT_MC_SRAM_LCD_DONE_MSK		0x00010000  
//#define VDMA_INT_MC_DSPDM_LCD_DONE_SHFT		17
//#define VDMA_INT_MC_DSPDM_LCD_DONE_MSK		0x00020000

//AIF
#define VDMA_INT_MC_AIF_SRAM_DONE_SHFT		18//read
#define VDMA_INT_MC_AIF_SRAM_DONE_MSK		0x00040000  

#define VDMA_INT_MC_AIF_DSPDM_DONE_SHFT		19          
#define VDMA_INT_MC_AIF_DSPDM_DONE_MSK		0x00080000

//I2S
#define VDMA_INT_MC_I2S_DSPDM_DONE_SHFT		20          
#define VDMA_INT_MC_I2S_DSPDM_DONE_MSK		0x00100000

#define VDMA_INT_MC_I2S_SRAM_DONE_SHFT		21
#define VDMA_INT_MC_I2S_SRAM_DONE_MSK		0x00200000

//22
#define VDMA_INT_MC_SRAM_DSPDM_DONE_SHFT     22
#define VDMA_INT_MC_SRAM_DSPDM_DONE_MSK      0x00400000
//23 
#define VDMA_INT_MC_DSPDM_SRAM_DONE_SHFT     23
#define VDMA_INT_MC_DSPDM_SRAM_DONE_MSK      0x00800000

//AIF
#define VDMA_INT_MC_SRAM_AIF_DONE_SHFT		24//write
#define VDMA_INT_MC_SRAM_AIF_DONE_MSK		0x01000000

#define VDMA_INT_MC_DSPDM_AIF_DONE_SHFT		25
#define VDMA_INT_MC_DSPDM_AIF_DONE_MSK		0x02000000

#define VDMA_INT_MC_DSPDM_I2S_DONE_SHFT     26
#define VDMA_INT_MC_DSPDM_I2S_DONE_MSK      0x04000000

#define VDMA_INT_MC_SRAM_I2S_DONE_SHFT      27
#define VDMA_INT_MC_SRAM_I2S_DONE_MSK       0x08000000


#define VDMA_INT_MC_HDS_SRAM_DONE_SHFT	28
#define VDMA_INT_MC_HDS_SRAM_DONE_MSK		0x10000000

#define VDMA_INT_MC_HDS_DSPDM_DONE_SHFT	29
#define VDMA_INT_MC_HDS_DSPDM_DONE_MSK		0x20000000

#define VDMA_INT_MC_SRAM_HDS_DONE_SHFT	30
#define VDMA_INT_MC_SRAM_HDS_DONE_MSK		0x40000000

#define VDMA_INT_MC_DSPDM_HDS_DONE_SHFT	31
#define VDMA_INT_MC_DSPDM_HDS_DONE_MSK		0x80000000

#define VDMA_INT_MC_SRAM_SPDIF_DONE_SHFT		19
#define VDMA_INT_MC_SRAM_SPDIF_DONE_MSK		0x00080000


/******************************************************************************
 						ESPI DMA Register Bits
*******************************************************************************/
#define VDMA_INT_MC_SPI1_SRAM_DONE_SHFT_1        13
#define VDMA_INT_MC_SPI1_SRAM_DONE_MSK_1         (0x01 << 13)
#define VDMA_INT_MC_SRAM_SPI1_DONE_SHFT_1				 15          
#define VDMA_INT_MC_SRAM_SPI1_DONE_MSK_1				 (0x01 << 15)



#define VDMA_INT_MC_BB_ALL_MSK			(VDMA_INT_MC_BB_SRAM_OVER_MSK | VDMA_INT_MC_SRAM_BB_UNDER_MSK | \
	VDMA_INT_MC_BB_SRAM_DONE_MSK | VDMA_INT_MC_SRAM_BB_DONE_MSK)

/* interrupt state */
#define VDMA_INT_STATE_BB_SRAM_OVER_SHFT		0
#define VDMA_INT_STATE_BB_SRAM_OVER_MSK		0x00000001
#define VDMA_INT_STATE_SRAM_BB_UNDER_SHFT		1
#define VDMA_INT_STATE_SRAM_BB_UNDER_MSK		0x00000002
#define VDMA_INT_STATE_AES_SRAM_OVER_SHFT      2
#define VDMA_INT_STATE_AES_SRAM_OVER_MSK       0x00000004
#define VDMA_INT_STATE_SRAM_AES_UNDER_SHFT     3
#define VDMA_INT_STATE_SRAM_AES_UNDER_MSK      0x00000008
#define VDMA_INT_STATE_USB_SRAM_OVER_SHFT0		4               
#define VDMA_INT_STATE_USB_SRAM_OVER_MSK0		  0x00000010      
#define VDMA_INT_STATE_USB_SRAM_OVER_SHFT1		5               
#define VDMA_INT_STATE_USB_SRAM_OVER_MSK1		  0x00000020      
#define VDMA_INT_STATE_USB_SRAM_OVER_SHFT2		6               
#define VDMA_INT_STATE_USB_SRAM_OVER_MSK2		  0x00000040      
#define VDMA_INT_STATE_SRAM_USB_OVER_SHFT0		7               
#define VDMA_INT_STATE_SRAM_USB_OVER_MSK0		  0x00000080      
#define VDMA_INT_STATE_SRAM_USB_OVER_SHFT1		8               
#define VDMA_INT_STATE_SRAM_USB_OVER_MSK1		  0x00000100      
#define VDMA_INT_STATE_SRAM_USB_OVER_SHFT2		9               
#define VDMA_INT_STATE_SRAM_USB_OVER_MSK2		  0x00000200      
#define VDMA_INT_STATE_SD_SRAM_OVER_SHFT		  10              
#define VDMA_INT_STATE_SD_SRAM_OVER_MSK		    0x00000400      
#define VDMA_INT_STATE_SD_DSPDM_OVER_SHFT		  11              
#define VDMA_INT_STATE_SD_DSPDM_OVER_MSK		  0x00000800      
#define VDMA_INT_STATE_SRAM_SD_UNDER_SHFT		  12              
#define VDMA_INT_STATE_SRAM_SD_UNDER_MSK		  0x00001000      
#define VDMA_INT_STATE_SPI_SRAM_OVER_SHFT		  13              
#define VDMA_INT_STATE_SPI_SRAM_OVER_MSK		  0x00002000      
#define VDMA_INT_STATE_SPI_DSPPM_OVER_SHFT		14              
#define VDMA_INT_STATE_SPI_DSPPM_OVER_MSK		  0x00004000      
#define VDMA_INT_STATE_SRAM_SPI_UNDER_SHFT		15              
#define VDMA_INT_STATE_SRAM_SPI_UNDER_MSK		  0x00008000      
#define VDMA_INT_STATE_SRAM_LCD_UNDER_SHFT		16              
#define VDMA_INT_STATE_SRAM_LCD_UNDER_MSK		  0x00010000      
#define VDMA_INT_STATE_DSPDM_LCD_UNDER_SHFT	  17              
#define VDMA_INT_STATE_DSPDM_LCD_UNDER_MSK	 	0x00020000      
#define VDMA_INT_STATE_AIF_SRAM_OVER_SHFT		  18              
#define VDMA_INT_STATE_AIF_SRAM_OVER_MSK		  0x00040000      
#define VDMA_INT_STATE_AIF_DSPDM_OVER_SHFT		19              
#define VDMA_INT_STATE_AIF_DSPDM_OVER_MSK		  0x00080000      
#define VDMA_INT_STATE_I2S_DSPDM_OVER_SHFT		20              
#define VDMA_INT_STATE_I2S_DSPDM_OVER_MSK		  0x00100000      
#define VDMA_INT_STATE_SRAM_AIF_UNDER_SHFT		21              
#define VDMA_INT_STATE_SRAM_AIF_UNDER_MSK		  0x00200000      
#define VDMA_INT_STATE_DSPDM_AIF_UNDER_SHFT	  22              
#define VDMA_INT_STATE_DSPDM_AIF_UNDER_MSK		0x00400000      
#define VDMA_INT_STATE_DSPDM_SRAM_OVER_SHFT	  23              
#define VDMA_INT_STATE_DSPDM_SRAM_OVER_MSK		0x00800000      
#define VDMA_INT_STATE_SRAM_DSPDM_UNDER_SHFT	24
#define VDMA_INT_STATE_SRAM_DSPDM_UNDER_MSK	    0x01000000
#define VDMA_INT_STATE_SRAM_DSPPM_UNDER_SHFT	25
#define VDMA_INT_STATE_SRAM_DSPPM_UNDER_MSK	    0x02000000
#define VDMA_INT_STATE_DSPDM_I2S_UNDER_SHFT     26
#define VDMA_INT_STATE_DSPDM_I2S_UNDER_MSK      0x04000000
#define VDMA_INT_STATE_SRAM_I2S_UNDER_SHFT      27
#define VDMA_INT_STATE_SRAM_I2S_UNDER_MSK       0x08000000                 
				 
#define VDMA_INT_STATE_BB_SRAM_DONE_SHFT		0
#define VDMA_INT_STATE_BB_SRAM_DONE_MSK		0x00000001
#define VDMA_INT_STATE_SRAM_BB_DONE_SHFT		1
#define VDMA_INT_STATE_SRAM_BB_DONE_MSK		0x00000002
#define VDMA_INT_STATE_AES_SRAM_DONE_SHFT      2
#define VDMA_INT_STATE_AES_SRAM_DONE_MSK       0x00000004
#define VDMA_INT_STATE_SRAM_AES_DONE_SHFT      3
#define VDMA_INT_STATE_SRAM_AES_DONE_MSK       0x00000008
#define VDMA_INT_STATE_USB_SRAM_DONE_SHFT0		4              
#define VDMA_INT_STATE_USB_SRAM_DONE_MSK0		  0x00000010     
#define VDMA_INT_STATE_USB_SRAM_DONE_SHFT1		5              
#define VDMA_INT_STATE_USB_SRAM_DONE_MSK1		  0x00000020     
#define VDMA_INT_STATE_USB_SRAM_DONE_SHFT2		6              
#define VDMA_INT_STATE_USB_SRAM_DONE_MSK2		  0x00000040     
#define VDMA_INT_STATE_SRAM_USB_DONE_SHFT0		7              
#define VDMA_INT_STATE_SRAM_USB_DONE_MSK0		  0x00000080     
#define VDMA_INT_STATE_SRAM_USB_DONE_SHFT1		8              
#define VDMA_INT_STATE_SRAM_USB_DONE_MSK1		  0x00000100     
#define VDMA_INT_STATE_SRAM_USB_DONE_SHFT2		9              
#define VDMA_INT_STATE_SRAM_USB_DONE_MSK2		  0x00000200     
#define VDMA_INT_STATE_SD_SRAM_DONE_SHFT		  10             
#define VDMA_INT_STATE_SD_SRAM_DONE_MSK		    0x00000400     
#define VDMA_INT_STATE_SD_DSPDM_DONE_SHFT		  11             
#define VDMA_INT_STATE_SD_DSPDM_DONE_MSK		  0x00000800     
#define VDMA_INT_STATE_SRAM_SD_DONE_SHFT		  12             
#define VDMA_INT_STATE_SRAM_SD_DONE_MSK		    0x00001000     
#define VDMA_INT_STATE_SPI_SRAM_DONE_SHFT		  13             
#define VDMA_INT_STATE_SPI_SRAM_DONE_MSK		  0x00002000     
#define VDMA_INT_STATE_SPI_DSPPM_DONE_SHFT		14             
#define VDMA_INT_STATE_SPI_DSPPM_DONE_MSK		  0x00004000     
#define VDMA_INT_STATE_SRAM_SPI_DONE_SHFT		  15             
#define VDMA_INT_STATE_SRAM_SPI_DONE_MSK		  0x00008000     
#define VDMA_INT_STATE_SRAM_LCD_DONE_SHFT		  16             
#define VDMA_INT_STATE_SRAM_LCD_DONE_MSK		  0x00010000     
#define VDMA_INT_STATE_DSPDM_LCD_DONE_SHFT		17             
#define VDMA_INT_STATE_DSPDM_LCD_DONE_MSK		  0x00020000     
#define VDMA_INT_STATE_AIF_SRAM_DONE_SHFT		  18             
#define VDMA_INT_STATE_AIF_SRAM_DONE_MSK		  0x00040000     
#define VDMA_INT_STATE_AIF_DSPDM_DONE_SHFT		19             
#define VDMA_INT_STATE_AIF_DSPDM_DONE_MSK		  0x00080000     
#define VDMA_INT_STATE_I2S_DSPDM_DONE_SHFT		20             
#define VDMA_INT_STATE_I2S_DSPDM_DONE_MSK		  0x00100000     
//#define VDMA_INT_STATE_SRAM_AIF_DONE_SHFT		  21             
//#define VDMA_INT_STATE_SRAM_AIF_DONE_MSK		  0x00200000     
#define VDMA_INT_STATE_DSPDM_AIF_DONE_SHFT		22             
#define VDMA_INT_STATE_DSPDM_AIF_DONE_MSK		  0x00400000     
#define VDMA_INT_STATE_DSPDM_SRAM_DONE_SHFT	  23             
#define VDMA_INT_STATE_DSPDM_SRAM_DONE_MSK		0x00800000    

#define VDMA_INT_STATE_SRAM_AIF_DONE_SHFT	    24
#define VDMA_INT_STATE_SRAM_AIF_DONE_MSK		0x01000000

#define VDMA_INT_STATE_SRAM_DSPPM_DONE_SHFT	    25
#define VDMA_INT_STATE_SRAM_DSPPM_DONE_MSK		0x02000000
#define VDMA_INT_STATE_DSPDM_I2S_DONE_SHFT     26
#define VDMA_INT_STATE_DSPDM_I2S_DONE_MSK      0x04000000
#define VDMA_INT_STATE_SRAM_I2S_DONE_SHFT      27
#define VDMA_INT_STATE_SRAM_I2S_DONE_MSK       0x08000000

#define VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_BITMODE_SHFT			4
#define VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_BITMODE_MSK				0x00000010
#define VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_SRC_SHFT			7
#define VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_SRC_MSK				0x00000080
#define VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_EN_SHFT				2
#define VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_EN_MSK				0x0000000C
#define VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_ABORT_SHFT			1
#define VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_ABORT_MSK			0x00000002
#define VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_START_SHFT			0
#define VDMA_REG_SPDIF_WR_CTRL_SPDIF_WR_START_MSK			0x00000001

#define VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_STOP_SHFT           4
#define VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_STOP_MSK            0x00000010
#define VDMA_REG_SPDIF_WR_STAT_SPDIF_WRINTR_SRC_SHFT        2
#define VDMA_REG_SPDIF_WR_STAT_SPDIF_WRINTR_SRC_MSK         0x0000000C
#define VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_FE1_SHFT            1
#define VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_FE1_MSK             0x00000002
#define VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_FE0_SHFT            0
#define VDMA_REG_SPDIF_WR_STAT_SPDIF_WR_FE0_MSK             0x00000001

#define VDMA_REG_SPDIF_WR_ADD0_END_BUF0_SHFT              31
#define VDMA_REG_SPDIF_WR_ADD0_END_BUF0_MSK               0x80000000
#define VDMA_REG_SPDIF_WR_ADD0_SPDIF_WR_ADD0_SHFT           0
#define VDMA_REG_SPDIF_WR_ADD0_SPDIF_WR_ADD0_MSK            0x7FFFFFFF

#define VDMA_REG_SPDIF_WR_ADD1_END_BUF1_SHFT              31
#define VDMA_REG_SPDIF_WR_ADD1_END_BUF1_MSK               0x80000000
#define VDMA_REG_SPDIF_WR_ADD1_SPDIF_WR_ADD1_SHFT           0
#define VDMA_REG_SPDIF_WR_ADD1_SPDIF_WR_ADD1_MSK            0x7FFFFFFF

//EP3-EP5
#define VDMA_INT_MC_USB_SRAM_DONE_SHFT3		4           
#define VDMA_INT_MC_USB_SRAM_DONE_MSK3		0x00000010  
#define VDMA_INT_MC_USB_SRAM_DONE_SHFT4		5           
#define VDMA_INT_MC_USB_SRAM_DONE_MSK4		0x00000020  
#define VDMA_INT_MC_USB_SRAM_DONE_SHFT5		6           
#define VDMA_INT_MC_USB_SRAM_DONE_MSK5		0x00000040  
#define VDMA_INT_MC_SRAM_USB_DONE_SHFT3		7           
#define VDMA_INT_MC_SRAM_USB_DONE_MSK3		0x00000080  
#define VDMA_INT_MC_SRAM_USB_DONE_SHFT4		8           
#define VDMA_INT_MC_SRAM_USB_DONE_MSK4		0x00000100  
#define VDMA_INT_MC_SRAM_USB_DONE_SHFT5		9           
#define VDMA_INT_MC_SRAM_USB_DONE_MSK5		0x00000200
#define VDMA_INT_STATE_USB_SRAM_DONE_SHFT3		4              
#define VDMA_INT_STATE_USB_SRAM_DONE_MSK3		  0x00000010     
#define VDMA_INT_STATE_USB_SRAM_DONE_SHFT4		5              
#define VDMA_INT_STATE_USB_SRAM_DONE_MSK4		  0x00000020     
#define VDMA_INT_STATE_USB_SRAM_DONE_SHFT5		6              
#define VDMA_INT_STATE_USB_SRAM_DONE_MSK5		  0x00000040     
#define VDMA_INT_STATE_SRAM_USB_DONE_SHFT3		7              
#define VDMA_INT_STATE_SRAM_USB_DONE_MSK3		  0x00000080     
#define VDMA_INT_STATE_SRAM_USB_DONE_SHFT4		8              
#define VDMA_INT_STATE_SRAM_USB_DONE_MSK4		  0x00000100     
#define VDMA_INT_STATE_SRAM_USB_DONE_SHFT5		9              
#define VDMA_INT_STATE_SRAM_USB_DONE_MSK5		  0x00000200 

typedef enum {
	VDMA_DEV_BB2SRAM,
	VDMA_DEV_SRAM2BB,
	VDMA_DEV_AES2SRAM,
	VDMA_DEV_SRAM2AES,
	VDMA_DEV_USB2SRAM0,
	VDMA_DEV_USB2SRAM1,
	VDMA_DEV_USB2SRAM2,
	VDMA_DEV_SRAM2USB0,
	VDMA_DEV_SRAM2USB1,
	VDMA_DEV_SRAM2USB2,
	VDMA_DEV_SD2SRAM,//[10]
	VDMA_DEV_SRAM2DSPPM, //VDMA_DEV_SD2DSPDM,// 
	VDMA_DEV_SRAM2SD,//[12]
	VDMA_DEV_SPI2SRAM,//[13]
	VDMA_DEV_SPI2DSPPM,// 
	VDMA_DEV_SRAM2SPI,//[15]
	VDMA_DEV_SRAM2LCD,//[16]
	VDMA_DEV_DSPDM2LCD,// 
	VDMA_DEV_AIF2SRAM,//[18]
	VDMA_DEV_AIF2DSPDM,//[19]
	VDMA_DEV_I2S2DSPDM,//[20]
	VDMA_DEV_I2S2SRAM,//[21]
	VDMA_DEV_SRAM2DSPDM,// 
	VDMA_DEV_DSPDM2SRAM,// 
	VDMA_DEV_SRAM2AIF,//[24]VDMA_DEV_SRAM2DSPDM
	VDMA_DEV_DSPDM2AIF,//
	VDMA_DEV_DSPDM2I2S,
	VDMA_DEV_SRAM2I2S,
	VDMA_DEV_HDS2SRAM,
	VDMA_DEV_HDS2DSPDM,
	VDMA_DEV_SRAM2HDS,
	VDMA_DEV_DSPDM2HDS,
	VDMA_DEV_TOTAL
}VDMA_DEV;
/*
INTR_FNSH_MASK  :     path finish Interrupt mask Register
  [31]:  dspdm2hds_wr_done_msk  1'h0 dspdm2hds write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [30]:  sram2hds_wr_done_msk  1'h0 sram2hds write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [29]:  hds2dspdm_rd_done_msk  1'h0 hds2dspdm read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [28]:  hds2sram_rd_done_msk  1'h0 hds2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [27]:  sram2i2s_wr_done_msk  1'h0 sram2i2s write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [26]:  dspdm2i2s_wr_done_msk  1'h0 dspdm2i2s write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [25]:  dspdm2aif_wr_done_msk  1'h0 dspdm2aif write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [24]:  sram2aif_wr_done_msk  1'h0 sram2aif write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [23]:  dspdm2sram_done_msk  1'h0 dspdm2sram transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [22]:  sram2dspdm_done_msk  1'h0 sram2dspdm transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [21]:  i2s2sram_rd_done_msk  1'h0 i2s2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [20]:  i2s2dspdm_rd_done_msk  1'h0 i2s2dspdm read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [19]:  aif2dspdm_rd_done_msk  1'h0 aif2dspdm read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [18]:  aif2sram_rd_done_msk  1'h0 aif2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [17]:  reserved  1'h0 reserved
  [16]:  sram2lcd_wr_done_msk  1'h0 sram2lcd write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [15]:  sram2spi_wr_done_msk  1'h0 sram2spi write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [14]:  reserved  1'h0 reserved
  [13]:  spi2sram_rd_done_msk  1'h0 spi2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [12]:  sram2sd_wr_done_msk  1'h0 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [11]:  sram2dsppm_done_msk  1'h0 sram2dsppm transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [10]:  sd2sram_rd_done_msk  1'h0 sd2sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [9]:  sram2usb_wr_done_msk2  1'h0 sram2usb2 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [8]:  sram2usb_wr_done_msk1  1'h0 sram2usb1 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [7]:  sram2usb_wr_done_msk0  1'h0 sram2usb0 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [6]:  usb2sram_rd_done_msk2  1'h0 usb2sram2 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [5]:  usb2sram_rd_done_msk1  1'h0 usb2sram1 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [4]:  usb2sram_rd_done_msk0  1'h0 usb2sram0 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [3]:  sram2aes_wr_done_msk  1'h0 sram2aes write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [2]:  aes2sram_rd_done_msk  1'h0 aes2sram read transfer done interrupt mask register.1 h1: aes2sram_rd_done is muted. 1 h0: aes2sram_rd_done is unmasked.
  [1]:  sram2bb_wr_done_msk  1'h0 sram2bb write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [0]:  bb2sram_rd_done_msk  1'h0 bb2sram read transfer done interrupt mask register.1 h1: bb2sram_rd_done is muted. 1 h0: bb2sram_rd_done is unmasked.
*/
typedef enum {
	VDMA_DEV1_0,	// reserved
	VDMA_DEV1_1,	// reserved
	VDMA_DEV1_2,	// reserved
	VDMA_DEV1_3,	// reserved
	VDMA_DEV1_USB2SRAM3,
	VDMA_DEV1_USB2SRAM4,
	VDMA_DEV1_USB2SRAM5,
	VDMA_DEV1_SRAM2USB3,
	VDMA_DEV1_SRAM2USB4,
	VDMA_DEV1_SRAM2USB5,
	VDMA_DEV1_10,	// reserved
	VDMA_DEV1_11,	// reserved
	VDMA_DEV1_12,	// reserved
	VDMA_DEV1_SPI12SRAM,//[13]
	VDMA_DEV1_14,	// reserved
	VDMA_DEV1_SRAM2SPI1,	//  [15]:  sram2spi1_wr_done_msk
	VDMA_DEV1_16,	// reserved
	VDMA_DEV1_17,	// reserved
	VDMA_DEV1_18,	// reserved 
	VDMA_DEV1_SRAM2SPDIF,//[19]:  sram2spdif
	VDMA_DEV1_I2S02DSPDM,//[20]
	VDMA_DEV1_I2S02SRAM,//[21]
	VDMA_DEV1_22,	// reserved 
	VDMA_DEV1_23,	// reserved 
	VDMA_DEV1_24,	// reserved 
	VDMA_DEV1_25,	// reserved 
	VDMA_DEV1_DSPDM2I2S0,
	VDMA_DEV1_SRAM2I2S0,
    VDMA_DEV1_28,	// reserved 
    VDMA_DEV1_29,	// reserved 
    VDMA_DEV1_30,	// reserved 
    VDMA_DEV1_31,	// reserved 
	VDMA_DEV1_TOTAL
}VDMA_DEV1;
/*
INTR_FNSH_MASK1  :     path finish Interrupt mask Register
  [31]:  reserved  1'h0 reserved
  [30]:  reserved  1'h0 reserved
  [29]:  reserved  1'h0 reserved
  [28]:  reserved  1'h0 reserved
  [27]:  sram2i2s0_wr_done_msk  1'h0 sram2i2s0 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [26]:  dspdm2i2s0_wr_done_msk  1'h0 dspdm2i2s0 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [25]:  reserved  1'h0 reserved
  [24]:  reserved  1'h0 reserved
  [23]:  reserved  1'h0 reserved
  [22]:  reserved  1'h0 reserved
  [21]:  i2s02sram_rd_done_msk  1'h0 i2s02sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [20]:  i2s02dspdm_rd_done_msk  1'h0 i2s02dspdm read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [19]:  sram2spdif_wr_done_msk  1'h0 sram2spdif write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [18]:  reserved  1'h0 dspdm2spdif write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.In va9638d, this bit is invalid.
  [17]:  reserved  1'h0 reserved
  [16]:  reserved  1'h0 reserved
  [15]:  sram2spi1_wr_done_msk  1'h0 sram2spi1 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [14]:  reserved  1'h0 reserved
  [13]:  spi12sram_rd_done_msk  1'h0 spi12sram read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [12]:  reserved  1'h0 reserved
  [11]:  reserved  1'h0 reserved
  [10]:  reserved  1'h0 reserved
  [9]:  sram2usb_wr_done_msk5  1'h0 sram2usb5 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [8]:  sram2usb_wr_done_msk4  1'h0 sram2usb4 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [7]:  sram2usb_wr_done_msk3  1'h0 sram2usb3 write transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [6]:  usb2sram_rd_done_msk5  1'h0 usb2sram5 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [5]:  usb2sram_rd_done_msk4  1'h0 usb2sram4 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [4]:  usb2sram_rd_done_msk3  1'h0 usb2sram3 read transfer done interrupt mask register.1 h1: this path finish interrupt is muted. 1 h0: this path finishi interrupt is unmasked.
  [3]:  reserved  1'h0 reserved
  [2]:  reserved  1'h0 reserved
  [1]:  reserved  1'h0 reserved
  [0]:  reserved  1'h0 reserved
*/


#define BOTH_CORE_DMA_INT_PINS  ((1<<VDMA_DEV_SPI2SRAM) | (1<<VDMA_DEV_SRAM2SPI) |(1<<VDMA_DEV_SRAM2I2S) |(1<<VDMA_DEV_USB2SRAM1)|(1<<VDMA_DEV_SRAM2USB2))
#define TP_CORE_DMA_INT_PINS   (((UINT32)(BIT4|BIT5|BIT6|BIT7|BIT8|BIT9|BIT10|BIT12|BIT16)) | BOTH_CORE_DMA_INT_PINS)
#define BT_CORE_DMA_INT_PINS   ((~TP_CORE_DMA_INT_PINS) | BOTH_CORE_DMA_INT_PINS|BIT5 | BIT4)

#define PULSE5_TP_CORE_DMA_INT_1_PINS  ((UINT32)(BIT4|BIT5|BIT6|BIT7|BIT8|BIT9)| 1<<VDMA_DEV1_SPI12SRAM | 1<<VDMA_DEV1_SRAM2SPI1 | 1<<VDMA_DEV1_SRAM2I2S0 | 1<<VDMA_DEV1_DSPDM2I2S0) //cxt 2021.6.9 VDMA_Interrupt_Routine_Service_1 ESPI
#define PULSE5_BT_CORE_DMA_INT_1_PINS  ~((UINT32)(BIT4|BIT5|BIT6|BIT7|BIT8|BIT9)| 1<<VDMA_DEV1_SPI12SRAM | 1<<VDMA_DEV1_SRAM2SPI1 | 1<<VDMA_DEV1_SRAM2I2S0 | 1<<VDMA_DEV1_DSPDM2I2S0) //cxt 2021.6.9 VDMA_Interrupt_Routine_Service_1 ESPI
#if (SYS_DMA_MulticoreState_MODE == 0)
#define TP_CORE_DMA_INT_1_PINS   ((UINT32)(BIT4|BIT5|BIT6|BIT7|BIT8|BIT9)| 1<<VDMA_DEV1_SPI12SRAM | 1<<VDMA_DEV1_SRAM2SPI1 )
#define BT_CORE_DMA_INT_1_PINS  ~((UINT32)(BIT4|BIT5|BIT6|BIT7|BIT8|BIT9)| 1<<VDMA_DEV1_SPI12SRAM | 1<<VDMA_DEV1_SRAM2SPI1 )
#elif (SYS_DMA_MulticoreState_MODE == 1)
#define TP_CORE_DMA_INT_1_PINS   PULSE5_TP_CORE_DMA_INT_1_PINS
#define BT_CORE_DMA_INT_1_PINS	 PULSE5_BT_CORE_DMA_INT_1_PINS
#endif

typedef enum {
	VDMA_ACTION_START,
	VDMA_ACTION_ABORT,
	VDMA_ACTION_STOP
}VDMA_ACTION;

typedef enum {
	VDMA_BUF_0 = 0,
	VDMA_BUF_1,
	VDMA_BUF_NONE
} VDMA_BUF;

typedef enum {
	VDMA_ERR_NONE,
	VDMA_ERR_OCCUR
} VDMA_ERR;

typedef enum {
	VDMA_USB_SRAM0 = 0,
	VDMA_USB_SRAM1,
	VDMA_USB_SRAM2,
	VDMA_USB_SRAM3,
	VDMA_USB_SRAM4,
	VDMA_USB_SRAM5,
} VDMA_USB_PATH;
#define VDMA_USB_PATH_LIMIT  VDMA_USB_SRAM5
typedef void (*VDMA_ISR_CALLBACK)(VDMA_ERR ErrorCode);

void VDMA_DSPDM2SRAM_IntCallBack(VDMA_ERR error);
void VDMA_SRAM2DSPDM_IntCallBack(VDMA_ERR error);
void VDMA_SRAM2DSPPM_IntCallBack(VDMA_ERR error);
void VDMA_RegisterBsp32Callback(UINT8 index, VDMA_ISR_CALLBACK p_Callback);
VDMA_ISR_CALLBACK VDMA_GetBsp32Callback(UINT8 index);
void VDMA_MaskAllInt(void);
void VDMA_MaskInt(UINT32 BitMasks);
void VDMA_MaskSdInt(UINT32 BitMasks);
void VDMA_ClearErrInt(UINT32 BitMasks);
void VDMA_ClearInt(UINT32 BitMasks);
void VDMA_UnmaskInt(UINT32 BitMasks);
void VDMA_SD_UnmaskInt(UINT32 BitMasks);
void VDMA_MaskAllInt_1(void);
void VDMA_MaskInt_1(UINT32 BitMasks);
void VDMA_MaskSdInt_1(UINT32 BitMasks);
void VDMA_ClearErrInt_1(UINT32 BitMasks);
void VDMA_ClearInt_1(UINT32 BitMasks);
void VDMA_UnmaskInt_1(UINT32 BitMasks);
void VDMA_SD_UnmaskInt_1(UINT32 BitMasks);
void VDMA_EnableInt(void);
void VDMA_DisableInt(void);
BOOL VDMA_IntPending(void);
void VDMA_RegisterISRCallback(VDMA_DEV Dev, BOOL bflag,  VDMA_ISR_CALLBACK p_Callback);
void VDMA_RegisterISRCallback_1(VDMA_DEV1 Dev, BOOL bflag,  VDMA_ISR_CALLBACK p_Callback);
void VDMA_Interrupt_Routine_Service(void);
void VDMA_Interrupt_Routine_Service_1(void);
void VDMA_SetSram2DevPriority(UINT8 Prio);
UINT8 VDMA_GetSram2DevPriority(void);
void VDMA_SetDspdm2DevPriority(UINT8 Prio);
UINT8 VDMA_GetDspdm2DevPriority(void);
BOOL VDMA_IsEnabled(void);
void VDMA_SetDspdma_Burstmode(UINT8 sel, UINT8  mode);
void VDMA_Hold_Dspcore(BOOL flag);
void VDMA_MaskErrInt(UINT32 BitMasks);
void VDMA_MaskErrInt_1(UINT32 BitMasks);
UINT32 VDMA_GetMaskInt(void);
UINT32 VDMA_GetMaskInt_1(void);
UINT32 VDMA_GetMaskErrInt(void);
UINT32 VDMA_GetMaskErrInt_1(void);

void VDMA_AIF_ReadEnable(UINT8 RdEn);
void VDMA_AIF_ReadConfig(BOOL RdSrc, BOOL RdDst, BOOL mode);
void VDMA_AIF_ReadCtrlSrc(BOOL RdCtrlSrc);
void VDMA_AIF_WriteEnable(UINT8 WrEn);
void VDMA_AIF_WriteConfig(BOOL WrSrc, BOOL WrDst, BOOL mode);
void VDMA_AIF_WriteCtrlSrc(BOOL WrCtrlSrc);
BOOL VDMA_AIF_IsReadEnabled(void);
BOOL VDMA_AIF_IsWriteEnabled(void);
void VDMA_AIF_ReadAction(VDMA_ACTION Action);
void VDMA_AIF_WriteAction(VDMA_ACTION Action);
void VDMA_AIF_ConfigReadPath(VDMA_BUF Buf, UINT32 RightAddr, UINT32 LeftAddr, UINT32 Len, BOOL Stop);
void VDMA_AIF_ConfigWritePath(VDMA_BUF Buf, UINT32 Addr, UINT32 Len, BOOL Stop);
void VDMA_AIF_GetReadBufAddress(UINT32* p_RBuf0, UINT32* p_RBuf1, UINT32* p_LBuf0, UINT32* p_LBuf1);
void VDMA_AIF_GetWriteBufAddress(UINT32* p_Buf0, UINT32* p_Buf1);
UINT32 VDMA_AIF_GetReadCount(void);
UINT32 VDMA_AIF_GetWritenCount(void);
BOOL VDMA_AIF_IsReadStopped(void);
BOOL VDMA_AIF_IsWriteStopped(void);
VDMA_BUF VDMA_AIF_GetReadIntSource(void);
VDMA_BUF VDMA_AIF_GetWriteIntSource(void);
VDMA_BUF VDMA_AIF_GetReadActiveBuffer(void);
VDMA_BUF VDMA_AIF_GetWriteActiveBuffer(void);
UINT32 VDMA_AIF_GetReadBufferLength(VDMA_BUF Buf);
UINT32 VDMA_AIF_GetWriteBufferLength(VDMA_BUF Buf);
UINT32 VDMA_AIF_GetTransferedCnt(UINT16* p_WFifoCnt, UINT16* p_RFifoCnt);
BOOL VDMA_AIF_IsDACIntPending(void);
BOOL VDMA_AIF_IsADCIntPending(void);
UINT32 VDMA_AIF_GetIntStat(void);
#if VCODEC_HWGAINSYC_SUPPORTED
void VDMA_GainSyncSet (UINT8 mode);
#endif
void VDMA_SPI_ReadEnable(BOOL RdEn);
void VDMA_SPI_WriteEnable(BOOL WrEn);
void VDMA_SPI_ReadDst(BOOL RdDst);
BOOL VDMA_SPI_IsReadEnabled(void);
BOOL VDMA_SPI_IsWriteEnabled(void);
void VDMA_SPI_ReadAction(VDMA_ACTION Action);
void VDMA_SPI_WriteAction(VDMA_ACTION Action);
void VDMA_SPI_ConfigReadPath(UINT32 Addr, UINT32 Len);
void VDMA_SPI_ConfigWritePath(UINT32 Addr, UINT32 Len);
UINT32 VDMA_SPI_GetReadBufAddress(void);
UINT32 VDMA_SPI_GetWriteBufAddress(void);
UINT32 VDMA_SPI_GetReadCount(void);
UINT32 VDMA_SPI_GetWritenCount(void);
BOOL VDMA_SPI_IsReadStopped(void);
BOOL VDMA_SPI_IsWriteStopped(void);

void VDMA_SPI1_ReadEnable(BOOL RdEn);
void VDMA_SPI1_WriteEnable(BOOL WrEn);
void VDMA_SPI1_ReadDst(BOOL RdDst);
BOOL VDMA_SPI1_IsReadEnabled(void);
BOOL VDMA_SPI1_IsWriteEnabled(void);
void VDMA_SPI1_ReadAction(VDMA_ACTION Action);
void VDMA_SPI1_WriteAction(VDMA_ACTION Action);
void VDMA_SPI1_ConfigReadPath(UINT32 Addr, UINT32 Len);
void VDMA_SPI1_ConfigWritePath(UINT32 Addr, UINT32 Len);
UINT32 VDMA_SPI1_GetReadBufAddress(void);
UINT32 VDMA_SPI1_GetWriteBufAddress(void);
UINT32 VDMA_SPI1_GetReadCount(void);
UINT32 VDMA_SPI1_GetWritenCount(void);
BOOL VDMA_SPI1_IsReadStopped(void);
BOOL VDMA_SPI1_IsWriteStopped(void);

void VDMA_BB_ReadEnable(BOOL RdEn);
void VDMA_BB_WriteEnable(BOOL WrEn);
BOOL VDMA_BB_IsReadEnabled(void);
BOOL VDMA_BB_IsWriteEnabled(void);
void VDMA_BB_ReadAction(VDMA_ACTION Action);
void VDMA_BB_WriteAction(VDMA_ACTION Action);
void VDMA_BB_ConfigReadPath(UINT32 Addr, UINT32 Len);
void VDMA_BB_ConfigWritePath(UINT32 Addr, UINT32 Len);
UINT32 VDMA_BB_GetReadBufAddress(void);
UINT32 VDMA_BB_GetWriteBufAddress(void);
UINT32 VDMA_BB_GetReadCount(void);
UINT32 VDMA_BB_GetWritenCount(void);
BOOL VDMA_BB_IsReadStopped(void);
BOOL VDMA_BB_IsWriteStopped(void);
void VDMA_BB_EnableReceive(BOOL is_enable, UINT32 Addr, UINT32 Len, UINT32 BBLE_sel);
void VDMA_BB_EnableTransmit(BOOL is_enable, UINT32 Addr, UINT32 Len, UINT32 BBLE_sel);
BOOL VDMA_BB_WaitTransferCnt(UINT32 threshold);

void VDMA_SD_ReadEnable(BOOL RdEn);
void VDMA_SD_WriteEnable(BOOL WrEn);
void VDMA_SD_ReadDst(BOOL RdDst);
void VDMA_SD_SetBitmode(UINT8 mode);
BOOL VDMA_SD_IsReadEnabled(void);
BOOL VDMA_SD_IsWriteEnabled(void);
void VDMA_SD_ReadAction(VDMA_ACTION Action);
void VDMA_SD_WriteAction(VDMA_ACTION Action);
void VDMA_SD_ConfigReadPath(UINT32 Addr, UINT32 Len);
void VDMA_SD_ConfigWritePath(UINT32 Addr, UINT32 Len);
UINT32 VDMA_SD_GetReadBufAddress(void);
UINT32 VDMA_SD_GetWriteBufAddress(void);
UINT32 VDMA_SD_GetReadCount(void);
UINT32 VDMA_SD_GetReadCurtCount(void);
UINT32 VDMA_SD_GetWritenCount(void);
UINT32 VDMA_SD_GetWritenCurtCount(void);
BOOL VDMA_SD_IsReadStopped(void);
BOOL VDMA_SD_IsWriteStopped(void);

void VDMA_LCD_WriteEnable(BOOL WrEn);
void VDMA_LCD_WriteSrc(BOOL WrSrc);
void VDMA_LCD_SetBitmode(UINT8 mode);
BOOL VDMA_LCD_IsWriteEnabled(void);
void VDMA_LCD_WriteAction(VDMA_ACTION Action);
void VDMA_LCD_ConfigWritePath(UINT32 Addr, UINT32 Len);
UINT32 VDMA_LCD_GetWriteBufAddress(void);
UINT32 VDMA_LCD_GetWritenCount(void);
BOOL VDMA_LCD_IsWriteStopped(void);

void VDMA_DSPPM_WriteEnable(BOOL WrEn);
BOOL VDMA_DSPPM_IsWriteEnabled(void);
void VDMA_DSPPM_WriteAction(VDMA_ACTION Action);
void VDMA_DSPPM_ConfigWritePath(UINT32 DsetAddr, UINT32 SrcAddr, UINT32 Len);
UINT32 VDMA_DSPPM_GetWriteBufAddress(void);
UINT32 VDMA_DSPPM_GetWritenCount(void);
BOOL VDMA_DSPPM_IsWriteStopped(void);

void VDMA_DSPDM_ReadEnable(BOOL RdEn);
void VDMA_DSPDM_WriteEnable(BOOL WrEn);
void VDMA_DSPDM_SetReadBitmode(UINT8 mode);
void VDMA_DSPDM_SetWriteBitmode(UINT8 mode);
BOOL VDMA_DSPDM_IsReadEnabled(void);
BOOL VDMA_DSPDM_IsWriteEnabled(void);
void VDMA_DSPDM_ReadAction(VDMA_ACTION Action);
void VDMA_DSPDM_WriteAction(VDMA_ACTION Action);
void VDMA_DSPDM_ConfigReadPath(UINT32 DsetAddr, UINT32 SrcAddr, UINT32 Len);
void VDMA_DSPDM_ConfigWritePath(UINT32 DsetAddr, UINT32 SrcAddr, UINT32 Len);
UINT32 VDMA_DSPDM_GetReadBufAddress(void);
UINT32 VDMA_DSPDM_GetWriteBufAddress(void);
UINT32 VDMA_DSPDM_GetReadCount(void);
UINT32 VDMA_DSPDM_GetWritenCount(void);
BOOL VDMA_DSPDM_IsReadStopped(void);
BOOL VDMA_DSPDM_IsWriteStopped(void);

void VDMA_USB_ReadEnable(VDMA_USB_PATH chanel, BOOL RdEn);
void VDMA_USB_ReadUrbSize(VDMA_USB_PATH chanel, BOOL size);
void VDMA_USB_WriteEnable(VDMA_USB_PATH chanel, BOOL WrEn);
void VDMA_USB_WriteUrbSize(VDMA_USB_PATH chanel, BOOL size);
void VDMA_USB_WriteLst(VDMA_USB_PATH chanel, BOOL lst);
void VDMA_USB_ReadAction(VDMA_USB_PATH chanel, VDMA_ACTION Action);
void VDMA_USB_WriteAction(VDMA_USB_PATH chanel, VDMA_ACTION Action);
void VDMA_USB_ConfigReadPath(VDMA_USB_PATH chanel, UINT32 Addr, UINT32 Len);
void VDMA_USB_ConfigWritePath(VDMA_USB_PATH chanel, UINT32 Addr, UINT32 Len);
UINT32 VDMA_USB_GetReadCount(VDMA_USB_PATH chanel);
UINT32 VDMA_USB_GetReadCurtCount(VDMA_USB_PATH chanel);

#endif

