/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-29     supperthomas first version
 * 2021-06-26     supperthomas fix led
 *
 */

#include <rtthread.h>
#include <rtdevice.h>
//#include "nrfx_usbreg.h"
//#include "usbd_config.h"
//#include "app_usbd.h"



#if 1
	#define DK_BOARD_LED_1  28
#define DK_BOARD_LED_2  29
#define DK_BOARD_LED_3  30
#define DK_BOARD_LED_4  31

#define DK_BOARD_BUTTON_1  23
#define DK_BOARD_BUTTON_2  24
#define DK_BOARD_BUTTON_3  8
#define DK_BOARD_BUTTON_4  9

#define HWTIMER_DEV_NAME   "timer2"     /* 定时器名称 */
/* 消息队列控制块 */
// struct rt_messagequeue rx_mq;

void button_1_callback(void *args)
{
    static int flag1 = 0;

	//  rt_pm_request(0);
	//		   rt_pm_release(3);
			   
    if(flag1 == 0)
    {
        flag1 = 1;
        rt_pin_write(DK_BOARD_LED_1, PIN_LOW);
    }
    else
    {
        flag1 = 0;
        rt_pin_write(DK_BOARD_LED_1, PIN_HIGH);
    }
}
static int fla=0;
/* 定时器超时回调函数 */
static rt_err_t timeout_cb(rt_device_t dev, rt_size_t size)
{
    //rt_kprintf("er!\n");
   // rt_kprintf("tick is :%d !\n", rt_tick_get());
   if(fla==0)
	{rt_pin_write(DK_BOARD_LED_1, PIN_LOW);fla=1;}
   else
	{rt_pin_write(DK_BOARD_LED_1, PIN_HIGH);fla=0;}
    return 0;
}
#define AHT11_I2C_BUS_NAME      "i2c1"  /* 传感器连接的I2C总线设备名称 */
#define AHT11_ADDR               0x50   /* 从机地址 */
struct rt_i2c_bus_device *i2c_bus1;      /* I2C总线设备句柄 */
#define SAMPLE_UART_NAME       "uart2"    /* 串口设备名称 */
#define SAMPLE_UART_NAME1       "uart3"    /* 串口设备名称 */
#if 0
/* 串口接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};


static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq, &msg, sizeof(msg));
    if ( result == -RT_EFULL)
    {
        /* 消息队列满 */
        rt_kprintf("message queue full！\n");
    }
    return result;
}
#endif
extern int rtthread_startup(void);
int app(void)
{
    
#if 1
	int count = 1;
	//struct rx_msg msg;
	  static char msg_pool[256];
		rt_err_t result;
		rt_uint32_t rx_length;
		static char rx_buffer[RT_SERIAL_RB_BUFSZ + 1];

    rt_pin_mode(DK_BOARD_LED_1, PIN_MODE_OUTPUT);
	rt_pin_mode(DK_BOARD_LED_2, PIN_MODE_OUTPUT);
	rt_pin_mode(DK_BOARD_LED_3, PIN_MODE_OUTPUT);
	rt_pin_mode(DK_BOARD_LED_4, PIN_MODE_OUTPUT);
	rt_pin_attach_irq(DK_BOARD_BUTTON_1, PIN_IRQ_MODE_RISING_FALLING,
									   button_1_callback, (void*) 1); //true: hi_accuracy(IN_EVENT),false: lo_accuracy(PORT_EVENT)
	   rt_pin_irq_enable(DK_BOARD_BUTTON_1, PIN_IRQ_ENABLE);
//	   rt_pm_request(3);
//			   rt_pm_release(0);
			 

	static rt_device_t serial,serial1;				  /* 串口设备句柄 */
	char str[] = "hello RT-Thread!\r\n";
	struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; /* 配置参数 */
	config.bufsz =0;
#if 0
	/* 查找串口设备 */
	serial = rt_device_find(SAMPLE_UART_NAME);
	serial1 = rt_device_find(SAMPLE_UART_NAME1);
	
	/* 以中断接收及轮询发送模式打开串口设备 */
	//rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
	/* 以中断接收及轮询发送模式打开串口设备 */
	//rt_device_open(serial1, RT_DEVICE_FLAG_INT_RX);
	/* 初始化消息队列 */
	   rt_mq_init(&rx_mq, "rx_mq",
				  msg_pool, 				/* 存放消息的缓冲区 */
				  sizeof(struct rx_msg),	/* 一条消息的最大长度 */
				  sizeof(msg_pool), 		/* 存放消息的缓冲区大小 */
				  RT_IPC_FLAG_FIFO);		/* 如果有多个线程等待，按照先来先得到的方法分配消息 */
	rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
	   /* 以 DMA 接收及轮询发送方式打开串口设备 */
	   rt_device_open(serial, RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX);
	   /* 设置接收回调函数 */
	   rt_device_set_rx_indicate(serial, uart_input);
#endif

    while (count++)
    {
   // rt_device_write(serial1, 0, str, 18);
      // rt_pin_write(DK_BOARD_LED_1, PIN_HIGH);
    rt_pin_write(DK_BOARD_LED_2, PIN_HIGH);
    rt_pin_write(DK_BOARD_LED_3, PIN_HIGH);
    rt_pin_write(DK_BOARD_LED_4, PIN_HIGH);
        rt_thread_mdelay(500);
	


      //  rt_pin_write(DK_BOARD_LED_1, PIN_LOW);
    rt_pin_write(DK_BOARD_LED_2, PIN_LOW);
    rt_pin_write(DK_BOARD_LED_3, PIN_LOW);
    rt_pin_write(DK_BOARD_LED_4, PIN_LOW);
        rt_thread_mdelay(500);
	#if 0
			rt_uint8_t buf[1];
		

			  buf[0]=0;
			/* 查找I2C总线设备，获取I2C总线设备句柄 */
		i2c_bus1 = (struct rt_i2c_bus_device *)rt_device_find(AHT11_I2C_BUS_NAME);
		
		struct rt_i2c_msg msgs;
		
		   msgs.addr = AHT11_ADDR;
		   msgs.flags = RT_I2C_WR;
		   msgs.buf = buf;
		   msgs.len = 1;
		
		   /* 调用I2C设备接口传输数据 */
		 //rt_i2c_transfer(i2c_bus1, &msgs, 1);
	//	   if (rt_i2c_transfer(i2c_bus1, &msgs, 1) == 1)
			   {
	//			   rt_kprintf("1");
			   }
#endif 
#if 0

	     rt_memset(&msg, 0, sizeof(msg));
		  /* 从消息队列中读取消息*/
		  result = rt_mq_recv(&rx_mq, &msg, sizeof(msg), RT_WAITING_FOREVER);
		  if (result == RT_EOK)
		  {
			  /* 从串口读取数据*/
			  rx_length = rt_device_read(msg.dev, 0, rx_buffer, msg.size);
			  rx_buffer[rx_length] = '\0';
			  /* 通过串口设备 serial 输出读取到的消息 */
			  rt_device_write(serial, 0, rx_buffer, rx_length+1);
			  /* 打印数据 */
			  rt_kprintf("%s\n",rx_buffer);
		  }
		  rt_device_write(serial, 0, str, 18);
#endif
	/* 发送字符串 */
	
	/* 发送字符串 */
		

		
    }
#endif 
#if 0
nrfx_usbreg_enable();
	//APP_ERROR_CHECK(ret);
//app_usbd_enable();
	//	app_usbd_start();

 while (true)
{
	while (app_usbd_event_queue_process())
	{
		/* Nothing to do */
	}
	//rt_thread_mdelay(1);


	//__WFE();
}
#endif
#if 0
	int count = 1;
	rt_pin_mode(DK_BOARD_LED_1, PIN_MODE_OUTPUT);

	rt_pin_mode(DK_BOARD_LED_2, PIN_MODE_OUTPUT);
	rt_pin_mode(DK_BOARD_LED_3, PIN_MODE_OUTPUT);
	rt_pin_mode(DK_BOARD_LED_4, PIN_MODE_OUTPUT);
		rt_pin_write(DK_BOARD_LED_1, PIN_HIGH);
	   rt_pin_write(DK_BOARD_LED_2, PIN_HIGH);
	   rt_pin_write(DK_BOARD_LED_3, PIN_HIGH);
	   rt_pin_write(DK_BOARD_LED_4, PIN_HIGH);

	rt_err_t ret = RT_EOK;
		rt_hwtimerval_t timeout_s;		/* 定时器超时值 */
		rt_device_t hw_dev = RT_NULL;	/* 定时器设备句柄 */
		rt_hwtimer_mode_t mode; 		/* 定时器模式 */
	 rt_kprintf("app begin \n");
		/* 查找定时器设备 */
		hw_dev = rt_device_find(HWTIMER_DEV_NAME);
		if (hw_dev == RT_NULL)
		{
			rt_kprintf("hwtimer sample run failed! can't find %s device!\n", HWTIMER_DEV_NAME);
			return RT_ERROR;
		}
	
		/* 以读写方式打开设备 */
		ret = rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
		if (ret != RT_EOK)
		{
			rt_kprintf("open %s device failed!\n", HWTIMER_DEV_NAME);
			return ret;
		}
        rt_kprintf("timer find open ok \n");
		
		/* 设置超时回调函数 */
			rt_device_set_rx_indicate(hw_dev, timeout_cb);
		
			/* 设置模式为周期性定时器 */
			mode = HWTIMER_MODE_PERIOD;
			ret = rt_device_control(hw_dev, HWTIMER_CTRL_MODE_SET, &mode);
			if (ret != RT_EOK)
			{
				rt_kprintf("set mode failed! ret is :%d\n", ret);
				return ret;
			}
		
			/* 设置定时器超时值为5s并启动定时器 */
			timeout_s.sec = 5;		/* 秒 */
			timeout_s.usec = 0; 	/* 微秒 */
		
			if (rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
			{
				rt_kprintf("set timeout value failed\n");
				return RT_ERROR;
			}
		
			/* 延时3500ms */
			rt_thread_mdelay(3500);
		
			/* 读取定时器当前值 */
			rt_device_read(hw_dev, 0, &timeout_s, sizeof(timeout_s));
			rt_kprintf("Read: Sec = %d, Usec = %d\n", timeout_s.sec, timeout_s.usec);
		 while (count++)
    {
      // rt_pin_write(DK_BOARD_LED_1, PIN_HIGH);
    rt_pin_write(DK_BOARD_LED_2, PIN_HIGH);
    rt_pin_write(DK_BOARD_LED_3, PIN_HIGH);
    rt_pin_write(DK_BOARD_LED_4, PIN_HIGH);
        rt_thread_mdelay(500);
	


      //  rt_pin_write(DK_BOARD_LED_1, PIN_LOW);
    rt_pin_write(DK_BOARD_LED_2, PIN_LOW);
    rt_pin_write(DK_BOARD_LED_3, PIN_LOW);
    rt_pin_write(DK_BOARD_LED_4, PIN_LOW);
        rt_thread_mdelay(500);
    }
#endif
#if 0
	rt_device_t i2s = rt_device_find("i2s");
	   if (i2s == RT_NULL)
			   {
				   rt_kprintf("run failed! can't find  device!\n");
				   return RT_ERROR;
			   }

	   rt_device_init(i2s);
	   
	   rt_device_control(i2s, RT_NULL, RT_NULL);
#endif
    return RT_EOK;
}

int main(void)
{
#ifndef __ARMCC_VERSION	
	rtthread_startup();
#endif	
    return RT_EOK;
}

#endif

