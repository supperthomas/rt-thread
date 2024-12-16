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
        vpla tmier

Description:
        irq Driver
Revision History:
        Date                  Author                   Description
    2022.0620           cao.xuetao              initial version
------------------------------------------------------------------------------*/
#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef _VPLA_PMU_H_
#define _VPLA_PMU_H_

#include "vstdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

 /**
 * @addtogroup Vpla_PMU
 * 
 * @{
 */

/// @brief 初始化VPMU模块
void VPMU_Init ( void );

/// @brief 设置PMU使能状态
/// @param IsEnable 是否使能PMU
void VPMU_SetPMUEn ( BOOL IsEnable );

/// @brief 获取充电器状态
/// @param p_State 充电器状态的指针
/// @return 充电器状态值
VRET_VALUE VPMU_GetChargerState ( UINT8 *p_State );

/// @brief 设置电池低电量水平
/// @param MBV 低电量水平值
void VPMU_SetBatteryLowLevel ( UINT8 MBV );

/// @brief 设置LDO5使能状态
/// @param IsEnable 是否使能LDO5
void VPMU_SetLDO5En ( BOOL IsEnable );

/// @brief 设置充电电压和电流
/// @param Voltage 充电电压
/// @param Current 充电电流
void VPMU_SetCharge ( UINT8 Voltage, UINT8 Current );

/// @brief 设置MCP参数
/// @param Mcp MCP值
void VPMU_SetMCP ( UINT8 Mcp );

/// @brief 设置DCDC模式
/// @param Mode DCDC模式值
void VPMU_SetDCDCMode ( UINT8 Mode );

/// @brief 获取库仑计的VD值
/// @param p_VD 库仑计VD值的指针
/// @return 库仑计VD值
VRET_VALUE VPMU_GetCoulombVD ( UINT8 *p_VD );

/// @brief 获取版本信息
/// @param p_VerInfo 版本信息的指针
/// @return 版本信息值
VRET_VALUE VPMU_GetVerInfo ( UINT8 *p_VerInfo );

/// @brief 设置旁路模式
/// @param Enable 是否使能旁路模式
void VPMU_Set_Bypass (UINT8 Enable);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif  /* _VPLA_PMU_H_ */
