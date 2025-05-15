/* Src/usbd_custom_hid.c */
#include "usbd_custom_hid.h"
#include "usbd_def.h"

__ALIGN_BEGIN uint8_t Custom_HID_ReportDesc[] __ALIGN_END = {
  0x06, 0x00, 0xFF,  // Usage Page (Vendor Defined 0xFF00)
  0x09, 0x01,        // Usage (Vendor Usage 1)
  0xA1, 0x01,        // Collection (Application)
	0x85, 0x02,       //   << REPORT ID 1
    0x15, 0x00,      //   Logical Minimum (0)
    0x26, 0xFF, 0x00,//   Logical Maximum (255)
    0x75, 0x08,      //   Report Size (8)
    0x95, 0x08,      //   Report Count (8)
    0x09, 0x01,      //   Usage (Vendor Usage 1) 
    0x81, 0x00,      //   Input (Data, Array)
    0x95, 0x08,      //   Report Count (8)
    0x09, 0x01,      //   Usage (Vendor Usage 1)
    0x91, 0x00,      //   Output (Data, Array)
  0xC0               // End Collection
};

static uint8_t CustomHIDRxBuffer[9];

uint8_t* USBD_CustomHID_GetReportDescriptor(uint16_t* length)
{
    *length = CUSTOM_HID_REPORT_DESC_SIZE;
    return (uint8_t*)Custom_HID_ReportDesc;
}
uint8_t USBD_CustomHID_Init(USBD_HandleTypeDef *pdev)
{
    /* Open IN endpoint 0x82 and OUT endpoint 0x02 for the custom HID */
    USBD_LL_OpenEP(pdev, 0x82, USBD_EP_TYPE_INTR, 9);   // IN endpoint
    USBD_LL_OpenEP(pdev, 0x02, USBD_EP_TYPE_INTR, 9);   // OUT endpoint
		USBD_LL_PrepareReceive(pdev, 0x02, CustomHIDRxBuffer, sizeof(CustomHIDRxBuffer));

    return USBD_OK;
}

uint8_t USBD_CustomHID_DataOut(USBD_HandleTypeDef *pdev)
{
        // پردازش دیتای دریافتی از CustomHIDRxBuffer (طول 9 بایت)
        // مثلاً: uint8_t data = CustomHIDRxBuffer[1]; (0 index همون Report ID ـه)

        USBD_LL_PrepareReceive(pdev, 0x02, CustomHIDRxBuffer, sizeof(CustomHIDRxBuffer));


    return USBD_OK;
}

uint8_t USBD_CustomHID_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    /* Handle custom HID class-specific requests (e.g. GET_REPORT, SET_REPORT, etc.) */
    return USBD_OK;
}
