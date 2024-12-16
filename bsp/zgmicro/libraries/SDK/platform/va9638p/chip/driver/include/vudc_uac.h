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
    2014.05.28      zhangqichen                initial version
------------------------------------------------------------------------------*/

#ifndef ALLOW_VKERNEL_INTERNAL_INCLUDES
#error "This file is for vkernel internal use only"
#endif
#ifndef __VUDC_DEVICE_UAC_H__
#define __VUDC_DEVICE_UAC_H__

/* Includes ------------------------------------------------------------------*/
#include "vdev_cfg.h"

#define UAC_ID_IF_CONTROL                        0x00
#define UAC_ID_IF_CONTROL2                      0x03
#define UAC_ID_IF_SPK_STREAMING            0x01 
#define UAC_ID_IF_MIC_STREAMING            0x02
#define UAC_ID_IF_HPE_STREAMING            0x04
#define UAC_ID_IF_HID                                 0x05

#define UAC_ID_ET_SPK_INPUT_TERM            0x01
#define UAC_ID_ET_SPK_FEAT_UINT              0x02
#define UAC_ID_ET_SPK_OUTPUT_TERM         0x03
#define UAC_ID_ET_HPE_INPUT_TERM            0x04
#define UAC_ID_ET_HPE_FEAT_UINT              0x05
#define UAC_ID_ET_HPE_OUTPUT_TERM         0x06
#define UAC_ID_ET_MIC_INPUT_TERM            0x08
#define UAC_ID_ET_MIC_FEAT_UNIT              0x09
#define UAC_ID_ET_MIC_OUTPUT_TERM         0x0a

#define UAC_ID_EP_SPK_ENDP                 1  
#define UAC_ID_EP_MIC_ENDP                 2
#define UAC_ID_EP_HPE_ENDP                 3
#define UAC_ID_EP_INT_IN_ENDP            4

#define UAC_HID_DESC_SIZE                 9
#define UAC_HID_RPT_DESC_SIZE         65

#define UAC_HID_CLASS_GET_REPORT            0x01
#define UAC_HID_CLASS_GET_IDLE                 0x02
#define UAC_HID_CLASS_GET_PROTOCOL        0x03
#define UAC_HID_CLASS_SET_REPORT            0x09
#define UAC_HID_CLASS_SET_IDLE                 0x0A
#define UAC_HID_CLASS_SET_PROTOCOL        0x0B

/* USB Audio HID Control Key Map */
#define UAC_HID_VOLUME_UP               (0x1)
#define UAC_HID_VOLUME_DOWN          (0x2)
#define UAC_HID_VOICE_MUTE              (0x4)
#define UAC_HID_PLAY_PAUSE              (0x8)
#define UAC_HID_SCAN_NEXT                (0x10)
#define UAC_HID_SCAN_PREV                (0x20)
#define UAC_HID_FAST_FORWARD         (0x40)
#define UAC_HID_STOP_MUSIC              (0x80)


typedef enum {
    MIC_STATE_INIT = 0,
    MIC_STATE_READY,
    MIC_STATE_WRITE_DONE,
}MIC_HANDSHAKE_STATE;

typedef struct _VUDC_UAC_INFO{
    UINT8 *p_mic;
    UINT8 *p_speaker;
    UINT8 *p_headphone;
    UINT8 *p_hid;
    UINT16 mic_size;
    UINT16 speaker_size;
    UINT16 headphone_size;
    UINT16 hid_size;
}VUDC_UAC_INFO;

#define MIC_HANDSHAKE_STATE(addr, st)                  *((volatile UINT16 *)(addr)) = st;

void VUDC_UAC_Init(void);
void VUDC_UAC_SpeakerProc(void);
void VUDC_UAC_MicProc(void);
void VUDC_UAC_HeadphoneProc(void);
void VUDC_UAC_HidProc(void);
void VUDC_UAC_IntfSetting(UINT8 streamId, UINT8 setting);
UINT8 VUDC_UAC_GetScenario(void);
void VUDC_UAC_MicHandle(void);
UINT8 VUDC_UAC_GetMode(void);

extern const UINT8 VUDC_UAC_headset_desc_cfg[];
extern const UINT8 VUDC_UAC_mic_desc_cfg[];
extern const UINT8 VUDC_UAC_game_headset_desc_cfg[];
extern const UINT8 VUDC_UAC_desc_String_3[];
extern const UINT8 VUDC_UAC_desc_String_4[];
extern const UINT8 VUDC_UAC_desc_String_5[];
extern const UINT8 VUDC_UAC_ReportDescriptor[] ;

#endif  /* multiple inclusion protection */
