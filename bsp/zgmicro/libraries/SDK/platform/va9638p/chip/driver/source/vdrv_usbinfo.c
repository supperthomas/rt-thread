#define ALLOW_VKERNEL_INTERNAL_INCLUDES
#include "vudc.h"
#include "vudc_serial.h"
//device ino
//device solution
DEVICE_SOL  usb_device_sol = {
    NULL,
};

//device audio
DEVICE_AUDIO_HDL usb_audio_device = {
    NULL,
};

//device msd
DEVICE_MSD_HDL usb_msd_device = {
    NULL,
    NULL,
    NULL,
    NULL,
};

DEVICE_SERIAL_HDL usb_serial_device = {
    VUDC_Serial_DataIn,
    VUDC_Serial_DataOut,
};

const USB_DEVICE_HDL Usb_device_hdl = {&usb_device_sol, &usb_audio_device, &usb_msd_device, &usb_serial_device};


