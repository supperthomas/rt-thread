#ifndef _REG_LCD_H_
#define _REG_LCD_H_

#include "vreg_def.h"

#if LCD_BASE

/*
LCDC_CTRL:
[16]:RW CLKGAT 1'h1 clock gate enable;0 = clock gated;1 = clock enabled
[15]:RW CLK96M 1'h0 LCDC Interface work clock select; 0 = from clock divider; 1 = clock 96MHz; 
[14:13]:RW RDN_FORM_SEL 2'h0 Four form timing select bit for RDN0 = form 1 (L active)1 = form 2 (active)2 = form 3 (active)3 = form 4 (high active)
[12:11]:RW WRN_FORM_SEL 2'h0 Four form timing select bit for WRN0 = form 1 (L active)1 = form 2 (active)2 = form 3 (active)3 = form 4 (high active)
[10]:RW CSN_FORM_SEL 1'h0 Tow form timing select bit for CSN0 = form 1 (L active)1 = form 2 (active)
[9]:RW SCLK_POL 1'h0 SCLK polarity0 = posedge;1 = negedge 
[8]:RW SDATA_ORDER 1'h0 Data transmission order for 3/4-wire/8080/6800 interface0 = LSB~MSB;(LSB first); 1 = MSB~LSB;(MSB first)
[7]:RW DMA_ORDER 1'h0 DMA fetch data order0 = Byte0~Byte3;(Byte0 first)1 = Byte3~Byte0;(Byte3 first)
[5]:RW WRITE 1'h1 Read/write signal;0 = read LCD status/data; 1 = write data to LCD (from DMA/APB) 
[4:3]:RW INF_MODE 2'h0 Interface mode select00 = 4-wire01 = 3-wire10 = 808011 = 6800
[2]:RW DMA_MODE 1'h0 DMA mode data transmission 0 = disable1 = enable
[1]:RW CMD_DAT_SEL 1'h0 Data /command select signal from APB bus1 = command0 =;data
[0]:RW LCDC_ENABLE 1'h0 LCDC enable signal.0 = disable1 = enable
*/
#define REG_LCD_LCDC_CTRL                                 			(LCD_BASE + 0x000)
#define REG_LCD_LCDC_CTRL_CLKGAT                          
#define REG_LCD_LCDC_CTRL_CLK96M                          
#define REG_LCD_LCDC_CTRL_RDN_FORM_SEL                    
#define REG_LCD_LCDC_CTRL_WRN_FORM_SEL                    
#define REG_LCD_LCDC_CTRL_CSN_FORM_SEL                    
#define REG_LCD_LCDC_CTRL_SCLK_POL                        
#define REG_LCD_LCDC_CTRL_SDATA_ORDER                     
#define REG_LCD_LCDC_CTRL_DMA_ORDER                       
#define REG_LCD_LCDC_CTRL_WRITE                           
#define REG_LCD_LCDC_CTRL_INF_MODE                        
#define REG_LCD_LCDC_CTRL_DMA_MODE                        
#define REG_LCD_LCDC_CTRL_CMD_DAT_SEL                     
#define REG_LCD_LCDC_CTRL_LCDC_ENABLE                     
#define REG_LCD_LCDC_CTRL_CLKGAT_MASK                     			(0x00010000)
#define REG_LCD_LCDC_CTRL_CLK96M_MASK                     			(0x00008000)
#define REG_LCD_LCDC_CTRL_RDN_FORM_SEL_MASK               			(0x00006000)
#define REG_LCD_LCDC_CTRL_WRN_FORM_SEL_MASK               			(0x00001800)
#define REG_LCD_LCDC_CTRL_CSN_FORM_SEL_MASK               			(0x00000400)
#define REG_LCD_LCDC_CTRL_SCLK_POL_MASK                   			(0x00000200)
#define REG_LCD_LCDC_CTRL_SDATA_ORDER_MASK                			(0x00000100)
#define REG_LCD_LCDC_CTRL_DMA_ORDER_MASK                  			(0x00000080)
#define REG_LCD_LCDC_CTRL_WRITE_MASK                      			(0x00000020)
#define REG_LCD_LCDC_CTRL_INF_MODE_MASK                   			(0x00000018)
#define REG_LCD_LCDC_CTRL_DMA_MODE_MASK                   			(0x00000004)
#define REG_LCD_LCDC_CTRL_CMD_DAT_SEL_MASK                			(0x00000002)
#define REG_LCD_LCDC_CTRL_LCDC_ENABLE_MASK                			(0x00000001)
#define REG_LCD_LCDC_CTRL_CLKGAT_BIT                      			(16)
#define REG_LCD_LCDC_CTRL_CLK96M_BIT                      			(15)
#define REG_LCD_LCDC_CTRL_RDN_FORM_SEL_BIT                			(13)
#define REG_LCD_LCDC_CTRL_WRN_FORM_SEL_BIT                			(11)
#define REG_LCD_LCDC_CTRL_CSN_FORM_SEL_BIT                			(10)
#define REG_LCD_LCDC_CTRL_SCLK_POL_BIT                    			(9)
#define REG_LCD_LCDC_CTRL_SDATA_ORDER_BIT                 			(8)
#define REG_LCD_LCDC_CTRL_DMA_ORDER_BIT                   			(7)
#define REG_LCD_LCDC_CTRL_WRITE_BIT                       			(5)
#define REG_LCD_LCDC_CTRL_INF_MODE_BIT                    			(3)
#define REG_LCD_LCDC_CTRL_DMA_MODE_BIT                    			(2)
#define REG_LCD_LCDC_CTRL_CMD_DAT_SEL_BIT                 			(1)
#define REG_LCD_LCDC_CTRL_LCDC_ENABLE_BIT                 			(0)
#define REG_LCD_LCDC_CTRL_CLKGAT_ADDR                     			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_CLK96M_ADDR                     			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_RDN_FORM_SEL_ADDR               			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_WRN_FORM_SEL_ADDR               			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_CSN_FORM_SEL_ADDR               			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_SCLK_POL_ADDR                   			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_SDATA_ORDER_ADDR                			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_DMA_ORDER_ADDR                  			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_WRITE_ADDR                      			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_INF_MODE_ADDR                   			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_DMA_MODE_ADDR                   			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_CMD_DAT_SEL_ADDR                			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_LCDC_ENABLE_ADDR                			(REG_LCD_LCDC_CTRL)
#define REG_LCD_LCDC_CTRL_CLKGAT_BITBAND                  			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 16))
#define REG_LCD_LCDC_CTRL_CLK96M_BITBAND                  			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 15))
#define REG_LCD_LCDC_CTRL_RDN_FORM_SEL_BITBAND            			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 13))
#define REG_LCD_LCDC_CTRL_WRN_FORM_SEL_BITBAND            			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 11))
#define REG_LCD_LCDC_CTRL_CSN_FORM_SEL_BITBAND            			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 10))
#define REG_LCD_LCDC_CTRL_SCLK_POL_BITBAND                			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 9))
#define REG_LCD_LCDC_CTRL_SDATA_ORDER_BITBAND             			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 8))
#define REG_LCD_LCDC_CTRL_DMA_ORDER_BITBAND               			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 7))
#define REG_LCD_LCDC_CTRL_WRITE_BITBAND                   			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 5))
#define REG_LCD_LCDC_CTRL_INF_MODE_BITBAND                			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 3))
#define REG_LCD_LCDC_CTRL_DMA_MODE_BITBAND                			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 2))
#define REG_LCD_LCDC_CTRL_CMD_DAT_SEL_BITBAND             			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 1))
#define REG_LCD_LCDC_CTRL_LCDC_ENABLE_BITBAND             			(PERI_BIT_BAND(REG_LCD_LCDC_CTRL, 0))

/*
LCDC_TRANS_START:
[0]:RW TRANS_START 1'h0 LCDC data transfer start(repeat write 5 times to start LCDC)
*/
#define REG_LCD_LCDC_TRANS_START                          			(LCD_BASE + 0x004)
#define REG_LCD_LCDC_TRANS_START_TRANS_START              
#define REG_LCD_LCDC_TRANS_START_TRANS_START_MASK         			(0x00000001)
#define REG_LCD_LCDC_TRANS_START_TRANS_START_BIT          			(0)
#define REG_LCD_LCDC_TRANS_START_TRANS_START_ADDR         			(REG_LCD_LCDC_TRANS_START)
#define REG_LCD_LCDC_TRANS_START_TRANS_START_BITBAND      			(PERI_BIT_BAND(REG_LCD_LCDC_TRANS_START, 0))

/*
LCDC_CMD_DATA:
[7:0]:RW CMD_DATA 8'h00 LCDC Command/DATA register (for APB mode)
*/
#define REG_LCD_LCDC_CMD_DATA                             			(LCD_BASE + 0x008)
#define REG_LCD_LCDC_CMD_DATA_CMD_DATA                    
#define REG_LCD_LCDC_CMD_DATA_CMD_DATA_MASK               			(0x000000FF)
#define REG_LCD_LCDC_CMD_DATA_CMD_DATA_BIT                			(0)
#define REG_LCD_LCDC_CMD_DATA_CMD_DATA_ADDR               			(REG_LCD_LCDC_CMD_DATA)
#define REG_LCD_LCDC_CMD_DATA_CMD_DATA_BITBAND            			(PERI_BIT_BAND(REG_LCD_LCDC_CMD_DATA, 0))

/*
LCDC_LENGTH:
[15:0]:RW TFCLKS_CINI 16'h0080 transfer byte number per a burst
*/
#define REG_LCD_LCDC_LENGTH                               			(LCD_BASE + 0x00C)
#define REG_LCD_LCDC_LENGTH_TFCLKS_CINI                   
#define REG_LCD_LCDC_LENGTH_TFCLKS_CINI_MASK              			(0x0000FFFF)
#define REG_LCD_LCDC_LENGTH_TFCLKS_CINI_BIT               			(0)
#define REG_LCD_LCDC_LENGTH_TFCLKS_CINI_ADDR              			(REG_LCD_LCDC_LENGTH)
#define REG_LCD_LCDC_LENGTH_TFCLKS_CINI_BITBAND           			(PERI_BIT_BAND(REG_LCD_LCDC_LENGTH, 0))

/*
LCDC_STATUS:
[8]:R TRANS_DONE 1'b0 When transmission is finished (DMA/APB), it goes high.
[7:0]:R RD_DAT_STAT 8'h00 Read data/status for 8080/6800 interface
*/
#define REG_LCD_LCDC_STATUS                               			(LCD_BASE + 0x010)
#define REG_LCD_LCDC_STATUS_TRANS_DONE                    
#define REG_LCD_LCDC_STATUS_RD_DAT_STAT                   
#define REG_LCD_LCDC_STATUS_TRANS_DONE_MASK               			(0x00000100)
#define REG_LCD_LCDC_STATUS_RD_DAT_STAT_MASK              			(0x000000FF)
#define REG_LCD_LCDC_STATUS_TRANS_DONE_BIT                			(8)
#define REG_LCD_LCDC_STATUS_RD_DAT_STAT_BIT               			(0)
#define REG_LCD_LCDC_STATUS_TRANS_DONE_ADDR               			(REG_LCD_LCDC_STATUS)
#define REG_LCD_LCDC_STATUS_RD_DAT_STAT_ADDR              			(REG_LCD_LCDC_STATUS)
#define REG_LCD_LCDC_STATUS_TRANS_DONE_BITBAND            			(PERI_BIT_BAND(REG_LCD_LCDC_STATUS, 8))
#define REG_LCD_LCDC_STATUS_RD_DAT_STAT_BITBAND           			(PERI_BIT_BAND(REG_LCD_LCDC_STATUS, 0))

/*
LCDC_TSCYC:
[7:0]:RW TSCYC_PARM 8'h1e 3/4-wire SPI : Serial clock cycle8080/6800 :; System cycle time
*/
#define REG_LCD_LCDC_TSCYC                                			(LCD_BASE + 0x014)
#define REG_LCD_LCDC_TSCYC_TSCYC_PARM                     
#define REG_LCD_LCDC_TSCYC_TSCYC_PARM_MASK                			(0x000000FF)
#define REG_LCD_LCDC_TSCYC_TSCYC_PARM_BIT                 			(0)
#define REG_LCD_LCDC_TSCYC_TSCYC_PARM_ADDR                			(REG_LCD_LCDC_TSCYC)
#define REG_LCD_LCDC_TSCYC_TSCYC_PARM_BITBAND             			(PERI_BIT_BAND(REG_LCD_LCDC_TSCYC, 0))

/*
LCDC_TSLW:
[7:0]:RW TSLW_PARM 8'h05 3/4-wire : LCDC Serial clock L pulse width8080; : Control L pulse width 6800;:;Enable H pulse width 
*/
#define REG_LCD_LCDC_TSLW                                 			(LCD_BASE + 0x018)
#define REG_LCD_LCDC_TSLW_TSLW_PARM                       
#define REG_LCD_LCDC_TSLW_TSLW_PARM_MASK                  			(0x000000FF)
#define REG_LCD_LCDC_TSLW_TSLW_PARM_BIT                   			(0)
#define REG_LCD_LCDC_TSLW_TSLW_PARM_ADDR                  			(REG_LCD_LCDC_TSLW)
#define REG_LCD_LCDC_TSLW_TSLW_PARM_BITBAND               			(PERI_BIT_BAND(REG_LCD_LCDC_TSLW, 0))

/*
LCDC_TSDS:
[7:0]:RW TSDS_PARM 8'h5 LCDC data setup time for 4-wire/8080/6800
*/
#define REG_LCD_LCDC_TSDS                                 			(LCD_BASE + 0x01C)
#define REG_LCD_LCDC_TSDS_TSDS_PARM                       
#define REG_LCD_LCDC_TSDS_TSDS_PARM_MASK                  			(0x000000FF)
#define REG_LCD_LCDC_TSDS_TSDS_PARM_BIT                   			(0)
#define REG_LCD_LCDC_TSDS_TSDS_PARM_ADDR                  			(REG_LCD_LCDC_TSDS)
#define REG_LCD_LCDC_TSDS_TSDS_PARM_BITBAND               			(PERI_BIT_BAND(REG_LCD_LCDC_TSDS, 0))

/*
LCDC_TSDH:
[7:0]:RW TSDH_PARM 8'h0 LCDC data hold time for 4-wire/8080/6800
*/
#define REG_LCD_LCDC_TSDH                                 			(LCD_BASE + 0x020)
#define REG_LCD_LCDC_TSDH_TSDH_PARM                       
#define REG_LCD_LCDC_TSDH_TSDH_PARM_MASK                  			(0x000000FF)
#define REG_LCD_LCDC_TSDH_TSDH_PARM_BIT                   			(0)
#define REG_LCD_LCDC_TSDH_TSDH_PARM_ADDR                  			(REG_LCD_LCDC_TSDH)
#define REG_LCD_LCDC_TSDH_TSDH_PARM_BITBAND               			(PERI_BIT_BAND(REG_LCD_LCDC_TSDH, 0))

/*
LCDC_TCSS:
[7:0]:RW TCSS_PARM 8'h5 3/4-wire : LCDC chip select setup time 8080; : Control H pulse width 6800; :;Enable L pulse width 
*/
#define REG_LCD_LCDC_TCSS                                 			(LCD_BASE + 0x024)
#define REG_LCD_LCDC_TCSS_TCSS_PARM                       
#define REG_LCD_LCDC_TCSS_TCSS_PARM_MASK                  			(0x000000FF)
#define REG_LCD_LCDC_TCSS_TCSS_PARM_BIT                   			(0)
#define REG_LCD_LCDC_TCSS_TCSS_PARM_ADDR                  			(REG_LCD_LCDC_TCSS)
#define REG_LCD_LCDC_TCSS_TCSS_PARM_BITBAND               			(PERI_BIT_BAND(REG_LCD_LCDC_TCSS, 0))

/*
LCDC_CLK_DIV:
[7:0]:RW CLK_DIV 2'h01 Lcdc work clock divider from lcdc_clk (96MHz)
*/
#define REG_LCD_LCDC_CLK_DIV                              			(LCD_BASE + 0x028)
#define REG_LCD_LCDC_CLK_DIV_CLK_DIV                      
#define REG_LCD_LCDC_CLK_DIV_CLK_DIV_MASK                 			(0x000000FF)
#define REG_LCD_LCDC_CLK_DIV_CLK_DIV_BIT                  			(0)
#define REG_LCD_LCDC_CLK_DIV_CLK_DIV_ADDR                 			(REG_LCD_LCDC_CLK_DIV)
#define REG_LCD_LCDC_CLK_DIV_CLK_DIV_BITBAND              			(PERI_BIT_BAND(REG_LCD_LCDC_CLK_DIV, 0))

/*
LCDC_UM_CTRL:
[5:0]:RW UM_CTRL 6'h00 Lcdc umonitor control bit
*/
#define REG_LCD_LCDC_UM_CTRL                              			(LCD_BASE + 0x02C)
#define REG_LCD_LCDC_UM_CTRL_UM_CTRL                      
#define REG_LCD_LCDC_UM_CTRL_UM_CTRL_MASK                 			(0x0000003F)
#define REG_LCD_LCDC_UM_CTRL_UM_CTRL_BIT                  			(0)
#define REG_LCD_LCDC_UM_CTRL_UM_CTRL_ADDR                 			(REG_LCD_LCDC_UM_CTRL)
#define REG_LCD_LCDC_UM_CTRL_UM_CTRL_BITBAND              			(PERI_BIT_BAND(REG_LCD_LCDC_UM_CTRL, 0))

/*
LCDC_HOLD_TIME:
[7:0]:RW HOLD_TIME 8'h10 Lcdc csn high hold time after a burst transfer
*/
#define REG_LCD_LCDC_HOLD_TIME                            			(LCD_BASE + 0x030)
#define REG_LCD_LCDC_HOLD_TIME_HOLD_TIME                  
#define REG_LCD_LCDC_HOLD_TIME_HOLD_TIME_MASK             			(0x000000FF)
#define REG_LCD_LCDC_HOLD_TIME_HOLD_TIME_BIT              			(0)
#define REG_LCD_LCDC_HOLD_TIME_HOLD_TIME_ADDR             			(REG_LCD_LCDC_HOLD_TIME)
#define REG_LCD_LCDC_HOLD_TIME_HOLD_TIME_BITBAND          			(PERI_BIT_BAND(REG_LCD_LCDC_HOLD_TIME, 0))

/*
LCDC_TWRS:
[7:0]:RW TWRS_PARM 8'h04 WRN signal setup time
*/
#define REG_LCD_LCDC_TWRS                                 			(LCD_BASE + 0x034)
#define REG_LCD_LCDC_TWRS_TWRS_PARM                       
#define REG_LCD_LCDC_TWRS_TWRS_PARM_MASK                  			(0x000000FF)
#define REG_LCD_LCDC_TWRS_TWRS_PARM_BIT                   			(0)
#define REG_LCD_LCDC_TWRS_TWRS_PARM_ADDR                  			(REG_LCD_LCDC_TWRS)
#define REG_LCD_LCDC_TWRS_TWRS_PARM_BITBAND               			(PERI_BIT_BAND(REG_LCD_LCDC_TWRS, 0))

/*
LCDC_TWRLW:
[7:0]:RW TWRLW_PARM 8'h0d WRN L pulse width
*/
#define REG_LCD_LCDC_TWRLW                                			(LCD_BASE + 0x038)
#define REG_LCD_LCDC_TWRLW_TWRLW_PARM                     
#define REG_LCD_LCDC_TWRLW_TWRLW_PARM_MASK                			(0x000000FF)
#define REG_LCD_LCDC_TWRLW_TWRLW_PARM_BIT                 			(0)
#define REG_LCD_LCDC_TWRLW_TWRLW_PARM_ADDR                			(REG_LCD_LCDC_TWRLW)
#define REG_LCD_LCDC_TWRLW_TWRLW_PARM_BITBAND             			(PERI_BIT_BAND(REG_LCD_LCDC_TWRLW, 0))

/*
LCDC_TWRCYC:
[7:0]:RW TWRCYC_PARM 8'h15 WRN cycle time
*/
#define REG_LCD_LCDC_TWRCYC                               			(LCD_BASE + 0x03C)
#define REG_LCD_LCDC_TWRCYC_TWRCYC_PARM                   
#define REG_LCD_LCDC_TWRCYC_TWRCYC_PARM_MASK              			(0x000000FF)
#define REG_LCD_LCDC_TWRCYC_TWRCYC_PARM_BIT               			(0)
#define REG_LCD_LCDC_TWRCYC_TWRCYC_PARM_ADDR              			(REG_LCD_LCDC_TWRCYC)
#define REG_LCD_LCDC_TWRCYC_TWRCYC_PARM_BITBAND           			(PERI_BIT_BAND(REG_LCD_LCDC_TWRCYC, 0))

/*
LCDC_TRDS:
[7:0]:RW TRDS_PARM 8'h4 RDN setup time
*/
#define REG_LCD_LCDC_TRDS                                 			(LCD_BASE + 0x040)
#define REG_LCD_LCDC_TRDS_TRDS_PARM                       
#define REG_LCD_LCDC_TRDS_TRDS_PARM_MASK                  			(0x000000FF)
#define REG_LCD_LCDC_TRDS_TRDS_PARM_BIT                   			(0)
#define REG_LCD_LCDC_TRDS_TRDS_PARM_ADDR                  			(REG_LCD_LCDC_TRDS)
#define REG_LCD_LCDC_TRDS_TRDS_PARM_BITBAND               			(PERI_BIT_BAND(REG_LCD_LCDC_TRDS, 0))

/*
LCDC_TRDLW:
[7:0]:RW TRDLW_PARM 8'hd RDN L pulse width
*/
#define REG_LCD_LCDC_TRDLW                                			(LCD_BASE + 0x044)
#define REG_LCD_LCDC_TRDLW_TRDLW_PARM                     
#define REG_LCD_LCDC_TRDLW_TRDLW_PARM_MASK                			(0x000000FF)
#define REG_LCD_LCDC_TRDLW_TRDLW_PARM_BIT                 			(0)
#define REG_LCD_LCDC_TRDLW_TRDLW_PARM_ADDR                			(REG_LCD_LCDC_TRDLW)
#define REG_LCD_LCDC_TRDLW_TRDLW_PARM_BITBAND             			(PERI_BIT_BAND(REG_LCD_LCDC_TRDLW, 0))

/*
LCDC_TRDCYC:
[7:0]:RW TRDCYC_PARM 8'h15 RDN cycle time
*/
#define REG_LCD_LCDC_TRDCYC                               			(LCD_BASE + 0x048)
#define REG_LCD_LCDC_TRDCYC_TRDCYC_PARM                   
#define REG_LCD_LCDC_TRDCYC_TRDCYC_PARM_MASK              			(0x000000FF)
#define REG_LCD_LCDC_TRDCYC_TRDCYC_PARM_BIT               			(0)
#define REG_LCD_LCDC_TRDCYC_TRDCYC_PARM_ADDR              			(REG_LCD_LCDC_TRDCYC)
#define REG_LCD_LCDC_TRDCYC_TRDCYC_PARM_BITBAND           			(PERI_BIT_BAND(REG_LCD_LCDC_TRDCYC, 0))

#endif /*LCD_BASE*/
#endif /*_REG_LCD_H_*/
