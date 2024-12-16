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
    reg

Description:
    reg

Revision History:
    Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef _REG_TIMER_H_
#define _REG_TIMER_H_

#include "vreg_def.h"

#if TIMER_BASE

/*
TCTRL:
[31]:RW SCLK_EN 1'h1 TMR_SCLK enable;0 = clock gated;1 = clock enabled
[30]:RW FCLK_EN 1'h1 TMR_FCLK enable;0 = clock gated;1 = clock enabled
[13]:RW WDMODE 1'h1 The mode selection when the counter is zero. This bit can be written when WDLOCK = 0;0 = interrupt;1 = system reset; 
[12]:RW WDCEN 1'h0 Watch Dog count enable, can be written only when WDLOCK = 0; 0 = count disable;1 = count enable 
[9]:RW TSCLK_RLD_EN 1'h0 TMR_SCLK counter auto reload enable; 0 = disable;1 = enable, when the counter is counted down to zero, the value in TSCLKCINI will be load to this counter,;then countdown again
[8]:RW TSCLK_CEN 1'h0 TMR_SCLK count enable; 0 = count disable;1 = count enable 
[7]:RW TFCLKH_RLD_EN 1'h0 TMR_FCLK counter auto reload enable; 0 = disable;1 = enable, when the counter is counted down to zero, the value in TFCLKHCINI will be load to this counter,;then countdown again
[6]:RW TFCLKH_CEN 1'h0 TMR_FCLKH count enable; 0 = count disable;1 = count enable 
[5]:RW TFCLKS_RLD_EN 1'h0 TMR_FCLK counter auto reload enable; 0 = disable;1 = enable, when the counter is counted down to zero, the value in TFCLKSCINI will be load to this counter,;then countdown again
[4]:RW TFCLKS_CEN 1'h0 TMR_FCLKS count enable; 0 = count disable;1 = count enable 
[1]:RW TFCLKL_RLD_EN 1'h0 TMR_FCLK counter auto reload enable; 0 = disable;1 = enable, when the counter is counted down to zero, the value in TFCLKLCINI will be load to this counter,;then countdown again
[0]:RW TFCLKL_CEN 1'h0 TMR_FCLKL count enable; 0 = count disable;1 = count enable 
*/
#define REG_TIMER_TCTRL                                                         (TIMER_BASE + 0x000)
#define REG_TIMER_TCTRL_SCLK_EN                           
#define REG_TIMER_TCTRL_FCLK_EN                           
#define REG_TIMER_TCTRL_WDMODE                            
#define REG_TIMER_TCTRL_WDCEN                             
#define REG_TIMER_TCTRL_TSCLK_RLD_EN                      
#define REG_TIMER_TCTRL_TSCLK_CEN                         
#define REG_TIMER_TCTRL_TFCLKH_RLD_EN                     
#define REG_TIMER_TCTRL_TFCLKH_CEN                        
#define REG_TIMER_TCTRL_TFCLKS_RLD_EN                     
#define REG_TIMER_TCTRL_TFCLKS_CEN                        
#define REG_TIMER_TCTRL_TFCLKL_RLD_EN                     
#define REG_TIMER_TCTRL_TFCLKL_CEN                        
#define REG_TIMER_TCTRL_SCLK_EN_MASK                                            (0x80000000)
#define REG_TIMER_TCTRL_FCLK_EN_MASK                                            (0x40000000)
#define REG_TIMER_TCTRL_WDMODE_MASK                                             (0x00002000)
#define REG_TIMER_TCTRL_WDCEN_MASK                                              (0x00001000)
#define REG_TIMER_TCTRL_TSCLK_RLD_EN_MASK                                       (0x00000200)
#define REG_TIMER_TCTRL_TSCLK_CEN_MASK                                          (0x00000100)
#define REG_TIMER_TCTRL_TFCLKH_RLD_EN_MASK                                      (0x00000080)
#define REG_TIMER_TCTRL_TFCLKH_CEN_MASK                                         (0x00000040)
#define REG_TIMER_TCTRL_TFCLKS_RLD_EN_MASK                                      (0x00000020)
#define REG_TIMER_TCTRL_TFCLKS_CEN_MASK                                         (0x00000010)
#define REG_TIMER_TCTRL_TFCLKL_RLD_EN_MASK                                      (0x00000002)
#define REG_TIMER_TCTRL_TFCLKL_CEN_MASK                                         (0x00000001)
#define REG_TIMER_TCTRL_SCLK_EN_BIT                                             (31)
#define REG_TIMER_TCTRL_FCLK_EN_BIT                                             (30)
#define REG_TIMER_TCTRL_WDMODE_BIT                                              (13)
#define REG_TIMER_TCTRL_WDCEN_BIT                                               (12)
#define REG_TIMER_TCTRL_TSCLK_RLD_EN_BIT                                        (9)
#define REG_TIMER_TCTRL_TSCLK_CEN_BIT                                           (8)
#define REG_TIMER_TCTRL_TFCLKH_RLD_EN_BIT                                       (7)
#define REG_TIMER_TCTRL_TFCLKH_CEN_BIT                                          (6)
#define REG_TIMER_TCTRL_TFCLKS_RLD_EN_BIT                                       (5)
#define REG_TIMER_TCTRL_TFCLKS_CEN_BIT                                          (4)
#define REG_TIMER_TCTRL_TFCLKL_RLD_EN_BIT                                       (1)
#define REG_TIMER_TCTRL_TFCLKL_CEN_BIT                                          (0)
#define REG_TIMER_TCTRL_SCLK_EN_ADDR                                            (REG_TIMER_TCTRL)
#define REG_TIMER_TCTRL_FCLK_EN_ADDR                                            (REG_TIMER_TCTRL)
#define REG_TIMER_TCTRL_WDMODE_ADDR                                             (REG_TIMER_TCTRL)
#define REG_TIMER_TCTRL_WDCEN_ADDR                                              (REG_TIMER_TCTRL)
#define REG_TIMER_TCTRL_TSCLK_RLD_EN_ADDR                                       (REG_TIMER_TCTRL)
#define REG_TIMER_TCTRL_TSCLK_CEN_ADDR                                          (REG_TIMER_TCTRL)
#define REG_TIMER_TCTRL_TFCLKH_RLD_EN_ADDR                                      (REG_TIMER_TCTRL)
#define REG_TIMER_TCTRL_TFCLKH_CEN_ADDR                                         (REG_TIMER_TCTRL)
#define REG_TIMER_TCTRL_TFCLKS_RLD_EN_ADDR                                      (REG_TIMER_TCTRL)
#define REG_TIMER_TCTRL_TFCLKS_CEN_ADDR                                         (REG_TIMER_TCTRL)
#define REG_TIMER_TCTRL_TFCLKL_RLD_EN_ADDR                                      (REG_TIMER_TCTRL)
#define REG_TIMER_TCTRL_TFCLKL_CEN_ADDR                                         (REG_TIMER_TCTRL)

/*
TFCLKLCINI:
[31:0]:RW TFCLKL_CINI 32'hffff_ffff Initial/reload value for TMR_FCLKL counter
*/
#define REG_TIMER_TFCLKLCINI                                                    (TIMER_BASE + 0x004)
#define REG_TIMER_TFCLKLCINI_TFCLKL_CINI                  
#define REG_TIMER_TFCLKLCINI_TFCLKL_CINI_MASK                                   (0xFFFFFFFF)
#define REG_TIMER_TFCLKLCINI_TFCLKL_CINI_BIT                                    (0)
#define REG_TIMER_TFCLKLCINI_TFCLKL_CINI_ADDR                                   (REG_TIMER_TFCLKLCINI)

/*
TFCLKLCNT:
[31:0]:R TFCLKL_CNT 32'hffff_ffff TMR_FCLKL counter value
*/
#define REG_TIMER_TFCLKLCNT                                                     (TIMER_BASE + 0x008)
#define REG_TIMER_TFCLKLCNT_TFCLKL_CNT                    
#define REG_TIMER_TFCLKLCNT_TFCLKL_CNT_MASK                                     (0xFFFFFFFF)
#define REG_TIMER_TFCLKLCNT_TFCLKL_CNT_BIT                                      (0)
#define REG_TIMER_TFCLKLCNT_TFCLKL_CNT_ADDR                                     (REG_TIMER_TFCLKLCNT)

/*
TFCLKSCINI:
[31:0]:RW TFCLKS_CINI 32'hffff_ffff Initial/reload value for TMRS_FCLK counter
*/
#define REG_TIMER_TFCLKSCINI                                                    (TIMER_BASE + 0x014)
#define REG_TIMER_TFCLKSCINI_TFCLKS_CINI                  
#define REG_TIMER_TFCLKSCINI_TFCLKS_CINI_MASK                                   (0xFFFFFFFF)
#define REG_TIMER_TFCLKSCINI_TFCLKS_CINI_BIT                                    (0)
#define REG_TIMER_TFCLKSCINI_TFCLKS_CINI_ADDR                                   (REG_TIMER_TFCLKSCINI)

/*
TFCLKSCNT:
[31:0]:R TFCLKS_CNT 32'hffff_ffff TMR_FCLKS counter value
*/
#define REG_TIMER_TFCLKSCNT                                                     (TIMER_BASE + 0x018)
#define REG_TIMER_TFCLKSCNT_TFCLKS_CNT                    
#define REG_TIMER_TFCLKSCNT_TFCLKS_CNT_MASK                                     (0xFFFFFFFF)
#define REG_TIMER_TFCLKSCNT_TFCLKS_CNT_BIT                                      (0)
#define REG_TIMER_TFCLKSCNT_TFCLKS_CNT_ADDR                                     (REG_TIMER_TFCLKSCNT)

/*
TFCLKHCINI:
[31:0]:RW TFCLKH_CINI 32'hffff_ffff Initial/reload value for TMRS_FCLK counter
*/
#define REG_TIMER_TFCLKHCINI                                                    (TIMER_BASE + 0x01C)
#define REG_TIMER_TFCLKHCINI_TFCLKH_CINI                  
#define REG_TIMER_TFCLKHCINI_TFCLKH_CINI_MASK                                   (0xFFFFFFFF)
#define REG_TIMER_TFCLKHCINI_TFCLKH_CINI_BIT                                    (0)
#define REG_TIMER_TFCLKHCINI_TFCLKH_CINI_ADDR                                   (REG_TIMER_TFCLKHCINI)

/*
TFCLKHCNT:
[31:0]:R TFCLKH_CNT 32'hffff_ffff TMR_FCLKH counter value
*/
#define REG_TIMER_TFCLKHCNT                                                     (TIMER_BASE + 0x020)
#define REG_TIMER_TFCLKHCNT_TFCLKH_CNT                    
#define REG_TIMER_TFCLKHCNT_TFCLKH_CNT_MASK                                     (0xFFFFFFFF)
#define REG_TIMER_TFCLKHCNT_TFCLKH_CNT_BIT                                      (0)
#define REG_TIMER_TFCLKHCNT_TFCLKH_CNT_ADDR                                     (REG_TIMER_TFCLKHCNT)

/*
TSCLKCINI:
[31:0]:RW TSCLK_CINI 32'hffff_ffff Initial/reload value for TMR_SCLK counter
*/
#define REG_TIMER_TSCLKCINI                                                     (TIMER_BASE + 0x024)
#define REG_TIMER_TSCLKCINI_TSCLK_CINI                    
#define REG_TIMER_TSCLKCINI_TSCLK_CINI_MASK                                     (0xFFFFFFFF)
#define REG_TIMER_TSCLKCINI_TSCLK_CINI_BIT                                      (0)
#define REG_TIMER_TSCLKCINI_TSCLK_CINI_ADDR                                     (REG_TIMER_TSCLKCINI)

/*
TSCLKCNT:
[31:0]:R TSCLK_CNT 32'hffff_ffff TMR_SCLK counter value
*/
#define REG_TIMER_TSCLKCNT                                                      (TIMER_BASE + 0x028)
#define REG_TIMER_TSCLKCNT_TSCLK_CNT                      
#define REG_TIMER_TSCLKCNT_TSCLK_CNT_MASK                                       (0xFFFFFFFF)
#define REG_TIMER_TSCLKCNT_TSCLK_CNT_BIT                                        (0)
#define REG_TIMER_TSCLKCNT_TSCLK_CNT_ADDR                                       (REG_TIMER_TSCLKCNT)

/*
TSTA:
[4]:R FH_STA 1'h0 TMR_FCLKH counter interrupt status, which is set by hardware and cleared by software;0 = no interrupt;1 = interrupt
[3]:R WD_STA 1'h0 WD counter interrupt status, which is set by hardware and cleared by software;0 = no interrupt;1 = interrupt
[2]:R S_STA 1'h0 TMR_SCLK counter interrupt status, which is set by hardware and cleared by software; 0 = no interrupt;1 = interrupt
[1]:R FS_STA 1'h0 TMR_FCLKS counter interrupt status, which is set by hardware and cleared by software;0 = no interrupt;1 = interrupt
[0]:R FL_STA 1'h0 TMR_FCLKL counter interrupt status, which is set by hardware and cleared by software;0 = no interrupt;1 = interrupt
*/
#define REG_TIMER_TSTA                                                          (TIMER_BASE + 0x030)
#define REG_TIMER_TSTA_FH_STA                             
#define REG_TIMER_TSTA_WD_STA                             
#define REG_TIMER_TSTA_S_STA                              
#define REG_TIMER_TSTA_FS_STA                             
#define REG_TIMER_TSTA_FL_STA                             
#define REG_TIMER_TSTA_FH_STA_MASK                                              (0x00000010)
#define REG_TIMER_TSTA_WD_STA_MASK                                              (0x00000008)
#define REG_TIMER_TSTA_S_STA_MASK                                               (0x00000004)
#define REG_TIMER_TSTA_FS_STA_MASK                                              (0x00000002)
#define REG_TIMER_TSTA_FL_STA_MASK                                              (0x00000001)
#define REG_TIMER_TSTA_FH_STA_BIT                                               (4)
#define REG_TIMER_TSTA_WD_STA_BIT                                               (3)
#define REG_TIMER_TSTA_S_STA_BIT                                                (2)
#define REG_TIMER_TSTA_FS_STA_BIT                                               (1)
#define REG_TIMER_TSTA_FL_STA_BIT                                               (0)
#define REG_TIMER_TSTA_FH_STA_ADDR                                              (REG_TIMER_TSTA)
#define REG_TIMER_TSTA_WD_STA_ADDR                                              (REG_TIMER_TSTA)
#define REG_TIMER_TSTA_S_STA_ADDR                                               (REG_TIMER_TSTA)
#define REG_TIMER_TSTA_FS_STA_ADDR                                              (REG_TIMER_TSTA)
#define REG_TIMER_TSTA_FL_STA_ADDR                                              (REG_TIMER_TSTA)

/*
TINTCLR:
[4]:WTC FH_CLR 1'h0 Write 1 to clear TMR_FCLKH interrupt status
[3]:WTC WD_CLR 1'h0 Write 1 to clear WD interrupt status
[2]:WTC S_CLR 1'h0 Write 1 to clear TMR_SCLK interrupt status
[1]:WTC FS_CLR 1'h0 Write 1 to clear TMR_FCLKS interrupt status
[0]:WTC FL_CLR 1'h0 Write 1 to clear TMR_FCLKL interrupt status
*/
#define REG_TIMER_TINTCLR                                                       (TIMER_BASE + 0x034)
#define REG_TIMER_TINTCLR_FH_CLR                          
#define REG_TIMER_TINTCLR_WD_CLR                          
#define REG_TIMER_TINTCLR_S_CLR                           
#define REG_TIMER_TINTCLR_FS_CLR                          
#define REG_TIMER_TINTCLR_FL_CLR                          
#define REG_TIMER_TINTCLR_FH_CLR_MASK                                           (0x00000010)
#define REG_TIMER_TINTCLR_WD_CLR_MASK                                           (0x00000008)
#define REG_TIMER_TINTCLR_S_CLR_MASK                                            (0x00000004)
#define REG_TIMER_TINTCLR_FS_CLR_MASK                                           (0x00000002)
#define REG_TIMER_TINTCLR_FL_CLR_MASK                                           (0x00000001)
#define REG_TIMER_TINTCLR_FH_CLR_BIT                                            (4)
#define REG_TIMER_TINTCLR_WD_CLR_BIT                                            (3)
#define REG_TIMER_TINTCLR_S_CLR_BIT                                             (2)
#define REG_TIMER_TINTCLR_FS_CLR_BIT                                            (1)
#define REG_TIMER_TINTCLR_FL_CLR_BIT                                            (0)
#define REG_TIMER_TINTCLR_FH_CLR_ADDR                                           (REG_TIMER_TINTCLR)
#define REG_TIMER_TINTCLR_WD_CLR_ADDR                                           (REG_TIMER_TINTCLR)
#define REG_TIMER_TINTCLR_S_CLR_ADDR                                            (REG_TIMER_TINTCLR)
#define REG_TIMER_TINTCLR_FS_CLR_ADDR                                           (REG_TIMER_TINTCLR)
#define REG_TIMER_TINTCLR_FL_CLR_ADDR                                           (REG_TIMER_TINTCLR)

/*
TSTA2:
[1]:R TSCLK_RLD_EN_SYN_BACK 1'h0 TMR_SCLK reload enable status.
[0]:R TSCLK_CEN_SYN_BACK 1'h0 TMR_SCLK working status.
*/
#define REG_TIMER_TSTA2                                                         (TIMER_BASE + 0x038)
#define REG_TIMER_TSTA2_TSCLK_RLD_EN_SYN_BACK             
#define REG_TIMER_TSTA2_TSCLK_CEN_SYN_BACK                
#define REG_TIMER_TSTA2_TSCLK_RLD_EN_SYN_BACK_MASK                              (0x00000002)
#define REG_TIMER_TSTA2_TSCLK_CEN_SYN_BACK_MASK                                 (0x00000001)
#define REG_TIMER_TSTA2_TSCLK_RLD_EN_SYN_BACK_BIT                               (1)
#define REG_TIMER_TSTA2_TSCLK_CEN_SYN_BACK_BIT                                  (0)
#define REG_TIMER_TSTA2_TSCLK_RLD_EN_SYN_BACK_ADDR                              (REG_TIMER_TSTA2)
#define REG_TIMER_TSTA2_TSCLK_CEN_SYN_BACK_ADDR                                 (REG_TIMER_TSTA2)

/*
TINTMASK:
[4]:RW INT_TMR_FCLK_H_MASK 1'h1 interrupt mask of timer_fclkh
[3]:RW INT_WD_MASK 1'h1 interrupt mask of watchdog
[2]:RW INT_TMR_SCLK_MASK 1'h1 interrupt mask of timer_sclk
[1]:RW INT_TMR_FCLK_S_MASK 1'h1 interrupt mask of timer_fclks
[0]:RW INT_TMR_FCLK_L_MASK 1'h1 interrupt mask of timer_fclkl
*/
#define REG_TIMER_TINTMASK                                                      (TIMER_BASE + 0x03C)
#define REG_TIMER_TINTMASK_INT_TMR_FCLK_H_MASK            
#define REG_TIMER_TINTMASK_INT_WD_MASK                    
#define REG_TIMER_TINTMASK_INT_TMR_SCLK_MASK              
#define REG_TIMER_TINTMASK_INT_TMR_FCLK_S_MASK            
#define REG_TIMER_TINTMASK_INT_TMR_FCLK_L_MASK            
#define REG_TIMER_TINTMASK_INT_TMR_FCLK_H_MASK_MASK                             (0x00000010)
#define REG_TIMER_TINTMASK_INT_WD_MASK_MASK                                     (0x00000008)
#define REG_TIMER_TINTMASK_INT_TMR_SCLK_MASK_MASK                               (0x00000004)
#define REG_TIMER_TINTMASK_INT_TMR_FCLK_S_MASK_MASK                             (0x00000002)
#define REG_TIMER_TINTMASK_INT_TMR_FCLK_L_MASK_MASK                             (0x00000001)
#define REG_TIMER_TINTMASK_INT_TMR_FCLK_H_MASK_BIT                              (4)
#define REG_TIMER_TINTMASK_INT_WD_MASK_BIT                                      (3)
#define REG_TIMER_TINTMASK_INT_TMR_SCLK_MASK_BIT                                (2)
#define REG_TIMER_TINTMASK_INT_TMR_FCLK_S_MASK_BIT                              (1)
#define REG_TIMER_TINTMASK_INT_TMR_FCLK_L_MASK_BIT                              (0)
#define REG_TIMER_TINTMASK_INT_TMR_FCLK_H_MASK_ADDR                             (REG_TIMER_TINTMASK)
#define REG_TIMER_TINTMASK_INT_WD_MASK_ADDR                                     (REG_TIMER_TINTMASK)
#define REG_TIMER_TINTMASK_INT_TMR_SCLK_MASK_ADDR                               (REG_TIMER_TINTMASK)
#define REG_TIMER_TINTMASK_INT_TMR_FCLK_S_MASK_ADDR                             (REG_TIMER_TINTMASK)
#define REG_TIMER_TINTMASK_INT_TMR_FCLK_L_MASK_ADDR                             (REG_TIMER_TINTMASK)

/*
WDLOCK:
[0]:RW WD_LOCK 1'h1 The register configuration of Watch Dog Lock or not;0 = unlock; 1 = lock
*/
#define REG_TIMER_WDLOCK                                                        (TIMER_BASE + 0x040)
#define REG_TIMER_WDLOCK_WD_LOCK                          
#define REG_TIMER_WDLOCK_WD_LOCK_MASK                                           (0x00000001)
#define REG_TIMER_WDLOCK_WD_LOCK_BIT                                            (0)
#define REG_TIMER_WDLOCK_WD_LOCK_ADDR                                           (REG_TIMER_WDLOCK)

/*
WDCINI:
[31:0]:RW WDCINI 32'hffff_ffff Initial/reload value for Watch Dog counter
*/
#define REG_TIMER_WDCINI                                                        (TIMER_BASE + 0x044)
#define REG_TIMER_WDCINI_WDCINI                           
#define REG_TIMER_WDCINI_WDCINI_MASK                                            (0xFFFFFFFF)
#define REG_TIMER_WDCINI_WDCINI_BIT                                             (0)
#define REG_TIMER_WDCINI_WDCINI_ADDR                                            (REG_TIMER_WDCINI)

/*
WDCNT:
[31:0]:R WDCNT 32'hffff_ffff Watch Dog counter value
*/
#define REG_TIMER_WDCNT                                                         (TIMER_BASE + 0x048)
#define REG_TIMER_WDCNT_WDCNT                             
#define REG_TIMER_WDCNT_WDCNT_MASK                                              (0xFFFFFFFF)
#define REG_TIMER_WDCNT_WDCNT_BIT                                               (0)
#define REG_TIMER_WDCNT_WDCNT_ADDR                                              (REG_TIMER_WDCNT)

/*
WDFEED:
[31:0]:WTC WDFEED 32'h0 write to this register will feed the Watch Dog
*/
#define REG_TIMER_WDFEED                                                        (TIMER_BASE + 0x04C)
#define REG_TIMER_WDFEED_WDFEED                           
#define REG_TIMER_WDFEED_WDFEED_MASK                                            (0xFFFFFFFF)
#define REG_TIMER_WDFEED_WDFEED_BIT                                             (0)
#define REG_TIMER_WDFEED_WDFEED_ADDR                                            (REG_TIMER_WDFEED)

/*
LTEN:
[7:6]:RW LT4_EN 2'h0 LED-timer 4 Enable00: always output low01: always output high1x: output as figure 4.1 in Timer MAS.
[5:4]:RW LT3_EN 2'h0 LED-timer 3 Enable00: always output low01: always output high1x: output as figure 4.1 in Timer MAS.
[3:2]:RW LT2_EN 2'h0 LED-timer 2 Enable00: always output low01: always output high1x: output as figure 4.1 in Timer MAS.
[1:0]:RW LT1_EN 2'h0 LED-timer 1 Enable00: always output low01: always output high1x: output as figure 4.1 in Timer MAS.
*/
#define REG_TIMER_LTEN                                                          (TIMER_BASE + 0x050)
#define REG_TIMER_LTEN_LT4_EN                            
#define REG_TIMER_LTEN_LT3_EN                             
#define REG_TIMER_LTEN_LT2_EN                             
#define REG_TIMER_LTEN_LT1_EN                             
#define REG_TIMER_LTEN_LT4_EN_MASK                                              (0x000000C0)
#define REG_TIMER_LTEN_LT3_EN_MASK                                              (0x00000030)
#define REG_TIMER_LTEN_LT2_EN_MASK                                              (0x0000000C)
#define REG_TIMER_LTEN_LT1_EN_MASK                                              (0x00000003)
#define REG_TIMER_LTEN_LT4_EN_BIT                                               (6)
#define REG_TIMER_LTEN_LT3_EN_BIT                                               (4)
#define REG_TIMER_LTEN_LT2_EN_BIT                                               (2)
#define REG_TIMER_LTEN_LT1_EN_BIT                                               (0)
#define REG_TIMER_LTEN_LT4_EN_ADDR                                              (REG_TIMER_LTEN)
#define REG_TIMER_LTEN_LT3_EN_ADDR                                              (REG_TIMER_LTEN)
#define REG_TIMER_LTEN_LT2_EN_ADDR                                              (REG_TIMER_LTEN)
#define REG_TIMER_LTEN_LT1_EN_ADDR                                              (REG_TIMER_LTEN)

/*
LT1_CTRL1:
[31:28]:RW LT1_ONOFF_TIMES 4'd1 on/off repeat times. Argument C in figure 4.1 in Timer MAS. Note this field can't be 0.
[27:14]:RW LT1_OFF_LEN 14'd500 off time. Timescale is 1 ms. Note this field can't be 0.
[13:0]:RW LT1_ON_LEN 14'd500 on time. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT1_CTRL1                                                     (TIMER_BASE + 0x054)
#define REG_TIMER_LT1_CTRL1_LT1_ONOFF_TIMES               
#define REG_TIMER_LT1_CTRL1_LT1_OFF_LEN                   
#define REG_TIMER_LT1_CTRL1_LT1_ON_LEN                    
#define REG_TIMER_LT1_CTRL1_LT1_ONOFF_TIMES_MASK                                (0xF0000000)
#define REG_TIMER_LT1_CTRL1_LT1_OFF_LEN_MASK                                    (0x0FFFC000)
#define REG_TIMER_LT1_CTRL1_LT1_ON_LEN_MASK                                     (0x00003FFF)
#define REG_TIMER_LT1_CTRL1_LT1_ONOFF_TIMES_BIT                                 (28)
#define REG_TIMER_LT1_CTRL1_LT1_OFF_LEN_BIT                                     (14)
#define REG_TIMER_LT1_CTRL1_LT1_ON_LEN_BIT                                      (0)
#define REG_TIMER_LT1_CTRL1_LT1_ONOFF_TIMES_ADDR                                (REG_TIMER_LT1_CTRL1)
#define REG_TIMER_LT1_CTRL1_LT1_OFF_LEN_ADDR                                    (REG_TIMER_LT1_CTRL1)
#define REG_TIMER_LT1_CTRL1_LT1_ON_LEN_ADDR                                     (REG_TIMER_LT1_CTRL1)

/*
LT1_CTRL2:
[27:24]:RW LT1_RPT_TIMES 4'h0 Repeat times of whole on/off/on/off cycles. Argument E in figure 4.1 in Timer MAS. If this is 0, it means repeat forever.
[21]:RW LT1_LEVEL_INV 1'b0 Output inverse level in state ON/OFF.0: output 1 in ON(state A), output 0 in OFF(state B).1: output 0 in ON(state A), output 1 in OFF(state B).
[20]:RW LT1_ONOFF_INTVL 1'b0 Output high/low during interval.
[19:0]:RW LT1_INTVL_LEN 20'd9000 Time length of interval. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT1_CTRL2                                                     (TIMER_BASE + 0x058)
#define REG_TIMER_LT1_CTRL2_LT1_RPT_TIMES                 
#define REG_TIMER_LT1_CTRL2_LT1_LEVEL_INV                 
#define REG_TIMER_LT1_CTRL2_LT1_ONOFF_INTVL               
#define REG_TIMER_LT1_CTRL2_LT1_INTVL_LEN                 
#define REG_TIMER_LT1_CTRL2_LT1_RPT_TIMES_MASK                                  (0x0F000000)
#define REG_TIMER_LT1_CTRL2_LT1_LEVEL_INV_MASK                                  (0x00200000)
#define REG_TIMER_LT1_CTRL2_LT1_ONOFF_INTVL_MASK                                (0x00100000)
#define REG_TIMER_LT1_CTRL2_LT1_INTVL_LEN_MASK                                  (0x000FFFFF)
#define REG_TIMER_LT1_CTRL2_LT1_RPT_TIMES_BIT                                   (24)
#define REG_TIMER_LT1_CTRL2_LT1_LEVEL_INV_BIT                                   (21)
#define REG_TIMER_LT1_CTRL2_LT1_ONOFF_INTVL_BIT                                 (20)
#define REG_TIMER_LT1_CTRL2_LT1_INTVL_LEN_BIT                                   (0)
#define REG_TIMER_LT1_CTRL2_LT1_RPT_TIMES_ADDR                                  (REG_TIMER_LT1_CTRL2)
#define REG_TIMER_LT1_CTRL2_LT1_LEVEL_INV_ADDR                                  (REG_TIMER_LT1_CTRL2)
#define REG_TIMER_LT1_CTRL2_LT1_ONOFF_INTVL_ADDR                                (REG_TIMER_LT1_CTRL2)
#define REG_TIMER_LT1_CTRL2_LT1_INTVL_LEN_ADDR                                  (REG_TIMER_LT1_CTRL2)

/*
LT2_CTRL1:
[31:28]:RW LT2_ONOFF_TIMES 4'd1 on/off repeat times. Argument C in figure 4.1 in Timer MAS. Note this field can't be 0.
[27:14]:RW LT2_OFF_LEN 14'd500 off time. Timescale is 1 ms. Note this field can't be 0.
[13:0]:RW LT2_ON_LEN 14'd500 on time. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT2_CTRL1                                                     (TIMER_BASE + 0x05C)
#define REG_TIMER_LT2_CTRL1_LT2_ONOFF_TIMES               
#define REG_TIMER_LT2_CTRL1_LT2_OFF_LEN                   
#define REG_TIMER_LT2_CTRL1_LT2_ON_LEN                    
#define REG_TIMER_LT2_CTRL1_LT2_ONOFF_TIMES_MASK                                (0xF0000000)
#define REG_TIMER_LT2_CTRL1_LT2_OFF_LEN_MASK                                    (0x0FFFC000)
#define REG_TIMER_LT2_CTRL1_LT2_ON_LEN_MASK                                     (0x00003FFF)
#define REG_TIMER_LT2_CTRL1_LT2_ONOFF_TIMES_BIT                                 (28)
#define REG_TIMER_LT2_CTRL1_LT2_OFF_LEN_BIT                                     (14)
#define REG_TIMER_LT2_CTRL1_LT2_ON_LEN_BIT                                      (0)
#define REG_TIMER_LT2_CTRL1_LT2_ONOFF_TIMES_ADDR                                (REG_TIMER_LT2_CTRL1)
#define REG_TIMER_LT2_CTRL1_LT2_OFF_LEN_ADDR                                    (REG_TIMER_LT2_CTRL1)
#define REG_TIMER_LT2_CTRL1_LT2_ON_LEN_ADDR                                     (REG_TIMER_LT2_CTRL1)

/*
LT2_CTRL2:
[27:24]:RW LT2_RPT_TIMES 4'h0 Repeat times of whole on/off/on/off cycles. Argument E in figure 4.1 in Timer MAS. If this is 0, it means repeat forever.
[21]:RW LT2_LEVEL_INV 1'b0 Output inverse level in state ON/OFF.0: output 1 in ON(state A), output 0 in OFF(state B).1: output 0 in ON(state A), output 1 in OFF(state B).
[20]:RW LT2_ONOFF_INTVL 1'b0 Output high/low during interval.
[19:0]:RW LT2_INTVL_LEN 20'd9000 Time length of interval. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT2_CTRL2                                                     (TIMER_BASE + 0x060)
#define REG_TIMER_LT2_CTRL2_LT2_RPT_TIMES                 
#define REG_TIMER_LT2_CTRL2_LT2_LEVEL_INV                 
#define REG_TIMER_LT2_CTRL2_LT2_ONOFF_INTVL               
#define REG_TIMER_LT2_CTRL2_LT2_INTVL_LEN                 
#define REG_TIMER_LT2_CTRL2_LT2_RPT_TIMES_MASK                                  (0x0F000000)
#define REG_TIMER_LT2_CTRL2_LT2_LEVEL_INV_MASK                                  (0x00200000)
#define REG_TIMER_LT2_CTRL2_LT2_ONOFF_INTVL_MASK                                (0x00100000)
#define REG_TIMER_LT2_CTRL2_LT2_INTVL_LEN_MASK                                  (0x000FFFFF)
#define REG_TIMER_LT2_CTRL2_LT2_RPT_TIMES_BIT                                   (24)
#define REG_TIMER_LT2_CTRL2_LT2_LEVEL_INV_BIT                                   (21)
#define REG_TIMER_LT2_CTRL2_LT2_ONOFF_INTVL_BIT                                 (20)
#define REG_TIMER_LT2_CTRL2_LT2_INTVL_LEN_BIT                                   (0)
#define REG_TIMER_LT2_CTRL2_LT2_RPT_TIMES_ADDR                                  (REG_TIMER_LT2_CTRL2)
#define REG_TIMER_LT2_CTRL2_LT2_LEVEL_INV_ADDR                                  (REG_TIMER_LT2_CTRL2)
#define REG_TIMER_LT2_CTRL2_LT2_ONOFF_INTVL_ADDR                                (REG_TIMER_LT2_CTRL2)
#define REG_TIMER_LT2_CTRL2_LT2_INTVL_LEN_ADDR                                  (REG_TIMER_LT2_CTRL2)

/*
LT3_CTRL1:
[31:28]:RW LT3_ONOFF_TIMES 4'd1 on/off repeat times. Argument C in figure 4.1 in Timer MAS. Note this field can't be 0.
[27:14]:RW LT3_OFF_LEN 14'd500 off time. Timescale is 1 ms. Note this field can't be 0.
[13:0]:RW LT3_ON_LEN 14'd500 on time. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT3_CTRL1                                                     (TIMER_BASE + 0x064)
#define REG_TIMER_LT3_CTRL1_LT3_ONOFF_TIMES               
#define REG_TIMER_LT3_CTRL1_LT3_OFF_LEN                   
#define REG_TIMER_LT3_CTRL1_LT3_ON_LEN                    
#define REG_TIMER_LT3_CTRL1_LT3_ONOFF_TIMES_MASK                                (0xF0000000)
#define REG_TIMER_LT3_CTRL1_LT3_OFF_LEN_MASK                                    (0x0FFFC000)
#define REG_TIMER_LT3_CTRL1_LT3_ON_LEN_MASK                                     (0x00003FFF)
#define REG_TIMER_LT3_CTRL1_LT3_ONOFF_TIMES_BIT                                 (28)
#define REG_TIMER_LT3_CTRL1_LT3_OFF_LEN_BIT                                     (14)
#define REG_TIMER_LT3_CTRL1_LT3_ON_LEN_BIT                                      (0)
#define REG_TIMER_LT3_CTRL1_LT3_ONOFF_TIMES_ADDR                                (REG_TIMER_LT3_CTRL1)
#define REG_TIMER_LT3_CTRL1_LT3_OFF_LEN_ADDR                                    (REG_TIMER_LT3_CTRL1)
#define REG_TIMER_LT3_CTRL1_LT3_ON_LEN_ADDR                                     (REG_TIMER_LT3_CTRL1)

/*
LT3_CTRL2:
[27:24]:RW LT3_RPT_TIMES 4'h0 Repeat times of whole on/off/on/off cycles. Argument E in figure 4.1 in Timer MAS. If this is 0, it means repeat forever.
[21]:RW LT3_LEVEL_INV 1'b0 Output inverse level in state ON/OFF.0: output 1 in ON(state A), output 0 in OFF(state B).1: output 0 in ON(state A), output 1 in OFF(state B).
[20]:RW LT3_ONOFF_INTVL 1'b0 Output high/low during interval.
[19:0]:RW LT3_INTVL_LEN 20'd9000 Time length of interval. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT3_CTRL2                                                     (TIMER_BASE + 0x068)
#define REG_TIMER_LT3_CTRL2_LT3_RPT_TIMES                 
#define REG_TIMER_LT3_CTRL2_LT3_LEVEL_INV                 
#define REG_TIMER_LT3_CTRL2_LT3_ONOFF_INTVL               
#define REG_TIMER_LT3_CTRL2_LT3_INTVL_LEN                 
#define REG_TIMER_LT3_CTRL2_LT3_RPT_TIMES_MASK                                  (0x0F000000)
#define REG_TIMER_LT3_CTRL2_LT3_LEVEL_INV_MASK                                  (0x00200000)
#define REG_TIMER_LT3_CTRL2_LT3_ONOFF_INTVL_MASK                                (0x00100000)
#define REG_TIMER_LT3_CTRL2_LT3_INTVL_LEN_MASK                                  (0x000FFFFF)
#define REG_TIMER_LT3_CTRL2_LT3_RPT_TIMES_BIT                                   (24)
#define REG_TIMER_LT3_CTRL2_LT3_LEVEL_INV_BIT                                   (21)
#define REG_TIMER_LT3_CTRL2_LT3_ONOFF_INTVL_BIT                                 (20)
#define REG_TIMER_LT3_CTRL2_LT3_INTVL_LEN_BIT                                   (0)
#define REG_TIMER_LT3_CTRL2_LT3_RPT_TIMES_ADDR                                  (REG_TIMER_LT3_CTRL2)
#define REG_TIMER_LT3_CTRL2_LT3_LEVEL_INV_ADDR                                  (REG_TIMER_LT3_CTRL2)
#define REG_TIMER_LT3_CTRL2_LT3_ONOFF_INTVL_ADDR                                (REG_TIMER_LT3_CTRL2)
#define REG_TIMER_LT3_CTRL2_LT3_INTVL_LEN_ADDR                                  (REG_TIMER_LT3_CTRL2)

/*
LT4_CTRL1:
[31:28]:RW LT4_ONOFF_TIMES 4'd1 on/off repeat times. Argument C in figure 4.1 in Timer MAS. Note this field can't be 0.
[27:14]:RW LT4_OFF_LEN 14'd500 off time. Timescale is 1 ms. Note this field can't be 0.
[13:0]:RW LT4_ON_LEN 14'd500 on time. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT4_CTRL1                                                     (TIMER_BASE + 0x06C)
#define REG_TIMER_LT4_CTRL1_LT4_ONOFF_TIMES               
#define REG_TIMER_LT4_CTRL1_LT4_OFF_LEN                   
#define REG_TIMER_LT4_CTRL1_LT4_ON_LEN                    
#define REG_TIMER_LT4_CTRL1_LT4_ONOFF_TIMES_MASK                                (0xF0000000)
#define REG_TIMER_LT4_CTRL1_LT4_OFF_LEN_MASK                                    (0x0FFFC000)
#define REG_TIMER_LT4_CTRL1_LT4_ON_LEN_MASK                                     (0x00003FFF)
#define REG_TIMER_LT4_CTRL1_LT4_ONOFF_TIMES_BIT                                 (28)
#define REG_TIMER_LT4_CTRL1_LT4_OFF_LEN_BIT                                     (14)
#define REG_TIMER_LT4_CTRL1_LT4_ON_LEN_BIT                                      (0)
#define REG_TIMER_LT4_CTRL1_LT4_ONOFF_TIMES_ADDR                                (REG_TIMER_LT4_CTRL1)
#define REG_TIMER_LT4_CTRL1_LT4_OFF_LEN_ADDR                                    (REG_TIMER_LT4_CTRL1)
#define REG_TIMER_LT4_CTRL1_LT4_ON_LEN_ADDR                                     (REG_TIMER_LT4_CTRL1)

/*
LT4_CTRL2:
[27:24]:RW LT4_RPT_TIMES 4'h0 Repeat times of whole on/off/on/off cycles. Argument E in figure 4.1 in Timer MAS. If this is 0, it means repeat forever.
[21]:RW LT4_LEVEL_INV 1'b0 Output inverse level in state ON/OFF.0: output 1 in ON(state A), output 0 in OFF(state B).1: output 0 in ON(state A), output 1 in OFF(state B).
[20]:RW LT4_ONOFF_INTVL 1'b0 Output high/low during interval.
[19:0]:RW LT4_INTVL_LEN 20'd9000 Time length of interval. Timescale is 1 ms. Note this field can't be 0.
*/
#define REG_TIMER_LT4_CTRL2                                                     (TIMER_BASE + 0x070)
#define REG_TIMER_LT4_CTRL2_LT4_RPT_TIMES                 
#define REG_TIMER_LT4_CTRL2_LT4_LEVEL_INV                 
#define REG_TIMER_LT4_CTRL2_LT4_ONOFF_INTVL               
#define REG_TIMER_LT4_CTRL2_LT4_INTVL_LEN                 
#define REG_TIMER_LT4_CTRL2_LT4_RPT_TIMES_MASK                                  (0x0F000000)
#define REG_TIMER_LT4_CTRL2_LT4_LEVEL_INV_MASK                                  (0x00200000)
#define REG_TIMER_LT4_CTRL2_LT4_ONOFF_INTVL_MASK                                (0x00100000)
#define REG_TIMER_LT4_CTRL2_LT4_INTVL_LEN_MASK                                  (0x000FFFFF)
#define REG_TIMER_LT4_CTRL2_LT4_RPT_TIMES_BIT                                   (24)
#define REG_TIMER_LT4_CTRL2_LT4_LEVEL_INV_BIT                                   (21)
#define REG_TIMER_LT4_CTRL2_LT4_ONOFF_INTVL_BIT                                 (20)
#define REG_TIMER_LT4_CTRL2_LT4_INTVL_LEN_BIT                                   (0)
#define REG_TIMER_LT4_CTRL2_LT4_RPT_TIMES_ADDR                                  (REG_TIMER_LT4_CTRL2)
#define REG_TIMER_LT4_CTRL2_LT4_LEVEL_INV_ADDR                                  (REG_TIMER_LT4_CTRL2)
#define REG_TIMER_LT4_CTRL2_LT4_ONOFF_INTVL_ADDR                                (REG_TIMER_LT4_CTRL2)
#define REG_TIMER_LT4_CTRL2_LT4_INTVL_LEN_ADDR                                  (REG_TIMER_LT4_CTRL2)

#endif /*TIMER_BASE*/
#endif /*_REG_TIMER_H_*/

