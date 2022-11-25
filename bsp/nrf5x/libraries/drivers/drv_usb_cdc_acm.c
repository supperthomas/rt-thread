/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-26     supperthomas first version
 *
 */


#include <board.h>
#include "rtdevice.h"
#include "rtservice.h"

#ifdef RT_USING_USB
#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_string_desc.h"
#include "app_usbd_cdc_acm.h"
#include "app_usbd_serial_num.h"
#include "nrfx_usbd.h"
#include "nrfx_clock.h"
#include "nrfx_usbreg.h"

static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event);

#define CDC_ACM_COMM_INTERFACE  0
#define CDC_ACM_COMM_EPIN       NRF_DRV_USBD_EPIN2

#define CDC_ACM_DATA_INTERFACE  1
#define CDC_ACM_DATA_EPIN       NRF_DRV_USBD_EPIN1
#define CDC_ACM_DATA_EPOUT      NRF_DRV_USBD_EPOUT1


/**
 * @brief CDC_ACM class instance
 * */
APP_USBD_CDC_ACM_GLOBAL_DEF(m_app_cdc_acm,
                            cdc_acm_user_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_AT_V250
);

#define READ_SIZE 7

static char m_rx_buffer[READ_SIZE];
static char m_tx_buffer[NRF_DRV_USBD_EPSIZE];
static bool m_send_flag = 0;

/**
 * @brief User event handler @ref app_usbd_cdc_acm_user_ev_handler_t (headphones)
 * */
static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event)
{
    app_usbd_cdc_acm_t const * p_cdc_acm = app_usbd_cdc_acm_class_get(p_inst);

    switch (event)
    {
        case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
        {
           // bsp_board_led_on(LED_CDC_ACM_OPEN);

            /*Setup first transfer*/
            ret_code_t ret = app_usbd_cdc_acm_read(&m_app_cdc_acm,
                                                   m_rx_buffer,
                                                   READ_SIZE);
            UNUSED_VARIABLE(ret);
            break;
        }
        case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
           // bsp_board_led_off(LED_CDC_ACM_OPEN);
            break;
        case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
           // bsp_board_led_invert(LED_CDC_ACM_TX);
            break;
        case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
        {
            ret_code_t ret;
           // NRF_LOG_INFO("Bytes waiting: %d", app_usbd_cdc_acm_bytes_stored(p_cdc_acm));
            do
            {
                /*Get amount of data transfered*/
                size_t size = app_usbd_cdc_acm_rx_size(p_cdc_acm);
             //   NRF_LOG_INFO("RX: size: %lu char: %c", size, m_rx_buffer[0]);

                /* Fetch data until internal buffer is empty */
                ret = app_usbd_cdc_acm_read(&m_app_cdc_acm,
                                            m_rx_buffer,
                                            READ_SIZE);
            } while (ret == NRF_SUCCESS);
			  rt_kprintf("\njs:");
            for(int i=0;i<7;i++)
                rt_kprintf("%c",m_rx_buffer[i]);
            //bsp_board_led_invert(LED_CDC_ACM_RX);
            break;
        }
        default:
            break;
    }
}

static void usbreg_handler(nrfx_usbreg_evt_t event)
{
switch (event)
    {
 
        case NRFX_USBREG_EVT_DETECTED:
           // NRF_LOG_INFO("USB power detected");

            if (!nrfx_usbd_is_enabled())
            {
                app_usbd_enable();
            }
            break;
        case NRFX_USBREG_EVT_REMOVED:
           // NRF_LOG_INFO("USB power removed");
            app_usbd_stop();
            break;
        case NRFX_USBREG_EVT_READY:
           // NRF_LOG_INFO("USB ready");
            app_usbd_start();
            break;
        default:
            break;
    }
}

static void clock_irq_handler(nrfx_clock_evt_type_t evt)
{

}




static int USB_CDC_ACM_init(void)
{
#if 1
	int result = RT_EOK;

    static const nrfx_usbreg_config_t usbreg_config= {
     .handler = usbreg_handler
     };

    //   nrfx_clock_init(clock_irq_handler);
   

   //    nrfx_clock_enable();
        
    //    nrfx_clock_lfclk_start();
    //nrfx_clock_hfclk_start();
		while(!nrfx_clock_lfclk_is_running())
    	{
        /* Just waiting */
    	}
    	// while (!(nrfx_clock_hfclk_is_running() &&
      //      nrfx_clock_lfclk_is_running()))
    {
        /* Just waiting */
    }
		nrfx_usbreg_init(&usbreg_config);

    app_usbd_init(NULL);
   
    //NRF_LOG_INFO("USBD CDC ACM example started.");

    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&m_app_cdc_acm);
    app_usbd_class_append(class_cdc_acm);
#endif
	rt_kprintf("\nusb init success");
	return RT_EOK;

}
INIT_DEVICE_EXPORT(USB_CDC_ACM_init);





static int usb_cdc_sample(int argc, char *argv[])
	
{
	ret_code_t ret;

#if 0
      
	  static const nrfx_usbreg_config_t usbreg_config= {
     .handler = usbreg_handler
     };

       nrfx_clock_init(clock_irq_handler);
   

       nrfx_clock_enable();
        
        nrfx_clock_lfclk_start();
		while(!nrfx_clock_lfclk_is_running())
    	{
        /* Just waiting */
    	}
		nrfx_usbreg_init(&usbreg_config);

    app_usbd_init(NULL);
   
    //NRF_LOG_INFO("USBD CDC ACM example started.");

    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&m_app_cdc_acm);
    app_usbd_class_append(class_cdc_acm);
#endif

		nrfx_usbreg_enable();
			//APP_ERROR_CHECK(ret);
		//app_usbd_enable();
			//  app_usbd_start();

		 while (true)
		{
			while (app_usbd_event_queue_process())
			{
				/* Nothing to do */
			}
			
			if(1)
			{
				static int	frame_counter;
	
				size_t size = sprintf(m_tx_buffer, "Hello USB CDC FA demo: %u\r\n", frame_counter);
	
				ret = app_usbd_cdc_acm_write(&m_app_cdc_acm, m_tx_buffer, size);
				if (ret == NRF_SUCCESS)
				{
					++frame_counter;
				}
			}

			//__WFE();
		}


}
MSH_CMD_EXPORT(usb_cdc_sample, usb sample);

#endif
