/* Src/usbd_desc.c */
#include "usbd_desc.h"
#include "usbd_def.h"
#include "usbd_hid_mouse.h"
#include "usbd_custom_hid.h"

/* Definitions for USB descriptors */
#define USBD_VID                      0x1234
#define USBD_PID                      0x5678
#define USBD_LANGID_STRING            1033

#define USBD_MANUFACTURER_STRING      "Your Manufacturer"
#define USBD_PRODUCT_STRING           "Composite HID Device"
#define USBD_SERIALNUMBER_STRING      "00000000001A"
#define USBD_CONFIGURATION_STRING     "Composite Config"

/* Define two interface strings for the two HID interfaces */
#define USBD_HID_MOUSE_INTERFACE_STRING   "HID Mouse Interface"
#define USBD_CUSTOM_HID_INTERFACE_STRING  "Custom HID Interface"

/* --- Device Descriptor --- */
#define USB_LEN_DEV_DESC              18
__ALIGN_BEGIN static uint8_t USBD_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END = {
  0x12,                       /* bLength */
  USB_DESC_TYPE_DEVICE,       /* bDescriptorType */
  0x00, 0x02,                 /* bcdUSB = 2.00 */
  0x00,                       /* bDeviceClass (defined per interface) */
  0x00,                       /* bDeviceSubClass */
  0x00,                       /* bDeviceProtocol */
  0x40,                       /* bMaxPacketSize0: 64 bytes */
  LOBYTE(USBD_VID), HIBYTE(USBD_VID), /* idVendor */
  LOBYTE(USBD_PID), HIBYTE(USBD_PID), /* idProduct */
  0x00, 0x02,                 /* bcdDevice = 2.00 */
  0x01,                       /* iManufacturer */
  0x02,                       /* iProduct */
  0x03,                       /* iSerialNumber */
  0x01                        /* bNumConfigurations */
};

/* --- Composite Configuration Descriptor ---
   We define two interfaces:
     Interface 0: Mouse HID, using bInterfaceNumber = 0 and iInterface = 0x04
     Interface 1: Custom HID, using bInterfaceNumber = 1 and iInterface = 0x05
*/

#define COMPOSITE_CONFIG_DESC_SIZE  (9 + (9+9+7) + (9+9+7+7))
__ALIGN_BEGIN uint8_t USBD_Composite_CfgDesc[COMPOSITE_CONFIG_DESC_SIZE] __ALIGN_END = {
  /* Configuration Descriptor */
  0x09,                               /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,        /* bDescriptorType: Configuration */
  LOBYTE(COMPOSITE_CONFIG_DESC_SIZE),   /* wTotalLength */
  HIBYTE(COMPOSITE_CONFIG_DESC_SIZE),
  0x02,                               /* bNumInterfaces: 2 interfaces */
  0x01,                               /* bConfigurationValue */
  0x00,                               /* iConfiguration */
  0xC0,                               /* bmAttributes: Self-powered */
  0x32,                               /* bMaxPower: 100 mA */

  /* --- Interface 0: Mouse HID --- */
  0x09,                               /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,            /* bDescriptorType: Interface */
  0x00,                               /* bInterfaceNumber: 0 (Mouse HID) */
  0x00,                               /* bAlternateSetting */
  0x01,                               /* bNumEndpoints: 1 */
  0x03,                               /* bInterfaceClass: HID */
  0x01,                               /* bInterfaceSubClass: Boot Interface */
  0x02,                               /* bInterfaceProtocol: Mouse */
  0x04,                               /* iInterface: Use string index 4 */

  /* HID Descriptor for Mouse */
	0x09,                        // bLength: HID Descriptor size
	0x21,                        // bDescriptorType: HID
	0x11, 0x01,                  // bcdHID: HID Class Spec release number (1.11)
	0x00,                        // bCountryCode
	0x01,                        // bNumDescriptors: 1
	0x22,                        // bDescriptorType: Report descriptor
	LOBYTE(HID_MOUSE_REPORT_DESC_SIZE), HIBYTE(HID_MOUSE_REPORT_DESC_SIZE),

  /* Endpoint Descriptor for Mouse IN endpoint */
  0x07,                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,             /* bDescriptorType: Endpoint */
  0x81,                               /* bEndpointAddress: IN (address 1) */
  0x03,                               /* bmAttributes: Interrupt */
  0x04, 0x00,                         /* wMaxPacketSize: 4 bytes */
  0x0A,                               /* bInterval: 10 ms */

  /* --- Interface 1: Custom HID --- */
  0x09,                               /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,            /* bDescriptorType: Interface */
  0x01,                               /* bInterfaceNumber: 1 (Custom HID) */
  0x00,                               /* bAlternateSetting */
  0x02,                               /* bNumEndpoints: 2 (IN and OUT) */
  0x03,                               /* bInterfaceClass: HID */
  0x00,                               /* bInterfaceSubClass: None */
  0x00,                               /* bInterfaceProtocol: None */
  0x05,                               /* iInterface: Use string index 5 */

  /* HID Descriptor for Custom HID */
  0x09,                               /* bLength: HID Descriptor size */
  0x21,                               /* bDescriptorType: HID */
  0x11, 0x01,                         /* bcdHID: HID Class Spec release number (1.11) */
  0x00,                               /* bCountryCode */
  0x01,                               /* bNumDescriptors: 1 */
  0x22,                               /* bDescriptorType: Report descriptor */
  LOBYTE(CUSTOM_HID_REPORT_DESC_SIZE), HIBYTE(CUSTOM_HID_REPORT_DESC_SIZE),

  /* Endpoint Descriptor for Custom HID IN endpoint */
  0x07,                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,             /* bDescriptorType: Endpoint */
  0x82,                               /* bEndpointAddress: IN (address 2) */
  0x03,                               /* bmAttributes: Interrupt */
  0x09, 0x00,                         /* wMaxPacketSize: 9 bytes */
  0x0A,                               /* bInterval: 10 ms */

  /* Endpoint Descriptor for Custom HID OUT endpoint */
  0x07,                               /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,             /* bDescriptorType: Endpoint */
  0x02,                               /* bEndpointAddress: OUT (address 2) */
  0x03,                               /* bmAttributes: Interrupt */
  0x09, 0x00,                         /* wMaxPacketSize: 9 bytes */
  0x0A                                /* bInterval: 10 ms */
};
uint16_t USBD_Composite_CfgDescSize = COMPOSITE_CONFIG_DESC_SIZE;

/* --- String Descriptors --- */
//#define USB_LEN_LANGID_STR_DESC       4
__ALIGN_BEGIN static uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
  USB_LEN_LANGID_STR_DESC,
  USB_DESC_TYPE_STRING,
  LOBYTE(USBD_LANGID_STRING),
  HIBYTE(USBD_LANGID_STRING),
};

/* Maximum string descriptor size */
#define USBD_MAX_STR_DESC_SIZ         64
static uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ];

/* Modified interface string descriptor callback:
   This function now returns different strings based on the requested index.
   (Indices 4 and 5 are used for our two HID interfaces.) */

uint8_t *USBD_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length, uint8_t index)
{
  switch(index)
  {
    case 4:
      USBD_GetString((uint8_t *)USBD_HID_MOUSE_INTERFACE_STRING, USBD_StrDesc, length);
      break;
    case 5:
      USBD_GetString((uint8_t *)USBD_CUSTOM_HID_INTERFACE_STRING, USBD_StrDesc, length);
      break;
    default:
      USBD_GetString((uint8_t *)"ffff", USBD_StrDesc, length);
      break;
  }
  return USBD_StrDesc;
}

/* Other string descriptor callbacks remain unchanged */
uint8_t *USBD_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  *length = sizeof(USBD_DeviceDesc);
  return USBD_DeviceDesc;
}

uint8_t *USBD_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  *length = sizeof(USBD_LangIDDesc);
  return USBD_LangIDDesc;
}

uint8_t *USBD_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  USBD_GetString((uint8_t *)USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
}

uint8_t *USBD_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  USBD_GetString((uint8_t *)USBD_PRODUCT_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
}

uint8_t *USBD_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  USBD_GetString((uint8_t *)USBD_SERIALNUMBER_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
}

uint8_t *USBD_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  USBD_GetString((uint8_t *)USBD_CONFIGURATION_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
}

/* Group all descriptor callback functions into one structure */
USBD_DescriptorsTypeDef FS_Desc = {
  USBD_DeviceDescriptor,
  USBD_LangIDStrDescriptor, 
  USBD_ManufacturerStrDescriptor,
  USBD_ProductStrDescriptor,
  USBD_SerialStrDescriptor,
  USBD_ConfigStrDescriptor,
  /* For interface string, your USB core should call your custom function passing the requested index.
     If not, modify the core accordingly. */
  USBD_InterfaceStrDescriptor,
};
