/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-15     xckhmf       First Verison
 * 2021-11-27     chenyingchun fix _master_xfer bug
 *
 */

#include <rtdevice.h>
#include <nrfx_twi_twim.h>
#include <nrfx_twim.h>
#include <drv_i2c.h>
//#include "eeprom_simulator.h"

#if defined(BSP_USING_I2C0) || defined(BSP_USING_I2C1) || defined(BSP_USING_I2C2) || defined(BSP_USING_I2C3)
typedef struct
{
    nrf_twim_frequency_t freq;
    uint32_t scl_pin;
    uint32_t sda_pin;
    nrfx_twim_t twi_instance;
}drv_i2c_cfg_t;

#ifdef BSP_USING_I2C0
static drv_i2c_cfg_t drv_i2c_0 =
{
    .freq = NRF_TWIM_FREQ_400K,
    .scl_pin = BSP_I2C0_SCL_PIN,
    .sda_pin = BSP_I2C0_SDA_PIN,
    .twi_instance = NRFX_TWIM_INSTANCE(0)
};
static struct rt_i2c_bus_device i2c0_bus;
#endif
#ifdef BSP_USING_I2C1
static drv_i2c_cfg_t drv_i2c_1 =
{
    .freq = NRF_TWIM_FREQ_400K,
    .scl_pin = BSP_I2C1_SCL_PIN,
    .sda_pin = BSP_I2C1_SDA_PIN,
    .twi_instance = NRFX_TWIM_INSTANCE(1)
};
static struct rt_i2c_bus_device i2c1_bus;
#endif
#ifdef BSP_USING_I2C2
static drv_i2c_cfg_t drv_i2c_2 =
{
    .freq = NRF_TWIM_FREQ_400K,
    .scl_pin = BSP_I2C2_SCL_PIN,
    .sda_pin = BSP_I2C2_SDA_PIN,
    .twi_instance = NRFX_TWIM_INSTANCE(2)
};
static struct rt_i2c_bus_device i2c2_bus;
#endif
#ifdef BSP_USING_I2C3
static drv_i2c_cfg_t drv_i2c_3 =
{
    .freq = NRF_TWIM_FREQ_400K,
    .scl_pin = BSP_I2C3_SCL_PIN,
    .sda_pin = BSP_I2C3_SDA_PIN,
    .twi_instance = NRFX_TWIM_INSTANCE(3)
};
static struct rt_i2c_bus_device i2c3_bus;
#endif


static int twi_master_init(struct rt_i2c_bus_device *bus)
{
    nrfx_err_t rtn;
    nrfx_twim_config_t config = NRFX_TWIM_DEFAULT_CONFIG(0,0);
    drv_i2c_cfg_t *p_cfg = bus->priv;
    nrfx_twim_t const * p_instance = &p_cfg->twi_instance;

    config.frequency = p_cfg->freq;
    config.scl = p_cfg->scl_pin;
    config.sda = p_cfg->sda_pin;

    nrfx_twi_twim_bus_recover(config.scl,config.sda);

    rtn = nrfx_twim_init(p_instance,&config,NULL,NULL);
    if (rtn != NRFX_SUCCESS)
    {
        return rtn;
    }
    nrfx_twim_enable(p_instance);
    return 0;
}

static rt_size_t _master_xfer(struct rt_i2c_bus_device *bus,
                              struct rt_i2c_msg msgs[],
                              rt_uint32_t num)
{
    struct rt_i2c_msg *msg;
    nrfx_twim_t const *p_instance = &((drv_i2c_cfg_t *)bus->priv)->twi_instance;
    nrfx_err_t ret = NRFX_ERROR_INTERNAL;
    uint32_t no_stop_flag = 0;
    rt_int32_t i = 0;

    for (i = 0; i < num; i++)
    {
        msg = &msgs[i];
        nrfx_twim_xfer_desc_t xfer = NRFX_TWIM_XFER_DESC_TX(msg->addr, msg->buf, msg->len);

        if (msg->flags & RT_I2C_RD)
        {
            xfer.type = NRFX_TWIM_XFER_RX;
        }
        else
        {
            xfer.type = NRFX_TWIM_XFER_TX;
            if (msg->flags & RT_I2C_NO_READ_ACK)
            {
                no_stop_flag = NRFX_TWIM_FLAG_TX_NO_STOP;
            }
        }

        ret = nrfx_twim_xfer(p_instance, &xfer, no_stop_flag);
        if (ret != NRFX_SUCCESS)
        {
            goto out;
        }
    }

out:
    return i;
}


static const struct rt_i2c_bus_device_ops _i2c_ops =
{
     _master_xfer,
     NULL,
     NULL,
};

int rt_hw_i2c_init(void)
{
#ifdef BSP_USING_I2C0
    i2c0_bus.ops= &_i2c_ops;
    i2c0_bus.timeout = 0;
    i2c0_bus.priv = (void *)&drv_i2c_0;
    twi_master_init(&i2c0_bus);
    rt_i2c_bus_device_register(&i2c0_bus, "i2c0");
#endif
#ifdef BSP_USING_I2C1
    i2c1_bus.ops= &_i2c_ops;
    i2c1_bus.timeout = 0;
    i2c1_bus.priv = (void *)&drv_i2c_1;
    twi_master_init(&i2c1_bus);
    rt_i2c_bus_device_register(&i2c1_bus, "i2c1");
#endif
#ifdef BSP_USING_I2C2
		i2c2_bus.ops= &_i2c_ops;
		i2c2_bus.timeout = 0;
		i2c2_bus.priv = (void *)&drv_i2c_2;
		twi_master_init(&i2c2_bus);
		rt_i2c_bus_device_register(&i2c2_bus, "i2c2");
#endif
#ifdef BSP_USING_I2C3
		i2c3_bus.ops= &_i2c_ops;
		i2c3_bus.timeout = 0;
		i2c3_bus.priv = (void *)&drv_i2c_3;
		twi_master_init(&i2c3_bus);
		rt_i2c_bus_device_register(&i2c3_bus, "i2c3");
#endif


    return 0;
}

INIT_BOARD_EXPORT(rt_hw_i2c_init);
#define AHT10_I2C_BUS_NAME      "i2c3"  /* 传感器连接的I2C总线设备名称 */
#define AHT10_ADDR               0x50   /* 从机地址 */
struct rt_i2c_bus_device *i2c_bus3;      /* I2C总线设备句柄 */




static int twi_sample(int argc, char *argv[])
	
{
#if 0
	ret_code_t ret;
	rt_uint8_t buf[1];

		ret = eeprom_simulator_init();
	  buf[0]=10;
	/* 查找I2C总线设备，获取I2C总线设备句柄 */
i2c_bus3 = (struct rt_i2c_bus_device *)rt_device_find(AHT10_I2C_BUS_NAME);

struct rt_i2c_msg msgs;

   msgs.addr = AHT10_ADDR;
   msgs.flags = RT_I2C_WR;
   msgs.buf = buf;
   msgs.len = 1;

   /* 调用I2C设备接口传输数据 */
   if (rt_i2c_transfer(i2c_bus3, &msgs, 1) == 1)
   {
	  // return RT_EOK;
   }
   else
   {
	  // return -RT_ERROR;
   }

   msgs.addr = AHT10_ADDR;
	msgs.flags = RT_I2C_RD;
	msgs.buf = buf;
	msgs.len = 1;
 while(1)  {
	/* 调用I2C设备接口传输数据 */
	if (rt_i2c_transfer(i2c_bus3, &msgs, 1) == 1)
	{
		rt_kprintf("2");
	}
	rt_thread_mdelay(500);

 	}
#endif
}
MSH_CMD_EXPORT(twi_sample, twi sample);

#endif /* defined(BSP_USING_I2C0) || defined(BSP_USING_I2C1) */
