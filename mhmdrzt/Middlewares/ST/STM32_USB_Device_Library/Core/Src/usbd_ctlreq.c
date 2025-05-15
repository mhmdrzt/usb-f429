/* Src/usbd_ctlreq.c */
#include "usbd_ctlreq.h"
#include "usbd_ioreq.h"

/* Private function prototypes */
static void USBD_GetDescriptor(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void USBD_SetAddress(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static USBD_StatusTypeDef USBD_SetConfig(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void USBD_GetConfig(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void USBD_GetStatus(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void USBD_SetFeature(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void USBD_ClrFeature(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_GetLen(uint8_t *buf);

/**
  * @brief  USBD_CtlReq handles control requests by dispatching to standard or class-
  *         specific handlers.
  * @param  pdev: device instance
  * @param  req: setup request pointer
  * @retval USBD_StatusTypeDef status
  */
USBD_StatusTypeDef USBD_CtlReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_StatusTypeDef ret = USBD_OK;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_STANDARD:
      ret = USBD_StdDevReq(pdev, req);
      break;
    case USB_REQ_TYPE_CLASS:
      /* Dispatch class-specific requests to the class setup callback */
      ret = (USBD_StatusTypeDef)pdev->pClass->Setup(pdev, req);
      break;
    case USB_REQ_TYPE_VENDOR:
      /* For vendor-specific requests, you might add a vendor handler here */
      ret = USBD_FAIL;
      break;
    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }
  return ret;
}

/**
  * @brief  USBD_StdDevReq handles standard USB device requests such as 
  *         GET_DESCRIPTOR, SET_ADDRESS, SET_CONFIGURATION, etc.
  * @param  pdev: device instance
  * @param  req: setup request pointer
  * @retval USBD_StatusTypeDef status
  */
USBD_StatusTypeDef USBD_StdDevReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_StatusTypeDef ret = USBD_OK;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_DESCRIPTOR:
          USBD_GetDescriptor(pdev, req);
          break;

        case USB_REQ_SET_ADDRESS:
          USBD_SetAddress(pdev, req);
          break;

        case USB_REQ_SET_CONFIGURATION:
          ret = USBD_SetConfig(pdev, req);
          break;

        case USB_REQ_GET_CONFIGURATION:
          USBD_GetConfig(pdev, req);
          break;

        case USB_REQ_GET_STATUS:
          USBD_GetStatus(pdev, req);
          break;

        case USB_REQ_SET_FEATURE:
          USBD_SetFeature(pdev, req);
          break;

        case USB_REQ_CLEAR_FEATURE:
          USBD_ClrFeature(pdev, req);
          break;

        default:
          USBD_CtlError(pdev, req);
          break;
      }
      break;

    case USB_REQ_TYPE_CLASS:
    case USB_REQ_TYPE_VENDOR:
      ret = (USBD_StatusTypeDef)pdev->pClass->Setup(pdev, req);
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }

  return ret;
}

/**
  * @brief  USBD_StdItfReq handles standard USB interface requests.
  *         (Typically, these are forwarded to the class Setup callback.)
  * @param  pdev: device instance
  * @param  req: setup request pointer
  * @retval USBD_StatusTypeDef status
  */
USBD_StatusTypeDef USBD_StdItfReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  if ((LOBYTE(req->wIndex)) < USBD_MAX_NUM_INTERFACES)
  {
    return (USBD_StatusTypeDef)pdev->pClass->Setup(pdev, req);
  }
  else
  {
    USBD_CtlError(pdev, req);
    return USBD_FAIL;
  }
}

/**
  * @brief  USBD_StdEPReq handles standard endpoint requests.
  *         (e.g., SET_FEATURE / CLEAR_FEATURE on endpoints.)
  * @param  pdev: device instance
  * @param  req: setup request pointer
  * @retval USBD_StatusTypeDef status
  */
USBD_StatusTypeDef USBD_StdEPReq(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  uint8_t ep_addr = LOBYTE(req->wIndex);
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_SET_FEATURE:
          if ((pdev->dev_state == USBD_STATE_CONFIGURED) && 
              (req->wValue == USB_FEATURE_EP_HALT))
          {
            if ((ep_addr != 0x00U) && (ep_addr != 0x80U))
            {
              (void)USBD_LL_StallEP(pdev, ep_addr);
            }
            USBD_CtlSendStatus(pdev);
          }
          break;
        case USB_REQ_CLEAR_FEATURE:
          if ((pdev->dev_state == USBD_STATE_CONFIGURED) && 
              (req->wValue == USB_FEATURE_EP_HALT))
          {
            if ((ep_addr != 0x00U) && (ep_addr != 0x80U))
            {
              (void)USBD_LL_ClearStallEP(pdev, ep_addr);
            }
            USBD_CtlSendStatus(pdev);
          }
          break;
        default:
          USBD_CtlError(pdev, req);
          break;
      }
      break;
    case USB_REQ_TYPE_CLASS:
    case USB_REQ_TYPE_VENDOR:
      return (USBD_StatusTypeDef)pdev->pClass->Setup(pdev, req);
    default:
      USBD_CtlError(pdev, req);
      break;
  }
  return USBD_OK;
}

/* Helper function to compute string length */
static uint8_t USBD_GetLen(uint8_t *buf)
{
  uint8_t len = 0;
  while(buf[len] != '\0') { len++; }
  return len;
}

/* USB standard request handlers below */

/**
  * @brief  USBD_GetDescriptor handles GET_DESCRIPTOR requests.
  * @param  pdev: device instance
  * @param  req: setup request pointer
  * @retval None
  */


static void USBD_GetDescriptor(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  uint16_t len = 0;
  uint8_t *pbuf = NULL;
  uint8_t err = 0U;

  switch (req->wValue >> 8)
  {
#if ((USBD_LPM_ENABLED == 1U) || (USBD_CLASS_BOS_ENABLED == 1U))
    case USB_DESC_TYPE_BOS:
      if (pdev->pDesc->GetBOSDescriptor != NULL)
      {
        pbuf = pdev->pDesc->GetBOSDescriptor(pdev->dev_speed, &len);
      }
      else
      {
        USBD_CtlError(pdev, req);
        err++;
      }
      break;
#endif
    case USB_DESC_TYPE_DEVICE:
      pbuf = pdev->pDesc->GetDeviceDescriptor(pdev->dev_speed, &len);
      break;

    case USB_DESC_TYPE_CONFIGURATION:
      if (pdev->dev_speed == USBD_SPEED_HIGH)
      {
        if (pdev->pClass->GetHSConfigDescriptor != NULL)
        {
          pbuf = pdev->pClass->GetHSConfigDescriptor(&len);
        }
      }
      else
      {
        if (pdev->pClass->GetFSConfigDescriptor != NULL)
        {
          pbuf = pdev->pClass->GetFSConfigDescriptor(&len);
        }
      }
      if(pbuf != NULL)
      {
        pbuf[1] = USB_DESC_TYPE_CONFIGURATION;
      }
      break;

    case USB_DESC_TYPE_STRING:
      switch ((uint8_t)(req->wValue))
      {
        case USBD_IDX_LANGID_STR:
          if (pdev->pDesc->GetLangIDStrDescriptor != NULL)
          {
            pbuf = pdev->pDesc->GetLangIDStrDescriptor(pdev->dev_speed, &len);
          }
          else
          {
            USBD_CtlError(pdev, req);
            err++;
          }
          break;

        case USBD_IDX_MFC_STR:
          if (pdev->pDesc->GetManufacturerStrDescriptor != NULL)
          {
            pbuf = pdev->pDesc->GetManufacturerStrDescriptor(pdev->dev_speed, &len);
          }
          else
          {
            USBD_CtlError(pdev, req);
            err++;
          }
          break;

        case USBD_IDX_PRODUCT_STR:
          if (pdev->pDesc->GetProductStrDescriptor != NULL)
          {
            pbuf = pdev->pDesc->GetProductStrDescriptor(pdev->dev_speed, &len);
          }
          else
          {
            USBD_CtlError(pdev, req);
            err++;
          }
          break;

        case USBD_IDX_SERIAL_STR:
          if (pdev->pDesc->GetSerialStrDescriptor != NULL)
          {
            pbuf = pdev->pDesc->GetSerialStrDescriptor(pdev->dev_speed, &len);
          }
          else
          {
            USBD_CtlError(pdev, req);
            err++;
          }
          break;

//        case USBD_IDX_CONFIG_STR:
//          if (pdev->pDesc->GetConfigurationStrDescriptor != NULL)
//          {
//            pbuf = pdev->pDesc->GetConfigurationStrDescriptor(pdev->dev_speed, &len);
//          }
//          else
//          {
//            USBD_CtlError(pdev, req);
//            err++;
//          }
//          break;

//        case USBD_IDX_INTERFACE_STR:
//          if (pdev->pDesc->GetInterfaceStrDescriptor != NULL)
//          {
//            pbuf = pdev->pDesc->GetInterfaceStrDescriptor(pdev->dev_speed, &len, );
//          }
//          else
//          {
//            USBD_CtlError(pdev, req);
//            err++;
//          }
//          break;
				
				default:
					if ((req->wValue >> 8) == USB_DESC_TYPE_STRING)
					{
						uint8_t str_index = (uint8_t)(req->wValue & 0xFF);

						if (pdev->pDesc->GetInterfaceStrDescriptor != NULL &&
								(str_index == 4 || str_index == 5))
						{
							pbuf = pdev->pDesc->GetInterfaceStrDescriptor(pdev->dev_speed, &len, str_index);
						}
				#if (USBD_SUPPORT_USER_STRING_DESC == 1U)
						else if (pdev->pClass->GetUsrStrDescriptor != NULL)
						{
							pbuf = pdev->pClass->GetUsrStrDescriptor(pdev, str_index, &len);
						}
				#endif
						else
						{
							USBD_CtlError(pdev, req);
							err++;
						}
					}
					else
					{
						USBD_CtlError(pdev, req);
						err++;
					}
					break;
        
      }
      break;

    case USB_DESC_TYPE_DEVICE_QUALIFIER:
      if (pdev->dev_speed == USBD_SPEED_HIGH)
      {
        //pbuf = pdev->pDesc->GetDeviceQualifierDescriptor(pdev->dev_speed, &len);
      }
      else
      {
        USBD_CtlError(pdev, req);
        err++;
      }
      break;

    case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
      if (pdev->dev_speed == USBD_SPEED_HIGH)
      {
        if (pdev->pClass->GetOtherSpeedConfigDescriptor != NULL)
        {
          pbuf = pdev->pClass->GetOtherSpeedConfigDescriptor(&len);
          pbuf[1] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
        }
      }
      else
      {
        USBD_CtlError(pdev, req);
        err++;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      err++;
      break;
  }

  if (err != 0U)
  {
    return;
  }

  if (req->wLength != 0U)
  {
    if (len != 0U)
    {
      len = MIN(len, req->wLength);
      USBD_CtlSendData(pdev, pbuf, len);
    }
    else
    {
      USBD_CtlError(pdev, req);
    }
  }
  else
  {
    USBD_CtlSendStatus(pdev);
  }
}

/**
  * @brief  USBD_SetAddress sets the device address.
  * @param  pdev: device instance
  * @param  req: setup request pointer
  * @retval None
  */
static void USBD_SetAddress(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  uint8_t dev_addr;

  if ((req->wIndex == 0U) && (req->wLength == 0U) && (req->wValue < 128U))
  {
    dev_addr = (uint8_t)(req->wValue) & 0x7FU;

    if (pdev->dev_state == USBD_STATE_CONFIGURED)
    {
      USBD_CtlError(pdev, req);
    }
    else
    {
      pdev->dev_address = dev_addr;
      USBD_LL_SetUSBAddress(pdev, dev_addr);
      USBD_CtlSendStatus(pdev);

      if (dev_addr != 0U)
      {
        pdev->dev_state = USBD_STATE_ADDRESSED;
      }
      else
      {
        pdev->dev_state = USBD_STATE_DEFAULT;
      }
    }
  }
  else
  {
    USBD_CtlError(pdev, req);
  }
}

/**
  * @brief  USBD_SetConfig handles SET_CONFIGURATION request.
  * @param  pdev: device instance
  * @param  req: setup request pointer
  * @retval USBD_StatusTypeDef status
  */
static USBD_StatusTypeDef USBD_SetConfig(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_StatusTypeDef ret = USBD_OK;
  uint8_t cfgidx = (uint8_t)(req->wValue);

  if (cfgidx > USBD_MAX_NUM_CONFIGURATION)
  {
    USBD_CtlError(pdev, req);
    return USBD_FAIL;
  }

  switch (pdev->dev_state)
  {
    case USBD_STATE_ADDRESSED:
      if (cfgidx != 0U)
      {
        pdev->dev_config = cfgidx;
        ret = USBD_SetClassConfig(pdev, cfgidx);
        if (ret != USBD_OK)
        {
          USBD_CtlError(pdev, req);
        }
        else
        {
          USBD_CtlSendStatus(pdev);
          pdev->dev_state = USBD_STATE_CONFIGURED;
        }
      }
      else
      {
        USBD_CtlSendStatus(pdev);
      }
      break;

    case USBD_STATE_CONFIGURED:
      if (cfgidx == 0U)
      {
        pdev->dev_state = USBD_STATE_ADDRESSED;
        pdev->dev_config = cfgidx;
        USBD_ClrClassConfig(pdev, cfgidx);
        USBD_CtlSendStatus(pdev);
      }
      else if (cfgidx != pdev->dev_config)
      {
        USBD_ClrClassConfig(pdev, (uint8_t)pdev->dev_config);
        pdev->dev_config = cfgidx;
        ret = USBD_SetClassConfig(pdev, cfgidx);
        if (ret != USBD_OK)
        {
          USBD_CtlError(pdev, req);
          USBD_ClrClassConfig(pdev, (uint8_t)pdev->dev_config);
          pdev->dev_state = USBD_STATE_ADDRESSED;
        }
        else
        {
          USBD_CtlSendStatus(pdev);
        }
      }
      else
      {
        USBD_CtlSendStatus(pdev);
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }

  return ret;
}

/**
  * @brief  USBD_GetConfig handles GET_CONFIGURATION request.
  * @param  pdev: device instance
  * @param  req: setup request pointer
  * @retval None
  */
static void USBD_GetConfig(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  if (req->wLength != 1U)
  {
    USBD_CtlError(pdev, req);
  }
  else
  {
    switch (pdev->dev_state)
    {
      case USBD_STATE_DEFAULT:
      case USBD_STATE_ADDRESSED:
        pdev->dev_default_config = 0U;
        USBD_CtlSendData(pdev, (uint8_t *)&pdev->dev_default_config, 1U);
        break;

      case USBD_STATE_CONFIGURED:
        USBD_CtlSendData(pdev, (uint8_t *)&pdev->dev_config, 1U);
        break;

      default:
        USBD_CtlError(pdev, req);
        break;
    }
  }
}

/**
  * @brief  USBD_GetStatus handles GET_STATUS request.
  * @param  pdev: device instance
  * @param  req: setup request pointer
  * @retval None
  */
static void USBD_GetStatus(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  switch (pdev->dev_state)
  {
    case USBD_STATE_DEFAULT:
    case USBD_STATE_ADDRESSED:
    case USBD_STATE_CONFIGURED:
      if (req->wLength != 2U)
      {
        USBD_CtlError(pdev, req);
        break;
      }
#if (USBD_SELF_POWERED == 1U)
      pdev->dev_config_status = USB_CONFIG_SELF_POWERED;
#else
      pdev->dev_config_status = 0U;
#endif
      if (pdev->dev_remote_wakeup != 0U)
      {
        pdev->dev_config_status |= USB_CONFIG_REMOTE_WAKEUP;
      }
      USBD_CtlSendData(pdev, (uint8_t *)&pdev->dev_config_status, 2U);
      break;

    default:
      USBD_CtlError(pdev, req);
      break;
  }
}

/**
  * @brief  USBD_SetFeature handles SET_FEATURE request.
  * @param  pdev: device instance
  * @param  req: setup request pointer
  * @retval None
  */
static void USBD_SetFeature(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  if (req->wValue == USB_FEATURE_REMOTE_WAKEUP)
  {
    pdev->dev_remote_wakeup = 1U;
    USBD_CtlSendStatus(pdev);
  }
}

/**
  * @brief  USBD_ClrFeature handles CLEAR_FEATURE request.
  * @param  pdev: device instance
  * @param  req: setup request pointer
  * @retval None
  */
static void USBD_ClrFeature(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  switch (pdev->dev_state)
  {
    case USBD_STATE_DEFAULT:
    case USBD_STATE_ADDRESSED:
    case USBD_STATE_CONFIGURED:
      if (req->wValue == USB_FEATURE_REMOTE_WAKEUP)
      {
        pdev->dev_remote_wakeup = 0U;
        USBD_CtlSendStatus(pdev);
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      break;
  }
}



/**
  * @brief  USBD_ParseSetupRequest
  *         Copy buffer into setup structure
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval None
  */
void USBD_ParseSetupRequest(USBD_SetupReqTypedef *req, uint8_t *pdata)
{
  uint8_t *pbuff = pdata;

  req->bmRequest = *(uint8_t *)(pbuff);

  pbuff++;
  req->bRequest = *(uint8_t *)(pbuff);

  pbuff++;
  req->wValue = SWAPBYTE(pbuff);

  pbuff++;
  pbuff++;
  req->wIndex = SWAPBYTE(pbuff);

  pbuff++;
  pbuff++;
  req->wLength = SWAPBYTE(pbuff);
}


/**
  * @brief  USBD_CtlError
  *         Handle USB low level Error
  * @param  pdev: device instance
  * @param  req: usb request
  * @retval None
  */
void USBD_CtlError(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  UNUSED(req);

  (void)USBD_LL_StallEP(pdev, 0x80U);
  (void)USBD_LL_StallEP(pdev, 0U);
}


/**
  * @brief  USBD_GetString
  *         Convert Ascii string into unicode one
  * @param  desc : descriptor buffer
  * @param  unicode : Formatted string buffer (unicode)
  * @param  len : descriptor length
  * @retval None
  */
void USBD_GetString(uint8_t *desc, uint8_t *unicode, uint16_t *len)
{
  uint8_t idx = 0U;
  uint8_t *pdesc;

  if (desc == NULL)
  {
    return;
  }

  pdesc = desc;
  *len = ((uint16_t)USBD_GetLen(pdesc) * 2U) + 2U;

  unicode[idx] = *(uint8_t *)len;
  idx++;
  unicode[idx] = USB_DESC_TYPE_STRING;
  idx++;

  while (*pdesc != (uint8_t)'\0')
  {
    unicode[idx] = *pdesc;
    pdesc++;
    idx++;

    unicode[idx] = 0U;
    idx++;
  }
}


