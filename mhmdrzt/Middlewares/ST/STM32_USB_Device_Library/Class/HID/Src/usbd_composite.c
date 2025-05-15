/* Src/usbd_composite.c */
#include "usbd_composite.h"
#include "usbd_def.h"
#include "usbd_hid_mouse.h"
#include "usbd_custom_hid.h"
#include "usbd_ioreq.h"


#include "usbd_desc.h"

/* Forward declarations of composite class callbacks */
static uint8_t Composite_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t Composite_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t Composite_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t Composite_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t Composite_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t Composite_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t* USBD_Composite_GetFSConfigDescriptor(uint16_t *length)
{
    *length = USBD_Composite_CfgDescSize;
    return USBD_Composite_CfgDesc;
}

/* Composite Class callbacks structure */
USBD_ClassTypeDef USBD_Composite =
{
  Composite_Init,
  Composite_DeInit,
  Composite_Setup,
  NULL,                       /* EP0_TxSent */
  Composite_EP0_RxReady,
  Composite_DataIn,
  Composite_DataOut,
  NULL,                       /* SOF */
  NULL,                       /* IsoINIncomplete */
  NULL,                       /* IsoOUTIncomplete */
  NULL,                       /* GetHSConfigDescriptor (not used for FS) */
  USBD_Composite_GetFSConfigDescriptor,
  NULL,                       /* GetOtherSpeedConfigDescriptor */
  NULL                        /* GetDeviceQualifierDescriptor */
};

/* Composite_Init: Initialize both HID interfaces */
static uint8_t Composite_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    uint8_t ret_mouse, ret_custom;
    ret_mouse = USBD_HID_MOUSE_Init(pdev);
    ret_custom = USBD_CustomHID_Init(pdev);
    
    if ((ret_mouse == USBD_OK) && (ret_custom == USBD_OK))
    {
        return USBD_OK;
    }
    else
    {
        return USBD_FAIL;
    }
}

/* Composite_DeInit: Deinitialize both HID interfaces */
static uint8_t Composite_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    /* Add per-interface deinitialization if necessary. */
    return USBD_OK;
}

/* Composite_Setup: Dispatch class-specific requests based on request type and interface (wIndex) */
static uint8_t Composite_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    uint8_t ret = USBD_OK;

    /* Handle class requests only */
    if ((req->bmRequest & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS)
    {
        if(req->wIndex == 0)
        {
            ret = USBD_HID_MOUSE_Setup(pdev, req);
        }
        else if(req->wIndex == 1)
        {
            ret = USBD_CustomHID_Setup(pdev, req);
        }
        else
        {
            ret = USBD_FAIL; // Unknown interface
        }
    }
    else if ((req->bmRequest & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD)
    {
			uint16_t len;
			uint8_t *pbuf;
        if ((req->wValue >> 8) == 0x22U)
				{
					if(req->wIndex == 0) {
						len = MIN(HID_MOUSE_REPORT_DESC_SIZE, req->wLength);
						pbuf = HID_Mouse_ReportDesc;
					} else if (req->wIndex == 1) {
						len = MIN( CUSTOM_HID_REPORT_DESC_SIZE, req->wLength);
						pbuf = Custom_HID_ReportDesc;
					} else {
						ret = USBD_FAIL; // Unknown interface
					}
					
				}
				else if ((req->wValue >> 8) == 0x21U)
				{
					pbuf = USBD_Composite_CfgDesc;
					len = MIN(USBD_Composite_CfgDescSize, req->wLength);
				}
				else {
					/* Handle in ctlreq.c */
					ret = USBD_OK;
				}
				if (pbuf != NULL && len >= 1) {
					(void)USBD_CtlSendData(pdev, pbuf, len);
				}
    }
    else
    {
        ret = USBD_FAIL;
    }
    return ret;
}

/* Composite_DataIn: Handle data IN events by endpoint number */
static uint8_t Composite_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    /* Dispatch based on endpoint number if needed.
       For example:
       if(epnum == 0x81) { return USBD_HID_MOUSE_DataIn(pdev, epnum); }
       else if(epnum == 0x82) { return USBD_CustomHID_DataIn(pdev, epnum); }
    */
    return USBD_OK;
}

/* Composite_DataOut: Handle data OUT events by endpoint number */
static uint8_t Composite_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
//    Dispatch if your custom HID OUT endpoint (e.g., address 0x02)
//       is used for receiving data.
//       For example:
       if(epnum == 0x02) { return USBD_CustomHID_DataOut(pdev); }
    
    return USBD_OK;
}

/* Composite_EP0_RxReady: Endpoint 0 Rx Ready callback */
static uint8_t Composite_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
    return USBD_OK;
}
