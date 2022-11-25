/*
 * COPYRIGHT (C) 2011-2021, Real-Thread Information Technology Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-09-12     heyuanjie87  first version.
 * 2014-03-04     Bernard      code cleanup
 */
#include <stdint.h>

#include <rtthread.h>
#include <nrfx_i2s.h>

#define RT_DEVICE_CTRL_I2S_TX_BUF_SET    (1) /* get timeout(in seconds) */
#define RT_DEVICE_CTRL_I2S_RX_BUF_SET    (2) /* set timeout(in seconds) */
#define RT_DEVICE_CTRL_I2S_TX_NEXT_BUF_SET   (3) /* get the left time before reboot(in seconds) */
#define RT_DEVICE_CTRL_I2S_RX_NEXT_BUF_SET      (4) /* refresh watchdog */
#define RT_DEVICE_CTRL_I2S_START          (5) /* start watchdog */
#define RT_DEVICE_CTRL_I2S_STOP           (6) /* stop watchdog */

struct rt_device i2s_dev;

//static rt_uint32_t       * p_rx_buffer;
//static rt_uint32_t       * p_tx_buffer;
//static rt_uint32_t       * p_next_rx_buffer;
//static rt_uint32_t       * p_next_tx_buffer;
#define LED_OK      0
#define LED_ERROR   0

#define I2S_DATA_BLOCK_WORDS    512
static uint32_t m_buffer_rx[2][I2S_DATA_BLOCK_WORDS];
static uint32_t m_buffer_tx[2][I2S_DATA_BLOCK_WORDS];

// Delay time between consecutive I2S transfers performed in the main loop
// (in milliseconds).
#define PAUSE_TIME          500
// Number of blocks of data to be contained in each transfer.
#define BLOCKS_TO_TRANSFER  20

static uint8_t volatile m_blocks_transferred     = 0;
static uint8_t          m_zero_samples_to_ignore = 0;
static uint16_t         m_sample_value_to_send;
static uint16_t         m_sample_value_expected;
static bool             m_error_encountered;


static rt_uint32_t       * volatile mp_block_to_fill  = NULL;
static rt_uint32_t const * volatile mp_block_to_check = NULL;
static void prepare_tx_data(uint32_t * p_block)
{
    // These variables will be both zero only at the very beginning of each
    // transfer, so we use them as the indication that the re-initialization
    // should be performed.
    if (m_blocks_transferred == 0 && m_zero_samples_to_ignore == 0)
    {
        // Number of initial samples (actually pairs of L/R samples) with zero
        // values that should be ignored - see the comment in 'check_samples'.
        m_zero_samples_to_ignore = 2;
        m_sample_value_to_send   = 0xCAFE;
        m_sample_value_expected  = 0xCAFE;
        m_error_encountered      = false;
    }

    // [each data word contains two 16-bit samples]
    uint16_t i;
    for (i = 0; i < I2S_DATA_BLOCK_WORDS; ++i)
    {
        uint16_t sample_l = m_sample_value_to_send - 1;
        uint16_t sample_r = m_sample_value_to_send + 1;
        ++m_sample_value_to_send;

        uint32_t * p_word = &p_block[i];
        ((uint16_t *)p_word)[0] = sample_l;
        ((uint16_t *)p_word)[1] = sample_r;
    }
}


static bool check_samples(uint32_t const * p_block)
{
    // [each data word contains two 16-bit samples]
    uint16_t i;
    for (i = 0; i < I2S_DATA_BLOCK_WORDS; ++i)
    {
        uint32_t const * p_word = &p_block[i];
        uint16_t actual_sample_l = ((uint16_t const *)p_word)[0];
        uint16_t actual_sample_r = ((uint16_t const *)p_word)[1];

        // Normally a couple of initial samples sent by the I2S peripheral
        // will have zero values, because it starts to output the clock
        // before the actual data is fetched by EasyDMA. As we are dealing
        // with streaming the initial zero samples can be simply ignored.
        if (m_zero_samples_to_ignore > 0 &&
            actual_sample_l == 0 &&
            actual_sample_r == 0)
        {
            --m_zero_samples_to_ignore;
        }
        else
        {
            m_zero_samples_to_ignore = 0;

            uint16_t expected_sample_l = m_sample_value_expected - 1;
            uint16_t expected_sample_r = m_sample_value_expected + 1;
            ++m_sample_value_expected;

            if (actual_sample_l != expected_sample_l ||
                actual_sample_r != expected_sample_r)
            {
                rt_kprintf("%3u: %04x/%04x, expected: %04x/%04x (i: %u)",
                    m_blocks_transferred, actual_sample_l, actual_sample_r,
                    expected_sample_l, expected_sample_r, i);
                return false;
            }
        }
    }
    rt_kprintf("%3u: OK", m_blocks_transferred);
    return true;
}


static void check_rx_data(uint32_t const * p_block)
{
    ++m_blocks_transferred;

    if (!m_error_encountered)
    {
        m_error_encountered = !check_samples(p_block);
    }

    if (m_error_encountered)
    {
       // bsp_board_led_off(LED_OK);
        //bsp_board_led_invert(LED_ERROR);
    }
    else
    {
       // bsp_board_led_off(LED_ERROR);
       // bsp_board_led_invert(LED_OK);
    }
}

static void data_handler(nrfx_i2s_buffers_t const * p_released,
                         uint32_t                      status)
{
    // 'nrf_drv_i2s_next_buffers_set' is called directly from the handler
    // each time next buffers are requested, so data corruption is not
    // expected.
    //ASSERT(p_released);

    // When the handler is called after the transfer has been stopped
    // (no next buffers are needed, only the used buffers are to be
    // released), there is nothing to do.
    if (!(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED))
    {
        return;
    }

    // First call of this handler occurs right after the transfer is started.
    // No data has been transferred yet at this point, so there is nothing to
    // check. Only the buffers for the next part of the transfer should be
    // provided.
    if (!p_released->p_rx_buffer)
    {
        nrfx_i2s_buffers_t const next_buffers = {
            .p_rx_buffer = m_buffer_rx[1],
            .p_tx_buffer = m_buffer_tx[1],
        };
        nrfx_i2s_next_buffers_set(&next_buffers);

        mp_block_to_fill = m_buffer_tx[1];
    }
    else
    {
        mp_block_to_check = p_released->p_rx_buffer;
        // The driver has just finished accessing the buffers pointed by
        // 'p_released'. They can be used for the next part of the transfer
        // that will be scheduled now.
      nrfx_i2s_next_buffers_set(p_released);

        // The pointer needs to be typecasted here, so that it is possible to
        // modify the content it is pointing to (it is marked in the structure
        // as pointing to constant data because the driver is not supposed to
        // modify the provided data).
        mp_block_to_fill = (uint32_t *)p_released->p_tx_buffer;
    }
}



/*
 * This function initializes watchdog
 */
static rt_err_t rt_i2s_init(struct rt_device *dev)
{
      rt_kprintf("rt_i2s_init \n");

    return (RT_EOK);
}

static rt_err_t rt_i2s_open(struct rt_device *dev, rt_uint16_t oflag)
{
	rt_kprintf("rt_i2s_open \n");

    return (RT_EOK);
}

static rt_err_t rt_i2s_close(struct rt_device *dev)
{
    rt_kprintf("rt_i2s_close \n");

    return (RT_EOK);
}
nrfx_i2s_buffers_t	initial_buffers ;

static rt_err_t rt_i2s_control(struct rt_device *dev,
                                    int              cmd,
                                    void             *args)
{
#if 0
	switch (cmd)
    {
    case RT_DEVICE_CTRL_I2S_TX_BUF_SET:
        p_tx_buffer=(rt_uint32_t*)args;
        break;
	case RT_DEVICE_CTRL_I2S_RX_BUF_SET:
        p_rx_buffer=(rt_uint32_t*)args;
        break;
	case RT_DEVICE_CTRL_I2S_TX_NEXT_BUF_SET:
        p_next_tx_buffer=(rt_uint32_t*)args;
        break;
	case RT_DEVICE_CTRL_I2S_RX_NEXT_BUF_SET:
        p_next_rx_buffer=(rt_uint32_t*)args;
        break;
    /* set watchdog timeout */
    case RT_DEVICE_CTRL_I2S_START:
       initial_buffers.p_rx_buffer=p_rx_buffer;
	   	initial_buffers.p_tx_buffer=p_tx_buffer;
        nrfx_i2s_start(&initial_buffers,*((rt_uint16_t*)args), 0);
        
        break;
	case RT_DEVICE_CTRL_I2S_STOP:
        nrfx_i2s_stop();
        break;

    default:
        return -RT_ERROR;
    }
#endif
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops i2s_ops =
{
    rt_i2s_init,
    rt_i2s_open,
    rt_i2s_close,
    RT_NULL,
    RT_NULL,
    rt_i2s_control,
};
#endif
rt_err_t rt_hw_i2s_register(struct rt_device *wtd,
                                 const char                *name,
                                 rt_uint32_t                flag,
                                 void                      *data); 

int rt_i2s_drv_init(void)
{
    uint32_t err_code;
	nrfx_i2s_config_t config = NRFX_I2S_DEFAULT_CONFIG(NRFX_I2S_CONFIG_SCK_PIN,NRFX_I2S_CONFIG_LRCK_PIN,NRFX_I2S_CONFIG_MCK_PIN,NRFX_I2S_CONFIG_SDOUT_PIN,NRFX_I2S_CONFIG_SDIN_PIN);
    // In Master mode the MCK frequency and the MCK/LRCK ratio should be
    // set properly in order to achieve desired audio sample rate (which
    // is equivalent to the LRCK frequency).
    // For the following settings we'll get the LRCK frequency equal to
    // 15873 Hz (the closest one to 16 kHz that is possible to achieve).
    //config.sdin_pin  = I2S_SDIN_PIN;
    //config.sdout_pin = I2S_SDOUT_PIN;
    config.mck_setup = NRF_I2S_MCK_32MDIV21;
    config.ratio     = NRF_I2S_RATIO_96X;
    config.channels  = NRF_I2S_CHANNELS_STEREO;
    err_code = nrfx_i2s_init(&config, data_handler);
	if(err_code!=NRFX_SUCCESS)
		{
			 rt_kprintf("nrfx_i2s_init fail\n");
			 return -RT_ERROR;
		}
    /* register watchdog device */
    //if (rt_hw_i2s_register(&i2s_dev, "i2s", RT_DEVICE_FLAG_DEACTIVATE, RT_NULL) != RT_EOK)
   // {
   //     return -RT_ERROR;
   // }
    return RT_EOK;
}
INIT_BOARD_EXPORT(rt_i2s_drv_init);

static int i2s_sample(int argc, char *argv[])
{
	   uint32_t err_code;
    
        m_blocks_transferred = 0;
        mp_block_to_fill  = NULL;
        mp_block_to_check = NULL;

        prepare_tx_data(m_buffer_tx[0]);

        nrfx_i2s_buffers_t const initial_buffers = {
            .p_tx_buffer = m_buffer_tx[0],
            .p_rx_buffer = m_buffer_rx[0],
        };
        err_code = nrfx_i2s_start(&initial_buffers, I2S_DATA_BLOCK_WORDS, 0);
        if(err_code!=NRFX_SUCCESS)
		{
			 rt_kprintf("nrfx_i2s_start fail\n");
			 return -RT_ERROR;
		}
        do {
            // Wait for an event.
            __WFE();
            // Clear the event register.
            __SEV();
            __WFE();

            if (mp_block_to_fill)
            {
                prepare_tx_data(mp_block_to_fill);
                mp_block_to_fill = NULL;
            }
            if (mp_block_to_check)
            {
                check_rx_data(mp_block_to_check);
                mp_block_to_check = NULL;
            }
        } while (m_blocks_transferred < BLOCKS_TO_TRANSFER);

        nrfx_i2s_stop();
    
	
}

MSH_CMD_EXPORT(i2s_sample, i2s sample);

/**
 * This function register a watchdog device
 */
rt_err_t rt_hw_i2s_register(struct rt_device *wtd,
                                 const char                *name,
                                 rt_uint32_t                flag,
                                 void                      *data)
{
    struct rt_device *device;
    RT_ASSERT(wtd != RT_NULL);

    //device = &(wtd->parent);

    device->type        = RT_Device_Class_Security;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    device->ops         = &i2s_ops;
#else
    device->init        = rt_i2s_init;
    device->open        = rt_i2s_open;
    device->close       = rt_i2s_close;
    device->read        = RT_NULL;
    device->write       = RT_NULL;
    device->control     = rt_i2s_control;
#endif
    device->user_data   = data;

    /* register a character device */
    return rt_device_register(device, name, flag);
}

                                 	
                                 	

