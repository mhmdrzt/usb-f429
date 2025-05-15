/* usbd_custom_hid.h */
#ifndef __USBD_CUSTOM_HID_H
#define __USBD_CUSTOM_HID_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_def.h"
extern uint8_t Custom_HID_ReportDesc[];
#define CUSTOM_HID_REPORT_DESC_SIZE    29//sizeof(Custom_HID_ReportDesc)

uint8_t USBD_CustomHID_Init(USBD_HandleTypeDef *pdev);
uint8_t USBD_CustomHID_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
uint8_t USBD_CustomHID_DataOut(USBD_HandleTypeDef *pdev);

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CUSTOM_HID_H */
