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
    WD

Description:
    Watchdog driver.

Revision History:
    Date                  Author                   Description
    2020.04.26            ly                       initial version
------------------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdcore.h"
#include "vreg_def.h"
#include "vdrv_trng.h"

#ifdef TRNG_GET_RANDOM_CACHE
    UINT8 g_random_num = 4;
    UINT8 g_random_index = 0;
#endif

/******************************************************************************
Description:
    set the TRNG to execute one of a number of actions

Parameters:
    mode - 0 nop,1 generate a random num,2 random reseed,3 nonce reseed

Return Value:
    None.
*****************************************************************************/
void TRNG_Setmode ( UINT8 mode )
{
    WRITE_REG ( TRNG_CTRL, mode );
}

/******************************************************************************
Description:
    set random num bit

Parameters:
    num- 0 128 bit,1 256 bit

Return Value:
    None.
*****************************************************************************/
void TRNG_Setnum ( UINT8 num )
{
    UINT32 reg;
    reg = READ_REG ( TRNG_MODE );

    /* 128bit */
    if ( num == TRNG_RANDOM_NUM_128BIT ) {
        reg &= ~0x08;
        WRITE_REG ( TRNG_MODE, reg );
    } else { /* 256bit */
        reg |= 0x08;
        WRITE_REG ( TRNG_MODE, reg );
    }
}
/******************************************************************************
Description:
    reseed TRNG by random type

Parameters:
    None

Return Value:
    None.
*****************************************************************************/
void TRNG_Reseed_Random ( void )
{
    //    UINT32 reg;
    /*first,reseed*/
    TRNG_Setmode ( TRNG_RESEED_RANDOM );
    /*second,check state bit */
    while ( ! ( ( READ_REG ( TRNG_ISTAT ) >> 0x01 ) & 0x01 ) );
    /*clear the flag*/
    WRITE_REG ( TRNG_ISTAT, READ_REG ( TRNG_ISTAT ) | 0x02 );
}
/******************************************************************************
Description:
    reseed TRNG by nonce type
  use the rand0~7 registers as the seed
Parameters:
    None

Return Value:
    None.
*****************************************************************************/
void TRNG_Reseed_Nonce ( void )
{
    UINT32 reg;
    /*set nonce mode to reseed*/
    reg = READ_REG ( TRNG_SMODE );
    WRITE_REG ( TRNG_SMODE, reg | 0x04 );
    /*wait nonce mode "1"*/
    while ( ! ( READ_REG ( TRNG_STAT ) & 0x04 ) );
    /*Load the 255-bit nonce into SEED0 through SEED7*/
    WRITE_REG ( TRNG_SEED0, READ_REG ( TRNG_RAND7 ) );
    WRITE_REG ( TRNG_SEED1, READ_REG ( TRNG_RAND6 ) );
    WRITE_REG ( TRNG_SEED2, READ_REG ( TRNG_RAND5 ) );
    WRITE_REG ( TRNG_SEED3, READ_REG ( TRNG_RAND4 ) );
    WRITE_REG ( TRNG_SEED4, READ_REG ( TRNG_RAND3 ) );
    WRITE_REG ( TRNG_SEED5, READ_REG ( TRNG_RAND2 ) );
    WRITE_REG ( TRNG_SEED6, READ_REG ( TRNG_RAND1 ) );
    WRITE_REG ( TRNG_SEED7, READ_REG ( TRNG_RAND0 ) );

    /*Write a 3 to CMD in the CTRL register*/
    TRNG_Setmode ( TRNG_RESEED_NONCE );
    /*write 0 to nonce_mode in mode register*/
    reg = READ_REG ( TRNG_SMODE );
    reg &= ~ ( 0x04 );
    WRITE_REG ( TRNG_SMODE, reg );

}
/******************************************************************************
Description:
    generate random num,must reseed before this function
Parameters:
    None

Return Value:
    None.
*****************************************************************************/
void TRNG_Generate ( void )
{
    //    UINT32 reg;
    /*first,set generate cmd*/
    TRNG_Setmode ( TRNG_GENERATE_RANDOM );
    /*second,check state bit */
    while ( ! ( ( READ_REG ( TRNG_ISTAT ) ) & 0x01 ) );
    /*clear the flag*/
    WRITE_REG ( TRNG_ISTAT, READ_REG ( TRNG_ISTAT ) | 0x01 );

    /* nonce reseed mode need wait */
    VCOMMON_Delay ( 10 );
}

/******************************************************************************
Description:
    random Init
Parameters:
    rand_num - num of random, TRNG_RANDOM_NUM_128BIT or TRNG_RANDOM_NUM_256BIT

Return Value:
    None.
*****************************************************************************/
#ifdef TRNG_GET_RANDOM_CACHE
void TRNG_Init ( UINT8 rand_num )
{
    g_random_num = ( rand_num == TRNG_RANDOM_NUM_128BIT ) ? 4 : 8;
    g_random_index = 0;

    TRNG_Setnum ( rand_num );
    TRNG_Reseed_Random();
    TRNG_Generate();
}
#endif

/******************************************************************************
Description:
    get random num,
Parameters:
    None

Return Value:
    None.
*****************************************************************************/
UINT32 TRNG_Get_Random ( void )
{
#ifdef TRNG_GET_RANDOM_CACHE
    UINT32 reg_addr;
    UINT32 val;

    if ( g_random_index >= g_random_num ) {
        g_random_index = 0;

        TRNG_Reseed_Random();
        TRNG_Generate();
    }

    reg_addr = TRNG_RAND0 + ( g_random_index * 4 );
    val = READ_REG ( reg_addr );

    g_random_index++;
#else
    UINT32 val;

    TRNG_Setnum ( TRNG_RANDOM_NUM_256BIT );
    TRNG_Reseed_Random();
    TRNG_Generate();

    val = READ_REG ( TRNG_RAND0 );
#endif

    return val;
}

#ifdef TRNG_INT_MODE

/* test for interrupt */
UINT8 TRNG_intflag = 0;

/******************************************************************************
Description:
    reseed TRNG by random type though interrput,need to set nvic before use this function

Parameters:
    None

Return Value:
    None.
*****************************************************************************/
void TRNG_Reseed_Random_interruput ( void )
{
    UINT32 reg;

    reg = READ_REG ( TRNG_IE );
    WRITE_REG ( TRNG_IE, reg | ( ( 0x01 << 31 ) + ( 0x02 ) ) );

    TRNG_Setmode ( TRNG_RESEED_RANDOM );

    /* wait the int flag set 1,then clear it */
    while ( !TRNG_intflag );

    TRNG_intflag = 0x00;

}

/******************************************************************************
Description:
    clear the int flag by set the bit "1"
  depended on IE register,SEED_DONE and RAND_RDY flags may both clear by this function

Parameters:
    None

Return Value:
    None.
*****************************************************************************/
void TRNG_Clear_intflag ( void )
{
    SINT32 reg;
    //  reg = READ_REG(TRNG_ISTAT);
    reg = READ_REG ( TRNG_IE );
    if ( reg & 0x01 ) {
        WRITE_REG ( TRNG_ISTAT, reg | 0x01 );
    }
    if ( reg & 0x02 ) {
        WRITE_REG ( TRNG_ISTAT, reg | 0x02 );
    }
}
/******************************************************************************
Description:
    enable TRNG interrupt,only in top core
Parameters:
    None

Return Value:
    None.
*****************************************************************************/
void TRNG_EnableInt ( void )
{
    VNVIC_INT_Enable ( IRQ_TRNG_INT );
}
/******************************************************************************
Description:
    generate random num by interrupt,must reseed before this function
Parameters:
    None

Return Value:
    None.
*****************************************************************************/
void TRNG_Generate_interrupt ( void )
{
    UINT32 reg;

    reg = READ_REG ( TRNG_IE );
    WRITE_REG ( TRNG_IE, reg | ( ( 0x01 << 31 ) + ( 0x01 ) ) );

    TRNG_Setmode ( TRNG_GENERATE_RANDOM );

    while ( !TRNG_intflag );

    TRNG_intflag = 0x00;
}
#endif


#if (TRNG_TEST == 1)

#define TRNG_printf(...)

void Printf_Rand ( void )
{
    UINT32 reg;

    reg = READ_REG ( TRNG_RAND0 );
    TRNG_printf ( "rand0 = 0x%x", reg );

    reg = READ_REG ( TRNG_RAND1 );
    TRNG_printf ( "  rand1 = 0x%x", reg );

    reg = READ_REG ( TRNG_RAND2 );
    TRNG_printf ( "  rand2 = 0x%x", reg );

    reg = READ_REG ( TRNG_RAND3 );
    TRNG_printf ( "  rand3 = 0x%x", reg );

    reg = READ_REG ( TRNG_RAND4 );
    TRNG_printf ( "  rand4 = 0x%x", reg );

    reg = READ_REG ( TRNG_RAND5 );
    TRNG_printf ( "  rand5 = 0x%x", reg );

    reg = READ_REG ( TRNG_RAND6 );
    TRNG_printf ( "  rand6 = 0x%x", reg );

    reg = READ_REG ( TRNG_RAND7 );
    TRNG_printf ( "  rand7 = 0x%x\r\n", reg );
}

void TRNG_Test ( void )
{
    Printf_Rand();
    while ( 1 ) {
        /* set 256 bit or 128 bit */
        TRNG_Setnum ( TRNG_RANDOM_NUM_256BIT );

        /* reseed and generate random num by interrupt mode */
        //      TRNG_EnableInt();
        //      TRNG_Reseed_Random_interruput();
        //      TRNG_Generate_interrupt();

        /* reseed and generate random num by polling mode */
        /* choose reseed type,random or nonce */
        if ( 1 ) {
            TRNG_Reseed_Random();
        } else {
            TRNG_Reseed_Nonce();
        }
        TRNG_Generate();

        VCOMMON_Delay ( 100 );
        Printf_Rand();
    }
}

#endif
