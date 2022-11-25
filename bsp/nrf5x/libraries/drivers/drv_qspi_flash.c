/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-04-29     supperthomas first version
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "nrfx_qspi.h"
#if defined(PKG_USING_FAL)
//#include <fal.h>

//log
#include <rtdbg.h>
#define LOG_TAG                "drv.qspiflash"

#define WAIT_FOR_PERIPH() do { \
        while (!m_finished) {} \
        m_finished = false;    \
    } while (0)

static volatile bool m_finished = false;




static void qspi_handler(nrfx_qspi_evt_t event, void *p_context)
{
    m_finished = true;
}

static void configure_memory()
{
#define QSPI_STD_CMD_WRSR   0x01
#define QSPI_STD_CMD_RSTEN  0x66
#define QSPI_STD_CMD_RST    0x99

    uint8_t temporary = 0x40;
    uint32_t err_code;
    nrf_qspi_cinstr_conf_t cinstr_cfg =
    {
        .opcode    = QSPI_STD_CMD_RSTEN,
        .length    = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = true,
        .io3_level = true,
        .wipwait   = true,
        .wren      = true
    };

    // Send reset enable
    err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    if (NRFX_SUCCESS != err_code)
    {
        LOG_E("\r\n ERROR: QSPI_STD_CMD_RSTEN:0x%x\n", err_code);
        return ;
    }
    // Send reset command
    cinstr_cfg.opcode = QSPI_STD_CMD_RST;
    err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    if (NRFX_SUCCESS != err_code)
    {
        LOG_E("\r\n ERROR: QSPI_STD_CMD_RST:0x%x\n", err_code);
        return ;
    }

    // Switch to qspi mode
    cinstr_cfg.opcode = QSPI_STD_CMD_WRSR;
    cinstr_cfg.length = NRF_QSPI_CINSTR_LEN_2B;
    err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, &temporary, NULL);
    if (NRFX_SUCCESS != err_code)
    {
        LOG_E("\r\n ERROR: QSPI_STD_CMD_WRSR:0x%x\n", err_code);
        return;
    }
}
static int init(void)
{
    uint32_t err_code;
    nrfx_qspi_config_t config = NRFX_QSPI_DEFAULT_CONFIG(17, 18,13,14,15,16);
                                                         

    err_code = nrfx_qspi_init(&config, qspi_handler, NULL);
    if (NRFX_SUCCESS != err_code)
    {
        LOG_E("\r\n ERROR: QSPI_init:0x%x\n", err_code);
        return -1;
    }
    configure_memory();
    return 0;
}

static int read(long offset, uint8_t *buf, size_t size)
{
    uint32_t err_code;
    m_finished = false;
    err_code = nrfx_qspi_read(buf, size, offset);
    WAIT_FOR_PERIPH();
    if (NRFX_SUCCESS == err_code)
    {
        return size;
    }
    else
    {
        LOG_E("\r\n ERROR: read:0x%x\n", err_code);
        return -1;
    }
}

static int write(long offset, const uint8_t *buf, size_t size)
{
    uint32_t err_code;
    m_finished = false;
    err_code = nrfx_qspi_write(buf, size, offset);
    WAIT_FOR_PERIPH();
    if (NRFX_SUCCESS == err_code)
    {
        return size;
    }
    else
    {
        LOG_E("\r\n ERROR: write:0x%x\n", err_code);
        return -1;
    }
}

static int erase(long offset, size_t size)
{
    uint32_t err_code;
    m_finished = false;
    err_code = nrfx_qspi_erase(NRF_QSPI_ERASE_LEN_64KB, offset);
    WAIT_FOR_PERIPH();
    if (NRFX_SUCCESS == err_code)
    {
        return size;
    }
    else
    {
        LOG_E("\r\n ERROR: erase:0x%x\n", err_code);
        return -1;
    }
}
int rt_qspi_init(void)
{


	if(init()==-1)
		{return -1;}
    return 0;
}

INIT_BOARD_EXPORT(rt_qspi_init);
#if 0
struct fal_flash_dev nor_flash0 =
{
    .name       = NOR_FLASH_DEV_NAME,
    .addr       = 0,
    .len        = QSPI_FLASH_SIZE_KB * 1024,
    .blk_size   = 4096,
    .ops        = {init, read, write, erase},
    .write_gran = 1
};
#endif
#define QSPI_TEST_DATA_SIZE 256

static uint8_t m_buffer_tx[QSPI_TEST_DATA_SIZE];
static uint8_t m_buffer_rx[QSPI_TEST_DATA_SIZE];
void qspi_sample(void)
{
    uint32_t i;
    uint32_t err_code;
    uint32_t f=0;
	uint32_t g=0;

   // rt_kprintf("QSPI write and read example");

    srand(1);
    for (i = 0; i < QSPI_TEST_DATA_SIZE; ++i)
    {
        m_buffer_tx[i] = (uint8_t)rand();
    }

    //err_code=init();
	//if(err_code==-1)
	//	{return;}
    //rt_kprintf("QSPI example started.");

    configure_memory();

 err_code=erase(0,0);
 if(err_code==-1)
		{return;}
    //rt_kprintf(" erasing ");

     err_code=write(0,m_buffer_tx, QSPI_TEST_DATA_SIZE);
 if(err_code==-1)
		{return;}


   // rt_kprintf("Process of writing");

     err_code=read(0,m_buffer_rx, QSPI_TEST_DATA_SIZE);
   if(err_code==-1)
		{return;}
   // rt_kprintf("Data read");

   // rt_kprintf("Compare...");
    if (memcmp(m_buffer_tx, m_buffer_rx, QSPI_TEST_DATA_SIZE) == 0)
    {
        rt_kprintf("ok\n");
       f++;
    }
    else
    {
        rt_kprintf("err\n");
       g++;
    }

   
}
MSH_CMD_EXPORT(qspi_sample, qspi sample);

#endif
