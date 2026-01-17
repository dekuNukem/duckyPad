/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v2.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */
#include "hid_task.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  // --- Report ID 1: Keyboard ---
  0x05, 0x01,                    // Usage Page (Generic Desktop Ctrls)
  0x09, 0x06,                    // Usage (Keyboard)
  0xA1, 0x01,                    // Collection (Application)
  0x85, 0x01,                    //   Report ID (1)

  // Modifiers: 8 bits (Left Ctrl, Shift, Alt, GUI, Right Ctrl, etc.)
  0x05, 0x07,                    //   Usage Page (Keyboard/Keypad)
  0x19, 0xE0,                    //   Usage Minimum (Keyboard Left Control)
  0x29, 0xE7,                    //   Usage Maximum (Keyboard Right GUI)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x25, 0x01,                    //   Logical Maximum (1)
  0x75, 0x01,                    //   Report Size (1)
  0x95, 0x08,                    //   Report Count (8)
  0x81, 0x02,                    //   Input (Data, Var, Abs)

  // Reserved Byte: Padding for compatibility
  0x95, 0x01,                    //   Report Count (1)
  0x75, 0x08,                    //   Report Size (8)
  0x81, 0x01,                    //   Input (Const, Array, Abs)

  // 6-Key Rollover: Array of 6 keycodes
  0x95, 0x06,                    //   Report Count (6)
  0x75, 0x08,                    //   Report Size (8)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x25, 0x94,                    //   Logical Maximum (148)
  0x05, 0x07,                    //   Usage Page (Keyboard/Keypad)
  0x19, 0x00,                    //   Usage Minimum (0)
  0x29, 0x94,                    //   Usage Maximum (148)
  0x81, 0x00,                    //   Input (Data, Array, Abs)

  // Keyboard LEDs (Num Lock, Caps Lock, Scroll Lock)
  0x05, 0x08,                    //   Usage Page (LEDs)
  0x19, 0x01,                    //   Usage Minimum (Num Lock)
  0x29, 0x03,                    //   Usage Maximum (Scroll Lock)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x25, 0x01,                    //   Logical Maximum (1)
  0x75, 0x01,                    //   Report Size (1)
  0x95, 0x03,                    //   Report Count (3)
  0x91, 0x02,                    //   Output (Data, Var, Abs)
  
  // LED Padding (5 bits to complete the byte)
  0x75, 0x05,                    //   Report Size (5)
  0x95, 0x01,                    //   Report Count (1)
  0x91, 0x03,                    //   Output (Const, Var, Abs)
  0xC0,                          // End Collection

  // --- Report ID 2: Media Keys ---
  0x05, 0x0C,                    // Usage Page (Consumer)
  0x09, 0x01,                    // Usage (Consumer Control)
  0xA1, 0x01,                    // Collection (Application)
  0x85, 0x02,                    //   Report ID (2)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x25, 0x01,                    //   Logical Maximum (1)
  0x75, 0x01,                    //   Report Size (1)
  0x95, 0x08,                    //   Report Count (8)
  0x09, 0xB5,                    //   Usage (Scan Next Track)
  0x09, 0xB6,                    //   Usage (Scan Previous Track)
  0x09, 0xB7,                    //   Usage (Stop)
  0x09, 0xB8,                    //   Usage (Eject)
  0x09, 0xCD,                    //   Usage (Play/Pause)
  0x09, 0xE2,                    //   Usage (Mute)
  0x09, 0xE9,                    //   Usage (Volume Increment)
  0x09, 0xEA,                    //   Usage (Volume Decrement)
  0x81, 0x02,                    //   Input (Data, Var, Abs)
  0xC0,                          // End Collection

  // --- Report ID 3: Mouse ---
  0x05, 0x01,                    // Usage Page (Generic Desktop Ctrls)
  0x09, 0x02,                    // Usage (Mouse)
  0xA1, 0x01,                    // Collection (Application)
  0x85, 0x03,                    //   Report ID (3)
  0x09, 0x01,                    //   Usage (Pointer)
  0xA1, 0x00,                    //   Collection (Physical)

 // Mouse Buttons (1 to 5)
  0x05, 0x09,                    // Usage Page (Button)
  0x19, 0x01,                    // Usage Minimum (1)
  0x29, 0x05,                    // Usage Maximum (5)
  0x15, 0x00,                    // Logical Minimum (0)
  0x25, 0x01,                    // Logical Maximum (1)
  0x95, 0x05,                    // Report Count (5) 
  0x75, 0x01,                    // Report Size (1)
  0x81, 0x02,                    // Input (Data, Var, Abs)

  // Button Padding (3 bits to complete the byte)
  0x95, 0x01,                    // Report Count (1)
  0x75, 0x03,                    // Report Size (3)  
  0x81, 0x03,                    // Input (Const, Var, Abs)

  // Movement: X, Y, and Vertical Wheel
  0x05, 0x01,                    //     Usage Page (Generic Desktop Ctrls)
  0x09, 0x30,                    //     Usage (X)
  0x09, 0x31,                    //     Usage (Y)
  0x09, 0x38,                    //     Usage (Wheel)
  0x15, 0x81,                    //     Logical Minimum (-127)
  0x25, 0x7F,                    //     Logical Maximum (127)
  0x75, 0x08,                    //     Report Size (8)
  0x95, 0x03,                    //     Report Count (3)
  0x81, 0x06,                    //     Input (Data, Var, Rel)

  // Horizontal Wheel (AC Pan)
  0x05, 0x0C,                    //     Usage Page (Consumer)
  0x0A, 0x38, 0x02,              //     Usage (AC Pan)
  0x15, 0x81,                    //     Logical Minimum (-127)
  0x25, 0x7F,                    //     Logical Maximum (127)
  0x75, 0x08,                    //     Report Size (8)
  0x95, 0x01,                    //     Report Count (1)
  0x81, 0x06,                    //     Input (Data, Var, Rel)
  0xC0,                          //   End Collection (Physical)
  0xC0,                          // End Collection (Application)

  // --- Report ID 4/5: Custom Communication ---
  0x05, 0x01,                    // Usage Page (Generic Desktop Ctrls)
  0x09, 0x3A,                    // Usage (Counted Buffer)
  0xA1, 0x01,                    // Collection (Application)
  0x15, 0x00,                    //   Logical Minimum (0)
  0x26, 0xFF, 0x00,              //   Logical Maximum (255)

  // IN Report (Device to Host)
  0x85, 0x04,                    //   Report ID (4)
  0x75, 0x08,                    //   Report Size (8)
  0x95, CUSTOM_HID_EPIN_SIZE,    //   Report Count (Byte length)
  0x09, 0x3A,                    //   Usage (Counted Buffer)
  0x81, 0x82,                    //   Input (Data, Var, Abs, Volatile)

  // OUT Report (Host to Device)
  0x85, 0x05,                    //   Report ID (5)
  0x75, 0x08,                    //   Report Size (8)
  0x95, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE, // Report Count (Byte length)
  0x09, 0x3A,                    //   Usage (Counted Buffer)
  0x91, 0x82,                    //   Output (Data, Var, Abs, Volatile)
  /* USER CODE END 0 */
  0xC0    /*     END_COLLECTION	             */
};

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
  /* USER CODE BEGIN 6 */
  uint8_t* usb_rx_buf = ((USBD_CUSTOM_HID_HandleTypeDef*) hUsbDeviceFS.pClassData)->Report_buf;
  handle_hid_command(usb_rx_buf);
  return (USBD_OK);
  /* USER CODE END 6 */
}

/* USER CODE BEGIN 7 */
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
*/
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

