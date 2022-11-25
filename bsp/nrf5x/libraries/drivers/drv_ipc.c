/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date         Author        Notes
 * 2020-09-08   Chenyingchun  first version
 */

#include "board.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <sys/time.h>
#include <nrfx_ipc.h>
#include <nrfx_gpiote.h>
#include <string.h>
#include <stdio.h>
#define COMPREHANSIVE_IPC
#ifdef BSP_USING_IPC
#ifdef COMPREHANSIVE_IPC /*COMPREHANSIVE_IPC*/
 
#ifdef PM__SRAM_PRIMARY_END_ADDRESS
#define SHARE_RAM_END_ADDR PM__SRAM_PRIMARY_END_ADDRESS
#else
#define SHARE_RAM_END_ADDR 0x2007e000
#endif
#define IPC_DATA_MAX_SIZE 0x1000/2  ////4kB RAM should be enough
#define SHARE_RAM_BASE_ADDR (SHARE_RAM_END_ADDR - IPC_DATA_MAX_SIZE * 2 )

#define MAGIC_REQUEST 0x20220408
#define MAGIC_RESPONSE 0x20220616   
#define CH_NO_SEND 0
#define CH_NO_RECEIVE 1
#define IPC_DATA_HEADER_LEN 16
#define IPC_RESPONSE_TIMEOUT 100 //ms

#define NET2APP_TEST 1
#define NET2APP_BT_ADDR_SEND 2
#define NET2APP_BT_CONN_STATUS 3
#define NET2APP_BT_NUS_RECV 4
#define NET2APP_BT_SMP_SEND 5
#define NET2APP_BT_SEND_MTU 6

#define APP2NET_TEST 0x81
#define APP2NET_SMP_GET_MTU 0x82
#define APP2NET_SMP_SEND 0x83
#define APP2NET_NUS_SEND 0x84




typedef void (*ipc_rx_callback_t)(uint8_t *data, uint16_t len);

typedef struct
{
	uint32_t requst; 
	uint32_t response;      
    uint32_t len; 
    uint8_t * data;             
} nrfx_ipc_data_t;

nrfx_ipc_data_t * ipc_tx_buf = (nrfx_ipc_data_t *) SHARE_RAM_BASE_ADDR;
nrfx_ipc_data_t * ipc_rx_buf = (nrfx_ipc_data_t *) (SHARE_RAM_BASE_ADDR+IPC_DATA_MAX_SIZE);
static ipc_rx_callback_t ipc_rx_callback;
//K_SEM_DEFINE(sem_ipc_respone, 0, 1);
struct rt_semaphore sem_ipc_respone;


/* TODO: This should come from DTS, possibly an overlay. */
#define CPUNET_UARTE_PIN_TX 33
#define CPUNET_UARTE_PIN_RX 32
#define CPUNET_UARTE_PIN_RTS 11
#define CPUNET_UARTE_PIN_CTS 10

static void remoteproc_mgr_config(void)
{
#if !defined(CONFIG_TRUSTED_EXECUTION_NONSECURE) || defined(CONFIG_BUILD_WITH_TFM)
	/* UARTE */
	/* Assign specific GPIOs that will be used to get UARTE from
	 * nRF5340 Network MCU.
	 */
	//nrf_gpio_pin_mcu_select(CPUNET_UARTE_PIN_TX, NRF_GPIO_PIN_MCUSEL_NETWORK);
	//nrf_gpio_pin_mcu_select(CPUNET_UARTE_PIN_RX, NRF_GPIO_PIN_MCUSEL_NETWORK);
	//nrf_gpio_pin_mcu_select(CPUNET_UARTE_PIN_RTS, NRF_GPIO_PIN_MCUSEL_NETWORK);
	//nrf_gpio_pin_mcu_select(CPUNET_UARTE_PIN_CTS, NRF_GPIO_PIN_MCUSEL_NETWORK);

#endif /* !defined(CONFIG_TRUSTED_EXECUTION_NONSECURE) || defined(CONFIG_BUILD_WITH_TFM) */

#if !defined(CONFIG_TRUSTED_EXECUTION_NONSECURE)
	/* Retain nRF5340 Network MCU in Secure domain (bus
	 * accesses by Network MCU will have Secure attribute set).
	 */
	NRF_SPU->EXTDOMAIN[0].PERM = 1 << 4;
#endif /* !defined(CONFIG_TRUSTED_EXECUTION_NONSECURE) */
}

static int remoteproc_mgr_boot(void)
{
	/* Secure domain may configure permissions for the Network MCU. */
	remoteproc_mgr_config();

#if !defined(CONFIG_TRUSTED_EXECUTION_SECURE)
	/*
	 * Building Zephyr with CONFIG_TRUSTED_EXECUTION_SECURE=y implies
	 * building also a Non-Secure image. The Non-Secure image will, in
	 * this case do the remainder of actions to properly configure and
	 * boot the Network MCU.
	 */

	/* Release the Network MCU, 'Release force off signal' */
	NRF_RESET->NETWORK.FORCEOFF = RESET_NETWORK_FORCEOFF_FORCEOFF_Release;

	//rt_kprintf("Network MCU released.");
#endif /* !CONFIG_TRUSTED_EXECUTION_SECURE */

	return 0;
}

static void nrfx_ipc_handler(uint32_t event_mask, void *p_context)
{
	if (event_mask == (1 << CH_NO_RECEIVE)) {

		if (ipc_rx_buf->response == MAGIC_RESPONSE)
		{
			//LOG_DBG("ipc isr response %x", ipc_tx_buf->data[0]);
			ipc_rx_buf->response = 0;
			//k_sem_give(&sem_ipc_respone);
			rt_sem_release(&sem_ipc_respone);
		}
		else if (ipc_rx_buf->requst == MAGIC_REQUEST)
		{
			//LOG_DBG("ipc isr requst %x", ipc_rx_buf->data[0]);
			if (ipc_rx_callback)
			{
				ipc_rx_callback(ipc_rx_buf->data, ipc_rx_buf->len);
			}
			ipc_tx_buf->response = MAGIC_RESPONSE;
			nrfx_ipc_signal(CH_NO_SEND);				
		}
		else
		{
			//LOG_WRN("invalid ipc data");
		}
	}
}

int nrfx_ipc_send(const uint8_t *data, uint16_t size)
{
	int ret;
	//LOG_DBG("ipc send %x", data[0]);
	if (size > (IPC_DATA_MAX_SIZE - IPC_DATA_HEADER_LEN) )
	{
		return -1;
	}
	if (ipc_tx_buf->requst == MAGIC_REQUEST)
	{
		//LOG_ERR("ipc is busy");
		return -1;
	}
	ipc_tx_buf->requst = MAGIC_REQUEST;
	ipc_tx_buf->len = size;
	memcpy(ipc_tx_buf->data, data, size);
	nrfx_ipc_signal(CH_NO_SEND);
//	ret = k_sem_take(&sem_ipc_respone, K_MSEC(IPC_RESPONSE_TIMEOUT));
	 
	ret = rt_sem_take(&sem_ipc_respone, (IPC_RESPONSE_TIMEOUT * RT_TICK_PER_SECOND) / 1000 ); 

	if (ret != RT_EOK)
	{
		return RT_EBUSY;
	}
	ipc_tx_buf->requst = 0;	
	return 0;
}

int init_ipc(ipc_rx_callback_t cb)
{
	int ret;
	rt_sem_init(&sem_ipc_respone, "sem_ipc_respone",	   0,	   RT_IPC_FLAG_PRIO);

	ret = nrfx_ipc_init(0, nrfx_ipc_handler, NULL);
	if (ret != NRFX_SUCCESS)
	{
		//LOG_ERR("ipc init err %x", ret);
		return -1;
	}
	ipc_rx_callback = cb;
	//IRQ_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_IPC), 6,
	//	    nrfx_isr, nrfx_ipc_irq_handler, 0);
	ipc_tx_buf->data = (void *)((uint32_t) ipc_tx_buf + IPC_DATA_HEADER_LEN);
	ipc_rx_buf->data = (void *)((uint32_t) ipc_rx_buf + IPC_DATA_HEADER_LEN);
	ipc_tx_buf->requst = 0;
	ipc_tx_buf->response = 0;
	ipc_rx_buf->requst = 0;
	ipc_rx_buf->response = 0;		

	nrf_ipc_send_config_set(NRF_IPC, CH_NO_SEND, 1 << CH_NO_SEND);
	nrf_ipc_receive_config_set(NRF_IPC, CH_NO_RECEIVE, 1 << CH_NO_RECEIVE);
	nrf_ipc_int_enable(NRF_IPC, 1 << CH_NO_RECEIVE);

	return 0;	
}

int app2net_test(uint8_t *data, uint16_t len)
{
	uint8_t buf[250];

	if (len > 249) len = 249;
	buf[0] = APP2NET_TEST;
	memcpy(&buf[1], data, len);

	return nrfx_ipc_send(buf, len+1);	
}
static int send_to_net(void)
{	
	int ret;
	static uint8_t cnt;
	char test_str[20];

	snprintf(test_str, 16, "I am from APP %c", cnt++);
	ret = app2net_test(test_str, 15);
	if (ret)
	{
		rt_kprintf("ipc test error %d", ret);
	}
	else
	{
		rt_kprintf("ipc test done %x", cnt-1);
	}

	return ret;	
}
static void ipc_app_rx_cb(uint8_t *data, uint16_t len)
{
	//ipc_temp.len = len;
	//ipc_temp.data = data;
	//k_sem_give(&sem_ipc_rx);
}
int rt_hw_ipc_init(void)
{
    
    //rt_kprintf("ipc init success");
    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_ipc_init);
void ipc_sample(void)
{
	//rt_kprintf("dual core communication sample \n");
	
		remoteproc_mgr_boot();
				
		init_ipc(ipc_app_rx_cb);

	
		//rt_kprintf("ipc init done");
	
		while (1) { 			   
			//rt_kprintf("app core start to send");
			 rt_thread_mdelay(5000);
			send_to_net();		  
		}

}

MSH_CMD_EXPORT(ipc_sample, ipc sample);



#else /*COMPREHANSIVE_IPC*/
#include <nrfx_ipc.h>
#include <nrfx_gpiote.h>

#define SHARE_RAM_END_ADDR 0x2007e000
#define IPC_DATA_MAX_SIZE 0x1000/2  ////4kB RAM should be enough
#define SHARE_RAM_BASE_ADDR (SHARE_RAM_END_ADDR - IPC_DATA_MAX_SIZE * 2 )
#define MAGIC_VALID 0x20220408  
#define CH_NO_SEND 0
#define CH_NO_RECEIVE 1
#define IPC_DATA_HEADER_LEN 16

typedef struct
{
    uint32_t valid;
	uint32_t busy;     
    uint32_t len; 
    void * data;             
} nrfx_ipc_data_t;

nrfx_ipc_data_t * ipc_tx_buf = (nrfx_ipc_data_t *) SHARE_RAM_BASE_ADDR;
nrfx_ipc_data_t * ipc_rx_buf = (nrfx_ipc_data_t *) (SHARE_RAM_BASE_ADDR+IPC_DATA_MAX_SIZE);


/* TODO: This should come from DTS, possibly an overlay. */
#define CPUNET_UARTE_PIN_TX 33
#define CPUNET_UARTE_PIN_RX 32
#define CPUNET_UARTE_PIN_RTS 11
#define CPUNET_UARTE_PIN_CTS 10


static void remoteproc_mgr_config(void)
{
#if !defined(CONFIG_TRUSTED_EXECUTION_NONSECURE) || defined(CONFIG_BUILD_WITH_TFM)
	/* UARTE */
	/* Assign specific GPIOs that will be used to get UARTE from
	 * nRF5340 Network MCU.
	 */
	//nrf_gpio_pin_mcu_select(CPUNET_UARTE_PIN_TX, NRF_GPIO_PIN_MCUSEL_NETWORK);
	//nrf_gpio_pin_mcu_select(CPUNET_UARTE_PIN_RX, NRF_GPIO_PIN_MCUSEL_NETWORK);
	//nrf_gpio_pin_mcu_select(CPUNET_UARTE_PIN_RTS, NRF_GPIO_PIN_MCUSEL_NETWORK);
	//nrf_gpio_pin_mcu_select(CPUNET_UARTE_PIN_CTS, NRF_GPIO_PIN_MCUSEL_NETWORK);

#endif /* !defined(CONFIG_TRUSTED_EXECUTION_NONSECURE) || defined(CONFIG_BUILD_WITH_TFM) */

#if !defined(CONFIG_TRUSTED_EXECUTION_NONSECURE)
	/* Retain nRF5340 Network MCU in Secure domain (bus
	 * accesses by Network MCU will have Secure attribute set).
	 */
	NRF_SPU->EXTDOMAIN[0].PERM = 1 << 4;
#endif /* !defined(CONFIG_TRUSTED_EXECUTION_NONSECURE) */
}

static int remoteproc_mgr_boot(void)
{
	/* Secure domain may configure permissions for the Network MCU. */
	remoteproc_mgr_config();

#if !defined(CONFIG_TRUSTED_EXECUTION_SECURE)
	/*
	 * Building Zephyr with CONFIG_TRUSTED_EXECUTION_SECURE=y implies
	 * building also a Non-Secure image. The Non-Secure image will, in
	 * this case do the remainder of actions to properly configure and
	 * boot the Network MCU.
	 */

	/* Release the Network MCU, 'Release force off signal' */
	NRF_RESET->NETWORK.FORCEOFF = RESET_NETWORK_FORCEOFF_FORCEOFF_Release;

	//rt_kprintf("Network MCU released.");
#endif /* !CONFIG_TRUSTED_EXECUTION_SECURE */

	return 0;
}


static void nrfx_ipc_handler(uint32_t event_mask, void *p_context)
{
	//rt_kprintf("event_mask %d", event_mask);
	if (event_mask == (1 << CH_NO_RECEIVE)) {
		// we just print out the data
		if (ipc_rx_buf->valid != MAGIC_VALID)
		{
			//rt_kprintf("invalid ipc data %x", ipc_rx_buf->valid);			
		}
		else
		{
                        
			//LOG_HEXDUMP_INF(ipc_rx_buf->data, ipc_rx_buf->len, "Received: ");
			/* after processe is done, you must reset the buffer to prepare for next receive. 
			 otherwise, you cannot get the next receive */
                         //rt_kprintf("\n");
                        // for(int i=0 ;i<ipc_rx_buf->len;i++)
                          //  rt_kprintf("%c",*((char *)ipc_rx_buf->data+i));
                             //rt_kprintf("\n");
			ipc_rx_buf->valid = 0;
			ipc_rx_buf->busy = 0;
		}
	}
}

int nrfx_ipc_send(const void *data, int size)
{
	if (size > (IPC_DATA_MAX_SIZE - IPC_DATA_HEADER_LEN) )
	{
		return -1;
	}
	if (ipc_tx_buf->valid == MAGIC_VALID && ipc_tx_buf->busy == 1)
	{
		//rt_kprintf("ipc is busy");
		ipc_tx_buf->busy = 0; //temp use only
		return -1;
	}
	ipc_tx_buf->valid = MAGIC_VALID;
	ipc_tx_buf->busy = 1;
	ipc_tx_buf->len = size;
	memcpy(ipc_tx_buf->data, data, size);
	nrfx_ipc_signal(CH_NO_SEND);
	return 0;
}

void send_to_net(void)
{
	int ret;
	static uint8_t cnt;
	char test_str[20];

	test_str[0] = 0x81;
	snprintf(&test_str[1], 16, "I am from APP %c", cnt++);
	ret = nrfx_ipc_send(test_str, 16);
	if (ret)
	{
		//rt_kprintf("nrfx_ipc_send error %d", ret);
	}
}








int rt_hw_ipc_init(void)
{
    
    //rt_kprintf("ipc init success");
    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_ipc_init);
void ipc_sample(void)
{
	//rt_kprintf("dual core communication sample \n");
	
		remoteproc_mgr_boot();
				
		nrfx_ipc_init(0, nrfx_ipc_handler, NULL);
		//IRQ_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_IPC), 1,
		//		nrfx_isr, nrfx_ipc_irq_handler, 0);
		
		ipc_tx_buf->data = (void *)((uint32_t) ipc_tx_buf + IPC_DATA_HEADER_LEN);
		ipc_rx_buf->data = (void *)((uint32_t) ipc_rx_buf + IPC_DATA_HEADER_LEN);
		ipc_tx_buf->valid = 0;
		ipc_tx_buf->busy = 0;
		ipc_rx_buf->valid = 0;
		ipc_rx_buf->busy = 0;		
	
		nrf_ipc_send_config_set(NRF_IPC, CH_NO_SEND, 1 << CH_NO_SEND);
		nrf_ipc_receive_config_set(NRF_IPC, CH_NO_RECEIVE, 1 << CH_NO_RECEIVE);
		nrf_ipc_int_enable(NRF_IPC, 1 << CH_NO_RECEIVE);
	
		//rt_kprintf("ipc init done");
	
		while (1) { 			   
			//rt_kprintf("app core start to send");
			 rt_thread_mdelay(5000);
			send_to_net();		  
		}

}

MSH_CMD_EXPORT(ipc_sample, ipc sample);
#endif /*COMPREHANSIVE_IPC*/
#endif /* BSP_USING_IPC */

