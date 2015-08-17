#include "BSP.h"

extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

void BSP_USB_Init(void)
{
	__GPIOA_CLK_ENABLE();
	MX_USB_DEVICE_Init();
	USBD_Interface_fops_FS.Init();
}
