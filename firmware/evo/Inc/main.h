/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SW11_Pin GPIO_PIN_13
#define SW11_GPIO_Port GPIOC
#define SW12_Pin GPIO_PIN_14
#define SW12_GPIO_Port GPIOC
#define SW13_Pin GPIO_PIN_15
#define SW13_GPIO_Port GPIOC
#define SW14_Pin GPIO_PIN_0
#define SW14_GPIO_Port GPIOF
#define SW15_Pin GPIO_PIN_1
#define SW15_GPIO_Port GPIOF
#define OLED_RESET_Pin GPIO_PIN_0
#define OLED_RESET_GPIO_Port GPIOA
#define MINUS_BUTTON_Pin GPIO_PIN_1
#define MINUS_BUTTON_GPIO_Port GPIOA
#define PLUS_BUTTON_Pin GPIO_PIN_2
#define PLUS_BUTTON_GPIO_Port GPIOA
#define CARD_DETECT_Pin GPIO_PIN_4
#define CARD_DETECT_GPIO_Port GPIOA
#define CARD_DETECT_EXTI_IRQn EXTI4_15_IRQn
#define SD_CS_Pin GPIO_PIN_0
#define SD_CS_GPIO_Port GPIOB
#define SW1_Pin GPIO_PIN_13
#define SW1_GPIO_Port GPIOB
#define SW2_Pin GPIO_PIN_14
#define SW2_GPIO_Port GPIOB
#define SW3_Pin GPIO_PIN_15
#define SW3_GPIO_Port GPIOB
#define SW4_Pin GPIO_PIN_8
#define SW4_GPIO_Port GPIOA
#define LED_DATA_EN_Pin GPIO_PIN_10
#define LED_DATA_EN_GPIO_Port GPIOA
#define SW5_Pin GPIO_PIN_15
#define SW5_GPIO_Port GPIOA
#define SW6_Pin GPIO_PIN_3
#define SW6_GPIO_Port GPIOB
#define SW7_Pin GPIO_PIN_4
#define SW7_GPIO_Port GPIOB
#define SW8_Pin GPIO_PIN_5
#define SW8_GPIO_Port GPIOB
#define SW9_Pin GPIO_PIN_6
#define SW9_GPIO_Port GPIOB
#define SW10_Pin GPIO_PIN_7
#define SW10_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;
// TIM_HandleTypeDef htim17;
//extern IWDG_HandleTypeDef hiwdg;

extern char instruction[];
extern char project_url[];

extern uint8_t fw_version_major;
extern uint8_t fw_version_minor;
extern uint8_t fw_version_patch;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
