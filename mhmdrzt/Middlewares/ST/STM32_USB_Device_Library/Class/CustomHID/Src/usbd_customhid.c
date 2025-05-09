/**
  ******************************************************************************
  * @file    usbd_customhid.c
  * @author  MCD Application Team
  * @brief   This file provides the CUSTOM_HID core functions.
  *
  * @verbatim
  *
  *          ===================================================================
  *                                CUSTOM_HID Class  Description
  *          ===================================================================
  *           This module manages the CUSTOM_HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (CUSTOM_HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - Usage Page : Generic Desktop
  *             - Usage : Vendor
  *             - Collection : Application
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
  *
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "usbd_customhid.h"
#include "usbd_ctlreq.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_CUSTOM_HID
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_CUSTOM_HID_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_CUSTOM_HID_Private_Macros
  * @{
  */
/**
  * @}
  */
/** @defgroup USBD_MOUSE_HID_Private_FunctionPrototypes
  * @{
  */

static uint8_t USBD_MOUSE_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_MOUSE_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_MOUSE_HID_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static uint8_t USBD_MOUSE_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_MOUSE_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_MOUSE_HID_EP0_RxReady(USBD_HandleTypeDef  *pdev);

static uint8_t *USBD_MOUSE_HID_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_MOUSE_HID_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_MOUSE_HID_GetOtherSpeedCfgDesc(uint16_t *length);
static uint8_t *USBD_MOUSE_HID_GetDeviceQualifierDesc(uint16_t *length);

/// *** MAMARZ *** ////
static uint8_t USBD_MAMARZ_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_MAMARZ_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_MAMARZ_HID_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static uint8_t USBD_MAMARZ_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_MAMARZ_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_MAMARZ_HID_EP0_RxReady(USBD_HandleTypeDef  *pdev);

static uint8_t *USBD_MAMARZ_HID_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_MAMARZ_HID_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_MAMARZ_HID_GetOtherSpeedCfgDesc(uint16_t *length);
static uint8_t *USBD_MAMARZ_HID_GetDeviceQualifierDesc(uint16_t *length);
/// *** MAMARZ *** ////


/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables
  * @{
  */

USBD_ClassTypeDef  USBD_HID_MOUSE =
{
  USBD_MOUSE_HID_Init,
  USBD_MOUSE_HID_DeInit,
  USBD_MOUSE_HID_Setup,
  NULL, /*EP0_TxSent*/
  USBD_MOUSE_HID_EP0_RxReady, /*EP0_RxReady*/ /* STATUS STAGE IN */
  USBD_MOUSE_HID_DataIn, /*DataIn*/
  USBD_MOUSE_HID_DataOut,
  NULL, /*SOF */
  NULL,
  NULL,
  USBD_MOUSE_HID_GetHSCfgDesc,
  USBD_MOUSE_HID_GetFSCfgDesc,
  USBD_MOUSE_HID_GetOtherSpeedCfgDesc,
  USBD_MOUSE_HID_GetDeviceQualifierDesc,
};

USBD_ClassTypeDef  USBD_MAMARZ_HID =
{
  USBD_MAMARZ_HID_Init,
  USBD_MAMARZ_HID_DeInit,
  USBD_MAMARZ_HID_Setup,
  NULL, /*EP0_TxSent*/
  USBD_MAMARZ_HID_EP0_RxReady, /*EP0_RxReady*/ /* STATUS STAGE IN */
  USBD_MAMARZ_HID_DataIn, /*DataIn*/
  USBD_MAMARZ_HID_DataOut,
  NULL, /*SOF */
  NULL,
  NULL,
  USBD_MAMARZ_HID_GetHSCfgDesc,
  USBD_MAMARZ_HID_GetFSCfgDesc,
  USBD_MAMARZ_HID_GetOtherSpeedCfgDesc,
  USBD_MAMARZ_HID_GetDeviceQualifierDesc,
};

#define USBD_HID_CUSTOM USBD_CUSTOM_HID

#define USBD_HID_0 USBD_HID_MOUSE
#define USBD_HID_1 USBD_HID_CUSTOM

USBD_ClassTypeDef *USBD_COMBINED_HID[]= 
{
  &USBD_HID_0,
  &USBD_HID_1,
  NULL
};

/* USB CUSTOM_HID device FS Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_MOUSE_HID_CfgFSDesc[USB_CUSTOM_HID_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09,                                               /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,                        /* bDescriptorType: Configuration */
  USB_CUSTOM_HID_CONFIG_DESC_SIZ,                     /* wTotalLength: Bytes returned */
  0x00,
  0x01,                                               /* bNumInterfaces: 1 interface */
  0x01,                                               /* bConfigurationValue: Configuration value */
  0x00,                                               /* iConfiguration: Index of string descriptor describing the configuration */
#if (USBD_SELF_POWERED == 1U)
  0xC0,                                               /* bmAttributes: Bus Powered according to user configuration */
#else
  0x80,                                               /* bmAttributes: Bus Powered according to user configuration */
#endif
  USBD_MAX_POWER,                                     /* MaxPower 100 mA: this current is used for detecting Vbus */

  /************** Descriptor of CUSTOM HID interface ****************/
  /* 09 */
  0x09,                                               /* bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,                            /* bDescriptorType: Interface descriptor type */
  0x00,                                               /* bInterfaceNumber: Number of Interface */
  0x00,                                               /* bAlternateSetting: Alternate setting */
  0x02,                                               /* bNumEndpoints*/
  0x03,                                               /* bInterfaceClass: CUSTOM_HID */
  0x00,                                               /* bInterfaceSubClass : 1=BOOT, 0=no boot */
  0x00,                                               /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
  0x00,                                               /* iInterface: Index of string descriptor */
  /******************** Descriptor of CUSTOM_HID *************************/
  /* 18 */
  0x09,                                               /* bLength: CUSTOM_HID Descriptor size */
  CUSTOM_HID_DESCRIPTOR_TYPE,                         /* bDescriptorType: CUSTOM_HID */
  0x11,                                               /* bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number */
  0x01,
  0x00,                                               /* bCountryCode: Hardware target country */
  0x01,                                               /* bNumDescriptors: Number of CUSTOM_HID class descriptors to follow */
  0x22,                                               /* bDescriptorType */
  USBD_CUSTOM_HID_REPORT_DESC_SIZE,                   /* wItemLength: Total length of Report descriptor */
  0x00,
  /******************** Descriptor of Custom HID endpoints ********************/
  /* 27 */
  0x07,                                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType: */

  CUSTOM_HID_EPIN_ADDR,                               /* bEndpointAddress: Endpoint Address (IN) */
  0x03,                                               /* bmAttributes: Interrupt endpoint */
  CUSTOM_HID_EPIN_SIZE,                               /* wMaxPacketSize: 2 Byte max */
  0x00,
  CUSTOM_HID_FS_BINTERVAL,                            /* bInterval: Polling Interval */
  /* 34 */

  0x07,                                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType: */
  CUSTOM_HID_EPOUT_ADDR,                              /* bEndpointAddress: Endpoint Address (OUT) */
  0x03,                                               /* bmAttributes: Interrupt endpoint */
  CUSTOM_HID_EPOUT_SIZE,                              /* wMaxPacketSize: 2 Bytes max  */
  0x00,
  CUSTOM_HID_FS_BINTERVAL,                            /* bInterval: Polling Interval */
  /* 41 */
};

/* USB CUSTOM_HID device HS Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_MOUSE_HID_CfgHSDesc[USB_CUSTOM_HID_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09,                                               /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,                        /* bDescriptorType: Configuration */
  USB_CUSTOM_HID_CONFIG_DESC_SIZ,                     /* wTotalLength: Bytes returned */
  0x00,
  0x01,                                               /* bNumInterfaces: 1 interface */
  0x01,                                               /* bConfigurationValue: Configuration value */
  0x00,                                               /* iConfiguration: Index of string descriptor describing the configuration */
#if (USBD_SELF_POWERED == 1U)
  0xC0,                                               /* bmAttributes: Bus Powered according to user configuration */
#else
  0x80,                                               /* bmAttributes: Bus Powered according to user configuration */
#endif
  USBD_MAX_POWER,                                     /* MaxPower 100 mA: this current is used for detecting Vbus */

  /************** Descriptor of CUSTOM HID interface ****************/
  /* 09 */
  0x09,                                               /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,                            /* bDescriptorType: Interface descriptor type */
  0x00,                                               /* bInterfaceNumber: Number of Interface */
  0x00,                                               /* bAlternateSetting: Alternate setting */
  0x02,                                               /* bNumEndpoints */
  0x03,                                               /* bInterfaceClass: CUSTOM_HID */
  0x00,                                               /* bInterfaceSubClass : 1=BOOT, 0=no boot */
  0x00,                                               /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
  0,                                                  /* iInterface: Index of string descriptor */
  /******************** Descriptor of CUSTOM_HID *************************/
  /* 18 */
  0x09,                                               /* bLength: CUSTOM_HID Descriptor size */
  CUSTOM_HID_DESCRIPTOR_TYPE,                         /* bDescriptorType: CUSTOM_HID */
  0x11,                                               /* bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number */
  0x01,
  0x00,                                               /* bCountryCode: Hardware target country */
  0x01,                                               /* bNumDescriptors: Number of CUSTOM_HID class descriptors to follow */
  0x22,                                               /* bDescriptorType */
  USBD_CUSTOM_HID_REPORT_DESC_SIZE,                   /* wItemLength: Total length of Report descriptor */
  0x00,
  /******************** Descriptor of Custom HID endpoints ********************/
  /* 27 */
  0x07,                                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType: */

  CUSTOM_HID_EPIN_ADDR,                               /* bEndpointAddress: Endpoint Address (IN) */
  0x03,                                               /* bmAttributes: Interrupt endpoint */
  CUSTOM_HID_EPIN_SIZE,                               /* wMaxPacketSize: 2 Byte max */
  0x00,
  CUSTOM_HID_HS_BINTERVAL,                            /* bInterval: Polling Interval */
  /* 34 */

  0x07,                                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType: */
  CUSTOM_HID_EPOUT_ADDR,                              /* bEndpointAddress: Endpoint Address (OUT) */
  0x03,                                               /* bmAttributes: Interrupt endpoint */
  CUSTOM_HID_EPOUT_SIZE,                              /* wMaxPacketSize: 2 Bytes max  */
  0x00,
  CUSTOM_HID_HS_BINTERVAL,                            /* bInterval: Polling Interval */
  /* 41 */
};

/* USB CUSTOM_HID device Other Speed Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_MOUSE_HID_OtherSpeedCfgDesc[USB_CUSTOM_HID_CONFIG_DESC_SIZ] __ALIGN_END =
{
  0x09,                                               /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,                        /* bDescriptorType: Configuration */
  USB_CUSTOM_HID_CONFIG_DESC_SIZ,                     /* wTotalLength: Bytes returned */
  0x00,
  0x01,                                               /* bNumInterfaces: 1 interface */
  0x01,                                               /* bConfigurationValue: Configuration value */
  0x00,                                               /* iConfiguration: Index of string descriptor describing the configuration */
#if (USBD_SELF_POWERED == 1U)
  0xC0,                                               /* bmAttributes: Bus Powered according to user configuration */
#else
  0x80,                                               /* bmAttributes: Bus Powered according to user configuration */
#endif
  USBD_MAX_POWER,                                     /* MaxPower 100 mA: this current is used for detecting Vbus */

  /************** Descriptor of CUSTOM HID interface ****************/
  /* 09 */
  0x09,                                               /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,                            /* bDescriptorType: Interface descriptor type */
  0x00,                                               /* bInterfaceNumber: Number of Interface */
  0x00,                                               /* bAlternateSetting: Alternate setting */
  0x02,                                               /* bNumEndpoints */
  0x03,                                               /* bInterfaceClass: CUSTOM_HID */
  0x00,                                               /* bInterfaceSubClass : 1=BOOT, 0=no boot */
  0x00,                                               /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
  0,                                                  /* iInterface: Index of string descriptor */
  /******************** Descriptor of CUSTOM_HID *************************/
  /* 18 */
  0x09,                                               /* bLength: CUSTOM_HID Descriptor size */
  CUSTOM_HID_DESCRIPTOR_TYPE,                         /* bDescriptorType: CUSTOM_HID */
  0x11,                                               /* bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number */
  0x01,
  0x00,                                               /* bCountryCode: Hardware target country */
  0x01,                                               /* bNumDescriptors: Number of CUSTOM_HID class descriptors to follow */
  0x22,                                               /* bDescriptorType */
  USBD_CUSTOM_HID_REPORT_DESC_SIZE,                   /* wItemLength: Total length of Report descriptor */
  0x00,
  /******************** Descriptor of Custom HID endpoints ********************/
  /* 27 */
  0x07,                                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType: */

  CUSTOM_HID_EPIN_ADDR,                               /* bEndpointAddress: Endpoint Address (IN) */
  0x03,                                               /* bmAttributes: Interrupt endpoint */
  CUSTOM_HID_EPIN_SIZE,                               /* wMaxPacketSize: 2 Bytes max */
  0x00,
  CUSTOM_HID_FS_BINTERVAL,                            /* bInterval: Polling Interval */
  /* 34 */

  0x07,                                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType: */
  CUSTOM_HID_EPOUT_ADDR,                              /* bEndpointAddress: Endpoint Address (OUT) */
  0x03,                                               /* bmAttributes: Interrupt endpoint */
  CUSTOM_HID_EPOUT_SIZE,                              /* wMaxPacketSize: 2 Bytes max */
  0x00,
  CUSTOM_HID_FS_BINTERVAL,                            /* bInterval: Polling Interval */
  /* 41 */
};

/* USB CUSTOM_HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_MOUSE_HID_Desc[USB_CUSTOM_HID_DESC_SIZ] __ALIGN_END =
{
  /* 18 */
  0x09,                                               /* bLength: CUSTOM_HID Descriptor size */
  CUSTOM_HID_DESCRIPTOR_TYPE,                         /* bDescriptorType: CUSTOM_HID */
  0x11,                                               /* bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number */
  0x01,
  0x00,                                               /* bCountryCode: Hardware target country */
  0x01,                                               /* bNumDescriptors: Number of CUSTOM_HID class descriptors to follow */
  0x22,                                               /* bDescriptorType */
  USBD_CUSTOM_HID_REPORT_DESC_SIZE,                   /* wItemLength: Total length of Report descriptor */
  0x00,
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_MOUSE_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Functions
  * @{
  */

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         Initialize the CUSTOM_HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_MOUSE_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_MOUSE_HID_HandleTypeDef *hhid;

  hhid = USBD_malloc(sizeof(USBD_MOUSE_HID_HandleTypeDef));

  if (hhid == NULL)
  {
    pdev->pClassData = NULL;
    return (uint8_t)USBD_EMEM;
  }

  pdev->pClassData = (void *)hhid;

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    pdev->ep_in[MOUSE_HID_EPIN_ADDR & 0xFU].bInterval = MOUSE_HID_HS_BINTERVAL;
    pdev->ep_out[MOUSE_HID_EPOUT_ADDR & 0xFU].bInterval = MOUSE_HID_HS_BINTERVAL;
  }
  else   /* LOW and FULL-speed endpoints */
  {
    pdev->ep_in[MOUSE_HID_EPIN_ADDR & 0xFU].bInterval = MOUSE_HID_FS_BINTERVAL;
    pdev->ep_out[MOUSE_HID_EPOUT_ADDR & 0xFU].bInterval = MOUSE_HID_FS_BINTERVAL;
  }

  /* Open EP IN */
  (void)USBD_LL_OpenEP(pdev, MOUSE_HID_EPIN_ADDR, USBD_EP_TYPE_INTR,
                       MOUSE_HID_EPIN_SIZE);

  pdev->ep_in[MOUSE_HID_EPIN_ADDR & 0xFU].is_used = 1U;

  /* Open EP OUT */
  (void)USBD_LL_OpenEP(pdev, MOUSE_HID_EPOUT_ADDR, USBD_EP_TYPE_INTR,
                       MOUSE_HID_EPOUT_SIZE);

  pdev->ep_out[MOUSE_HID_EPOUT_ADDR & 0xFU].is_used = 1U;

  hhid->state = MOUSE_HID_IDLE;

  ((USBD_MOUSE_HID_ItfTypeDef *)pdev->pUserData)->Init();

  /* Prepare Out endpoint to receive 1st packet */
  (void)USBD_LL_PrepareReceive(pdev, MOUSE_HID_EPOUT_ADDR, hhid->Report_buf,
                               USBD_MOUSE_HID_OUTREPORT_BUF_SIZE);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         DeInitialize the CUSTOM_HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_MOUSE_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);

  /* Close MOUSE_HID EP IN */
  (void)USBD_LL_CloseEP(pdev, MOUSE_HID_EPIN_ADDR);
  pdev->ep_in[MOUSE_HID_EPIN_ADDR & 0xFU].is_used = 0U;
  pdev->ep_in[MOUSE_HID_EPIN_ADDR & 0xFU].bInterval = 0U;

  /* Close MOUSE_HID EP OUT */
  (void)USBD_LL_CloseEP(pdev, MOUSE_HID_EPOUT_ADDR);
  pdev->ep_out[MOUSE_HID_EPOUT_ADDR & 0xFU].is_used = 0U;
  pdev->ep_out[MOUSE_HID_EPOUT_ADDR & 0xFU].bInterval = 0U;

  /* Free allocated memory */
  if (pdev->pClassData != NULL)
  {
    ((USBD_MOUSE_HID_ItfTypeDef *)pdev->pUserData)->DeInit();
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_Setup
  *         Handle the CUSTOM_HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_MOUSE_HID_Setup(USBD_HandleTypeDef *pdev,
                                     USBD_SetupReqTypedef *req)
{
  USBD_MOUSE_HID_HandleTypeDef *hhid = (USBD_MOUSE_HID_HandleTypeDef *)pdev->pClassData;
  uint16_t len = 0U;
  uint8_t  *pbuf = NULL;
  uint16_t status_info = 0U;
  USBD_StatusTypeDef ret = USBD_OK;

  if (hhid == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_CLASS:
      switch (req->bRequest)
      {
        case MOUSE_HID_REQ_SET_PROTOCOL:
          hhid->Protocol = (uint8_t)(req->wValue);
          break;

        case MOUSE_HID_REQ_GET_PROTOCOL:
          (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->Protocol, 1U);
          break;

        case MOUSE_HID_REQ_SET_IDLE:
          hhid->IdleState = (uint8_t)(req->wValue >> 8);
          break;

        case MOUSE_HID_REQ_GET_IDLE:
          (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->IdleState, 1U);
          break;

        case MOUSE_HID_REQ_SET_REPORT:
          hhid->IsReportAvailable = 1U;
          (void)USBD_CtlPrepareRx(pdev, hhid->Report_buf, req->wLength);
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_STATUS:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&status_info, 2U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_GET_DESCRIPTOR:
          if ((req->wValue >> 8) == MOUSE_HID_REPORT_DESC)
          {
            len = MIN(USBD_MOUSE_HID_REPORT_DESC_SIZE, req->wLength);
            pbuf = ((USBD_MOUSE_HID_ItfTypeDef *)pdev->pUserData)->pReport;
          }
          else
          {
            if ((req->wValue >> 8) == MOUSE_HID_DESCRIPTOR_TYPE)
            {
              pbuf = USBD_MOUSE_HID_Desc;
              len = MIN(USB_MOUSE_HID_DESC_SIZ, req->wLength);
            }
          }

          (void)USBD_CtlSendData(pdev, pbuf, len);
          break;

        case USB_REQ_GET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->AltSetting, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_SET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            hhid->AltSetting = (uint8_t)(req->wValue);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_CLEAR_FEATURE:
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }
  return (uint8_t)ret;
}

/**
  * @brief  USBD_MOUSE_HID_SendReport
  *         Send MOUSE_HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_MOUSE_HID_SendReport(USBD_HandleTypeDef *pdev,
                                   uint8_t *report, uint16_t len)
{
  USBD_MOUSE_HID_HandleTypeDef *hhid;

  if (pdev->pClassData == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hhid = (USBD_MOUSE_HID_HandleTypeDef *)pdev->pClassData;

  if (pdev->dev_state == USBD_STATE_CONFIGURED)
  {
    if (hhid->state == MOUSE_HID_IDLE)
    {
      hhid->state = CUSTOM_HID_BUSY;
      (void)USBD_LL_Transmit(pdev, MOUSE_HID_EPIN_ADDR, report, len);
    }
    else
    {
      return (uint8_t)USBD_BUSY;
    }
  }
  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_GetFSCfgDesc
  *         return FS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_MOUSE_HID_GetFSCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_MOUSE_HID_CfgFSDesc);

  return USBD_MOUSE_HID_CfgFSDesc;
}

/**
  * @brief  USBD_CUSTOM_HID_GetHSCfgDesc
  *         return HS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_MOUSE_HID_GetHSCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_MOUSE_HID_CfgHSDesc);

  return USBD_MOUSE_HID_CfgHSDesc;
}

/**
  * @brief  USBD_CUSTOM_HID_GetOtherSpeedCfgDesc
  *         return other speed configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_MOUSE_HID_GetOtherSpeedCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_MOUSE_HID_OtherSpeedCfgDesc);

  return USBD_MOUSE_HID_OtherSpeedCfgDesc;
}

/**
  * @brief  USBD_CUSTOM_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_MOUSE_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(epnum);

  /* Ensure that the FIFO is empty before a new transfer, this condition could
  be caused by  a new transfer before the end of the previous transfer */
  ((USBD_MOUSE_HID_HandleTypeDef *)pdev->pClassData)->state = MOUSE_HID_IDLE;

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_MOUSE_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(epnum);
  USBD_MOUSE_HID_HandleTypeDef *hhid;

  if (pdev->pClassData == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hhid = (USBD_MOUSE_HID_HandleTypeDef *)pdev->pClassData;

  /* USB data will be immediately processed, this allow next USB traffic being
  NAKed till the end of the application processing */
  ((USBD_MOUSE_HID_ItfTypeDef *)pdev->pUserData)->OutEvent(hhid->Report_buf[0],
                                                            hhid->Report_buf[1]);

  return (uint8_t)USBD_OK;
}


/**
  * @brief  USBD_CUSTOM_HID_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_MOUSE_HID_ReceivePacket(USBD_HandleTypeDef *pdev)
{
  USBD_MOUSE_HID_HandleTypeDef *hhid;

  if (pdev->pClassData == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hhid = (USBD_MOUSE_HID_HandleTypeDef *)pdev->pClassData;

  /* Resume USB Out process */
  (void)USBD_LL_PrepareReceive(pdev, MOUSE_HID_EPOUT_ADDR, hhid->Report_buf,
                               USBD_MOUSE_HID_OUTREPORT_BUF_SIZE);

  return (uint8_t)USBD_OK;
}


/**
  * @brief  USBD_CUSTOM_HID_EP0_RxReady
  *         Handles control request data.
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_MOUSE_HID_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_MOUSE_HID_HandleTypeDef *hhid = (USBD_MOUSE_HID_HandleTypeDef *)pdev->pClassData;

  if (hhid == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  if (hhid->IsReportAvailable == 1U)
  {
    ((USBD_MOUSE_HID_ItfTypeDef *)pdev->pUserData)->OutEvent(hhid->Report_buf[0],
                                                              hhid->Report_buf[1]);
    hhid->IsReportAvailable = 0U;
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  DeviceQualifierDescriptor
  *         return Device Qualifier descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_MOUSE_HID_GetDeviceQualifierDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_MOUSE_HID_DeviceQualifierDesc);

  return USBD_MOUSE_HID_DeviceQualifierDesc;
}

/// *** MAMARZ *** ////

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         Initialize the CUSTOM_HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
 static uint8_t USBD_MAMARZ_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
 {
   UNUSED(cfgidx);
   USBD_MAMARZ_HID_HandleTypeDef *hhid;
 
   hhid = USBD_malloc(sizeof(USBD_MAMARZ_HID_HandleTypeDef));
 
   if (hhid == NULL)
   {
     pdev->pClassData = NULL;
     return (uint8_t)USBD_EMEM;
   }
 
   pdev->pClassData = (void *)hhid;
 
   if (pdev->dev_speed == USBD_SPEED_HIGH)
   {
     pdev->ep_in[MAMARZ_HID_EPIN_ADDR & 0xFU].bInterval = MAMARZ_HID_HS_BINTERVAL;
     pdev->ep_out[MAMARZ_HID_EPOUT_ADDR & 0xFU].bInterval = MAMARZ_HID_HS_BINTERVAL;
   }
   else   /* LOW and FULL-speed endpoints */
   {
     pdev->ep_in[MAMARZ_HID_EPIN_ADDR & 0xFU].bInterval = MAMARZ_HID_FS_BINTERVAL;
     pdev->ep_out[MAMARZ_HID_EPOUT_ADDR & 0xFU].bInterval = MAMARZ_HID_FS_BINTERVAL;
   }
 
   /* Open EP IN */
   (void)USBD_LL_OpenEP(pdev, MAMARZ_HID_EPIN_ADDR, USBD_EP_TYPE_INTR,
                        MAMARZ_HID_EPIN_SIZE);
 
   pdev->ep_in[MAMARZ_HID_EPIN_ADDR & 0xFU].is_used = 1U;
 
   /* Open EP OUT */
   (void)USBD_LL_OpenEP(pdev, MAMARZ_HID_EPOUT_ADDR, USBD_EP_TYPE_INTR,
                        MAMARZ_HID_EPOUT_SIZE);
 
   pdev->ep_out[MAMARZ_HID_EPOUT_ADDR & 0xFU].is_used = 1U;
 
   hhid->state = MAMARZ_HID_IDLE;
 
   ((USBD_MAMARZ_HID_ItfTypeDef *)pdev->pUserData)->Init();
 
   /* Prepare Out endpoint to receive 1st packet */
   (void)USBD_LL_PrepareReceive(pdev, MAMARZ_HID_EPOUT_ADDR, hhid->Report_buf,
                                USBD_MAMARZ_HID_OUTREPORT_BUF_SIZE);
 
   return (uint8_t)USBD_OK;
 }
 
 /**
   * @brief  USBD_CUSTOM_HID_Init
   *         DeInitialize the CUSTOM_HID layer
   * @param  pdev: device instance
   * @param  cfgidx: Configuration index
   * @retval status
   */
 static uint8_t USBD_MAMARZ_HID_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
 {
   UNUSED(cfgidx);
 
   /* Close MAMARZ_HID EP IN */
   (void)USBD_LL_CloseEP(pdev, MAMARZ_HID_EPIN_ADDR);
   pdev->ep_in[MAMARZ_HID_EPIN_ADDR & 0xFU].is_used = 0U;
   pdev->ep_in[MAMARZ_HID_EPIN_ADDR & 0xFU].bInterval = 0U;
 
   /* Close MAMARZ_HID EP OUT */
   (void)USBD_LL_CloseEP(pdev, MAMARZ_HID_EPOUT_ADDR);
   pdev->ep_out[MAMARZ_HID_EPOUT_ADDR & 0xFU].is_used = 0U;
   pdev->ep_out[MAMARZ_HID_EPOUT_ADDR & 0xFU].bInterval = 0U;
 
   /* Free allocated memory */
   if (pdev->pClassData != NULL)
   {
     ((USBD_MAMARZ_HID_ItfTypeDef *)pdev->pUserData)->DeInit();
     USBD_free(pdev->pClassData);
     pdev->pClassData = NULL;
   }
 
   return (uint8_t)USBD_OK;
 }
 
 /**
   * @brief  USBD_CUSTOM_HID_Setup
   *         Handle the CUSTOM_HID specific requests
   * @param  pdev: instance
   * @param  req: usb requests
   * @retval status
   */
 static uint8_t USBD_MAMARZ_HID_Setup(USBD_HandleTypeDef *pdev,
                                      USBD_SetupReqTypedef *req)
 {
   USBD_MAMARZ_HID_HandleTypeDef *hhid = (USBD_MAMARZ_HID_HandleTypeDef *)pdev->pClassData;
   uint16_t len = 0U;
   uint8_t  *pbuf = NULL;
   uint16_t status_info = 0U;
   USBD_StatusTypeDef ret = USBD_OK;
 
   if (hhid == NULL)
   {
     return (uint8_t)USBD_FAIL;
   }
 
   switch (req->bmRequest & USB_REQ_TYPE_MASK)
   {
     case USB_REQ_TYPE_CLASS:
       switch (req->bRequest)
       {
         case MAMARZ_HID_REQ_SET_PROTOCOL:
           hhid->Protocol = (uint8_t)(req->wValue);
           break;
 
         case MAMARZ_HID_REQ_GET_PROTOCOL:
           (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->Protocol, 1U);
           break;
 
         case MAMARZ_HID_REQ_SET_IDLE:
           hhid->IdleState = (uint8_t)(req->wValue >> 8);
           break;
 
         case MAMARZ_HID_REQ_GET_IDLE:
           (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->IdleState, 1U);
           break;
 
         case MAMARZ_HID_REQ_SET_REPORT:
           hhid->IsReportAvailable = 1U;
           (void)USBD_CtlPrepareRx(pdev, hhid->Report_buf, req->wLength);
           break;
 
         default:
           USBD_CtlError(pdev, req);
           ret = USBD_FAIL;
           break;
       }
       break;
 
     case USB_REQ_TYPE_STANDARD:
       switch (req->bRequest)
       {
         case USB_REQ_GET_STATUS:
           if (pdev->dev_state == USBD_STATE_CONFIGURED)
           {
             (void)USBD_CtlSendData(pdev, (uint8_t *)&status_info, 2U);
           }
           else
           {
             USBD_CtlError(pdev, req);
             ret = USBD_FAIL;
           }
           break;
 
         case USB_REQ_GET_DESCRIPTOR:
           if ((req->wValue >> 8) == MAMARZ_HID_REPORT_DESC)
           {
             len = MIN(USBD_MAMARZ_HID_REPORT_DESC_SIZE, req->wLength);
             pbuf = ((USBD_MAMARZ_HID_ItfTypeDef *)pdev->pUserData)->pReport;
           }
           else
           {
             if ((req->wValue >> 8) == MAMARZ_HID_DESCRIPTOR_TYPE)
             {
               pbuf = USBD_MAMARZ_HID_Desc;
               len = MIN(USB_MAMARZ_HID_DESC_SIZ, req->wLength);
             }
           }
 
           (void)USBD_CtlSendData(pdev, pbuf, len);
           break;
 
         case USB_REQ_GET_INTERFACE:
           if (pdev->dev_state == USBD_STATE_CONFIGURED)
           {
             (void)USBD_CtlSendData(pdev, (uint8_t *)&hhid->AltSetting, 1U);
           }
           else
           {
             USBD_CtlError(pdev, req);
             ret = USBD_FAIL;
           }
           break;
 
         case USB_REQ_SET_INTERFACE:
           if (pdev->dev_state == USBD_STATE_CONFIGURED)
           {
             hhid->AltSetting = (uint8_t)(req->wValue);
           }
           else
           {
             USBD_CtlError(pdev, req);
             ret = USBD_FAIL;
           }
           break;
 
         case USB_REQ_CLEAR_FEATURE:
           break;
 
         default:
           USBD_CtlError(pdev, req);
           ret = USBD_FAIL;
           break;
       }
       break;
 
     default:
       USBD_CtlError(pdev, req);
       ret = USBD_FAIL;
       break;
   }
   return (uint8_t)ret;
 }
 
 /**
   * @brief  USBD_MOUSE_HID_SendReport
   *         Send MOUSE_HID Report
   * @param  pdev: device instance
   * @param  buff: pointer to report
   * @retval status
   */
 uint8_t USBD_MAMARZ_HID_SendReport(USBD_HandleTypeDef *pdev,
                                    uint8_t *report, uint16_t len)
 {
   USBD_MAMARZ_HID_HandleTypeDef *hhid;
 
   if (pdev->pClassData == NULL)
   {
     return (uint8_t)USBD_FAIL;
   }
 
   hhid = (USBD_MAMARZ_HID_HandleTypeDef *)pdev->pClassData;
 
   if (pdev->dev_state == USBD_STATE_CONFIGURED)
   {
     if (hhid->state == MAMARZ_HID_IDLE)
     {
       hhid->state = CUSTOM_HID_BUSY;
       (void)USBD_LL_Transmit(pdev, MAMARZ_HID_EPIN_ADDR, report, len);
     }
     else
     {
       return (uint8_t)USBD_BUSY;
     }
   }
   return (uint8_t)USBD_OK;
 }
 
 /**
   * @brief  USBD_CUSTOM_HID_GetFSCfgDesc
   *         return FS configuration descriptor
   * @param  speed : current device speed
   * @param  length : pointer data length
   * @retval pointer to descriptor buffer
   */
 static uint8_t *USBD_MAMARZ_HID_GetFSCfgDesc(uint16_t *length)
 {
   *length = (uint16_t)sizeof(USBD_MAMARZ_HID_CfgFSDesc);
 
   return USBD_MAMARZ_HID_CfgFSDesc;
 }
 
 /**
   * @brief  USBD_CUSTOM_HID_GetHSCfgDesc
   *         return HS configuration descriptor
   * @param  speed : current device speed
   * @param  length : pointer data length
   * @retval pointer to descriptor buffer
   */
 static uint8_t *USBD_MAMARZ_HID_GetHSCfgDesc(uint16_t *length)
 {
   *length = (uint16_t)sizeof(USBD_MAMARZ_HID_CfgHSDesc);
 
   return USBD_MAMARZ_HID_CfgHSDesc;
 }
 
 /**
   * @brief  USBD_CUSTOM_HID_GetOtherSpeedCfgDesc
   *         return other speed configuration descriptor
   * @param  speed : current device speed
   * @param  length : pointer data length
   * @retval pointer to descriptor buffer
   */
 static uint8_t *USBD_MAMARZ_HID_GetOtherSpeedCfgDesc(uint16_t *length)
 {
   *length = (uint16_t)sizeof(USBD_MAMARZ_HID_OtherSpeedCfgDesc);
 
   return USBD_MAMARZ_HID_OtherSpeedCfgDesc;
 }
 
 /**
   * @brief  USBD_CUSTOM_HID_DataIn
   *         handle data IN Stage
   * @param  pdev: device instance
   * @param  epnum: endpoint index
   * @retval status
   */
 static uint8_t USBD_MAMARZ_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
 {
   UNUSED(epnum);
 
   /* Ensure that the FIFO is empty before a new transfer, this condition could
   be caused by  a new transfer before the end of the previous transfer */
   ((USBD_MAMARZ_HID_HandleTypeDef *)pdev->pClassData)->state = MAMARZ_HID_IDLE;
 
   return (uint8_t)USBD_OK;
 }
 
 /**
   * @brief  USBD_CUSTOM_HID_DataOut
   *         handle data OUT Stage
   * @param  pdev: device instance
   * @param  epnum: endpoint index
   * @retval status
   */
 static uint8_t USBD_MAMARZ_HID_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
 {
   UNUSED(epnum);
   USBD_MAMARZ_HID_HandleTypeDef *hhid;
 
   if (pdev->pClassData == NULL)
   {
     return (uint8_t)USBD_FAIL;
   }
 
   hhid = (USBD_MAMARZ_HID_HandleTypeDef *)pdev->pClassData;
 
   /* USB data will be immediately processed, this allow next USB traffic being
   NAKed till the end of the application processing */
   ((USBD_MAMARZ_HID_ItfTypeDef *)pdev->pUserData)->OutEvent(hhid->Report_buf[0],
                                                             hhid->Report_buf[1]);
 
   return (uint8_t)USBD_OK;
 }
 
 
 /**
   * @brief  USBD_CUSTOM_HID_ReceivePacket
   *         prepare OUT Endpoint for reception
   * @param  pdev: device instance
   * @retval status
   */
 uint8_t USBD_MAMARZ_HID_ReceivePacket(USBD_HandleTypeDef *pdev)
 {
   USBD_MAMARZ_HID_HandleTypeDef *hhid;
 
   if (pdev->pClassData == NULL)
   {
     return (uint8_t)USBD_FAIL;
   }
 
   hhid = (USBD_MAMARZ_HID_HandleTypeDef *)pdev->pClassData;
 
   /* Resume USB Out process */
   (void)USBD_LL_PrepareReceive(pdev, MAMARZ_HID_EPOUT_ADDR, hhid->Report_buf,
                                USBD_MAMARZ_HID_OUTREPORT_BUF_SIZE);
 
   return (uint8_t)USBD_OK;
 }
 
 
 /**
   * @brief  USBD_CUSTOM_HID_EP0_RxReady
   *         Handles control request data.
   * @param  pdev: device instance
   * @retval status
   */
 static uint8_t USBD_MAMARZ_HID_EP0_RxReady(USBD_HandleTypeDef *pdev)
 {
   USBD_MAMARZ_HID_HandleTypeDef *hhid = (USBD_MAMARZ_HID_HandleTypeDef *)pdev->pClassData;
 
   if (hhid == NULL)
   {
     return (uint8_t)USBD_FAIL;
   }
 
   if (hhid->IsReportAvailable == 1U)
   {
     ((USBD_MAMARZ_HID_ItfTypeDef *)pdev->pUserData)->OutEvent(hhid->Report_buf[0],
                                                               hhid->Report_buf[1]);
     hhid->IsReportAvailable = 0U;
   }
 
   return (uint8_t)USBD_OK;
 }
 
 /**
   * @brief  DeviceQualifierDescriptor
   *         return Device Qualifier descriptor
   * @param  length : pointer data length
   * @retval pointer to descriptor buffer
   */
 static uint8_t *USBD_MAMARZ_HID_GetDeviceQualifierDesc(uint16_t *length)
 {
   *length = (uint16_t)sizeof(USBD_MAMARZ_HID_DeviceQualifierDesc);
 
   return USBD_MAMARZ_HID_DeviceQualifierDesc;
 }
 

/// *** MAMARZ *** ////

/**
  * @brief  USBD_CUSTOM_HID_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CUSTOMHID Interface callback
  * @retval status
  */
uint8_t USBD_CUSTOM_HID_RegisterInterface(USBD_HandleTypeDef *pdev,
                                          USBD_CUSTOM_HID_ItfTypeDef *fops)
{
  if (fops == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  pdev->pUserData = fops;

  return (uint8_t)USBD_OK;
}
/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
