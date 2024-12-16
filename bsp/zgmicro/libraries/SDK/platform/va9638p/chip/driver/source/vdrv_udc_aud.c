/*Includes ------------------------------------------------------------------*/
#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vstdlib.h"
#include "vstdio.h"
#include "vstdos.h"
#include "vstdcore.h"
#include "vdrv_otg.h"
#include "vudc.h"
#include "vdrv_udc_aud.h"
#include "vdrv_mbox.h"
#include "vdrv_ipc.h"

extern void VUDC_RX_ISO_EPx(UINT8 ep, UINT32 len, UINT32 addr, UINT32 offset);
extern USB_DEVICE_HDL Usb_device_hdl;
static IPC_STATUS_STRU g_UsbAudio_Status={0};
//static STREAM_FMT fmt = { 0 };
static UDC_AUD_StreamOut_CB_T UDC_AUD_StreamOut_callback = NULL;


/******************************************************************************
Description:
     USB Audio control interface endpoint request Handler - CS = Sample Freq Control
     get and set freq
Parameters:
    setup_req - setup request structure pointer
    msg -pointer of message for returning data and action
Return Value:
    Error code or No error
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VUDC_AUD_SampFreqCtrl(UOTGC_DEV_REQ *setup_req, VOTG_MSG *msg)
{
    VRET_VALUE ret = VRET_NO_ERROR;

    if((setup_req->bmRequestType & USB_CMD_DIRMASK) == USB_CMD_D2H_IN) //Get
    {
        VOTG_NPRINTF("VUDC_AUD_SampFreqCtrl Get req!!!\r\n");

        //data to be send back to PC
        msg->p_msg = (UINT32 *) dmatxbuf;
        msg->len = setup_req->wLength; //length should be 3 for SampFreq

        switch(setup_req->bRequest)
        {
          case AUDIO_REQUEST_GET_CUR:
          {
              dmatxbuf[0] = 0x00; //TODO: freq defines
            dmatxbuf[1] = 0xBB;
            dmatxbuf[2] = 0x80;
          }
          break;
          
          default:
            ret = VRET_DRV_USB_UNKCMD;
            VOTG_EPRINTF("VUDC_AUD_SampFreqCtrl unknown Get req!!!\r\n");
          break;
        }
    }
    else if((setup_req->bmRequestType & USB_CMD_DIRMASK) == USB_CMD_H2D_OUT) //Set
    {
        VOTG_NPRINTF("VUDC_AUD_SampFreqCtrl Set req!!!\r\n");
        switch(setup_req->bRequest)
        {
          case AUDIO_REQUEST_SET_CUR:
              //TODO: call function to set samp freq
          break;
          default:
            ret = VRET_DRV_USB_UNKCMD;
            VOTG_EPRINTF("VUDC_AUD_SampFreqCtrl unknown Set req!!!\r\n");
          break;
        }
    }
    else 
    {
        ret = VRET_DRV_USB_UNKCMD;
        VOTG_EPRINTF("VUDC_AUD_SampFreqCtrl unknown direction !!!\r\n");
    }

    return ret;

}

/******************************************************************************
Description:
     USB Audio control interface endpoint request Handler
Parameters:
    setup_req - setup request structure pointer
    msg - pointer of message for returning data and action
Return Value:
    Error code or No error
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VUDC_AUD_EpReqHdlr(UOTGC_DEV_REQ *setup_req, VOTG_MSG *msg)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    
    UINT8 bIdxH = setup_req->wIndex>>8;
    UINT8 bEndP = setup_req->wIndex;
    UINT8 bCS = setup_req->wValue>>8;
    UINT8 bValL = setup_req->wValue;

    if(bEndP == AUDIO_ID_EP_ISO_OUT_ENDP && bIdxH == 0) //high 8bit must be 0 for Endpoint request
    {
      //VOTG_NPRINTF("VUDC_AUD_EpReqHdlr EP %d req !\r\n", bEndP);
      
      if(bCS == AUDIO_SAMPLING_FREQ_CONTROL && bValL == 0) //low 8 bit must be 0
      {
        VUDC_AUD_SampFreqCtrl(setup_req, msg);
      }
      /*else if () //add more function here, e.g. pitch control
       {
       }*/
      else
      {
        ret = VRET_DRV_USB_UNKCMD;
        VOTG_EPRINTF("VUDC_AUD_EpReqHdlr unknown control select!!!\r\n");
      }
    }
    else
    {
      ret = VRET_DRV_USB_UNKCMD;
      VOTG_EPRINTF("VUDC_AUD_EpReqHdlr unknown endpoint 0x%x!!!\r\n", setup_req->wIndex);
    }

    return ret;

}

/******************************************************************************
Description:
     USB Audio control interface Interface request Handler - Feature Unit - CS = Vol Control
     used to set and get vol info
Parameters:
    setup_req - setup request structure pointer
    msg - pointer of message for returning data and action
Return Value:
    Error code or No error
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VUDC_AUD_FeatUnitVolCtrl(UOTGC_DEV_REQ *setup_req, VOTG_MSG *msg)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    static UINT8 tmpL,tmpH ;

    if((setup_req->bmRequestType & USB_CMD_DIRMASK) == USB_CMD_D2H_IN) //Get
    {
        VOTG_NPRINTF("AUD Vol Get %d %d\r\n",tmpH, tmpL);

        //data to be send back to PC
        msg->p_msg = (UINT32 *) dmatxbuf;
        msg->len = setup_req->wLength; //length should be 2 for vol

        switch(setup_req->bRequest)
        {
          case AUDIO_REQUEST_GET_CUR:
              dmatxbuf[0] = tmpL; // 
              dmatxbuf[1] = tmpH; 
          break;
          
          case AUDIO_REQUEST_GET_MIN:
              dmatxbuf[0] = 0x00;  
              dmatxbuf[1] = 0x00; 
          break;
          
          case AUDIO_REQUEST_GET_MAX:
              dmatxbuf[0] = 100;  
              dmatxbuf[1] = 0;//0x7F; 
          break;
          
          case AUDIO_REQUEST_GET_RES:
              dmatxbuf[0] = 0x01;  
              dmatxbuf[1] = 0x0; 
          break;
          
          default:
            ret = VRET_DRV_USB_UNKCMD;
            VOTG_EPRINTF("AUD Vol Ctrl unknown Get req!!!\r\n");
          break;
        }
    }
    else if((setup_req->bmRequestType & USB_CMD_DIRMASK) == USB_CMD_H2D_OUT) //Set
    {
        switch(setup_req->bRequest)
        {
          case AUDIO_REQUEST_SET_CUR:
              tmpL = dmabuf[8+0]; // 8 is the offset
            tmpH = dmabuf[8+1];
            VOTG_NPRINTF("AUD Vol SET_CUR %d %d\r\n", tmpH, tmpL);
              //call function to set vol
            if(Usb_device_hdl.device_sol->vsol_sendmsg){
                //Usb_device_hdl.device_sol->vsol_sendmsg(VMMI_MSG_USB_AUDIO_VOLUME,((tmpH<<8)|tmpL));
            }
          break;

          default:
            ret = VRET_DRV_USB_UNKCMD;
            VOTG_EPRINTF("AUD Vol Ctrl unknown Set req!!!\r\n");
          break;
        }
    }
    else 
    {
        ret = VRET_DRV_USB_UNKCMD;
        VOTG_EPRINTF("AUD Vol Ctrl unknown direction !!!\r\n");
    }

    return ret;

}

/******************************************************************************
Description:
     USB Audio control interface Interface request Handler - Feature Unit - CS = Mutel Control
     used to set and get mute info
Parameters:
    setup_req - setup request structure pointer
    msg - pointer of message for returning data and action
Return Value:
    Error code or No error
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VUDC_AUD_FeatUnitMuteCtrl(UOTGC_DEV_REQ *setup_req, VOTG_MSG *msg)
{
    VRET_VALUE ret = VRET_NO_ERROR;

    if((setup_req->bmRequestType & USB_CMD_DIRMASK) == USB_CMD_D2H_IN) //Get
    {
        //VOTG_NPRINTF("AUD Mute Get\r\n");

        //data to be send back to PC
        msg->p_msg = (UINT32 *) dmatxbuf;
        msg->len = setup_req->wLength; //length should be 1 for Mute 

        switch(setup_req->bRequest)
        {
          case AUDIO_REQUEST_GET_CUR:
              dmatxbuf[0] = FALSE;//TODO: always return FALSE for now, should call function to get info             
          break;
          
          default:
            ret = VRET_DRV_USB_UNKCMD;
            VOTG_EPRINTF("AUD Mute Ctrl unknown Get req!!!\r\n");
          break;
        }
    }
    else if((setup_req->bmRequestType & USB_CMD_DIRMASK) == USB_CMD_H2D_OUT) //Set
    {
        //VOTG_NPRINTF("AUD Mute Set\r\n");
        switch(setup_req->bRequest)
        {
          case AUDIO_REQUEST_SET_CUR:
              //call function to mute
            if(Usb_device_hdl.device_sol->vsol_sendmsg){
                //Usb_device_hdl.device_sol->vsol_sendmsg(VMMI_MSG_USB_AUDIO_MUTE,dmabuf[0]);
                }
          break;
          default:
            ret = VRET_DRV_USB_UNKCMD;
            VOTG_EPRINTF("AUD Mute Ctrl unknown Set req!!!\r\n");
          break;
        }
    }
    else 
    {
        ret = VRET_DRV_USB_UNKCMD;
        VOTG_EPRINTF("AUD Mute Ctrl unknown direction !!!\r\n");
    }

    return ret;

}

/******************************************************************************
Description:
     USB Audio control interface Interface request Handler - Feature Unit handler
Parameters:
    setup_req - setup request structure pointer
    msg - pointer of message for returning data and action
Return Value:
    Error code or No error
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VUDC_AUD_FeatUnitReqHdlr(UOTGC_DEV_REQ *setup_req, VOTG_MSG *msg)
{
    VRET_VALUE ret = VRET_NO_ERROR;

    UINT8 bCS = setup_req->wValue>>8;
    UINT8 bCN = setup_req->wValue;

    if(bCS == AUDIO_MUTE_CONTROL && bCN == 0) //TODO: define the channel number
    {
      //VOTG_NPRINTF("AUD FeatUnit Mute Control !\r\n");
      VUDC_AUD_FeatUnitMuteCtrl(setup_req, msg);
    }
    else if(bCS == AUDIO_VOLUME_CONTROL && bCN == 0) //more control select can be added here
    {
      //VOTG_NPRINTF("AUD FeatUnit Vol Control !\r\n");
      VUDC_AUD_FeatUnitVolCtrl(setup_req, msg);
    }
    else
    {
      ret = VRET_DRV_USB_UNKCMD;
      VOTG_EPRINTF("AUD FeatUnit unknown Control Select!!!\r\n");
    }

    return ret;
}

/******************************************************************************
Description:
     USB Audio control interface Interface request Handler
Parameters:
    setup_req - setup request structure pointer
    msg - pointer of message for returning data and action
Return Value:
    Error code or No error
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VUDC_AUD_IntfReqHdlr(UOTGC_DEV_REQ *setup_req, VOTG_MSG *msg)
{
    VRET_VALUE ret = VRET_NO_ERROR;
    UINT8 bIDh;
    UINT8 bIfl;

    bIDh = setup_req->wIndex>>8;
    bIfl = setup_req->wIndex;

    if((bIfl == AUDIO_ID_IF_CONTROL) && (bIDh == AUDIO_ID_ET_FEAT_UINT))//match feature unit
    {
      VUDC_AUD_FeatUnitReqHdlr(setup_req, msg);
    }
    /*else if((bIfl ==  ) && (bIDh == )) //add more entity or terminal here
    {
    }*/
    else if (bIfl == AUDIO_ID_IF_HID)
    {
        /* HID Report */
        /* 0x01 Get Report */
        /* 0x02 Get Idle */
        /* 0x03 Get Protocol */
        /* 0x09 Set Report */
        /* 0x0A Set Idle */
        /* 0x0B Set Protocol */
        //ignore HID class request
    }
    else
    {
      ret = VRET_DRV_USB_UNKCMD;
      VOTG_EPRINTF("AUD interface req Get unknown !!!\r\n");
    }

    return ret;
    
}

/******************************************************************************
Description:
     USB Audio Class Request handler
     parse the request and then call Endpoint or FeatureUnit handler to carry out the request
Parameters:
    setup_req - setup request structure pointer
    msg - pointer of message for returning data and action
Return Value:
    Error code or No error
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
VRET_VALUE VUDC_AUD_ClassReqHdlr(UOTGC_DEV_REQ *setup_req, VOTG_MSG *msg)
{
    VRET_VALUE ret = VRET_NO_ERROR;

    if((setup_req->bmRequestType & USB_CMD_RECIPMASK) == USB_CMD_RECIP_IF) //receip is interface
    {
      ret = VUDC_AUD_IntfReqHdlr(setup_req, msg);
    }
    else if((setup_req->bmRequestType & USB_CMD_RECIPMASK) == USB_CMD_RECIP_EP) //receip is endpoint
    {
      ret = VUDC_AUD_EpReqHdlr(setup_req, msg);
    }
    else 
    {
      ret = VRET_DRV_USB_UNKCMD;
      VOTG_EPRINTF("AUD class req Get unknown receip!!!\r\n");
    }
    
    return ret;
}

        



/******************************************************************************
Description:
      VUDC_AUD_MusicPlay
Parameters:
     None.
Return Value:
   None
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
void VUDC_AUD_MusicPlay(void)
{
    static UAC_PLAY_CONTROL g_UsbAudio_PlayControl = { 0, 0};
    VRET_VALUE ret = VRET_NO_ERROR;

    /*static UINT32 tt = 0;//dbg 
    if(tt++ %1000 == 0)
    {
        VUA_FPRINTF("[dbg][%s] usbaudio:%d\n\r",__func__,tt);
    }*/
#if  (USB_AUDIO_SELFTEST == 0)

    ASSERT(UDC_AUD_StreamOut_callback != NULL);
    if(UDC_AUD_StreamOut_callback == NULL)
    {
        VUDC_RX_ISO_EPx(AUDIO_ID_EP_ISO_OUT_ENDP,USB_PKT_SIZE,((UINT32)(&(dmabuf1[0]))),0);
        //g_UsbAudio_Status.lost++;
        static UINT32 t = 0;
        if(t++ %100 == 0)
        {VUA_FPRINTF("[dbg][%s] callback is null!!\n\r",__func__);}
        return;
    }
 
    if(g_UsbAudio_PlayControl.frame_ready == 0){
        VUDC_RX_ISO_EPx(AUDIO_ID_EP_ISO_OUT_ENDP,USB_PKT_SIZE,((UINT32)(&(dmabuf1[0]))),0);
        g_UsbAudio_PlayControl.frame_ready = 1;
    }else{
        if(g_UsbAudio_PlayControl.buff_switch == 0){
            //ret = UDC_AUD_StreamOut_callback((void*)&fmt, dmabuf1, USB_PKT_SIZE);
            //VUDC_RX_ISO_EPx(AUDIO_ID_EP_ISO_OUT_ENDP,USB_PKT_SIZE,((UINT32)(&(dmausbbuf2[0]))),0);
            g_UsbAudio_PlayControl.buff_switch = 1;
            }else{
           //ret = UDC_AUD_StreamOut_callback((void*)&fmt, dmausbbuf2, USB_PKT_SIZE);  
           VUDC_RX_ISO_EPx(AUDIO_ID_EP_ISO_OUT_ENDP,USB_PKT_SIZE,((UINT32)(&(dmabuf1[0]))),0); 
           g_UsbAudio_PlayControl.buff_switch = 0;
            }
        if(ret != 0)
        {
            VUA_FPRINTF("[dbg][%s] ret error:%d!!\n\r",__func__,ret);
            g_UsbAudio_Status.lost++;
            return;
        }
        g_UsbAudio_Status.total ++;
    }
    
#else
         VUDC_RX_ISO_EPx(AUDIO_ID_EP_ISO_OUT_ENDP,USB_PKT_SIZE,((UINT32)(&(dmabuf1[0]))),0);         
#endif    
        
}

#if (VDEV_USB_AUDIO_INTER_IPC > 0)

/* Private variables ---------------------------------------------------------*/

void VUDC_AUD_MusicStreamOutRegistor(UDC_AUD_StreamOut_CB_T cb) {
    VUA_NPRINTF("[dbg][%s]ok\r\n",__func__);
    UDC_AUD_StreamOut_callback = cb;
}


/******************************************************************************
Description:
      IPC Isr for USB audio when select internal ipc communication.
Parameters:
     None.
Return Value:
  0:  the interrupt is from internal ipc communication
  1:  the interrupt is from standard ipc communication
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
UINT8 VUDC_AUD_IPC_Isr(void)
{
    VUA_NPRINTF("[dbg][%s]should not run\r\n",__func__);
            return 0;
        }



/******************************************************************************
Description:
      Get the address of memory from BT core
Parameters:
     None.
Return Value:
   the address of memory
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
    

/******************************************************************************
Description:
      Send msg to BT core when audio data be copied to memory buffer.
Parameters:
     None.
Return Value:
   None
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/


/******************************************************************************
Description:
      VUDC_AUD_Init
Parameters:
     None.
Return Value:
   None
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
void VUDC_AUD_Init(void)
{        
    //VCPU_INT_SrcEnable(IRQ_MCUIPC_INT);
      #if  (USB_AUDIO_SELFTEST > 0)
      VOTG_SetUsbMode(UOTGC_MODE_DEVICE_AUDIO); 
      VOTG_MSG_Send(VOTG_MSG_DEV_START_UAC, NULL);
      #endif
    
    VUA_NPRINTF("[dbg][%s]USB Audio Init\r\n",__func__);
}



/******************************************************************************
Description:
      VUDC_AUD_Start
Parameters:
     None.
Return Value:
   None
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
void VUDC_AUD_Start(void)
{

    g_UsbAudio_Status.total     = 0;
    g_UsbAudio_Status.lost      = 0;
    g_UsbAudio_Status.int_num   = 0;
    g_UsbAudio_Status.int_lost  = 0;

    VOTG_Function_Set(UOTGC_MODE_DEVICE_AUDIO);     

    VUA_NPRINTF("[dbg][%s] start\r\n",__func__);
    
}

/******************************************************************************
Description:
      VUDC_AUD_Stop
Parameters:
     None.
Return Value:
   None
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
void VUDC_AUD_Stop(void)
{
    VOTG_Function_Set(UOTGC_MODE_DEVICE_SERIAL_COM_FIX);     
    VUA_NPRINTF("[dbg][%s] stop\r\n",__func__);
}

#else  //#if (VDEV_USB_AUDIO_INTER_IPC > 0)

static ipc_msg_t *g_UsbAudio_IpcMsg = NULL;

/******************************************************************************
Description:
      Get the address of memory from BT core
Parameters:
     None.
Return Value:
   the address of memory
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
static UINT32 VUDC_AUD_PopDmaBuf(void)
{
    api_msg_t *api_msg = NULL;
    if (g_UsbAudio_IpcMsg)
    {
        api_msg = (api_msg_t *)g_UsbAudio_IpcMsg->data;
        return (UINT32)(api_msg->data);
    }
        
    return 0xFFFFFFFF;
}

/******************************************************************************
Description:
      Send msg to BT core when audio data be copied to memory buffer.
Parameters:
     None.
Return Value:
   None
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
static void VUDC_AUD_PushDmaBuf(void)
{
    bt_usb_aduio_play((UINT8 *)g_UsbAudio_IpcMsg, USB_PKT_SIZE);
}

/******************************************************************************
Description:
      VUDC_AUD_Init
Parameters:
     None.
Return Value:
   None
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
void VUDC_AUD_Init(void)
{
      if(g_UsbAudio_IpcMsg == NULL){
        g_UsbAudio_IpcMsg = ipc_msg_new(NULL, MSG_HEAD_SIZE(api_msg_t) + USB_PKT_SIZE);
        if(g_UsbAudio_IpcMsg == NULL)
        {
            VCOMMON_WPRINTF("Create IpcMsg Instance failure!!!\r\n");
            return;
        }
        
        g_UsbAudio_IpcMsg->type = IPC_MSG_TYP_REQEVT;

        api_msg_t *api_msg = NULL;
        api_msg                 = (api_msg_t *)(g_UsbAudio_IpcMsg->data);
        api_msg->event          = EVENT_USER_USB_AUD_PLAY;
        api_msg->size           = USB_PKT_SIZE;    
      }
        
    VOTG_SetUsbMode(UOTGC_MODE_DEVICE_AUDIO);  
      #if  (USB_AUDIO_SELFTEST > 0)
      VOTG_MSG_Send(VOTG_MSG_DEV_START_UAC, NULL);
      #endif
    
    VUA_NPRINTF("[dbg][%s]ok\r\n",__func__);
}

/******************************************************************************
Description:
      VUDC_AUD_Start
Parameters:
     None.
Return Value:
   None
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
void VUDC_AUD_Start(void)
{
    VOTG_Function_Set(UOTGC_MODE_DEVICE_AUDIO);     
    VUA_NPRINTF("[dbg][%s]ok\r\n",__func__);
}

/******************************************************************************
Description:
      VUDC_AUD_Stop
Parameters:
     None.
Return Value:
   None
Remarks:
    None.
Example:
     None.
See Also:
    None.
*****************************************************************************/
void VUDC_AUD_Stop(void)
{
    VOTGC_CfgConnect(FALSE);    
    
    VUA_NPRINTF("[dbg][%s]ok\r\n",__func__);
}
#endif //#if (VDEV_USB_AUDIO_INTER_IPC > 0)
