/* usbd_composite.h */
#ifndef __USBD_COMPOSITE_H
#define __USBD_COMPOSITE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_def.h"

/* Declaration of the composite class structure */
extern USBD_ClassTypeDef USBD_Composite;
#define USBD_COMPOSITE_CLASS &USBD_Composite

#ifdef __cplusplus
}
#endif

#endif /* __USBD_COMPOSITE_H */