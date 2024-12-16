/*------------------------------------------------------------------------------
                    COPYRIGHT (C) 2017, Vimicro Corporation
                              ALL RIGHTS RESERVED

This source code has been made available to you by VIMICRO on an AS-IS basis.
Anyone receiving this source code is licensed under VIMICRO copyrights to use
it in any way he or she deems fit, including copying it, modifying it,
compiling it, and redistributing it either with or without modifications. Any
person who transfers this source code or any derivative work must include the
VIMICRO copyright notice and this paragraph in the transferred software.

Module:
    main

Description:
    test

Revision History:
    Date                  Author                   Description
    2022.05.30            cxt                       9648
------------------------------------------------------------------------------*/
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vstdos.h"
#include "vstddrv.h"

#define Dbg_PRINT        vcom_printf

void VSYS_SOC_FaultHandler(void * p_StkPtr)
{
	UINT32 return_addr = __GET_RETURN_ADDR();

	vdirect_printf( "Fault return[0x%08X]\r\n", return_addr );
	vdirect_printf ( "p_StkPtr:0x%08X StkPtr_return:0x%08X\r\n",p_StkPtr,*((UINT32 *)p_StkPtr+5));

	do {
		vdirect_printf( "Fault return[0x%08X]\r\n", return_addr );
		vdirect_printf ( "p_StkPtr:0x%08X StkPtr_return:0x%08X\r\n",p_StkPtr,*((UINT32 *)p_StkPtr+5));
		vdelayus(1000 * 100 * PERIODIC_PFM_SECONDS);
	} while(PERIODIC_PFM_EN);
}

int main ( void )
{
	UINT32* addr1, *addr2;
    VCPU_CoreInit(); 
    VCPU_INT_Enable();
	VSYS_CKM_Init();
	
    Dbg_PRINT("API test...%x %x\r\n",__CPUPcGet(),__CPUSpGet());
	
	VSYS_GPIO_Init(VSYS_GPIO_PIN_9, VSYS_GPIO_MODE_OUTPUT, VSYS_GPIO_OUTPUT_HIGH_STRENGTH);
	
	addr1 = VShareMemInit((SINT8*)"abcd",12,TRUE);
	Dbg_PRINT("VShareMemInit 0x%08x\r\n",addr1);
	
	addr2 = VShareMemInit((SINT8*)"cfg",56,FALSE);
	Dbg_PRINT("VShareMemInit 0x%08x\r\n",addr2);
	
	addr2 = VShareMemInit(NULL,51,FALSE);
	Dbg_PRINT("VShareMemInit 0x%08x\r\n",addr2);
	
	addr2 = VShareMemInit(NULL,56,FALSE);
	Dbg_PRINT("VShareMemInit 0x%08x\r\n",addr2);
	
	void (*func_type)();
	func_type = (void*)0x20000000;
	func_type();
	
	while(1){
		
		addr1 = vmalloc(2);
		addr2 = vmalloc(3);
		vfree(addr1);
		vfree(addr2);
		addr1 = vmalloc(9);
		addr2 = vmalloc(13);
		vfree(addr1);
		vfree(addr2);
		addr1 = vmalloc(8);
		addr2 = vmalloc(17);
		vfree(addr1);
		vfree(addr2);
		VSYS_GPIO_SetPinOutput(VSYS_GPIO_PIN_9, TRUE);
		vdelayus(1000*500);
		VSYS_GPIO_SetPinOutput(VSYS_GPIO_PIN_9, FALSE);
		vdelayus(1000*500);
	}
	
	VSYS_FLASH_Init();
	VSYS_FLASH_Erase(VSYS_FLASH_INPACKAGE,0,VSYS_FLASH_ALL_SIZE);
	
    VSYS_LED_Init(VSYS_LED_INDEX_0);
    VSYS_LED_Init(VSYS_LED_INDEX_1);
    VSYS_LED_Init(VSYS_LED_INDEX_2);
    VSYS_LED_Init(VSYS_LED_INDEX_3);

    VDRVLEDConfig_t led02_config = {0};
    led02_config.on_length =  100;
    led02_config.off_length =  100;
    led02_config.onoff_times =  3;
    led02_config.onoff_level =  TRUE;
    led02_config.intvl_length =  500;
    led02_config.repeat_times =  0;
    led02_config.intvl_state =  TRUE;
    VDRVLEDConfig_t led1_config = {1};
    VDRVLEDConfig_t led3_config = {0};

    VSYS_LED_SetMode(VSYS_LED_INDEX_0 ,&led02_config);
    VSYS_LED_SetMode(VSYS_LED_INDEX_1 ,&led1_config);
    VSYS_LED_SetMode(VSYS_LED_INDEX_2 ,&led02_config);
    VSYS_LED_SetMode(VSYS_LED_INDEX_3 ,&led3_config);
	
	VDRVI2CConfig_t i2c_config = {.mode = VSYS_I2C_MODE_HDS};
	VSYS_I2C_Init(VSYS_I2C_SLAVE_ANA, &i2c_config);
	
	extern void Enable_VA9900 ( void );
	Enable_VA9900();
    while(1){
        UINT8 reg_arr[6] = {0xA5,};
        VSYS_I2C_Write(VSYS_I2C_SLAVE_ANA,0x34,0xF0,reg_arr,1);
        VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,0x34,0xF0,reg_arr,6);
        VSYS_I2C_BitOperat(VSYS_I2C_SLAVE_ANA,0x34,0xF0,VSYS_I2C_Bit_SET,0x0A);
        VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,0x34,0xF0,reg_arr,6);
        VSYS_I2C_BitOperat(VSYS_I2C_SLAVE_ANA,0x34,0xF0,VSYS_I2C_Bit_CLR,0x80);
        VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,0x34,0xF0,reg_arr,6);
		
		VSYS_I2C_Write(VSYS_I2C_SLAVE_ANA,0x25,0x00,reg_arr,1);
        VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,0x25,0x00,reg_arr,6);
		VSYS_I2C_BitOperat(VSYS_I2C_SLAVE_ANA,0x25,0x00,VSYS_I2C_Bit_CLR,0x08);
        VSYS_I2C_Read(VSYS_I2C_SLAVE_ANA,0x25,0x00,reg_arr,6);

    }

}
