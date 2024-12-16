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
    OTG device

Description:
     OTG device Driver

Revision History:
     Date                  Author                   Description
     2014.05.28        zhangqichen                   initial version
------------------------------------------------------------------------------*/

#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef __VUDC_DEVICE_AUD_H__
#define __VUDC_DEVICE_AUD_H__

/* Includes ------------------------------------------------------------------*/
#include "vdrv_otg.h"

//interface, terminal, entity and endpoint ID
//for 9628
#define USB_AUDIO_SELFTEST             0
#define HID_IS_USED                    0
#define VDEV_USB_AUDIO_INTER_IPC       1
#define USB_AUDIO_DUMP_DATA_EN         0

#define USB_AUDIO_MODE_SWITCH  2
#define USB_AUDIO_MODE_ON   1
#define USB_AUDIO_MODE_OFF  0

#if (HID_IS_USED == 1)
#define AUDIO_NUM_OF_INTF               3 // 1 AS + 1 AC + 1 HID
#else
#define AUDIO_NUM_OF_INTF               2 // 1 AS + 1 AC
#endif
#define AUDIO_ID_IF_CONTROL                   0x00
#define AUDIO_ID_ET_INPUT_TERM                0x01
#define AUDIO_ID_ET_FEAT_UINT                 0x02
#define AUDIO_ID_ET_OUTPUT_TERM               0x03

#define AUDIO_ID_IF_STREAMING                0x01 
#define AUDIO_ID_EP_ISO_OUT_ENDP                 1  



#define AUDIO_ID_IF_HID                      0x02
#define AUDIO_ID_EP_INT_IN_ENDP                 2 

/* Exported types ------------------------------------------------------------*/

 
/* Audio Request Codes */ 
#define AUDIO_REQUEST_UNDEFINED                 0x00 
#define AUDIO_REQUEST_SET_CUR                   0x01 
#define AUDIO_REQUEST_GET_CUR                   0x81 
#define AUDIO_REQUEST_SET_MIN                   0x02 
#define AUDIO_REQUEST_GET_MIN                   0x82 
#define AUDIO_REQUEST_SET_MAX                   0x03 
#define AUDIO_REQUEST_GET_MAX                   0x83 
#define AUDIO_REQUEST_SET_RES                   0x04 
#define AUDIO_REQUEST_GET_RES                   0x84 
#define AUDIO_REQUEST_SET_MEM                   0x05 
#define AUDIO_REQUEST_GET_MEM                   0x85 
#define AUDIO_REQUEST_GET_STAT                  0xFF 
 
 
/* Audio Control Selector Codes */ 
#define AUDIO_CONTROL_UNDEFINED                 0x00    /* Common Selector */ 
 
/*  Terminal Control Selectors */ 
#define AUDIO_COPY_PROTECT_CONTROL              0x01 
 
/*  Feature Unit Control Selectors */ 
#define AUDIO_MUTE_CONTROL                      0x01 
#define AUDIO_VOLUME_CONTROL                    0x02 
#define AUDIO_BASS_CONTROL                      0x03 
#define AUDIO_MID_CONTROL                       0x04 
#define AUDIO_TREBLE_CONTROL                    0x05 
#define AUDIO_GRAPHIC_EQUALIZER_CONTROL         0x06 
#define AUDIO_AUTOMATIC_GAIN_CONTROL            0x07 
#define AUDIO_DELAY_CONTROL                     0x08 
#define AUDIO_BASS_BOOST_CONTROL                0x09 
#define AUDIO_LOUDNESS_CONTROL                  0x0A 

/*  Processing Unit Control Selectors: */ 
#define AUDIO_ENABLE_CONTROL                    0x01    /* Common Selector */ 
#define AUDIO_MODE_SELECT_CONTROL               0x02    /* Common Selector */ 
 
/*  - Up/Down-mix Control Selectors */ 
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */ 
/*      AUDIO_MODE_SELECT_CONTROL               0x02       Common Selector */ 
 
/*  - Dolby Prologic Control Selectors */ 
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */ 
/*      AUDIO_MODE_SELECT_CONTROL               0x02       Common Selector */ 
 
/*  - 3D Stereo Extender Control Selectors */ 
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */ 
#define AUDIO_SPACIOUSNESS_CONTROL              0x02 
 
/*  - Reverberation Control Selectors */ 
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */ 
#define AUDIO_REVERB_LEVEL_CONTROL              0x02 
#define AUDIO_REVERB_TIME_CONTROL               0x03 
#define AUDIO_REVERB_FEEDBACK_CONTROL           0x04 
 
/*  - Chorus Control Selectors */ 
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */ 
#define AUDIO_CHORUS_LEVEL_CONTROL              0x02 
#define AUDIO_SHORUS_RATE_CONTROL               0x03 
#define AUDIO_CHORUS_DEPTH_CONTROL              0x04 
 
/*  - Dynamic Range Compressor Control Selectors */ 
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */ 
#define AUDIO_COMPRESSION_RATE_CONTROL          0x02 
#define AUDIO_MAX_AMPL_CONTROL                  0x03 
#define AUDIO_THRESHOLD_CONTROL                 0x04 
#define AUDIO_ATTACK_TIME_CONTROL               0x05 
#define AUDIO_RELEASE_TIME_CONTROL              0x06 
 
/*  Extension Unit Control Selectors */ 
/*      AUDIO_ENABLE_CONTROL                    0x01       Common Selector */ 
 
/*  Endpoint Control Selectors */ 
#define AUDIO_SAMPLING_FREQ_CONTROL             0x01 
#define AUDIO_PITCH_CONTROL                     0x02 
 
 
/* Audio Format Specific Control Selectors */ 
 
/*  MPEG Control Selectors */ 
#define AUDIO_MPEG_CONTROL_UNDEFINED            0x00 
#define AUDIO_MPEG_DUAL_CHANNEL_CONTROL         0x01 
#define AUDIO_MPEG_SECOND_STEREO_CONTROL        0x02 
#define AUDIO_MPEG_MULTILINGUAL_CONTROL         0x03 
#define AUDIO_MPEG_DYN_RANGE_CONTROL            0x04 
#define AUDIO_MPEG_SCALING_CONTROL              0x05 
#define AUDIO_MPEG_HILO_SCALING_CONTROL         0x06 
 
/*  AC-3 Control Selectors */ 
#define AUDIO_AC3_CONTROL_UNDEFINED             0x00 
#define AUDIO_AC3_MODE_CONTROL                  0x01 
#define AUDIO_AC3_DYN_RANGE_CONTROL             0x02 
#define AUDIO_AC3_SCALING_CONTROL               0x03 
#define AUDIO_AC3_HILO_SCALING_CONTROL          0x04 


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

VRET_VALUE VUDC_AUD_ClassReqHdlr(UOTGC_DEV_REQ *setup_req, VOTG_MSG *msg);


#define  USB_PKT_SIZE      192

typedef struct {
    UINT32 total;
    UINT32 lost;
    UINT32 int_num;
    UINT32 int_lost;
}USB_AUDIO_MSG_STATUS;


#if (VDEV_USB_AUDIO_INTER_IPC>0)
typedef struct usbaudio_msgbox_s{
    UINT8    ping_status;    //indicate the status of ping/pong buffer, invalid or is be using or ready
    UINT8    pong_status;
    UINT8   next_index;
    UINT8    cur_index;    //which buffer is in using now.
    UINT8    ping_buf[USB_PKT_SIZE+4];    //ping buffer
    UINT8    pong_buf[USB_PKT_SIZE+4];    //pong buffer
}usbaudio_msgbox_t;

/* Private typedef -----------------------------------------------------------*/

typedef struct _IPC_STATUS_STRU {
    UINT32 total;
    UINT32 lost;
    UINT32 int_num;
    UINT32 int_lost;
}IPC_STATUS_STRU;

typedef struct _UAC_PLAY_CONTROL{
    UINT8 frame_ready;
    UINT8 buff_switch;
}UAC_PLAY_CONTROL;
    
#define STATUS_INVALID      (0x00)
#define  STATUS_BTSET       (0x01)
#define  STATUS_TPCLR       (0x02)

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
//#define IPC_FLAG        (0x9638)

//#define CMD_START       (0x01)
//#define CMD_STOP        (0x02)
typedef int (* UDC_AUD_StreamOut_CB_T)(void* , UINT8* ,UINT32 );

void VUDC_AUD_Init(void);
void VUDC_AUD_Start(void);
void VUDC_AUD_Stop(void);
void VUDC_AUD_MusicPlay(void);
static UINT32 *VUDC_AUD_PopDmaBuf(void);
static void VUDC_AUD_PushDmaBuf(void);
extern UINT8 VUDC_AUD_IPC_Isr(void);
//UINT8 HLP_UsbAudioMsgHandle(void);
void VUDC_AUD_MusicStreamOutRegistor(UDC_AUD_StreamOut_CB_T cb);

#else


#endif

static void VUDC_TX_EP0(UINT32 len, UINT32 addr, UINT32 offset, BOOL dataend);
static void VUDC_EP0_RxServiced_STALL(void);
static void VUDC_EP0_RxServiced_NoData(void);
static void VUDC_RX_EP0(UINT32 len, UINT32 addr, UINT32 offset, BOOL dataend);
#endif    /* multiple inclusion protection */
