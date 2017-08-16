/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

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
#define E2B_Pin GPIO_PIN_0
#define E2B_GPIO_Port GPIOA
#define E2A_Pin GPIO_PIN_1
#define E2A_GPIO_Port GPIOA
#define E2_SW_Pin GPIO_PIN_3
#define E2_SW_GPIO_Port GPIOA
#define CARD_PRESENT_Pin GPIO_PIN_4
#define CARD_PRESENT_GPIO_Port GPIOA
#define SD_CS_Pin GPIO_PIN_0
#define SD_CS_GPIO_Port GPIOB
#define E1_SW_Pin GPIO_PIN_1
#define E1_SW_GPIO_Port GPIOB
#define E1A_Pin GPIO_PIN_2
#define E1A_GPIO_Port GPIOB
#define E1B_Pin GPIO_PIN_10
#define E1B_GPIO_Port GPIOB
#define BUTTON_2_Pin GPIO_PIN_11
#define BUTTON_2_GPIO_Port GPIOB
#define BUTTON_1_Pin GPIO_PIN_12
#define BUTTON_1_GPIO_Port GPIOB
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

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
