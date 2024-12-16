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
#include "vpla_ckm.h"
#include "vpla_i2c.h"
#include "vpla_gpio.h"
#include "vdrv_gpio.h"
#include "vdrv_i2c.h"
#include "vdrv_9900.h"


#define Dbg_PRINT        vcom_printf

#define VA9900_I2C_SLAVE_ADDR       0x25
#define PMU_I2C_SLAVE_ADDR      0x34


UINT8 reg_buff[4] = {0};
int main ( void )
{
    int i  = 0 , j = 0, k = 0, loop_num = 0;
    UINT8 reg_value = 0;
    UINT8 reg_buff[8] = {0};
	
    VCPU_CoreInit(); 
    VCPU_INT_Enable();
    VSYS_CKM_Init();

#if 1
	
	while(1){
        VDRVI2CConfig_t i2c_config = {.mode = VSYS_I2C_MODE_STANDARD};
		VSYS_I2C_Init(VSYS_I2C_SLAVE_PROM,&i2c_config);
		reg_buff[0] = 0x12;
		reg_buff[1] = 0x04;
		VSYS_I2C_Write(VSYS_I2C_SLAVE_PROM,VA9900_I2C_SLAVE_ADDR,0x00,reg_buff,2);
		vmemset(reg_buff,0,sizeof(reg_buff));
		VSYS_I2C_Read(VSYS_I2C_SLAVE_PROM,VA9900_I2C_SLAVE_ADDR,0x00,reg_buff,2);
		reg_buff[0] = 0x23;
		reg_buff[1] = 0x01;
		VSYS_I2C_Write(VSYS_I2C_SLAVE_PROM,VA9900_I2C_SLAVE_ADDR,0x00,reg_buff,2);
		vmemset(reg_buff,0,sizeof(reg_buff));
		VSYS_I2C_Read(VSYS_I2C_SLAVE_PROM,VA9900_I2C_SLAVE_ADDR,0x00,reg_buff,2);
		VSYS_I2C_Uninit(VSYS_I2C_SLAVE_PROM);
		
		i2c_config.scl_pin = VSYS_GPIO_PIN_4;
		i2c_config.sda_pin = VSYS_GPIO_PIN_3;
		VSYS_I2C_Init(VSYS_I2C_SLAVE_SOFTWARE,&i2c_config);
		reg_buff[0] = 0x12;
		reg_buff[1] = 0x04;
		VSYS_I2C_Write(VSYS_I2C_SLAVE_SOFTWARE,VA9900_I2C_SLAVE_ADDR,0x00,reg_buff,2);
		vmemset(reg_buff,0,sizeof(reg_buff));
		VSYS_I2C_Read(VSYS_I2C_SLAVE_SOFTWARE,VA9900_I2C_SLAVE_ADDR,0x00,reg_buff,2);
		reg_buff[0] = 0x23;
		reg_buff[1] = 0x01;
		VSYS_I2C_Write(VSYS_I2C_SLAVE_SOFTWARE,VA9900_I2C_SLAVE_ADDR,0x00,reg_buff,2);
		vmemset(reg_buff,0,sizeof(reg_buff));
		VSYS_I2C_Read(VSYS_I2C_SLAVE_SOFTWARE,VA9900_I2C_SLAVE_ADDR,0x00,reg_buff,2);
		VSYS_I2C_Uninit(VSYS_I2C_SLAVE_PROM);
	}
#else
	VI2C_Continue_write(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, 0x56);
    VI2C_register_read_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, &reg_value);
    VI2C_Transfer_Done_irq();
	
	Enable_VA9900();
	VI2C_register_read(I2C_SLAVE_ANA, I2C_MODE_STANDARD, VA9900_I2C_SLAVE_ADDR, 0x00, reg_buff);
	VI2C_register_write(I2C_SLAVE_ANA, I2C_MODE_STANDARD, VA9900_I2C_SLAVE_ADDR, 0, 0xFF);
    VI2C_register_read_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, VA9900_I2C_SLAVE_ADDR, 0x00, &reg_value);
    VI2C_Transfer_Done_irq();
	


    VI2C_register_read(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, reg_buff);
	VI2C_Continue_read(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, reg_buff);


    while(1){  
        VI2C_register_write_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, 0x56);
        VI2C_register_read_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, &reg_value);//56
        VI2C_Transfer_Done_irq();
        VI2C_register_set_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, 0x80);
        VI2C_register_read_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, &reg_value);//D6
        VI2C_Transfer_Done_irq();
        VI2C_register_clr_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, 0x04);
        VI2C_register_read_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, &reg_value);//D2
        VI2C_Transfer_Done_irq();
        VI2C_Continue_clr_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, 0x12);
        VI2C_register_read_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, &reg_value);//C0
        VI2C_Transfer_Done_irq();
        VI2C_Continue_set_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, 0x21);
        VI2C_register_read_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, &reg_value);//E1
        VI2C_Transfer_Done_irq();
        VI2C_register_write_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, 0); 
        VI2C_register_read_irq(I2C_SLAVE_ANA, I2C_MODE_STANDARD, PMU_I2C_SLAVE_ADDR, 0xF0, &reg_value);//00
        VI2C_Transfer_Done_irq();

        // i2c_irq_test();

        // i2c_register_read_write_time_test();
        // i2c_i2c0_Continue_test();
        //i2c_i2c0_Transmit_Receive_test();  
        //i2c_gyroscope_test();
    }
#endif
}


void i2c_gyroscope_test()
{
    #if 1
    /*******陀螺仪SC7A20******************/

    /***使用驱动前请根据实际接线情况配置（7bit）IIC地址******/
    /**SC7A20的SDO 脚接地：            0x18****************/
    /**SC7A20的SDO 脚接电源：           0x19****************/
    #define SC7A20_ADDR          (0x19U)
    /*******************************************************/
    /**reg map***************/
    #define SC7A20_OUT_TEMP_L       0x0C
    #define SC7A20_OUT_TEMP_H       0x0D
    #define SC7A20_WHO_AM_I         0x0F
    #define SC7A20_USER_CAL_START   0x13
    #define SC7A20_USER_CAL_END     0x1A
    #define SC7A20_NVM_WR           0x1E
    #define SC7A20_TEMP_CFG         0x1F
    #define SC7A20_CTRL_REG1        0x20
    #define SC7A20_CTRL_REG2        0x21
    #define SC7A20_CTRL_REG3        0x22
    #define SC7A20_CTRL_REG4        0x23
    #define SC7A20_CTRL_REG5        0x24
    #define SC7A20_CTRL_REG6        0x25
    #define SC7A20_REFERENCE        0x26
    #define SC7A20_STATUS_REG       0x27
    #define SC7A20_OUT_X_L          0x28
    #define SC7A20_OUT_X_H          0x29
    #define SC7A20_OUT_Y_L          0x2A
    #define SC7A20_OUT_Y_H          0x2B
    #define SC7A20_OUT_Z_L          0x2C
    #define SC7A20_OUT_Z_H          0x2D
    #define SC7A20_FIFO_CTRL_REG    0x2E
    #define SC7A20_SRC_REG          0x2F
    #define SC7A20_INT1_CFG         0x30
    #define SC7A20_INT1_SOURCE      0x31
    #define SC7A20_INT1_THS         0x32
    #define SC7A20_INT1_DURATION    0x33
    #define SC7A20_INT2_CFG         0x34
    #define SC7A20_INT2_SOURCE      0x35
    #define SC7A20_INT2_THS         0x36
    #define SC7A20_INT2_DURATION    0x37
    #define SC7A20_CLICK_CFG        0x38
    #define SC7A20_CLICK_SRC        0x39
    #define SC7A20_CLICK_THS        0x3A
    #define SC7A20_TIME_LIMIT       0x3B
    #define SC7A20_TIME_LATENCY     0x3C
    #define SC7A20_TIME_WINDOW      0x3D
    #define SC7A20_ACT_THS          0x3E
    #define SC7A20_ACT_DURATION     0x3F
    #endif
    int i  = 0 , j = 0, k = 0, loop_num = 0, ErrNum = 0;
    UINT8 reg_value = 0;

    /*I2C_SCL GPIO[7] J4 15; I2C_SDA GPIO[8] J4 16*/
    VGPIO_SetMuxFunction ( GPIO_MUX_FUNC_I2C, GPIO_MUX_FUNC_I2C );
    VGPIO_SetMux2Function ( GPIO_MUX_FUNC_I2C, GPIO_MUX_FUNC_NONE );
    Dbg_PRINT("I2C TEST\r\n");

    for(i = 0; i<=0x57; i++){

        //SL_MEMS_i2cRead(SC7A20_ADDR, SC7A20_CTRL_REG1, 4, &reg_value);
        VI2C_register_read(I2C_SLAVE_PROM,I2C_MODE_STANDARD,SC7A20_ADDR,i,&reg_value);
        Dbg_PRINT("[%02x]:%02x\r\n ", i, reg_value);
        
    }
    Dbg_PRINT("SC7A20_reg_default finish!\r\n");

    /*I2C读写陀螺仪*/
    for(j = 0; j<=0xff; j++){
        VI2C_register_write(I2C_SLAVE_PROM,I2C_MODE_STANDARD,SC7A20_ADDR,SC7A20_TIME_LATENCY,j);
        VI2C_register_read(I2C_SLAVE_PROM,I2C_MODE_STANDARD,SC7A20_ADDR,SC7A20_TIME_LATENCY,&reg_value);
    
        //if(reg_value == j)  Dbg_PRINT("right:0x%x 0x%x ",  reg_value,j);
        if(reg_value != j){
        Dbg_PRINT("err:0x%x 0x%x ",  reg_value,j);
        ErrNum++;
        }  
    }

    loop_num++;
    Dbg_PRINT("SC7A20_reg_write_read[%d] finish, err=%d\r\n",loop_num,ErrNum);
}
