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

UINT8 rdata[VSYS_FLASH_SECTOR_SIZE];
UINT8 pdata[VSYS_FLASH_SECTOR_SIZE];
UINT8 edata[VSYS_FLASH_SECTOR_SIZE];

UINT8 rdata_page[VSYS_FLASH_PAGE_SIZE];
UINT8 pdata_page[VSYS_FLASH_PAGE_SIZE];

UINT32 ERR_FLAG = 0, LOOP_NUM = 0;

int main ( void )
{
	VRET_VALUE err = VRET_NO_ERROR;
    int i  = 0 , j = 0, k = 0, m = 0, n = 0;
    int time_e0 = 0, time_e1 = 0, time_e = 0, 
    time_p0 = 0, time_p1 = 0, time_p = 0, 
    time_r0 = 0, time_r1 = 0, time_r = 0;
    int time_e2 = 0,time_e22 = 0;

    VCPU_CoreInit(); 
    VCPU_INT_Enable();
    VSYS_CKM_Init();

    VSYS_FLASH_Init();
	
	#if 0
	Dbg_PRINT("OTP_read test...\r\n");
	err = VSYS_FLASH_Erase(VSYS_FLASH_INPACKAGE_OTP ,0x3000, 0x400);
	Dbg_PRINT("OTP_read ret%d 0x%x\r\n",err,__VERRNO);
	err = VSYS_FLASH_Read(VSYS_FLASH_INPACKAGE_OTP ,0x3000, rdata, 0x400);
	Dbg_PRINT("OTP_read ret%d 0x%x\r\n",err,__VERRNO);
	for(j = 0;j < 1024; j++){
		pdata[j] = j;
	}
	err = VSYS_FLASH_Program(VSYS_FLASH_INPACKAGE_OTP ,0x3000, pdata, 0x400);
	Dbg_PRINT("OTP_read ret%d 0x%x\r\n",err,__VERRNO);
	err = VSYS_FLASH_Read(VSYS_FLASH_INPACKAGE_OTP ,0x3000, rdata, 0x400);
	Dbg_PRINT("OTP_read ret%d 0x%x\r\n",err,__VERRNO);
	err = VSYS_FLASH_GetParam(VSYS_FLASH_INPACKAGE_OTP ,VSYS_FLASH_UID, rdata_page, sizeof(rdata_page));
	Dbg_PRINT("OTP_read ret%d 0x%x\r\n",err,__VERRNO);
	while(1);
	#endif


    Dbg_PRINT("erase_read write_read test...\r\n");

    #if 0
    for(k = 0;k < 1024; k++){
      pdata[k] = k;
    }    
    memset(rdata,0x00,1024);
    int mt_cycle = __CPUCycleGet();
    //err = VSPI_FLASH_Erase ( VSPI_FLASH_INPACKAGE, 0x00 * 1024, 1024*1024*2 );
    VSPI_FLASH_EraseChip(VSPI_FLASH_INPACKAGE);
    mt_cycle = __CPUCycleGet() - mt_cycle;    
    err = VSPI_FLASH_Erase ( VSPI_FLASH_INPACKAGE, 0x00 * 1024, VSPI_FLASH_SECTOR_SIZE );
    Dbg_PRINT("%d\r\n",mt_cycle);

    err = VSPI_FLASH_Read ( VSPI_FLASH_INPACKAGE, 0x00 * 1024, rdata, 1024);
    Dbg_PRINT("%d\r\n",err);

    err = VSPI_FLASH_Program ( VSPI_FLASH_INPACKAGE, 0x00 * 1024, pdata, 1024 );
    Dbg_PRINT("%d\r\n",err);
    err = VSPI_FLASH_Read ( VSPI_FLASH_INPACKAGE, 0x00 * 1024, rdata, 1024);
    Dbg_PRINT("%d\r\n",err);

    for(i = 0; i < 1024; i++){
      if(rdata[i] != pdata[i]){
        Dbg_PRINT("write_read:err[0x%x] %d [0x%x]\r\n",rdata[i],i,pdata[i]);
        ERR_FLAG++;
        break;
      }
    }
    
    Dbg_PRINT("write_read:finish\r\n");
    //while(1);
    #endif

#if 0
    //VSPI_FLASH_EXTERNAL
    //1. erase
    //err = VSPI_FLASH_Erase ( VSPI_FLASH_INPACKAGE, 0, VSPI_FLASH_ALL_SIZE );
    
    while(1){
      /*2M 1.8V Flash*/
      for(j = 0;j < 1024; j++){
        //memset(pdata,(0xA0+(j&0x0F)),sizeof(pdata));
        for(k = 0;k < VSPI_FLASH_SECTOR_SIZE; k++){
          pdata[k] = k+j;
        }
                
        memset(rdata,0x00,sizeof(rdata));
        /*SPI_FLASH_Erase(扇区) 计时，单位us*/
        time_e0 = READ_REG ( REG_TIMER_TFCLKLCNT );         
        err = VSPI_FLASH_Erase ( VSPI_FLASH_EXTERNAL, VSPI_FLASH_SECTOR_SIZE*j, VSPI_FLASH_SECTOR_SIZE );
        time_e1 = READ_REG ( REG_TIMER_TFCLKLCNT );
        time_e += (time_e0 - time_e1);

        err = VSPI_FLASH_Read ( VSPI_FLASH_EXTERNAL, VSPI_FLASH_SECTOR_SIZE*j, rdata, sizeof(rdata));

        for(i = 0;i < sizeof(rdata);i++){
          if(rdata[i] != 0xFF){
            Dbg_PRINT("erase_read:err[0x%x] %d %d [0x%x]\r\n",rdata[i],j,i,0xFF);
            ERR_FLAG++;
            break;
          }
        }
        
        /*每个扇区16页*/
        for(m=0; m<16; m++){
          
          for(n = 0;n < VSPI_FLASH_PAGE_SIZE; n++){
            pdata_page[n] = n+m;

          } 

          memset(rdata_page,0x00,sizeof(rdata_page));
          /*SPI_FLASH_Program（按页） 计时，单位us*/
          time_p0 = READ_REG ( REG_TIMER_TFCLKLCNT );     
          //err = VSPI_FLASH_Program ( VSPI_FLASH_INPACKAGE, VSPI_FLASH_SECTOR_SIZE*j, pdata, sizeof(pdata) );
          err = VSPI_FLASH_Program ( VSPI_FLASH_EXTERNAL, VSPI_FLASH_SECTOR_SIZE*j + VSPI_FLASH_PAGE_SIZE*m, pdata_page, sizeof(pdata_page) );
          time_p1 = READ_REG ( REG_TIMER_TFCLKLCNT );
          time_p += (time_p0 - time_p1);
          
          /*VSPI_FLASH_Read（按页） 计时，单位us*/
          time_r0 = READ_REG ( REG_TIMER_TFCLKLCNT );  
          //err = VSPI_FLASH_Read ( VSPI_FLASH_INPACKAGE, VSPI_FLASH_SECTOR_SIZE*j, rdata, sizeof(rdata));
          err = VSPI_FLASH_Read ( VSPI_FLASH_EXTERNAL, VSPI_FLASH_SECTOR_SIZE*j + VSPI_FLASH_PAGE_SIZE*m, rdata_page, sizeof(rdata_page));
          time_r1 = READ_REG ( REG_TIMER_TFCLKLCNT );
          time_r += (time_r0 - time_r1);
          
          for(i = 0; i < sizeof(rdata_page); i++){
            if(rdata_page[i] != pdata_page[i]){
              Dbg_PRINT("write_read:err[0x%x] %d %d [0x%x]\r\n",rdata_page[i],j,i,pdata_page[i]);
              ERR_FLAG++;
              break;
            }
          }

          
        }     
       
      }
      LOOP_NUM++;

      Dbg_PRINT("FLASH_EX_LOOP_NUM = %d ERR_FLAG = %d \r\n",LOOP_NUM,ERR_FLAG);
      Dbg_PRINT("FLASH_EX_erase time[%d]us \r\n",time_e/j);
      Dbg_PRINT("FLASH_EX_program time[%d]us \r\n",time_p/(j*16));
      Dbg_PRINT("FLASH_EX_read time[%d]us \r\n",time_r/(j*16));
      time_e = 0;
      time_p = 0;
      time_r = 0;




    }
      
    #endif 
    
#if 1
    //VSPI_FLASH_INPACKAGE
    //err = VSPI_FLASH_Erase ( VSPI_FLASH_INPACKAGE, 0, VSPI_FLASH_ALL_SIZE );
    
    while(1){
      /*2M 1.8V Flash*/
	  int start = 0;
      for(j = start;j < 1024; j++){
        //memset(pdata,(0xA0+(j&0x0F)),sizeof(pdata));
        for(k = 0;k < VSYS_FLASH_SECTOR_SIZE; k++){
          pdata[k] = k+j;
        }
                
        vmemset(rdata,0x00,sizeof(rdata));
        /*SPI_FLASH_Erase(扇区) 计时，单位us*/
        time_e0 = vtimeusget();         
        err = VSYS_FLASH_Erase ( VSYS_FLASH_INPACKAGE, VSYS_FLASH_SECTOR_SIZE*j, VSYS_FLASH_SECTOR_SIZE );
        time_e1 = vtimeusget();
        time_e += vtimeusinterval(time_e0 ,time_e1);

        err = VSYS_FLASH_Read ( VSYS_FLASH_INPACKAGE, VSYS_FLASH_SECTOR_SIZE*j, rdata, sizeof(rdata));

        for(i = 0;i < sizeof(rdata);i++){
          if(rdata[i] != 0xFF){
            Dbg_PRINT("erase_read:err%d[0x%x] %d %d [0x%x]\r\n",err,rdata[i],j,i,0xFF);
            ERR_FLAG++;
            break;
          }
        }
        
        /*每个扇区16页*/
        for(m=0; m<16; m++){
          
          for(n = 0;n < VSYS_FLASH_PAGE_SIZE; n++){
            pdata_page[n] = n+m;

          } 

          vmemset(rdata_page,0x00,sizeof(rdata_page));
          /*SPI_FLASH_Program（按页） 计时，单位us*/
          time_p0 = vtimeusget();
          //err = VSPI_FLASH_Program ( VSPI_FLASH_INPACKAGE, VSPI_FLASH_SECTOR_SIZE*j, pdata, sizeof(pdata) );
          err = VSYS_FLASH_Program ( VSYS_FLASH_INPACKAGE, VSYS_FLASH_SECTOR_SIZE*j + VSYS_FLASH_PAGE_SIZE*m, pdata_page, sizeof(pdata_page) );
          time_p1 = vtimeusget();
          time_p += vtimeusinterval(time_e0 ,time_e1);
          
          /*VSPI_FLASH_Read（按页） 计时，单位us*/
          time_r0 = vtimeusget();
          //err = VSPI_FLASH_Read ( VSPI_FLASH_INPACKAGE, VSPI_FLASH_SECTOR_SIZE*j, rdata, sizeof(rdata));
          err = VSYS_FLASH_Read ( VSYS_FLASH_INPACKAGE, VSYS_FLASH_SECTOR_SIZE*j + VSYS_FLASH_PAGE_SIZE*m, rdata_page, sizeof(rdata_page));
          time_r1 = vtimeusget();
          time_r += vtimeusinterval(time_e0 ,time_e1);
          
          for(i = 0; i < sizeof(rdata_page); i++){
            if(rdata_page[i] != pdata_page[i]){
              Dbg_PRINT("write_read:err[0x%x] %d %d [0x%x]\r\n",rdata_page[i],j,i,pdata_page[i]);
              ERR_FLAG++;
              break;
            }
          }

          
        }     
       
      }
      LOOP_NUM++;

      Dbg_PRINT("FLASH_IN_LOOP_NUM = %d ERR_FLAG = %d \r\n",LOOP_NUM,ERR_FLAG);
      Dbg_PRINT("FLASH_IN_erase time[%d]us \r\n",time_e/(j-start));
      Dbg_PRINT("FLASH_IN_program time[%d]us \r\n",time_p/((j-start)*16));
      Dbg_PRINT("FLASH_IN_read time[%d]us \r\n",time_r/((j-start)*16));
      time_e = 0;
      time_p = 0;
      time_r = 0;


    }
      
    #endif 

    #if 0
     for(j = 0;j < 1024;j++){
      memset(rdata,0x00,sizeof(pdata));
      err = VSPI_FLASH_Read ( VSPI_FLASH_INPACKAGE, VSPI_FLASH_SECTOR_SIZE*j, rdata, sizeof(rdata));
      }
	Dbg_PRINT("erase_read write_read test finish\r\n");
    while ( 1 );
    return 0;
    #endif

}




